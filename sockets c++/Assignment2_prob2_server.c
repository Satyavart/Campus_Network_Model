//server side
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

int scan(int n,char *a){
    FILE *fp;
    char* line=NULL;
    int len=0,read;
    int j=0,flag=0;
    if(n==0) fp=fopen("states.txt","r");
    else fp=fopen("capital.txt","r");
    while (getline(&line, &len, fp)!= -1) {
        if(strcmp(a,line)==0){flag=1; break;}
        //if(strcmp(a,line)!=0) printf("diff : %d\n",strcmp(a,line) );
        j++;
    }
    fclose(fp);
    //printf("flag : %d\nj : %d",flag,j);
    if(flag==0) return -1;
    return j+1;
}

char* send_cli(int n,int i){
    FILE *fp;
    char* line=NULL;
    int len=0,read;
    if(n==0) fp=fopen("states.txt","r");
    else fp=fopen("capital.txt","r");
    while (i--) {
        getline(&line, &len, fp);
    }
    //printf("line : %s",line);
    fclose(fp);
    return line;
}

void append(int n,char *a){
    FILE *fp;
    char* line=NULL;
    int len=0,read;
    if(n==0) fp=fopen("states.txt","a");
    else fp=fopen("capital.txt","a");
    char *t="\r\n";
    fprintf(fp,"%s",a);
    fprintf(fp,"%s",t);
    fclose(fp);
}

int main(){
    int socket_server;
    socket_server =socket(AF_INET,SOCK_STREAM,0);

    //defining address
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(10000);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //bind ip and port
    bind(socket_server,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));

    //listening
    listen(socket_server,8);

    //accepting connection
    int client;
    client=accept(socket_server,NULL,NULL);

    //program
    while(1){    
        char a[50];
        recv(client,a,sizeof(a),0);
        if(strcmp(a,"exit\r\n")==0) break;
        int i,j=0;

        //scanning states
        i=scan(0,a);
        if(i>=0){
            int k=1;
            send(client,&k,sizeof(k),0);
            char *abc=send_cli(1,i); char fl[50];
            strcpy(fl,abc);
            send(client,fl,sizeof(fl),0);
            continue;
        }
        //scaning capitals
        if(i==-1) {
            i=scan(1,a);
            printf("%d",i);
            if(i>=0){
                int k=1;
                send(client,&k,sizeof(k),0);
                char *abc=send_cli(0,i); char fl[50];
                strcpy(fl,abc);
                send(client,fl,sizeof(fl),0);
                continue;
            }
        }
        //appending new data
        if(i==-1){
            append(0,a);
            int k=0;
            send(client,&k,sizeof(k),0);
            char temp[50];
            recv(client,temp,sizeof(temp),0);
            append(1,temp);
        }

    
    }
    int close(client);

}