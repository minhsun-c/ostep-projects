#include "../kernel/pstat.h"
#include "../kernel/types.h"
#include "user.h"

int main() {
  settickets(10);
  struct pstat ps;
  if (getpinfo(&ps) == 0) {
    for (int i=0;i<NPROC;i++)
      if (ps.inuse[i])
        printf("pid=%d tickets=%d ticks=%d\n",
               ps.pid[i], ps.tickets[i], ps.ticks[i]);
  }
  exit(0);
}
