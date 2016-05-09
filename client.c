
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int main(int argc , char *argv[])
{
    if (argc < 3) {
        printf("Usage: Client SERVER_ADDRESS PORT\n");
        exit(-2);
    }
    
    struct sockaddr_in server;
    
    const char *address = argv[1];
    const uint16_t port = (uint16_t)atoi(argv[2]);
    
    printf("\n\nConnected\n");
    
    //Create socket
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    
    if (sock == -1) {
        printf("Could not create socket.\n");
        exit(-1);
    }
    
    printf("Socket created.\n");
    
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    
    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection Failed.\n");
        exit(-2);
    }
    
    printf("Connected\n");
    
    // Keep communicating with server
    
    int pid;
    
    // Sends the command to the server
    if ((pid = fork())) {
        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(message));
        printf("> ");
        while (fgets(message, BUFFER_SIZE, stdin) == message) {
            if (strncmp(message, "logout", strlen(message)-1) == 0 || strncmp(message, "exit", strlen(message)-1) == 0)
                break;
                
            // Sends data through socket
            send(sock, message, strlen(message) + 1, 0);
        }
        kill(pid, SIGKILL);
    } else {
        char reply[BUFFER_SIZE];
        memset(reply, 0, sizeof(reply));
        ssize_t filled = 0;
        while ((filled = recv(sock, reply, BUFFER_SIZE, 0))) {
            reply[filled] = '\0';
            printf("%s", reply);
            fflush(stdout);
        }
    }
    
    close(sock);
    
    return 0;
    
}


