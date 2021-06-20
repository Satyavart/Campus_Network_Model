#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

int main(){
    int ser_socket;

    //creating socket
    ser_socket=socket(AF_INET,SOCK_STREAM,0);
    
    //specifing server address
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8080);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //binding
    bind(ser_socket,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
    
    //listening
    listen(ser_socket,5);

    //accepting connection
    int client_socket;
    client_socket = accept(ser_socket,NULL,NULL);


    char a[100];
    recv(client_socket,a,sizeof(a),0);
    printf("1st is : %s\n",a);

    char b[100];
    recv(client_socket,b,sizeof(b),0);
    printf("2nd is : %s\n",b);

    int c;
    recv(client_socket,&c,sizeof(c),0);
    printf("Integer is : %d\n",c);
   
    //closing connection
    int close(ser_socket);
    
    return 0;
}