#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int x = 9;

    char *msg = malloc(256);
    sprintf(msg, "hello %d world!\n", x);
    printf("msg: %s", msg);
    return 0;
}