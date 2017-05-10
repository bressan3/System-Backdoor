
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <signal.h>

// Message Size
#define BUFFER_SIZE 1024
#define ADDRESS "127.0.0.1"
#define PORT 8888

int main(int argc , char *argv[])
{
    int serverSocket, clientSocket, pid;
    struct sockaddr_in server, client;
    
    // Creates socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (serverSocket == -1) {
        printf("Error. Could not create socket.\n");
        exit(-1);
    }
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ADDRESS);
    server.sin_port = htons(PORT);
    
    bind(serverSocket, (struct sockaddr *)&server, sizeof(server));
    
    // Listen for new connections
    listen(serverSocket , 1);
    printf("Listening on Port %d...Waiting for incoming connections...\n", PORT);
    
    FILE *stream = fdopen(dup(STDERR_FILENO), "w");
    setbuf(stream, NULL);
    
    char clientMessage[BUFFER_SIZE];
    memset(clientMessage, 0, sizeof(clientMessage));
    
    socklen_t clientSize = sizeof(client);
    
    while ((clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientSize))) {
        printf("Client connected.\n");
        if ((pid = fork())) printf("Child PID = %d\n", pid);
        else {
            pid = getpid();
            dup2(clientSocket, STDOUT_FILENO);
            while (true){
                ssize_t readc = 0;
                int filled = 0;
                while (true) {
                    readc = recv(clientSocket, clientMessage+filled, BUFFER_SIZE-filled-1, 0);
                    if (!readc) break;
                    filled += readc;
                    // Finished receiving message
                    if (clientMessage[filled - 1] == '\0') break;
                }
                if (!readc)
                    break;
                system(clientMessage);
                send(clientSocket, "> ", 3, (intptr_t)signal(SIGPIPE, SIG_IGN));
            }
            close(clientSocket);
            exit(0);
        }
    }
    
    fclose(stream);
    close(serverSocket);
    
    return 0;
}

