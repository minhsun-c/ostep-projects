#include "types.h"

static uint64 randseed = 1;

void
srand(uint64 seed)
{
  randseed = seed;
}

int
rand(void)
{
  randseed = randseed * 1664525 + 1013904223;
  return (randseed >> 16) & 0x7FFF; // 0..32767
}
