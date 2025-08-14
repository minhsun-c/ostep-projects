#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

#include "func.h"
#include "header.h"

static command_t *parse_command(char *str);
static group_t *split_command_with_and(char *str);
static int add_command(group_t *group, char *str);
static fd_t split_with_redir(char *str);

int open_shell(int prompt)
{
    char *buf = NULL;
    size_t buf_size = 0;
    int str_len;

    while (1) {
        if (prompt)
            printf("wish>");
        str_len = getline(&buf, &buf_size, stdin);
        if (str_len < 0) {
            if (errno == ENOMEM) {
                perror("getline");
                return -ENOMEM;
            } else if (errno == EINVAL) {
                perror("getline");
                return -EINVAL;
            } else {  // EOF
                free(buf);
                exit(0);
            }
        } else if (str_len <= 1) {  // only newline
            continue;
        } else {
            break;
        }
    }
    buf[str_len - 1] = 0;
    group_t *group = split_command_with_and(buf);
    if (group == NULL) {
        return -1;
    }

    return 1;
}

static group_t *split_command_with_and(char *str)
{
    group_t *group = (group_t *) malloc(sizeof(group_t));
    if (group == NULL) {
        perror("malloc");
        return NULL;
    }
    group->used = 0;
    group->parallel = 0;

    char *split_and;

    while (1) {
        split_and = memchr(str, '&', strlen(str));
        if (split_and == NULL) {
            break;
        }
        group->parallel++;
        *split_and = 0;
        add_command(group, str);
        str = split_and + 1;
    }
    add_command(group, str);
    return group;
}

static int add_command(group_t *group, char *str)
{
    char *trim_str = trim(str);
    if (trim_str == NULL) {
        printf("Empty Command\n");
        goto INVAL_COMMAND;
    }

    fd_t fd_out = split_with_redir(trim_str);
    if (fd_out < 0) {
        goto INVAL_COMMAND;
    }

    command_t *cmd = parse_command(trim_str);
    if (cmd == NULL) {
        goto INVAL_COMMAND;
    }
    cmd->fd_out = fd_out;
    group->cmds[group->used] = cmd;
    group->used++;
    return 1;

INVAL_COMMAND:
    group->parallel--;
    return -1;
}

static command_t *parse_command(char *str)
{
    printf("Get Command: [%s]\n", str);
    return NULL;
}

static fd_t split_with_redir(char *str)
{
    char *redir = memchr(str, '>', strlen(str));
    if (redir == NULL) {  // no redir in this command
        return FD_STDOUT;
    } else if (memchr(redir + 1, '>', strlen(redir + 1))) {
        printf("Multiple Redirection\n");
        return REDIR_EMUL;
    }

    *redir = 0;
    str = trim(redir + 1);
    if (str == NULL || find_space(str)) {  // != 1 arguments
        printf("Invalid Arguments for Redirection\n");
        return REDIR_EINVAL;
    }

    fd_t fd_out = open(str, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out < 0) {
        perror("open");
        return REDIR_OPEN;
    }
    return fd_out;
}