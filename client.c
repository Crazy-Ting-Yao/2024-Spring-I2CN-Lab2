#include "header.h"
/*
 * @description
 * Write your own function to create header. 
 *
 * @param
 * s: The test segment sent from server
*/
void myheadercreater(Segment* s){
    // calculate pseudoheader
    uint32_t sourceIP = inet_addr(s->l3info.SourceIpv4);
    uint32_t destIP = inet_addr(s->l3info.DestIpv4);
    uint16_t protocol = s->l3info.protocol;
    uint16_t HeaderLength = s->l4info.HeaderLen << 2;
    char pseudoheader[12];
    for(int i=0;i<4;i++){
        pseudoheader[i] = (sourceIP >> (i*8)) & 0xFF;
        pseudoheader[i+4] = (destIP >> (i*8)) & 0xFF;
    }
    pseudoheader[8] = (protocol >> 8) & 0xFF;
    pseudoheader[9] = protocol & 0xFF;
    pseudoheader[10] = (HeaderLength >> 8) & 0xFF;
    pseudoheader[11] = HeaderLength & 0xFF;
    // calculate header
    uint32_t AckNum = s->l4info.AckNum;
    uint32_t SeqNum = s->l4info.SeqNum;
    uint32_t SourcePort = s->l4info.SourcePort;
    uint32_t DestPort = s->l4info.DestPort;
    uint32_t Flag = s->l4info.Flag;
    uint32_t WindowSize = s->l4info.WindowSize;
    uint32_t Checksum = 0;
    char header[20];
    for(int i=0;i<2;i++){
        header[1-i] = (SourcePort >> (i*8)) & 0xFF;
        header[3-i] = (DestPort >> (i*8)) & 0xFF;
        header[15-i] = (WindowSize >> (i*8)) & 0xFF;
    }
    for(int i=0;i<4;i++){
        header[7-i] = (SeqNum >> (i*8)) & 0xFF;
        header[11-i] = (AckNum >> (i*8)) & 0xFF;
    }
    header[12] = 0b01010000;
    header[13] = 0b00010000;
    uint32_t sum = 0;
    for(int i=0;i<8;i++){
        sum += (header[2*i] << 8) & 0xFF00;
        sum += header[2*i+1] & 0xFF;
    }
    for(int i=0;i<6;i++){
        sum += (pseudoheader[2*i] << 8) & 0xFF00;
        sum += pseudoheader[2*i+1] & 0xFF;
    }
    while(sum >> 16){
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    Checksum = ~sum;
    for(int i=0;i<2;i++){
        header[17-i] = (Checksum >> (i*8)) & 0xFF;
    }
    memcpy(s->header, header, 20);
    memcpy(s->pseudoheader, pseudoheader, 12);
    
}


/*
 * @description
 * In this function, You need to implement the client side
 * that sends messages to and from the server.
 *
 *        Receive the message: "Enter "test" to start or "quit" to leave!"
 * Server ----------------------------------------> Client
 *        client type: "test" or "quit" or {wrong input}
 * Server <---------------------------------------- Client
 *        test: 1. Receive "Receive Data from server..." from server
 *              2. receive test segment from server.
 *        quit: Close the socket.
 *        {wrong input}: do nothing
 * Server ----------------------------------------> Client
 *
 * @param
 * sockfd: The socket descriptor used for sending/receiving data  
 *           with a client that has connected to server.
 * s     : The test segment sent from server
*/
void receiveData(int sockfd, Segment* s){
    char message[1024] = {0};
    recv(sockfd, message, sizeof(message), 0);
    printf("server: %s\n",message);
    char buffer[1024] = {0};
    scanf("%s", buffer);
    send(sockfd, buffer, sizeof(buffer), 0);
    while(strcmp(buffer, "quit") && strcmp(buffer, "test")){
        char message[1024];
        recv(sockfd, message, sizeof(message), 0);
        printf("server: %s\n",message);
        scanf("%s", buffer);
        send(sockfd, buffer, sizeof(buffer), 0);
    }
    if(strcmp(buffer, "test") == 0){
        char message[1024];
        printf("Receive Data from server...\n");
        recv(sockfd, s, sizeof(Segment), 0);
        printSegment(s);
        myheadercreater(s);
        printHeader(s->header);
        close(sockfd);
    }
    else if(strcmp(buffer, "quit") == 0){
        printf("close Socket\n");
        close(sockfd);
    }
}


/*
 * @description
 * Task1
 * 1. create a socket and connect to server.
 *    (server's IP address = "127.0.0.1")
 *    (server's Port number = 45525)
 * 2. Receive message: "Hi, I'm server {Your_student_ID}..." from server and print it out.
 *    (The message you sent from server)
 * 
 * Task2
 * 1. Instantiate a Segment
 * 2. Pass your socket_fd and the instance of Segment into the receiveData()
 * 3. Write your own function to create header.
 * 4. Print the header.
 *    Example: 
 *        Segment s;
 *        receivedata(sock_fd,&s); 
 *        myheadercreater(&s); 
 *        printHeader(s.header); 
*/  
int main(int argc , char *argv[])
{   
    //Create TCP socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //Set up server's address.
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(45525);
    //Connect to server's socket.
    connect(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    //Receive message from server and print it out.
    char message[1024];
    recv(socket_fd, message, sizeof(message), 0);
    printf("%s\n",message);
    Segment s;
    receiveData(socket_fd, &s);
    close(socket_fd);
    return 0;
}