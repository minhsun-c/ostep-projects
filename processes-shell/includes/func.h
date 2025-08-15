#ifndef __FUNC_H
#define __FUNC_H

struct group;

// shell
int open_shell(int);

// parse
struct group *get_command_group(char *);

// exec
int execute_command_group(struct group *);

// path
int path_init(void);
int path_set(char *);
int path_clear();
int path_find(char *);
unsigned int path_size(void);
char *path_index(unsigned int);


// helper
char *trim(char *str);
char *ltrim(char *str);
char *find_space(char *str);

#endif  // __FUNC_H