#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "header.h"
#include "func.h"

int main(int argc, char *argv[])
{
    fd_t fd_in = FD_STDIN;
    int prompt = 1;
    if (argc > 2) {
        fprintf(stderr, "Shell with invalid arguments\n");
        exit(1);
    } else if (argc == 2) {
        fd_in = open(argv[1], O_RDONLY);
        prompt = 0;
        if (fd_in < 0) {
            perror("open");
            exit(1);
        }
        if (dup2(fd_in, FD_STDIN) < 0) {
            perror("dup2");
            close(fd_in);
            exit(1);
        }
    }

    while (1) {
        int res = open_shell(prompt);
        if (res == 0) {
            break;
        } else if (res < 0) {
            exit(1);
        }
    }
}