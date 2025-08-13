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
    int pid_par = getpid();
    int tickets = 5;
    check(settickets(tickets) == 0, "settickets");

    if (fork() == 0) {
        int pid_chd = getpid();
        struct pstat st;

        check(getpinfo(&st) == 0, "getpinfo");
        int tickets_par = -1, tickets_chd = -1;
        int i;
        for (i = 0; i < NPROC; i++) {
            if (st.pid[i] == pid_par) {
                tickets_par = st.tickets[i];
            } else if (st.pid[i] == pid_chd) {
                tickets_chd = st.tickets[i];
            }
        }
        check(tickets_par == tickets,
              "Tickets number in pinfo should be correct");
        printf("parent: %d, child: %d\n", tickets_par, tickets_chd);
        check(tickets_chd == tickets,
              "Expected the number of tickets in the child process to equal "
              "the number of tickets in the parent process");
        printf("Should print 1 then 2\n");
        exit(0);
    }
    while (wait(NULL) > 0)
        ;
    exit(0);
}