#include "../kernel/pstat.h"
#include "../kernel/types.h"
#include "user.h"
#define NULL ((void *) 0)

#define check(exp, msg)                                                   \
    if (exp) {                                                            \
    } else {                                                              \
        printf("%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, \
               msg);                                                      \
        exit(0);                                                          \
    }

int main(int argc, char *argv[])
{
    struct pstat st;

    check(getpinfo(&st) == 0, "getpinfo");
    check(getpinfo(NULL) == -1, "getpinfo with bad pointer");
    check(getpinfo((struct pstat *) 1000000) == -1,
          "getpinfo with bad pointer");
    printf("Should print 1 then 2\n");
    exit(0);
}