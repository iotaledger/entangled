
#include "random_trits.h"
#include <stdlib.h>
#include <time.h>

void get_random_trits(long* RandomTrits, int length) {
  int i = 0;
  srand(time(NULL));
  while (i < length) {
    RandomTrits[i] = rand() % 3 - 1;
    i++;
  }
}
