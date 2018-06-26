#include "hamming.h"
#include <stdio.h>
#include "ptrit.h"
#include "search.h"
#include "trit.h"

short test(PCurl *const curl, unsigned short const security) {
  unsigned short i, j, k;
  signed short sum;
  for (i = 0; i < sizeof(ptrit_s) * 8; i++) {
    sum = 0;

    for (j = 0; j < security; j++) {
      for (k = j * HASH_LENGTH / 3; k < (j + 1) * HASH_LENGTH / 3; k++) {
        if ((curl->state[k].low & (1uLL << i)) == 0) {
          sum--;
        } else if ((curl->state[k].high & (1uLL << i)) == 0) {
          sum++;
        }
      }

      if (sum == 0 && j < security - 1) {
        sum = 1;
        break;
      }
    }

    if (sum == 0) {
      return i;
    }
  }

  return -1;
}

PearlDiverStatus hamming(Curl *const ctx, unsigned short const offset,
                         unsigned short const end,
                         unsigned short const security) {
  return pd_search(ctx, offset, end, test, security);
}
