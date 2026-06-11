#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include "utils.h"

void getRequest(int fd) {
    char buffer[256];
    memset(buffer, 0, 256);
    int bytes_received = recv(fd, buffer, 256, 0);
    printf("%d bytes received\n", bytes_received);
    printf("Request received: \n%s\n", buffer);
}

void sendResponse(int fd) {
    int bytes_sent;
    char *msg, *body;
    body = load_file("index.html");

    msg = "HTTP/1.1 200 OK\r\n";
    bytes_sent = send(fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);
    
    msg = malloc(256);
    sprintf(msg, "Content-Length: %d\r\n", (int)strlen(body));
    bytes_sent = send(fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);

    msg = "Content-Type: text/html\r\n";
    bytes_sent = send(fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);
    
    msg = "Connection: close\r\n";
    bytes_sent = send(fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);
    msg = "\r\n";
    bytes_sent = send(fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);

    bytes_sent = send(fd, body, strlen(body), 0);
    printf("%d bytes sent.\n", bytes_sent);
}

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: %s <port_no>\n", argv[0]);
        return 0;
    }

    int port = atoi(argv[1]);

    print_network_interfaces();
    
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1) {
        perror("Socket error: ");
        exit(-1);
    }


    struct sockaddr_in *addr = get_internet_v4_interface();
    if(addr == NULL) {
        fprintf(stderr, "Failed to find a wireless inet4 interface\n");
        exit(-1);
    }
    addr->sin_port = htons(port);
    printf("Found inet interface: %s\n", sock_addr_to_str((struct sockaddr*)addr));

    if(bind(sock_fd, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Bind error: ");
        exit(-1);
    }


    int backlog = 10;
    if(listen(sock_fd, backlog) != 0) {
        perror("Listen error: ");
        exit(-1);
    }

    struct sockaddr_in client_addr;
    int client_addr_length, new_fd;
    while(1) {
        printf("Listening on %s:%d\n", inet_ntoa(addr->sin_addr), port);
        new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_length);
        client_addr_length = sizeof(struct sockaddr_in);
        if(new_fd == -1) {
            perror("accept() error: ");
            exit(-1);
        }

        // getting http request
        getRequest(new_fd);
        
        // sending response
        sendResponse(new_fd);
    }
    

    // cleanup and release resources
    close(sock_fd);
    close(new_fd);
    return 0;
}
