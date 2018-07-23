/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_LIST_H__
#define __COMMON_NETWORK_LIST_H__

#include <stdbool.h>

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

#define DECLARE_VTABLE_LIST_OF(TYPE)                \
  typedef struct list_of_##TYPE##_s list_of_##TYPE; \
  typedef struct vtable_list_of_##TYPE##_s {        \
    TYPE *(*front)(list_of_##TYPE *const self);     \
    TYPE *(*back)(list_of_##TYPE *const self);      \
    bool (*empty)(list_of_##TYPE *const self);      \
    size_t (*size)(list_of_##TYPE *const self);     \
  } vtable_list_of_##TYPE##_t;                      \
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
  list_status_t LIST_OF_##TYPE##_INIT(list_of_##TYPE **const self);

/**
 * Destroys a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_DESTROY_LIST_OF(TYPE) \
  list_status_t LIST_OF_##TYPE##_DESTROY(list_of_##TYPE **const self);

/**
 * Returns pointer to the first element in the list
 *
 * @param self The list
 *
 * @return Pointer to the first element
 */
#define DECLARE_FUNCTION_FRONT_LIST_OF(TYPE) \
  TYPE *LIST_OF_##TYPE##_FRONT(list_of_##TYPE *const self);

/**
 * Returns pointer to the last element in the list
 *
 * @param self The list
 *
 * @return Pointer to the last element
 */
#define DECLARE_FUNCTION_BACK_LIST_OF(TYPE) \
  TYPE *LIST_OF_##TYPE##_BACK(list_of_##TYPE *const self);

/**
 * Checks if the list has no elements
 *
 * @param self The list
 *
 * @return true if the list is empty, false otherwise
 */
#define DECLARE_FUNCTION_EMPTY_LIST_OF(TYPE) \
  bool LIST_OF_##TYPE##_EMPTY(list_of_##TYPE *const self);

/**
 * Returns the number of elements in the list
 *
 * @param self The list
 *
 * @return The number of elements in the list
 */
#define DECLARE_FUNCTION_SIZE_LIST_OF(TYPE) \
  size_t LIST_OF_##TYPE##_SIZE(list_of_##TYPE *const self);

#define DECLARE_LIST_OF(TYPE)            \
  DECLARE_STRUCT_LIST_NODE_OF(TYPE)      \
  DECLARE_VTABLE_LIST_OF(TYPE)           \
  DECLARE_STRUCT_LIST_OF(TYPE)           \
  DECLARE_FUNCTION_INIT_LIST_OF(TYPE)    \
  DECLARE_FUNCTION_DESTROY_LIST_OF(TYPE) \
  DECLARE_FUNCTION_FRONT_LIST_OF(TYPE)   \
  DECLARE_FUNCTION_BACK_LIST_OF(TYPE)    \
  DECLARE_FUNCTION_EMPTY_LIST_OF(TYPE)   \
  DECLARE_FUNCTION_SIZE_LIST_OF(TYPE)

#endif  // __COMMON_NETWORK_LIST_H__
