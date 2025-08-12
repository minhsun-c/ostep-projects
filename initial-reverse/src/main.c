#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "header.h"


int main(int argc, char *argv[])
{
    fd_t fds[2];
    check_args(argc, argv, fds);
    reverse_file(fds);

    if (fds[0] != 0) close(fds[0]);
    if (fds[1] != 1) close(fds[1]);
}
