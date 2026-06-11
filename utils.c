#include "utils.h"
#include <stdio.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

char *load_file(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if(file == NULL) {
        perror("File opening error: ");
        exit(-1);
    }

    int length, bytes_read;
    char *buffer;

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    
    buffer = malloc(length + 1);
    if(buffer == NULL) {
        perror("malloc failed: ");
        exit(-1);
    }

    fseek(file, 0, SEEK_SET);
    bytes_read = fread(buffer, 1, length, file);
    if(bytes_read != length) {
        fprintf(stderr, "%d bytes read but file size is %d bytes\n", bytes_read, length);
        exit(-1);
    }
    buffer[length] = 0;
    return buffer;
}

char *addr_family_to_str(int af) {
    if(af == AF_INET) {
        return "INET4";
    } else if(af == AF_INET6) {
        return "INET6";
    } else if(af == AF_UNIX) {
        return "UNIX";
    } else {
        return "UNKNOWN";
    }
}
char *sock_addr_to_str(const struct sockaddr *addr) {
    if(addr == NULL)    return "NULL";
    char *str = calloc(1000, 1);
    strcat(str, "{ ");

    strcat(str, "addr_family: ");
    strcat(str, addr_family_to_str(addr->sa_family));
    
    if(addr->sa_family == AF_INET) {
        struct sockaddr_in *in_addr = (struct sockaddr_in *)addr;
        strcat(str, ", address: ");
        strcat(str, inet_ntoa(in_addr->sin_addr));
    }
    
    strcat(str, " }");

    return str;
}

static void print_ifaddrs(struct ifaddrs *if_addr) {
    printf(
        "if_addr: {\n" 
        "\tinterface name: %s,\n"
        "\taddress: %s,\n"
        "\tnetmask: %s,\n"
        "}", 
        if_addr->ifa_name, 
        sock_addr_to_str(if_addr->ifa_addr),
        sock_addr_to_str(if_addr->ifa_netmask)
    );
}

void print_network_interfaces(void) {
    struct ifaddrs *if_list;
    if(getifaddrs(&if_list) != 0) {
        perror("getifaddrs: ");
        exit(-1);
    }
    
    printf("Open network interfaces to this device are: ");
    for(struct ifaddrs *if_addr = if_list; if_addr != NULL; if_addr = if_addr->ifa_next) {
        print_ifaddrs(if_addr);
        printf("\n");
    }
    printf("==============================================\n\n");
}


struct sockaddr_in *get_internet_v4_interface() {
    struct ifaddrs *if_list;
    if(getifaddrs(&if_list) != 0) {
        perror("getifaddrs: ");
        exit(-1);
    }
    
    for(struct ifaddrs *if_addr = if_list; if_addr != NULL; if_addr = if_addr->ifa_next) {
        if(strcmp(if_addr->ifa_name, "wlp1s0") == 0 && if_addr->ifa_addr->sa_family == AF_INET) {
            return (struct sockaddr_in*)if_addr->ifa_addr;
        }
        
    }
    return NULL;
}