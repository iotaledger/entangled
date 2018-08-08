/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_H__
#define __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_H__

#include <stdbool.h>

#include "utils/handles/lock.h"

typedef enum concurrent_list_status_e {
  CONCURRENT_LIST_SUCCESS,
  CONCURRENT_LIST_NULL_SELF,
  CONCURRENT_LIST_NULL_DATA,
  CONCURRENT_LIST_NULL_CMP_FUNC,
  CONCURRENT_LIST_ALLOC_FAILED,
  CONCURRENT_LIST_EMPTY
} concurrent_list_status_t;

#define DECLARE_STRUCT_CONCURRENT_LIST_NODE_OF(TYPE)  \
  typedef struct concurrent_list_node_of_##TYPE##_s { \
    TYPE data;                                        \
    struct concurrent_list_node_of_##TYPE##_s *prev;  \
    struct concurrent_list_node_of_##TYPE##_s *next;  \
  } concurrent_list_node_of_##TYPE;

#define DECLARE_VTABLE_CONCURRENT_LIST_OF(TYPE)                               \
  typedef struct concurrent_list_of_##TYPE##_s concurrent_list_of_##TYPE;     \
  typedef struct vtable_concurrent_list_of_##TYPE##_s {                       \
    TYPE *(*front)(concurrent_list_of_##TYPE *const self);                    \
    TYPE *(*back)(concurrent_list_of_##TYPE *const self);                     \
    bool (*empty)(concurrent_list_of_##TYPE *const self);                     \
    size_t (*size)(concurrent_list_of_##TYPE *const self);                    \
    concurrent_list_status_t (*push_front)(                                   \
        concurrent_list_of_##TYPE *const self, TYPE const data);              \
    concurrent_list_status_t (*push_back)(                                    \
        concurrent_list_of_##TYPE *const self, TYPE const data);              \
    concurrent_list_status_t (*pop_front)(                                    \
        concurrent_list_of_##TYPE *const self, TYPE *const data);             \
    concurrent_list_status_t (*pop_back)(                                     \
        concurrent_list_of_##TYPE *const self, TYPE *const data);             \
    TYPE *(*find)(concurrent_list_of_##TYPE *const self, TYPE const data);    \
    bool (*contains)(concurrent_list_of_##TYPE *const self, TYPE const data); \
    bool (*remove)(concurrent_list_of_##TYPE *const self, TYPE const data);   \
  } vtable_concurrent_list_of_##TYPE##_t;                                     \
  extern vtable_concurrent_list_of_##TYPE##_t const                           \
      vtable_concurrent_list_of_##TYPE##_g;

#define DECLARE_STRUCT_CONCURRENT_LIST_OF(TYPE)                  \
  typedef struct concurrent_list_of_##TYPE##_s {                 \
    vtable_concurrent_list_of_##TYPE##_t const *vtable;          \
    bool (*cmp)(TYPE const *const self, TYPE const *const data); \
    concurrent_list_node_of_##TYPE *front;                       \
    concurrent_list_node_of_##TYPE *back;                        \
    size_t size;                                                 \
    lock_handle_t lock;                                          \
  } concurrent_list_of_##TYPE;

#define INIT_CONCURRENT_LIST_OF(TYPE, NAME, CMP_FUNC) \
  CONCURRENT_LIST_OF_##TYPE##_INIT(&NAME, CMP_FUNC)

#define DESTROY_CONCURRENT_LIST_OF(TYPE, NAME) \
  CONCURRENT_LIST_OF_##TYPE##_DESTROY(&NAME)

/**
 * Initializes a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_INIT_CONCURRENT_LIST_OF(TYPE)       \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_INIT( \
      concurrent_list_of_##TYPE **const self,                \
      bool (*cmp)(TYPE const *const self, TYPE const *const data));

/**
 * Destroys a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_DESTROY_CONCURRENT_LIST_OF(TYPE)       \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_DESTROY( \
      concurrent_list_of_##TYPE **const self);

/**
 * Returns pointer to the first element in the list
 *
 * @param self The list
 *
 * @return Pointer to the first element
 */
#define DECLARE_FUNCTION_FRONT_CONCURRENT_LIST_OF(TYPE) \
  TYPE *CONCURRENT_LIST_OF_##TYPE##_FRONT(              \
      concurrent_list_of_##TYPE *const self);

/**
 * Returns pointer to the last element in the list
 *
 * @param self The list
 *
 * @return Pointer to the last element
 */
#define DECLARE_FUNCTION_BACK_CONCURRENT_LIST_OF(TYPE) \
  TYPE *CONCURRENT_LIST_OF_##TYPE##_BACK(concurrent_list_of_##TYPE *const self);

/**
 * Checks if the list has no elements
 *
 * @param self The list
 *
 * @return true if the list is empty, false otherwise
 */
#define DECLARE_FUNCTION_EMPTY_CONCURRENT_LIST_OF(TYPE) \
  bool CONCURRENT_LIST_OF_##TYPE##_EMPTY(concurrent_list_of_##TYPE *const self);

/**
 * Returns the number of elements in the list
 *
 * @param self The list
 *
 * @return The number of elements in the list
 */
#define DECLARE_FUNCTION_SIZE_CONCURRENT_LIST_OF(TYPE) \
  size_t CONCURRENT_LIST_OF_##TYPE##_SIZE(             \
      concurrent_list_of_##TYPE *const self);

/**
 * Adds an element at the front of the list
 *
 * @param self The list
 * @param data The data to add
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_PUSH_FRONT_CONCURRENT_LIST_OF(TYPE)       \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_PUSH_FRONT( \
      concurrent_list_of_##TYPE *const self, TYPE const data);

/**
 * Adds an element at the back of the list
 *
 * @param self The list
 * @param data The data to add
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_PUSH_BACK_CONCURRENT_LIST_OF(TYPE)       \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_PUSH_BACK( \
      concurrent_list_of_##TYPE *const self, TYPE const data);

/**
 * Removes an element from the front of the list
 *
 * @param self The list
 * @param data The data where to store the element
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_POP_FRONT_CONCURRENT_LIST_OF(TYPE)       \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_POP_FRONT( \
      concurrent_list_of_##TYPE *const self, TYPE *const data);

/**
 * Removes an element from the back of the list
 *
 * @param self The list
 * @param data The data where to store the element
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_POP_BACK_CONCURRENT_LIST_OF(TYPE)       \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_POP_BACK( \
      concurrent_list_of_##TYPE *const self, TYPE *const data);

/**
 * Find an element in a list
 *
 * @param self The list
 * @param data The data to find
 *
 * @return null or a pointer to the data
 */
#define DECLARE_FUNCTION_FIND_CONCURRENT_LIST_OF(TYPE) \
  TYPE *CONCURRENT_LIST_OF_##TYPE##_FIND(              \
      concurrent_list_of_##TYPE *const self, TYPE const data);

/**
 * Checks whether an element is in a list or not
 *
 * @param self The list
 * @param data The data to check
 *
 * @return whether the element is in the list or not
 */
#define DECLARE_FUNCTION_CONTAINS_CONCURRENT_LIST_OF(TYPE) \
  bool CONCURRENT_LIST_OF_##TYPE##_CONTAINS(               \
      concurrent_list_of_##TYPE *const self, TYPE const data);

/**
 * Removes elements from a list
 *
 * @param self The list
 * @param data The data to remove
 *
 * @return whether elements were removed
 */
#define DECLARE_FUNCTION_REMOVE_CONCURRENT_LIST_OF(TYPE) \
  bool CONCURRENT_LIST_OF_##TYPE##_REMOVE(               \
      concurrent_list_of_##TYPE *const self, TYPE const data);

#define DECLARE_CONCURRENT_LIST_OF(TYPE)               \
  DECLARE_STRUCT_CONCURRENT_LIST_NODE_OF(TYPE)         \
  DECLARE_VTABLE_CONCURRENT_LIST_OF(TYPE)              \
  DECLARE_STRUCT_CONCURRENT_LIST_OF(TYPE)              \
  DECLARE_FUNCTION_INIT_CONCURRENT_LIST_OF(TYPE)       \
  DECLARE_FUNCTION_DESTROY_CONCURRENT_LIST_OF(TYPE)    \
  DECLARE_FUNCTION_FRONT_CONCURRENT_LIST_OF(TYPE)      \
  DECLARE_FUNCTION_BACK_CONCURRENT_LIST_OF(TYPE)       \
  DECLARE_FUNCTION_EMPTY_CONCURRENT_LIST_OF(TYPE)      \
  DECLARE_FUNCTION_SIZE_CONCURRENT_LIST_OF(TYPE)       \
  DECLARE_FUNCTION_PUSH_FRONT_CONCURRENT_LIST_OF(TYPE) \
  DECLARE_FUNCTION_PUSH_BACK_CONCURRENT_LIST_OF(TYPE)  \
  DECLARE_FUNCTION_POP_FRONT_CONCURRENT_LIST_OF(TYPE)  \
  DECLARE_FUNCTION_POP_BACK_CONCURRENT_LIST_OF(TYPE)   \
  DECLARE_FUNCTION_FIND_CONCURRENT_LIST_OF(TYPE)       \
  DECLARE_FUNCTION_CONTAINS_CONCURRENT_LIST_OF(TYPE)   \
  DECLARE_FUNCTION_REMOVE_CONCURRENT_LIST_OF(TYPE)

#endif  // __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_H__
