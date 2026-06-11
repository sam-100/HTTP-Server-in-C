#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include "utils.h"

int port = 8080;

int main(int argc, char **argv) {
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
    int client_addr_length = sizeof(struct sockaddr_in);
    printf("Listening on %s:%d\n", inet_ntoa(addr->sin_addr), port);
    int new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_length);
    if(new_fd == -1) {
        perror("accept() error: ");
        exit(-1);
    }
    printf("Client connection established on fd %d\n", new_fd);


    char buffer[256];
    memset(buffer, 0, 256);

    // getting http request
    int bytes_received = recv(new_fd, buffer, 256, 0);
    printf("%d bytes received\n", bytes_received);
    printf("Request received: \n%s\n", buffer);

    // sending response
    int bytes_sent;
    char *msg, *body;
    body = load_file("index.html");

    msg = "HTTP/1.1 403 Forbidden\r\n";
    bytes_sent = send(new_fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);
    
    msg = malloc(256);
    sprintf(msg, "Content-Length: %d\r\n", (int)strlen(body));
    bytes_sent = send(new_fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);

    msg = "\r\n";
    bytes_sent = send(new_fd, msg, strlen(msg), 0);
    printf("%d bytes sent.\n", bytes_sent);

    bytes_sent = send(new_fd, body, strlen(body), 0);
    printf("%d bytes sent.\n", bytes_sent);

    

    // cleanup and release resources
    close(sock_fd);
    close(new_fd);
    return 0;
}
