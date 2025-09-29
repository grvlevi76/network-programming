
//disclaimer - 
//1. our program is made considering there can't be any other possibility than 2 linked list allocated by getaddrinfo() , one havng ipv4 and other ipv6
//2. our program is using ipv6 for connection inside socket , its suppossed to handle both ipv4 and ipv6 incoming connections

//next task->
//make it multi client
//add a feature to let server keep accepting new connection even after recieving stuff from one client
// u can  make array with size backlog for total new socket to make n accept for new connections 
//(done)u can just allocate ipv6 socket to port 8080 it will handle both  ipv4 and ipv6
//(done)now send anything(response) from server to client

//long term goal -> 
// make it accept request for http and then respond with html file?
//can we send and recieve files like images , videos etc 
//make it like group chat messanger 

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For inet_pton
#include <string.h> // For memset
#include <unistd.h> // For close

#define PORT "8080"
#define BACKLOG 5 // How many pending connections queue will hold
#define buff_size 100     //message buffer size


int main(){
    struct addrinfo hints , *servinfo, *p;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    char buff[buff_size];
    int sockfd4,sockfd6,client_sockfd, copied;
    memset(&hints,0,sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags= AI_PASSIVE;
    
    //getting the address information
    if(getaddrinfo(NULL,PORT,&hints,&servinfo)!=0) {
        printf("error: getaddinfo() failed\n");
        return 1;
    }
    
    //socket creation and binding for ipv6 (cause ipv6 can handle upcoming ipv4 too)
    
    for(p=servinfo;p!=NULL;p=p->ai_next) {
        if(p->ai_family==AF_INET6) {

            /*printf("ip type : %d\n",p->ai_family);  // to check which ip style we got allocated
            printf("lets see : %s\n",p->ai_next);*/

            //socket for ipv6
            sockfd6 = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
            if(sockfd6<0) {
                printf("socket alllocation for ipv6 failed\n");
                return 1;
            } 
            //binding the ipv6
            printf("binding for ipv6 connection...\n");
            if(bind(sockfd6,p->ai_addr,p->ai_addrlen)<0) {    //binds to all available address with ipv6 (except ipv4 too)
                perror("bind error : ");
                return 1;
            }else {
                printf("bind successfull\n");
                break;
            }
        }
    }

    if(p==NULL) {
        printf("error : no ipv6 connection found , binding failed!");
        freeaddrinfo(servinfo);
        close(sockfd6);
        return 1;
    }

    //litening to upcoming connection
    printf("\nserver is listening to port : %s...\n",PORT);
    if(listen(sockfd6,BACKLOG)<0) {
        perror("listen error : ");
        return 1;
    }
    //providing new socket to connection by  accepting them

    while(1) {
        printf("\n<------waiting for new client or operation----->\n");

        //accepting new connection
        addr_size = sizeof(client_addr);
        if((client_sockfd = accept(sockfd6,(struct sockaddr *)&client_addr,&addr_size))<0) {
            perror("accept error : ");
            continue;
        }

        //storing clients address
        struct sockaddr_in *c= (struct sockaddr_in *)&client_addr;
        char c_ipaddr[50];
        inet_ntop(c->sin_family,&c->sin_addr,c_ipaddr,sizeof(c_ipaddr));
        
        printf("connection accepted for client(%s)\n",c_ipaddr);

        if(fork()==0) {
            close(sockfd6);     //closes mainfd for child process cause we dont need it in child process

            int a;
            printf("\n a = ");
            scanf("%d",&a);

            //recieving fom client -->
            if((copied = recv(client_sockfd,buff,buff_size-1,0))<0) {
                perror("recv error : ");
                exit(0);
            }

            buff[copied] ='\0';
            printf("recieved from client(%s) : '%s'\n",c_ipaddr,buff);

            //responding(sending) to client-->
            printf("responding back to client(%s).....\n",c_ipaddr);
            strcpy(buff,"i'm doing good client ");
            strcat(buff,c_ipaddr);

            if(send(client_sockfd,buff,strlen(buff),0)<0) {
                perror("send error : ");
                exit(0);
            }
            printf("responded successfully\n");

             printf("\n a = ");
            scanf("%d",&a);

            close(client_sockfd);
            printf("client(%s)'s operation ended\n",c_ipaddr);
            printf("\n<------waiting for new client or operation----->\n");
            exit(0);
        }
        close(client_sockfd);
    }

    freeaddrinfo(servinfo);
    close(client_sockfd);
    close(sockfd6);
    return 0;
}

//ques -

//we definitely need to handle the case when client is connected to server but its not sending anything then server will get stuck on recieving forever
//what goes inside accept()'s 2nd and 3rd parameter 
//why there's any need of using  getaddrinfo() for server cause server need to listen to all address not specfic