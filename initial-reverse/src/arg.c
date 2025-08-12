#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "header.h"

#define ERR_INV_ARGS    "usage: reverse <input> <output>\n"
#define ERR_OPEN_FILE   "reverse: cannot open file '%s'\n"
#define ERR_SAME_FILE   "reverse: input and output file must differ\n"

static int is_same_file(const char *in_file, const char *out_file)
{
    if (strcmp(in_file, out_file) == 0) 
        return 1;

    struct stat in_stat, out_stat;
    if (stat(in_file, &in_stat) < 0 || stat(out_file, &out_stat) < 0) {
        perror("stat");
        exit(1);
    }

    return ((in_stat.st_dev == out_stat.st_dev) && (in_stat.st_ino == out_stat.st_ino));
}

int check_args(int argc, char *argv[], fd_t *fds) 
{
    fds[0] = 0;
    fds[1] = 1;
    
    if (argc > 3) {
        fprintf(stderr, ERR_INV_ARGS);
        exit(1);
    }

    if (argc >= 2) {
        fds[0] = open(argv[1], O_RDONLY);
        if (fds[0] < 0) {
            fprintf(stderr, ERR_OPEN_FILE, argv[1]);
            exit(1);
        }
    }

    if (argc == 3) {
        fds[1] = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fds[1] < 0) {
            fprintf(stderr, ERR_OPEN_FILE, argv[2]);
            exit(1);
        }
        if (is_same_file(argv[1], argv[2])) {
            fprintf(stderr, ERR_SAME_FILE);
            exit(1);
        }
    }

    return 1;
}