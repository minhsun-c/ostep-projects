#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

#include "func.h"
#include "header.h"

#define REDIR_EMUL (-1)
#define REDIR_EINVAL (-2)
#define REDIR_OPEN (-3)

static group_t *split_command_with_and(group_t *group, char *str);
static fd_t split_with_redir(char *str);
static int add_command(group_t *group, char *str);
static command_t *parse_command(char *str);
static void set_command_argv(command_t *cmd, char *str, int *name_set);
static void print_command(command_t *cmd);

group_t *get_command_group(char *str)
{
    group_t *group = (group_t *) malloc(sizeof(group_t));
    if (group == NULL) {
        perror("malloc");
        return NULL;
    }
    group->used = 0;
    return split_command_with_and(group, str);
}

static group_t *split_command_with_and(group_t *group, char *str)
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

static int add_command(group_t *group, char *str)
{
    char *trim_str = trim(str);
    if (trim_str == NULL) {
        printf("Empty Command\n");
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
    printf("Get Command: [%s]\n", str);
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
    print_command(cmd);
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

void print_command(command_t *cmd)
{
    printf("==== CMD INFO ====\n");
    printf("NAME: %s\n", cmd->name);
    printf("ARGC: %d\n", cmd->argc);
    for (uint32_t i = 0; i < cmd->argc; i++)
        printf("ARG[%d]: %s\n", i, cmd->argv[i]);
    printf("===================\n");
}