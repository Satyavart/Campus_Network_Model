#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<sys/types.h>
#include<signal.h>


//basic global variable
#define NAME_LEN 32
#define MAX_CLIENT 100
#define BUFFER 2048
#define FILEBUFF 4096
#define MAX_LINE 4096

ssize_t total=0;
static _Atomic unsigned int cli_count = 0;
static int uid=10;

//client structure
typedef struct{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[NAME_LEN];
} client_t;

client_t *clients[MAX_CLIENT];

//threading
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

//clearing buffer
void str_overwrite_stdout(){
    printf("\r%s","> ");
    fflush(stdout);
}

//modifing file end
void str_trim_lf(char* arr,int len){
    for(int i=0;i<len;i++){
        if(arr[i]=='\n'){
            arr[i]='\0';
            break;
        }
    }
} 

//adding client to queue
void queue_add(client_t *cl){
    pthread_mutex_lock(&client_mutex);
    for(int i=0;i<MAX_CLIENT;i++){
        if(!clients[i]){
            clients[i]=cl;
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);
}

//remove client from queue
void queue_remove(int uid1){
    pthread_mutex_lock(&client_mutex);
    for(int i=0;i<MAX_CLIENT;i++){
        if(clients[i]){
            if(clients[i]->uid==uid1){
                clients[i]=NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&client_mutex);
   
}

//send message to all
void send_message(char *s,int uid1){
    pthread_mutex_lock(&client_mutex);
    for(int i=0;i<MAX_CLIENT;i++){
        if(clients[i]){
            if(clients[i]->uid!=uid1){
                if(write(clients[i]->sockfd,s,strlen(s))<0){
                    printf("ERROR: Write to descriptor\n");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&client_mutex);
}

//printing IP
void print_ip_addr(struct sockaddr_in addr){
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr&0xff,
        (addr.sin_addr.s_addr&0xff00)>>8,
        (addr.sin_addr.s_addr&0xff0000)>>16,
        (addr.sin_addr.s_addr&0xff000000)>>24);
}


int receive_file(int socket,char *filename){
    FILE *f = fopen(filename,"wb");
    unsigned long filesize;
    recv(socket,&filesize,sizeof(filesize),0);
    char buffer[filesize];
    int received=0;
    int z=0;
    while(received<filesize){
        int receving_size;
        recv(socket,&receving_size,sizeof(receving_size),0);
        char temp_buf[receving_size];
        z = recv(socket,temp_buf,sizeof(temp_buf),0);
        if(z == -1) break;
        for(int i=0;i<receving_size;i++){
            buffer[received+i]=temp_buf[i];
        }
        received += receving_size;
    }
    fwrite(buffer,1,sizeof(buffer),f);
    fclose(f);
    return z;
}



int send_files(int socket,char* filename){
    FILE *f = fopen(filename,"rb");
    fseek(f, 0, SEEK_END);
    unsigned long filesize = ftell(f);
    send(socket,&filesize,sizeof(filesize),0);
    rewind(f);
    char *buffer = (char*)malloc(sizeof(char)*filesize);
    fread(buffer,sizeof(char),filesize,f);
    int total_send=0;
    int z=0;
    while(total_send<filesize){
        int send_size = (filesize-1)%1024 +1;   
        send(socket,&send_size,sizeof(send_size),0);
        z = send(socket,buffer+total_send,send_size,0); 
        if(z==-1) break;
        total_send += z;
    }
    fclose(f);
    return z;
}

/* Handle all communication with the client */
void *handle_client(void *arg){
    char buffer[BUFFER];
    char name[NAME_LEN];
    int leave_flag=0;

    cli_count++;
    client_t *cli=(client_t*)arg;

    if(recv(cli->sockfd,name,NAME_LEN,0)<=0||strlen(name)<2||strlen(name)>=NAME_LEN-1){
        printf("Enter the name correctly\n");
        leave_flag=1;
    }
    else{
        strcpy(cli->name,name);
        sprintf(buffer,"\033[1;32m%s\033[0m has joined\n",cli->name);
        printf("%s",buffer);
        send_message(buffer,cli->uid);
    }
    bzero(buffer,BUFFER);
    
    while(1){
        if(leave_flag) break;
        int recieve = recv(cli->sockfd,buffer,BUFFER,0); 
        if(recieve == 0 || strcmp(buffer,"exit")==0){
            sprintf(buffer,"\n\033[1;33m%s\033[0m has left the chat\n",cli->name);
            printf("%s",buffer);
            send_message(buffer,cli->uid);
            leave_flag=1;
        }
        //Sending to client
        else if(recieve > 0 && strcmp(buffer,"#down")==0){
            char filename[NAME_LEN];
            recv(cli->sockfd,filename,sizeof(filename),0);
            bzero(buffer,BUFFER);
            sprintf(buffer,"%s is requesting to \033[1;35mdownload\033[0m %s\n",cli->name,filename);
            printf("%s\n",buffer);
            send_message(buffer,cli->uid);    
            send_files(cli->sockfd,filename);
            printf("[+]Data written in the file successfully.\n");
            bzero(filename,NAME_LEN);
        }
        //Downloading on server
        else if(recieve > 0 && strcmp(buffer,"#up")==0){
            char filename[NAME_LEN];
            recv(cli->sockfd,filename,sizeof(filename),0);
            bzero(buffer,BUFFER);
            sprintf(buffer,"%s is trying to \033[1;35mUpload\033[0m %s on the server\n",cli->name,filename);
            printf("%s\n",buffer);
            send_message(buffer,cli->uid);
            receive_file(cli->sockfd,filename);
            printf("[+]Data written in the file successfully.\n");
            bzero(filename,NAME_LEN);
        }
        //sending normal message
        else if(recieve>0){
            if(strlen(buffer)>0){
                send_message(buffer,cli->uid);
                str_trim_lf(buffer,strlen(buffer));
                printf("%s\n",buffer);
            }
        }
        //Error detection
        else{
            printf("ERROR: -1\n");
            leave_flag=1;
        }
        bzero(buffer,BUFFER);
    }
    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());

    return NULL;
}

int main(){
    //basic variables
    int option = 1; int listenfd=0; int conn=0;

    //creating socket
    struct sockaddr_in client;
    struct sockaddr_in server;
    pthread_t tid;

    //socket setting
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_port=htons(8999);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    //signal to close socket 
    signal(SIGPIPE,SIG_IGN);

    //specifing the protocol level 
    if(setsockopt(listenfd,SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option,sizeof(option))<0){
        printf("ERROR: Specify setsocketopt\n");
        return EXIT_FAILURE;
    }

    //Bind
    if(bind(listenfd,(struct sockaddr*)&server,sizeof(struct sockaddr_in))<0){
        printf("ERROR: Bind\n");
        return EXIT_FAILURE;
    }

    //listen
    if(listen(listenfd,8877)<0){
        printf("ERROR: Listen\n");
        return EXIT_FAILURE;
    }

    //Intro
    printf("Welcome to the CSE class\n");

    //loop
    while(1){
        socklen_t clilen=sizeof(client);
        conn = accept(listenfd,(struct sockaddr*)&client,&clilen);

        //checking for max client
        if(cli_count+1==MAX_CLIENT){
            printf("MAX client connected. Connection Rejected\n");
            print_ip_addr(client);
            close(conn);
            continue;
        }

        //Client details
        client_t *cli=(client_t *)malloc(sizeof(client_t));
        cli->address=client;
        cli->sockfd=conn;
        cli->uid=uid++;

        //add client to queue
        queue_add(cli);
        pthread_create(&tid,NULL,&handle_client,(void *)cli);

        //reduse CPU usage
        sleep(1);
    }


    return EXIT_SUCCESS;
}