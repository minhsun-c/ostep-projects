#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>

#include "error.h"
#include "func.h"
#include "header.h"

static int is_path_valid(char *);

char path[NUM_OF_PATH][PATH_SIZE];
uint32_t path_used;

int path_init()
{
    strcpy(path[0], "/usr/bin");
    strcpy(path[1], "/bin");
    path_used = 2;
    return 0;
}

int path_clear()
{
    path_used = 0;
    return 0;
}

int path_set(char *str)
{
    if (is_path_valid(str) < 0) {
        print_error(ERR_PATH);
        return -1;
    }
    if (path_used >= NUM_OF_PATH)
        return -1;
    strcpy(path[path_used], str);
    path_used++;
    return 0;
}

int path_find(char *str)
{
    for (uint32_t i = 0; i < path_used; i++) {
        if (strcmp(path[i], str) == 0)
            return i;
    }
    return -1;
}

uint32_t path_size()
{
    return path_used;
}

char *path_index(uint32_t idx)
{
    if (idx >= path_used)
        return NULL;
    return path[idx];
}

static int is_path_valid(char *str)
{
    struct stat status;
    if (stat(str, &status) < 0) {
        return -1;
    } else if (!S_ISDIR(status.st_mode)) {
        return -1;
    }
    return 0;
}
