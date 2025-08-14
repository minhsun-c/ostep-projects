#ifndef __FUNC_H
#define __FUNC_H

struct group;

// shell
int open_shell(int);

// parse
struct group *get_command_group(char *);

// exec
int execute_command_group(struct group *);

// helper
char *trim(char *str);
char *ltrim(char *str);
char *find_space(char *str);

#endif  // __FUNC_H