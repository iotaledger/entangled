/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#define DEFINE_VTABLE_LIST_OF(TYPE)                             \
  vtable_list_of_##TYPE##_t const vtable_list_of_##TYPE##_g = { \
      LIST_OF_##TYPE##_SIZE};

#define DEFINE_FUNCTION_INIT_LIST_OF(TYPE)      \
  list_status_t LIST_OF_##TYPE##_INIT(          \
      struct list_of_##TYPE##_s **const self) { \
    return LIST_SUCCESS;                        \
  }

#define DEFINE_FUNCTION_DESTROY_LIST_OF(TYPE)   \
  list_status_t LIST_OF_##TYPE##_DESTROY(       \
      struct list_of_##TYPE##_s **const self) { \
    return LIST_SUCCESS;                        \
  }

#define DEFINE_FUNCTION_SIZE_LIST_OF(TYPE)                              \
  size_t LIST_OF_##TYPE##_SIZE(struct list_of_##TYPE##_s *const self) { \
    return self->size;                                                  \
  }

#define DEFINE_LIST_OF(TYPE)            \
  DEFINE_VTABLE_LIST_OF(TYPE)           \
  DEFINE_FUNCTION_INIT_LIST_OF(TYPE)    \
  DEFINE_FUNCTION_DESTROY_LIST_OF(TYPE) \
  DEFINE_FUNCTION_SIZE_LIST_OF(TYPE)\
