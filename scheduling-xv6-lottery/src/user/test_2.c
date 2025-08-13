// user: burn.c
#include "../kernel/pstat.h"
#include "../kernel/types.h"
#include "user.h"

volatile unsigned long sink;
int main(int argc, char **argv) {
  int ms = (argc>1) ? atoi(argv[1]) : 200; // ~200 ticks
  settickets(10);
  uint t0 = uptime();
  while (uptime() - t0 < ms) sink++;
  struct pstat ps;
  getpinfo(&ps);
  for (int i=0;i<NPROC;i++)
    if (ps.inuse[i] && ps.pid[i]==getpid())
      printf("me: pid=%d tickets=%d ticks=%d\n",
             ps.pid[i], ps.tickets[i], ps.ticks[i]);
  exit(0);
}
