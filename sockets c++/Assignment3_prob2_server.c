#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

char em_id[30][50]={"CA001","CA002","CA003","CA004","CA005","CA006","CA007","CA008","CA009","CA010","CA011","CA012","CA013","CA014","CA015","CA016","CA017","CA018","CA019","CA020","CA021","CA022","CA023","CA024","CA025","CA026","CA027","CA028","CA029","CA030"};
int hol[30]={22,16,6,28,18,20,7,9,24,23,24,8,16,10,24,24,19,13,15,8,8,13,23,27,13,11,17,16,27,5};
int pend[30]={15,16,6,5,6,18,5,9,24,23,21,8,12,7,5,9,14,13,8,4,7,5,12,0,3,10,12,8,12,0};
int extr[30]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int itt(char *a){
    int n=-1;
    /*for(int j=0;a[j]!='\0';j++){
            printf("%d ",a[j]);
        }*/
    for(int i=0;i<30;i++){
        /*for(int j=0;em_id[i][j]!='\0';j++){
            printf("%d ",em_id[i][j]);
        }*/
        //printf("%s  %s\n",a,em_id[i]);
        if(strcmp(a,em_id[i])==0) { n=i; break;}
    }
    return n;
}

int main(){
    int socket_server;
    socket_server =socket(AF_INET,SOCK_STREAM,0);

    //defining address
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8085);
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
        char a[50]; int n;
        recv(client,a,sizeof(a),0);
        if(strcmp(a,"exit")==0) break;
        recv(client,&n,sizeof(n),0);
        int index=itt(a);
        if(index == -1){
            send(client,&index,sizeof(index),0);
            continue;
        }
        send(client,&hol[index],sizeof(hol[index]),0);
        if(n>pend[index]){
            n=n-pend[index];
            pend[index]=0;
            extr[index]=n;
        }
        else{
            pend[index]-=n;
        }
        send(client,&pend[index],sizeof(pend[index]),0);
        send(client,&extr[index],sizeof(extr[index]),0);
        
        
    }
    int close(client);
    return 0;

}