#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "func.h"
#include "header.h"

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
    path_init();
    while (1) {
        open_shell(prompt);
    }
}