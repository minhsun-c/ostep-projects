#ifndef __ERROR_H
#define __ERROR_H

#include <string.h>
#include <unistd.h>

#ifdef OSTEP_TESTING
#define DEFAULT_MSG "An error has occurred\n"

// Map every error to the single required line
#define ERR_SHELL_ARG DEFAULT_MSG
#define ERR_REDIR_ARG DEFAULT_MSG
#define ERR_REDIR_MUL DEFAULT_MSG
#define ERR_CMD DEFAULT_MSG
#define ERR_EXIT DEFAULT_MSG
#define ERR_CD DEFAULT_MSG
#define ERR_PATH DEFAULT_MSG
#define ERR_EXEC DEFAULT_MSG

static inline void print_error(const char *msg)
{
    (void) msg;
    write(STDERR_FILENO, DEFAULT_MSG, sizeof(DEFAULT_MSG) - 1);
}
#else
// Descriptive messages for non-testing builds
#define ERR_SHELL_ARG "bad invocation: wish expects 0 or 1 argument\n"
#define ERR_REDIR_ARG "redirection error: exactly one output file required\n"
#define ERR_REDIR_MUL "redirection error: multiple '>' not allowed\n"
#define ERR_CMD "command not found or not executable\n"
#define ERR_EXIT "exit takes no arguments\n"
#define ERR_CD "cd takes exactly one argument\n"
#define ERR_PATH "invalid path entry (not a searchable directory)\n"
#define ERR_EXEC "failed to execute program\n"

static inline void print_error(const char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
}
#endif

#endif // __ERROR_H