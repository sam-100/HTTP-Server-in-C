
#ifndef UTILS_H
#define UTILS_H

#include <sys/socket.h>

char *load_file(const char *file_path);
char *addr_family_to_str(int af);
char *sock_addr_to_str(const struct sockaddr *addr);
void print_network_interfaces(void);
struct sockaddr_in *get_internet_v4_interface();

#endif