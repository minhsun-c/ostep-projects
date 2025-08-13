#include "../kernel/pstat.h"
#include "../kernel/types.h"
#include "user.h"

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
    int pid = getpid();
    int defaulttickets = 0;
    check(getpinfo(&st) == 0, "getpinfo");

    printf("\n **** PInfo **** \n");
    int i;
    for (i = 0; i < NPROC; i++) {
        if (st.inuse[i]) {
            if (st.pid[i] == pid) {
                defaulttickets = st.tickets[i];
                printf("pid: %d tickets: %d ticks: %d\n", st.pid[i],
                       st.tickets[i], st.ticks[i]);
            }
        }
    }

    check(defaulttickets == 1,
          "The default number of tickets for each process should be 1");
    printf("Should print 1 then 2\n");
    exit(0);
}