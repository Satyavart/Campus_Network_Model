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
	char s[20][20]={"wurly", "prolusion" ,"gymnasiarchy", "discomania", "quern", "modiste", "plutonomy" ,"quadrivoltine", "chaise", "electuary", "ophidiarium", "immunifacient", "terrigenous", "velveret", "apotelesm", "quilombo", "porlocking", "lyterian", "oculiform", "meristic"};
    printf("\nPress -1 to exit\n");
    while(1){ 
        int n;
        printf("\nEnter a number randomly : ");
        scanf("%d",&n);
        send(net_socket,&n,sizeof(n),0);
        if(n==-1) break;
        n=n%20;
        printf("\nSending \"%s\" to server. \n",s[n]);
        send(net_socket,&s[n],sizeof(s[n]),0);
        printf("\nWord Send.\n");
        int present;
        recv(net_socket,&present,sizeof(present),0);
        //printf("%d",present);
        char meaning[100];
        if(present>=0){
            recv(net_socket,meaning,sizeof(meaning),0);
            printf("\nmeaning of %s is : %s",s[n],meaning);
        }
        else{
            printf("\nEnter Meaning of given word : ");
            scanf("%s",meaning);
            printf("\nSending Meaning to server...");
            send(net_socket,&meaning,sizeof(meaning),0);
            printf("\nMeaning Sent.\nServer list is updated.");
        }
    }
    //exit
    printf("\nExiting\n");
    int close(net_socket);

}