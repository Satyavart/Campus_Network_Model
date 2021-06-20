//client side

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main(){
    int net_socket;
    //creating socket
    net_socket=socket(AF_INET,SOCK_STREAM,0);
    
    //specifying server address
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(10000);
    server_address.sin_addr.s_addr=INADDR_ANY;

    //connecting
    int status = connect(net_socket,(struct sockaddr*)&server_address,sizeof(struct sockaddr_in));
    if(status == -1) {printf("\n Coneection error \n"); return 0;}

    //program
    printf("\nType exit to exit\n");
    while(1){ 
        char c[50];
        printf("\nEnter state or capital : ");
        scanf("%s",&c);
        strcat(c,"\r\n");
        send(net_socket,&c,sizeof(c),0);
        if(strcmp(c,"exit\r\n")==0)  break;
        char val[50];
        int n;
        recv(net_socket,&n,sizeof(n),0);
        if(n==0){
            char z[50];
            printf("\nEnter the remaining info to save : ");
            scanf("%s",&z);
            send(net_socket,&z,sizeof(z),0);
            printf("\nServer list is updated");
            continue;
        }
        recv(net_socket,val,sizeof(val),0);
        printf("\n%s",val);
        
    }
    //exit
    printf("\nExiting\n");
    int close(net_socket);

}