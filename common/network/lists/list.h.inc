/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_LIST_H__
#define __COMMON_NETWORK_LIST_H__

#include <stdlib.h>

typedef enum list_status_e {
  LIST_SUCCESS,
  LIST_NULL_SELF,
  LIST_ALLOC_FAILED
} list_status_t;

#define DECLARE_STRUCT_LIST_NODE_OF(TYPE)  \
  typedef struct list_node_of_##TYPE##_s { \
    TYPE data;                             \
    struct list_node_of_##TYPE##_s *prev;  \
    struct list_node_of_##TYPE##_s *next;  \
  } list_node_of_##TYPE##_t;

#define DECLARE_VTABLE_LIST_OF(TYPE)                       \
  struct list_of_##TYPE##_s;                               \
  typedef struct vtable_list_of_##TYPE##_s {               \
    size_t (*size)(struct list_of_##TYPE##_s *const self); \
  } vtable_list_of_##TYPE##_t;                             \
  extern vtable_list_of_##TYPE##_t const vtable_list_of_##TYPE##_g;

#define DECLARE_STRUCT_LIST_OF(TYPE)         \
  typedef struct list_of_##TYPE##_s {        \
    vtable_list_of_##TYPE##_t const *vtable; \
    list_node_of_##TYPE##_t *front;          \
    list_node_of_##TYPE##_t *back;           \
    size_t size;                             \
  } list_of_##TYPE;

#define INIT_LIST_OF(TYPE, NAME) LIST_OF_##TYPE##_INIT(&NAME)

#define DESTROY_LIST_OF(TYPE, NAME) LIST_OF_##TYPE##_DESTROY(&NAME)

/**
 * Initializes a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_INIT_LIST_OF(TYPE) \
  list_status_t LIST_OF_##TYPE##_INIT(struct list_of_##TYPE##_s **const self);

/**
 * Destroys a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_DESTROY_LIST_OF(TYPE) \
  list_status_t LIST_OF_##TYPE##_DESTROY(      \
      struct list_of_##TYPE##_s **const self);

/**
 * Returns the number of elements in the list
 *
 * @param self The list
 *
 * @return The number of elements in the list
 */
#define DECLARE_FUNCTION_SIZE_LIST_OF(TYPE) \
  size_t LIST_OF_##TYPE##_SIZE(struct list_of_##TYPE##_s *const self);

#define DECLARE_LIST_OF(TYPE)            \
  DECLARE_STRUCT_LIST_NODE_OF(TYPE)      \
  DECLARE_VTABLE_LIST_OF(TYPE)           \
  DECLARE_STRUCT_LIST_OF(TYPE)           \
  DECLARE_FUNCTION_INIT_LIST_OF(TYPE)    \
  DECLARE_FUNCTION_DESTROY_LIST_OF(TYPE) \
  DECLARE_FUNCTION_SIZE_LIST_OF(TYPE)

#endif  // __COMMON_NETWORK_LIST_H__
