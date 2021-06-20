//server side
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

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
    //printf("Listening.....");
    listen(socket_server,8);

    //accepting connection
    int client;
    client=accept(socket_server,NULL,NULL);

    //creating dict
    char word_dic[20][20]={"prolusion", "discomania", "modiste", "plutonomy" ,"quadrivoltine", "chaise", "electuary", "ophidiarium", "immunifacient", "terrigenous", "velveret", "quilombo", "porlocking", "lyterian", "oculiform"};
    char mean_dic[20][100]={"activity or display", "obsession for disco music", "stone mill worked by hand", "a maker or seller of fashionable dresses and hats", "economics", "having four litters in a single year", "light open carriage for one or two people", "medicine mixed with honey or syrup", "snake-house", "causing immunity", "produced on land", "soft velvety cotton and silk blend", "remote settlement where fugitive slaves hide", "irksome intrusion or interruption", "indicating the end of a disease", "shaped like an eye"};
    int last=15;

    //program
    while(1){    
        char word[20];
        int present=-1;
        int n;
        recv(client,&n,sizeof(n),0);
        if(n==-1) break;
        recv(client,word,sizeof(word),0);
        for(int i=0;i<last;i++){
            if(strcmp(word,word_dic[i])==0){ present=i; break;}
        }
        if(present<0) {
            strcpy(word_dic[last],word);
            char temp[100]; 
            send(client,&present,sizeof(present),0);
            recv(client,temp,sizeof(temp),0);
            strcpy(mean_dic[last],temp);
            last++;
            printf("\n Server list is updated ");
        }
        else {
            send(client,&present,sizeof(present),0);
            send(client,&mean_dic[present],sizeof(mean_dic[present]),0);
        }
    }
    int close(client);

}