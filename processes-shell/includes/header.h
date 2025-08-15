#ifndef _HEADER_H
#define _HEADER_H

// types
typedef unsigned int uint32_t;

// string
#define BUF_SIZE 1024   // input size
#define CMD_SIZE 16     // command name
#define NUM_OF_ARG 8    // number of arguments
#define ARG_SIZE 32     // size of argument
#define NUM_OF_CMD 8    // number of commands
#define PATH_SIZE 64    // size of each path
#define NUM_OF_PATH 16  // number of path

// file descripter
#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2
typedef int fd_t;

// command
typedef struct command {
    char name[CMD_SIZE];
    char argv[NUM_OF_ARG][ARG_SIZE];
    uint32_t argc;
    fd_t fd_out;
} command_t;

struct group {
    command_t *cmds[NUM_OF_CMD];
    uint32_t used;  // number of used command
};

// fork
typedef int pid_t;

#endif  // _HEADER_H