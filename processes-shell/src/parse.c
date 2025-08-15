#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

#include "error.h"
#include "func.h"
#include "header.h"

#define REDIR_EMUL (-1)
#define REDIR_EINVAL (-2)
#define REDIR_OPEN (-3)

static struct group *split_command_with_and(struct group *group, char *str);
static fd_t split_with_redir(char *str);
static int add_command(struct group *group, char *str);
static command_t *parse_command(char *str);
static void set_command_argv(command_t *cmd, char *str, int *name_set);

struct group *get_command_group(char *str)
{
    struct group *group = (struct group *) malloc(sizeof(struct group));
    if (group == NULL) {
        perror("malloc");
        return NULL;
    }
    group->used = 0;
    return split_command_with_and(group, str);
}

static struct group *split_command_with_and(struct group *group, char *str)
{
    char *split_and;

    while (1) {
        split_and = memchr(str, '&', strlen(str));
        if (split_and == NULL) {
            break;
        }
        *split_and = 0;
        add_command(group, str);
        str = split_and + 1;
    }
    add_command(group, str);
    return group;
}

static int add_command(struct group *group, char *str)
{
    char *trim_str = trim(str);
    if (trim_str == NULL) {
        return -1;
    }

    fd_t fd_out = split_with_redir(trim_str);
    if (fd_out < 0) {
        return -1;
    }

    command_t *cmd = parse_command(trim_str);
    if (cmd == NULL) {
        return -1;
    }
    cmd->fd_out = fd_out;
    group->cmds[group->used] = cmd;
    group->used++;
    return 1;
}

static command_t *parse_command(char *str)
{
    str = trim(str);
    if (str == NULL) {
        return NULL;
    }

    command_t *cmd = (command_t *) malloc(sizeof(command_t));
    if (cmd == NULL) {
        perror("malloc");
        return NULL;
    }
    cmd->argc = 0;

    char *sep;
    int name_set = 0;
    size_t len = strlen(str);
    while (1) {
        sep = memchr(str, ' ', len);
        if (sep == NULL) {
            break;
        }
        *sep = 0;
        set_command_argv(cmd, str, &name_set);
        str = ltrim(sep + 1);
        if (str == NULL) {
            return cmd;
        }
    }
    set_command_argv(cmd, str, &name_set);
    return cmd;
}

static void set_command_argv(command_t *cmd, char *str, int *name_set)
{
    if (*name_set == 0) {
        *name_set = 1;
        strcpy(cmd->name, str);
    } else {
        strcpy(cmd->argv[cmd->argc], str);
        cmd->argc++;
    }
}

static fd_t split_with_redir(char *str)
{
    char *redir = memchr(str, '>', strlen(str));
    if (redir == NULL) {  // no redir in this command
        return FD_STDOUT;
    } else if (memchr(redir + 1, '>', strlen(redir + 1))) {
        print_error(ERR_REDIR_MUL);
        return REDIR_EMUL;
    }

    *redir = 0;
    str = trim(redir + 1);
    if (str == NULL || find_space(str)) {  // != 1 arguments
        print_error(ERR_REDIR_ARG);
        return REDIR_EINVAL;
    }

    fd_t fd_out = open(str, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out < 0) {
        perror("open");
        return REDIR_OPEN;
    }
    return fd_out;
}