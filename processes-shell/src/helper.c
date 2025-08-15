#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "func.h"
#include "header.h"


char *trim(char *str)
{
    if (*str == 0)
        return NULL;
    while (*str && isspace(*str)) {
        str++;
    }
    size_t len = strlen(str);
    while (len != 0 && isspace(str[len - 1])) {
        str[len - 1] = 0;
        len--;
    }
    if (len == 0)
        return NULL;
    return str;
}

char *ltrim(char *str)
{
    while (*str && isspace(*str)) {
        str++;
    }
    if (*str == 0)
        return NULL;
    return str;
}

char *find_space(char *str)
{
    while (str && *str) {
        if (isspace(*str))
            return str;
        str++;
    }
    return NULL;
}