#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

#include "func.h"
#include "header.h"

#define TYPE_EXIT 0
#define TYPE_CD 1
#define TYPE_PATH 2
#define TYPE_SINGLE 3

static int get_command_type(command_t *cmd);
static int exec_parallel(struct group *group);
static int exec_single(struct group *group);
static int do_exit(struct group *group);
static int do_cd(command_t *cmd);
static int do_path(command_t *cmd);
static int do_fork(command_t *cmd);

int execute_command_group(struct group *group)
{
    if (group->used == 1) {
        return exec_single(group);
    } else {
        return exec_parallel(group);
    }
}

static int exec_parallel(struct group *group) {}

static int exec_single(struct group *group)
{
    command_t *cmd = group->cmds[0];
    int type = get_command_type(cmd);
    int res;
    switch (type) {
    case TYPE_EXIT:
        res = do_exit(group);
        break;
    case TYPE_CD:
        res = do_cd(cmd);
        break;
    case TYPE_PATH:
        res = do_path(cmd);
        break;
    default:
        res = do_fork(cmd);
        break;
    }
    return res;
}

static int get_command_type(command_t *cmd)
{
    if (strcmp(cmd->name, "exit") == 0)
        return TYPE_EXIT;
    if (strcmp(cmd->name, "cd") == 0)
        return TYPE_CD;
    if (strcmp(cmd->name, "path") == 0)
        return TYPE_PATH;
    return TYPE_SINGLE;
}

static int do_exit(struct group *group)
{
    if (group->cmds[0]->argc != 0) {
        fprintf(stderr, "Invalid Arguments for Exit\n");
        return -1;
    }
    free(group);
    exit(0);
}

static int do_cd(command_t *cmd)
{
    if (cmd->argc != 1) {
        fprintf(stderr, "Invalid arguments for cd\n");
        return -1;
    }
    if (chdir(cmd->argv[0]) != 0) {
        perror("chdir");
        return -1;
    }
    return 0;
}

static int do_path(command_t *cmd)
{
    for (uint32_t i = 0; i < cmd->argc; i++)
        path_set(cmd->argv[i]);
    return 0;
}


static int do_fork(command_t *cmd) {}
