#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

#include "func.h"
#include "header.h"

int open_shell(int prompt)
{
    char *buf = NULL;
    size_t buf_size = 0;
    int str_len;

    while (1) {
        if (prompt)
            printf("wish> ");
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
    struct group *group = get_command_group(buf);
    if (group == NULL) {
        return -1;
    }
    /* execute command */
    int res = execute_command_group(group);
    free(group);
    return res;
}