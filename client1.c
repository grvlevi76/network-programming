//this program is for testing purpose

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>      //contains - struct addrinfo
#include<arpa/inet.h> // For inet_pton
#include<string.h> // For memset
#include<unistd.h> // For close
#include<signal.h>
#include<sys/prctl.h>   //for prctl() - kills child if parent process dies

#define PORT "8080"
#define SERVER_IP "127.0.0.1" //  server's(ur own system) loopback address
#define buff_size 100

int main(){

    struct addrinfo hints,*servinfo;
    char buff[buff_size];
    int copyied;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     //both allowed ipv 4 and 6
    hints.ai_socktype = SOCK_STREAM;

    //to store server address information
    if(getaddrinfo(SERVER_IP,PORT,&hints,&servinfo)!=0) {       //try localhost in 1st parameter
        printf("getaddrinfo error\n");
        return 1;
    } 

    //printf("ip style : %d\n",servinfo->ai_family);
    //printf("lets see : %d\n",servinfo->ai_next->ai_family);

    //socket creation
    int sockfd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
    if (sockfd < 0) {
        perror("socket error :");
        return 1;
    }

    printf("Client is ready to connect to %s:%s\n", SERVER_IP, PORT);

    //connect to server address
    if(connect(sockfd,servinfo->ai_addr,servinfo->ai_addrlen)!=0) {
        close(sockfd);
        perror("connecterror :");
        return 1;
    }
    printf("Connected successfully to server!\n");

    //for infinite recieving-->
    if(fork()==0) {                                     //creates child process, returns 1(if failed) or pid(if worked) in case of parent process , for child it returns 0 if sucessfull
        prctl(PR_SET_PDEATHSIG,SIGTERM);         // kills child if parent process dies

        //recieving from server
        while(1) {
            if((copyied = recv(sockfd,buff,buff_size-1,0))<0) {
                perror("recv error ");
                return 1;
            }
            if(copyied==0) {
                printf("\n!!!the server has closed the connection!!!\n");
                printf("\npls enter 'q' to exit the program = ");
                return 0;
            }

            buff[copyied] = '\0';
            printf("\nresponse from server : '%s'\n",buff);
            printf("\n//child enter 'q' to exit from program \nenter the message : ");    // this fulfills the terminal gui requirement
            fflush(stdout);
        }

    }

    while(1) {
        //sending to server
        printf("\n// enter 'q' to exit from program\nenter the message : ");
        fgets(buff,buff_size,stdin);
        buff[strcspn(buff, "\n")] = 0;  //removes the new line character which fgets() adds automativally at the end of string
        if(buff[0] == 'q'){
            freeaddrinfo(servinfo);
            close(sockfd);
            return 0;
        }else {

            if(send(sockfd,buff,strlen(buff),0)<0) {
                perror("send error : ");
                return 1;
            }else {
                printf("message sent successfully to server!\n");
            }
        }
        
    }
}

//(done)try local host in getaddrinfo()'s 1st parameter to see if it would go look for localhost address which is "127.0.0.1"
//(done)whcih ip type gets selected when AF_UNSPEC gets use , how it chooses ? 
//(done)when a already declared variable of struct addrinfo like inai (in our program) then passing through getaddrinfo() does it fills the inai variable with new things or creates new linked list which has nothing to do with inai and assigns list to pointer variable whichever is in its 4th parameter