#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>      //contains - struct addrinfo
#include<arpa/inet.h> // For inet_pton
#include<string.h> // For memset
#include<unistd.h> // For close

#define PORT 8080
#define SERVER_IP "127.0.0.1" //  server's(ur own system) loopback address


int main(){
    int sockfd = socket(AF_UNSPEC,SOCK_STREAM,0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // This struct holds the server's address information
    struct sockaddr_in server_addr;

    // Zero out the struct so it ensure all fields r initialised to 0
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert the IP address from string to binary format
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return 1;
    }

    printf("Client is ready to connect to %s:%d\n", SERVER_IP, PORT);

    //The next step would be to connect to the server:
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        perror("connect failed");
        return 1;
    }

    printf("Connected successfully!\n");

    close(sockfd);
    return 0;
}