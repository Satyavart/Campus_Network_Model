#include<bits/stdc++.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
using namespace std;
int Buffer_size = 512;


int sendfile(int socket,char* filename){
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

int main(){
    int socket_server;
    socket_server =socket(AF_INET,SOCK_STREAM,0);

    //defining address
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8095);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //bind ip and port
    bind(socket_server,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));

    //listening
    listen(socket_server,8);

    //accepting connection
    int client;
    client=accept(socket_server,NULL,NULL);

    char a[5],file[100];
    recv(client,a,sizeof(a),0); //recieving download or upload info
    recv(client,file,sizeof(file),0); //recieving file name

    //uploading on server
    if(strcmp(a,"put")==0){
        receive_file(client,file);
    }
    if(strcmp(a,"get")==0){
        sendfile(client,file);
    }


    int close(client);
    return 0;
}