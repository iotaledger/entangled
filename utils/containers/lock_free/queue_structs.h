/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/*
 * UMM - unbounded queue, multiple producers, multiple consumers
 */

#ifndef __UTILS_CONTAINERS_LOCK_FREE_QUEUE_STRUCTS_H__
#define __UTILS_CONTAINERS_LOCK_FREE_QUEUE_STRUCTS_H__

#include <stddef.h>
#include <stdint-gcc.h>

#include <liblfds711.h>
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  struct lfds711_queue_umm_element queue_element_dummy;
  struct lfds711_queue_umm_state queue;
  struct lfds711_freelist_state freelist;
} iota_lf_umm_queue_t;

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_LOCK_FREE_QUEUE_STRUCTS_H__
