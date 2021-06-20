#include<bits/stdc++.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
using namespace std;

int Buffer_size = 512;
void error(char *msg){
    perror(msg);
    exit(1);

}

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
    return z;
}

int main(int argc, char** argv){
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //getting values
    char* IP=argv[1];
    int port=atoi(argv[2]);
    char file[100];
    strcpy(file,argv[4]);

    /*for(int i=0;i<argc;i++){
        cout<<argv[i]<<": ";
        for(int j=0;j<8;j++){
            cout<<int(argv[i][j])<<" ";
        }
        cout<<endl;
    }*/

    int net_socket;
    //creating socket
    net_socket=socket(AF_INET,SOCK_STREAM,0);


    //defining address
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(8095);
    //server_address.sin_addr.s_addr=INADDR_ANY;
    
    //any IP
    if(strcmp(IP,"INADDR_ANY")==0)  server_address.sin_addr.s_addr=INADDR_ANY;
    //specific IP
    else server_address.sin_addr.s_addr=inet_addr(IP);

    //connecting
    int status = connect(net_socket,(struct sockaddr*)&server_address,sizeof(struct sockaddr_in));
    if(status == -1) {printf("\nConeection error \n"); return 0;}

    cout<<"\nConnected \n";

    char a[5];
    strcpy(a,argv[3]);
    send(net_socket,a,sizeof(a),0); //sending download or upload info
    send(net_socket,file,sizeof(file),0);  //sending file name

    //Uploading on server
    if(strcmp(argv[3],"put")==0){
        cout<<"Uploading file";
        sendfile(net_socket,file);
        cout<<"\nFile Uploaded";
    }
    if(strcmp(argv[3],"get")==0){
        cout<<"Downloading file";
        receive_file(net_socket,file);
        cout<<"\nFile Downloaded";
    }

} 