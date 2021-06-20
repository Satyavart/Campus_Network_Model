#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<string.h>
#include <stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<sys/types.h>
#include<signal.h>

#define NAME_LEN 64
#define BUFFER 2048
#define FILEBUFF 4096
#define MAX_LINE 4096


//basic global variable
int silent=0;
volatile sig_atomic_t flag=0;
int sockfd=0;
char name[NAME_LEN];
ssize_t total=0;

//exiting on ^C
void catch_ctrl_c(int sig){
    flag=1;
    fflush(stdout);
}

//dynamic prining of a starter
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

//dynamic printing of your name
void prac(){
    printf("\r> \033[0;34mYou: \033[0m");
    fflush(stdout);
}

//Downloading from server to client

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
    return z;
}


//thread for sending message
void send_meassage_handler(){
    char message[BUFFER]={};
    char buffer[BUFFER+NAME_LEN+NAME_LEN]={};

    while(1){
        prac();
        fgets(message,BUFFER,stdin);
        str_trim_lf(message,BUFFER);
        if(strcmp(message,"exit")==0) break;
        else if(strcmp(message,"#down")==0){
            send(sockfd,message,strlen(message),0);
            printf("Enter name of file to download: ");
            //Getting file name
            char file[NAME_LEN]={};
            fgets(file,NAME_LEN,stdin);
            str_trim_lf(file,NAME_LEN);
            send(sockfd,file,sizeof(file),0);
            receive_file(sockfd,file);
            printf("[+]File Downloaded successfully.\n");
            //printf("Receive Success, NumBytes = %ld\n", total);
            bzero(file,NAME_LEN);
        }
        else if(strncmp(message,"#up",3)==0){
            send(sockfd,message,strlen(message),0);
            //Getting filename
            printf("Enter name of file to upload: ");
            char file[NAME_LEN]={};
            fgets(file,NAME_LEN,stdin);
            str_trim_lf(file,NAME_LEN);
            send(sockfd,file,sizeof(file),0);
            send_files(sockfd,file);
            printf("[+]File uploaded successfully.\n");
            bzero(file,NAME_LEN);
        }
        else {
            sprintf(buffer,"\033[1;31m%s :\033[0m %s\n",name, message);
            send(sockfd,buffer,strlen(buffer),0);
        }
        bzero(buffer,BUFFER+NAME_LEN);
        bzero(message,BUFFER);
    }

    catch_ctrl_c(2);
}

//thread for recieving message
void recieve_meassage_handler(){
    char message[BUFFER]={};
    while(1){
        int recieve = recv(sockfd,message,BUFFER,0);
        if(recieve>0){
            str_overwrite_stdout();
            printf("%s ",message);
            str_overwrite_stdout();
        }
        else if(recieve==0) break;
        prac();
        bzero(message,BUFFER);
    }
}


int main(int argc,char **argv){
    if(argc!=3){
        printf("ERROR: Incomplete variables\n");
        return EXIT_FAILURE;
    }
    int port = atoi(argv[2]);
    char *ip=argv[1];

    //signals to close socket
    signal(SIGINT,catch_ctrl_c);

    printf("Enter your name : ");
    fgets(name,NAME_LEN,stdin);
    str_trim_lf(name,strlen(name));
    if(strlen(name)>NAME_LEN-1 || strlen(name)<2){
        printf("Enter name correctly\n");
        return EXIT_FAILURE;
    }

    //defining socket
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd < 0) {
        printf("ERROR: opening socket\n");
        return EXIT_FAILURE;
    }

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip);
    //connecting to server
    int er = connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(er==-1){
        printf("ERROR: Connect\n");
        return EXIT_FAILURE;
    }

    //send name to server
    send(sockfd,name,NAME_LEN,0);

    //Intro
    printf("%s, Welcome to the CSE class\n",name);

    //thread for sending message
   pthread_t send_mess;
    if(pthread_create(&send_mess,NULL,(void *)send_meassage_handler,NULL)!=0){
        printf("ERROR: Phtread message not send\n");
        return EXIT_FAILURE;
    } 

    //thread for recieving message

    pthread_t recieve_mess;
    if(pthread_create(&recieve_mess,NULL,(void *)recieve_meassage_handler,NULL)!=0){
        printf("ERROR: Phtread message not recieved\n");
        return EXIT_FAILURE;
    }

    while (1){
        if(flag){
            printf("\nBYE\n");
            break;
        }
    }
    close(sockfd);
    
    return EXIT_SUCCESS;



}