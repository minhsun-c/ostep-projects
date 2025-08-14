#ifndef __FUNC_H
#define __FUNC_H

// shell
int open_shell(int);

// parse
group_t *get_command(char *);

// helper
char *trim(char *str);
char *find_space(char *str);

#endif  // __FUNC_H