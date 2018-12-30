/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_PERSON_EXAMPLE_H__
#define __UTILS_CONTAINERS_PERSON_EXAMPLE_H__

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct person_example_s {
  char name[128];
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
} person_example_t;

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_PERSON_EXAMPLE_H__
