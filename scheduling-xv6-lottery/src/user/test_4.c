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

void spin()
{
    int i = 0;
    int j = 0;
    int k = 0;
    for (i = 0; i < 50; ++i) {
        for (j = 0; j < 400000; ++j) {
            k = j % 10;
            k = k + 1;
        }
    }
}

int main(int argc, char *argv[])
{
    int numtickets[] = {20, 10, 30};
    int pids[3];

    pids[0] = getpid();
    settickets(numtickets[0]);

    int i;
    for (i = 1; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            settickets(numtickets[i]);
            for (;;)
                spin();
        }
    }

    struct pstat st;
    int time = 0;
    int ticks[3] = {0, 0, 0};

    printf("pid:%d, pid:%d, pid:%d\n", pids[0], pids[1], pids[2]);
    printf("tickets:%d, tickets:%d, tickets:%d\n", numtickets[0], numtickets[1],
           numtickets[2]);

    while (time < 50) {
        if (getpinfo(&st) != 0) {
            printf("check failed: getpinfo\n");
            goto Cleanup;
        }

        int j;
        int pid;
        for (i = 0; i < 3; i++) {
            pid = pids[i];
            for (j = 0; j < NPROC; j++) {
                if (st.pid[j] == pid) {
                    ticks[i] = st.ticks[j];
                }
            }
        }

        printf("pid %d: [%d], pid %d: [%d], pid %d: [%d]\n", pids[0], ticks[0],
               pids[1], ticks[1], pids[2], ticks[2]);

        spin();
        time++;
    }

Cleanup:
    for (i = 0; pids[i] > 0; i++) {
        kill(pids[i]);
    }
    while (wait(NULL) > -1)
        ;

    exit(0);
}