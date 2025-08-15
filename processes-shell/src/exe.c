#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error.h"
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
static int do_fork(struct group *group);
static int do_fork_child(command_t *cmd);
static int dup2_redir(command_t *cmd);
static int find_full_path(command_t *cmd, char *path);

int execute_command_group(struct group *group)
{
    if (group->used == 1) {
        return exec_single(group);
    } else {
        return exec_parallel(group);
    }
}

static int exec_parallel(struct group *group)
{
    return do_fork(group);
}

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
        res = do_fork(group);
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
        print_error(ERR_EXIT);
        return -1;
    }
    free(group);
    exit(0);
}

static int do_cd(command_t *cmd)
{
    if (cmd->argc != 1) {
        print_error(ERR_CD);
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
    path_clear();
    for (uint32_t i = 0; i < cmd->argc; i++)
        path_set(cmd->argv[i]);
    return 0;
}


static int do_fork(struct group *group)
{
    uint32_t success = 0;
    for (uint32_t i = 0; i < group->used; i++) {
        pid_t pid = fork();
        switch (pid) {
        case -1:
            perror("fork");
            break;
        case 0:
            do_fork_child(group->cmds[i]);
            return -1;
        default:
            success++;
            break;
        }
    }
    for (uint32_t i = 0; i < success; i++) {
        wait(NULL);
    }
    return 0;
}

static int do_fork_child(command_t *cmd)
{
    if (dup2_redir(cmd) < 0)
        goto EXEC_FAIL;

    char full_path[CMD_SIZE + PATH_SIZE];
    if (find_full_path(cmd, full_path) < 0)
        goto EXEC_FAIL;

    char *args[NUM_OF_ARG + 2];
    args[0] = cmd->name;
    for (uint32_t i = 0; i < cmd->argc; i++) {
        args[i + 1] = cmd->argv[i];
    }
    args[cmd->argc + 1] = NULL;

    execv(full_path, args);

EXEC_FAIL:
    print_error(ERR_EXEC);
    close(cmd->fd_out);
    exit(1);
}

static int dup2_redir(command_t *cmd)
{
    if (dup2(cmd->fd_out, FD_STDOUT) < 0) {
        perror("dup2");
        return -1;
    }
    return 0;
}

static int find_full_path(command_t *cmd, char *path)
{
    uint32_t psize = path_size();
    char full_path[PATH_SIZE + CMD_SIZE];
    for (uint32_t i = 0; i < psize; i++) {
        char *pname = path_index(i);
        snprintf(path, PATH_SIZE + CMD_SIZE, "%s/%s", pname, cmd->name);
        if (access(path, X_OK) == 0) {
            return i;
        }
    }
    return -1;
}