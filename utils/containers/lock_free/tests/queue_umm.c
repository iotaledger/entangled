/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdbool.h>
#include <stdlib.h>
#include <unity/unity.h>
#include "common/errors.h"
#include "utils/containers/lock_free/lf_queue_int.h"

void queue_integers_same_thread() {
  iota_lf_umm_queue_t queue;
  iota_lf_umm_queue_int_init_owner(&queue);
  iota_lf_umm_queue_int_init_user(&queue);
  for (int i = 0; i < 1000; ++i) {
    iota_lf_umm_queue_int_enqueue(&queue, &i);
  }

  enum lfds711_misc_flag dummy_element_flag;
  int expected_current = 0;
  while (iota_lf_umm_queue_int_count(&queue)) {
    iota_lf_queue_umm_int_t* dequeued_element =
        iota_lf_umm_queue_int_dequeue(&queue);
    TEST_ASSERT_EQUAL_INT((dequeued_element->data), expected_current++);
  }

  iota_lf_umm_queue_int_free(&queue);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(queue_integers_same_thread);

  return UNITY_END();
}
