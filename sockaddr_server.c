#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h> // For inet_pton
#include<string.h> // For memset
#include<unistd.h> // For close

#define PORT 8080
#define BACKLOG 5 // How many pending connections queue will hold


int main(){
    int server_fd, new_socket;
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // This struct holds the server's own address information
    struct sockaddr_in server_ownaddr;    //cause thats from where server will be listening to all req

    // Zero out the struct so it ensure all fields r initialised to 0
    memset(&server_ownaddr, 0, sizeof(server_ownaddr));

    server_ownaddr.sin_family = AF_INET;
    server_ownaddr.sin_port = htons(PORT);
    server_ownaddr.sin_addr.s_addr = htonl(INADDR_ANY); //binds to all available address

    // Binding the socket to the address and port
    if(bind(server_fd,(struct sockaddr *)&server_ownaddr,sizeof(server_ownaddr))<0) {
        printf("bind failed");
        close(server_fd);
        return 1;
    }else {
        printf("bind successfull\n");
    }

    //Listen for incoming connections
    if(listen(server_fd,BACKLOG)<0) {
        printf("listen connection failed");
        close(server_fd);
        return 1;
    }

    printf("server is listening to port : %d...\n",PORT);

    //while(1);

    // Accept an incoming connection
    new_socket = accept(server_fd, NULL, NULL); // We can ignore client address for now
    printf("Connection accepted!\n");

    close(new_socket);
    close(server_fd); 
    return 0;
}