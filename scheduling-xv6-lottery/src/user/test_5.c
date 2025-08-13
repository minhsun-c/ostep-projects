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

#define rounds 250
#define tol 150

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
    int numprocs = 46;
    int numtickets = 251;
    int pid_chds[numprocs];
    int start_pid = getpid();
    char buffer[numprocs + 1];
    int i = 0;
    int pipe_par[2];
    int pipe_chd[2];
    check(settickets(numtickets) == 0, "settickets");
    printf(
        "Goal: To have a random, not simply uniform, scheduler\n(Pid "
        "difference variance > %d, Direction change > 15%%)\n\n",
        tol);

    pipe(pipe_par);
    pipe(pipe_chd);
    for (; i < numprocs; i++) {
        pid_chds[i] = fork();
        // printf("Child pid %d \n", pid_chds[i]);
        if (pid_chds[i] == 0) {
            close(pipe_chd[0]);
            close(pipe_par[1]);
            // Wait for ready signal
            int pid = getpid();
            read(pipe_par[0], buffer, sizeof(char));
            for (;;) {
                // printf("pid %d running\n",pid);
                fprintf(pipe_chd[1], "%d#", pid);
                sleep(1);  // Yield to scheduler
                           // spin();
            }
        }
    }
    if (settickets(100000000) != 0) {
        printf("check failed: settickets\n");
        goto Cleanup;
    }
    close(pipe_par[0]);
    close(pipe_chd[1]);
    // Send ready signal
    for (i = 0; i < numprocs; i++) {
        buffer[i] = 'a';
    }
    printf("Spinning...\n");
    write(pipe_par[1], buffer, numprocs * sizeof(char));

    // Begin collecting child running data
    int chosen_pids[rounds];
    int sum = 0, var = 0, negcount = 0;
    for (i = 0; i < rounds; i++) {
        int j = -1;
        do {
            j++;
            read(pipe_chd[0], buffer + j, sizeof(char));
        } while (j < 15 && buffer[j] != '#');
        // Store processes that are scheduled in chosen_pids array
        chosen_pids[i] = atoi(buffer);
        if (chosen_pids[i] <= 0 || chosen_pids[i] > pid_chds[numprocs - 1]) {
            i--;
            continue;
        }
        if (i > 0) {
            chosen_pids[i - 1] =
                chosen_pids[i] - chosen_pids[i - 1];  // Difference
            sum += chosen_pids[i - 1];
            if (i > 1 && ((chosen_pids[i - 1] >= 0) ==
                          (chosen_pids[i - 2] < 0)))  // Direction change
                negcount++;
        }
        // printf("Chosen pid: %d\n",chosen_pids[i]);
    }

    if (settickets(100000000) != 0) {
        printf("check failed: settickets\n");
        goto Cleanup;
    }

    sum /= (rounds - 1);  // Average difference
    for (i = 0; i < rounds - 1; i++) {
        var += (sum - chosen_pids[i]) * (sum - chosen_pids[i]);
    }
    var /= (rounds - 1);
    printf("Variance: %d\nDirection change percentage: %d%%\n", var,
           negcount * 100 / rounds);
    if (var < tol || negcount * 15 < rounds) {
        printf("check failed: Scheduling not random enough\n");
        goto Cleanup;
    }
    printf("Should print 1 then 2\n");

Cleanup:
    for (i = 1; i <= numprocs; i++)
        kill(start_pid + i);
    while (wait(NULL) > -1)
        ;
    exit(0);
}