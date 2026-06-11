#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    if(argc != 3) {
        printf("Usage: %s <ipv4_addr> <port_no>\n", argv[0]);
        return 0;
    }
    int port = atoi(argv[2]);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1) {
        perror("Socket error: ");
        exit(-1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(port);
    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Connect error: ");
        exit(-1);
    }
    
    const char *msg = "Hello there!";
    int bytes_sent = send(sock_fd, msg, strlen(msg), 0);
    printf("Bytes sent = %d\n", bytes_sent);


    char buffer[256];
    int bytes_received = recv(sock_fd, buffer, 256, 0);
    buffer[bytes_received] = 0;
    printf("Bytes received = %d\n", bytes_received);
    printf("msg received = %s\n", buffer);
    return 0;
}