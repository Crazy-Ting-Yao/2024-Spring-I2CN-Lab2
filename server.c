#include "header.h"
/*
 * @description
 * In this function, You need to implement the server side
 * that sends messages to and from the client.
 *
 *        Enter ""test" to start or "quit" to leave!"
 * Server ----------------------------------------> Client
 *        Receive "test" or "quit" or {wrong input} from client
 * Server <---------------------------------------- Client
 *        test: Read text file from input and send test segment.
 *        quit: End loop.
 *        {wrong input}: Server send "Wrong input! Enter "test" to start or "quit" to leave!"
 * Server ----------------------------------------> Client
 *
 * @param
 * clientfd: The socket descriptor used for sending/receiving data  
 *           with a client that has connected to server.
 * fileName: The file name read from sample input. e.g. "sample_input1.txt"
*/
void serverFunction(int clientfd, char* fileName){
    char message[] = "Enter \"test\" to start or \"quit\" to leave!";
    send(clientfd, message, sizeof(message), 0);
    char buffer[1024] = {0};
    read(clientfd, buffer, 1024);
    char wrongInput[] = "Wrong input! Enter \"test\" to start or \"quit\" to leave!";
    while(strcmp(buffer, "test") && strcmp(buffer, "quit")){
        send(clientfd, wrongInput, sizeof(wrongInput), 0);
        read(clientfd, buffer, 1024);
    }
    if(strcmp(buffer, "test") == 0){
        // Read text file from input and send test segment.
        Segment s;
        readFile(&s, fileName);
        send(clientfd, &s, sizeof(Segment), 0);
        close(clientfd);
    }
    else if(strcmp(buffer, "quit") == 0){
        char message[] = "close Socket";
        send(clientfd, message, sizeof(message), 0);
        close(clientfd);
    }
}

/*
 * @description
 * Task1
 * 1. Create a TCP socket bind to port 45525.   
 * 2. Listen the TCP socket.
 * 3. Accept the connect and get the Client socket file descriptor.
 * 4. Send 1 message "Hi, I'm server {Your_student_ID}" to client.
 * 5. Go finish the client.c TASK 1 
 * 
 * Task2
 * 1. Pass the client fd and fileName into serverFunction()
 *    Example:
 *        serverFunction(client_fd, argv[1])
*/
int main(int argc , char *argv[]){
    //Create TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //Set up server's address.
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(45525);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
    //Bind socket to the address. 
    bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    //Listening the socket.
    listen(sockfd, 10);
    //Accept the connect request.
    int new_socket = accept(sockfd, NULL, NULL);
    printf("New connection\n");
    //Send message to client.
    char message[] = "Hi, I'm server 111062109";
    send(new_socket, message, sizeof(message), 0);
    sleep(1);
    //Pass the client fd and fileName into serverFunction()
    serverFunction(new_socket, argv[1]);
}