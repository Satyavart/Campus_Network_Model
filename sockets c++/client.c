#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

int main(){
    int net_socket;

    //creating socket
    net_socket=socket(AF_INET,SOCK_STREAM,0);
    
    //specifing server address
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8080);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //connnecting
    int status=connect(net_socket,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));

    //verifying connection 
    if(status==-1){
        printf("Connection Unavialabe\n");
        return -1;
    }

    //char array to char array
    printf("Enter 1st string : ");
    char a[100];
    scanf("%s",&a);
    send(net_socket,a,sizeof(a),0);

    //pointer array to character array
    printf("Enter 2nd string : ");
    char *b;
    scanf("%s",&b);
    send(net_socket,&b,sizeof(b),0);

    //integer to integer
    printf("Enter a integer : ");
    int c;
    scanf("%d",&c);
    send(net_socket,&c,sizeof(c),0);


    //closing connection*/
    int close(net_socket);
    
    return 0;
}