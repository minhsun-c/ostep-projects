#ifndef __ARGS_H
#define __ARGS_H

#define BUF_SIZE 1024

typedef int fd_t;

typedef struct {
    void *data;
    int capacity;
    int used;
} mem_t;

int check_args(int argc, char *argv[], fd_t *fds) ;
void reverse_file(fd_t *fd);

#endif // __ARGS_H