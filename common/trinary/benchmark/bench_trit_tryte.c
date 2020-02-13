/*
 * Copyright (c) 2020 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common/trinary/trit_tryte.h"

// The minimum and maximum input/output tryte size for perfomance testing
#define MIN_TRYTE_SIZE 16
#define MAX_TRYTE_SIZE 2048
// The number of times of the same input size testing
#define TEST_TIMES 20

tryte_t tryte_chars[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                           'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '9'};
trit_t trit_nums[3] = {-1, 0, 1};

long diff_in_nanosec(struct timespec start, struct timespec end) {
  struct timespec diff;

  if (end.tv_nsec - start.tv_nsec < 0) {
    diff.tv_sec = end.tv_sec - start.tv_sec - 1;
    diff.tv_nsec = end.tv_nsec - start.tv_nsec + 1000000000;
  } else {
    diff.tv_sec = end.tv_sec - start.tv_sec;
    diff.tv_nsec = end.tv_nsec - start.tv_nsec;
  }

  return (diff.tv_sec * 1000000000 + diff.tv_nsec);
}

void test_trits_to_trytes(unsigned int trit_size, unsigned int times) {
  struct timespec start, end;
  long run_time;
  long min = 0, max = 0, sum = 0;

  trit_t *trits = malloc(sizeof(trit_t) * trit_size);
  tryte_t *trytes = malloc(sizeof(tryte_t) * trit_size / 3);

  for (unsigned int count = 0; count < times; count++) {
    // Generate random trits
    for (unsigned int idx = 0; idx < trit_size; idx++) {
      memset(trits + idx, trit_nums[rand() % 3], 1);
    }

    // Execution time measurement
    clock_gettime(CLOCK_MONOTONIC, &start);
    trits_to_trytes(trits, trytes, trit_size);
    clock_gettime(CLOCK_MONOTONIC, &end);
    run_time = diff_in_nanosec(start, end);

    max = (count == 0 || run_time > max) ? run_time : max;
    min = (count == 0 || run_time < min) ? run_time : min;
    sum += run_time;
  }

  printf("Input trit size: %d\n", trit_size);
  printf("        minimum: %ld nsec\n", min);
  printf("        maximum: %ld nsec\n", max);
  printf("        average: %ld nsec\n", sum / times);

  free(trits);
  free(trytes);
}

void test_trytes_to_trits(unsigned int tryte_size, unsigned int times) {
  struct timespec start, end;
  long run_time;
  long min = 0, max = 0, sum = 0;

  tryte_t *trytes = malloc(sizeof(tryte_t) * tryte_size);
  trit_t *trits = malloc(sizeof(trit_t) * 3 * tryte_size);

  for (unsigned int count = 0; count < times; count++) {
    // Generate random trytes
    for (unsigned int idx = 0; idx < tryte_size; idx++) {
      memset(trytes + idx, tryte_chars[rand() % 27], 1);
    }

    // Execution time measurement
    clock_gettime(CLOCK_MONOTONIC, &start);
    trytes_to_trits(trytes, trits, tryte_size);
    clock_gettime(CLOCK_MONOTONIC, &end);
    run_time = diff_in_nanosec(start, end);

    max = (count == 0 || run_time > max) ? run_time : max;
    min = (count == 0 || run_time < min) ? run_time : min;
    sum += run_time;
  }

  printf("Input tryte size: %d\n", tryte_size);
  printf("         minimum: %ld nsec\n", min);
  printf("         maximum: %ld nsec\n", max);
  printf("         average: %ld nsec\n", sum / times);

  free(trytes);
  free(trits);
}

int main(void) {
  unsigned int size;

  // Set random seed
  srand(time(NULL));

  printf("trytes_to_trits\n");

  for (size = MIN_TRYTE_SIZE; size <= MAX_TRYTE_SIZE; size++) {
    test_trytes_to_trits(size, TEST_TIMES);
  }

  printf("\n");
  printf("trits_to_trytes\n");

  for (size = MIN_TRYTE_SIZE; size <= MAX_TRYTE_SIZE; size++) {
    test_trits_to_trytes(size * 3, TEST_TIMES);
  }

  return 0;
}
