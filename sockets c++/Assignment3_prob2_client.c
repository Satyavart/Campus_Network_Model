#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

int main(){
    int net_socket;
    //creating socket
    net_socket=socket(AF_INET,SOCK_STREAM,0);
    
    //specifying server address
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(8085);
    server_address.sin_addr.s_addr=INADDR_ANY;

    //connecting
    int status = connect(net_socket,(struct sockaddr*)&server_address,sizeof(struct sockaddr_in));
    if(status == -1) {printf("\n Coneection error \n"); return 0;}

    printf("\nType exit to disconnect!!!");
    while(1){ 
        char c[50];
        int n;
        printf("\nEnter Employee ID : ");
        scanf("%s",&c);
        send(net_socket,&c,sizeof(c),0);
        if(strcmp(c,"exit")==0)  break;
        printf("\nEnter No. of leaves required: ");
        scanf("%d",&n);
        send(net_socket,&n,sizeof(n),0);
        int i,j,k;
        recv(net_socket,&i,sizeof(i),0);
        if(i==-1){
            printf("\nID no found\n");
            continue;
        }
        recv(net_socket,&j,sizeof(j),0);
        recv(net_socket,&k,sizeof(k),0);
        printf("\nNo. Total Leaves : %d\nNo. of Current Balance leaves: %d\nNo. of extra leaves : %d\n",i,j,k);
    }
    //exit
    printf("\nExiting\n");
    int close(net_socket);
    return 0;
}