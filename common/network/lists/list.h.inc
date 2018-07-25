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
  LIST_NULL_DATA,
  LIST_ALLOC_FAILED,
  LIST_EMPTY
} list_status_t;

#define DECLARE_STRUCT_LIST_NODE_OF(TYPE)  \
  typedef struct list_node_of_##TYPE##_s { \
    TYPE data;                             \
    struct list_node_of_##TYPE##_s *prev;  \
    struct list_node_of_##TYPE##_s *next;  \
  } list_node_of_##TYPE##_t;

#define DECLARE_VTABLE_LIST_OF(TYPE)                                          \
  typedef struct list_of_##TYPE##_s list_of_##TYPE;                           \
  typedef struct vtable_list_of_##TYPE##_s {                                  \
    TYPE *(*front)(list_of_##TYPE const *const self);                         \
    TYPE *(*back)(list_of_##TYPE const *const self);                          \
    bool (*empty)(list_of_##TYPE const *const self);                          \
    size_t (*size)(list_of_##TYPE const *const self);                         \
    list_status_t (*push_front)(list_of_##TYPE *const self, TYPE const data); \
    list_status_t (*push_back)(list_of_##TYPE *const self, TYPE const data);  \
    list_status_t (*pop_front)(list_of_##TYPE *const self, TYPE *const data); \
    list_status_t (*pop_back)(list_of_##TYPE *const self, TYPE *const data);  \
    bool (*contains)(list_of_##TYPE *const self, TYPE const data);            \
  } vtable_list_of_##TYPE##_t;                                                \
  extern vtable_list_of_##TYPE##_t const vtable_list_of_##TYPE##_g;

#define DECLARE_STRUCT_LIST_OF(TYPE)               \
  typedef struct list_of_##TYPE##_s {              \
    vtable_list_of_##TYPE##_t const *vtable;       \
    bool (*cmp)(TYPE const self, TYPE const data); \
    list_node_of_##TYPE##_t *front;                \
    list_node_of_##TYPE##_t *back;                 \
    size_t size;                                   \
  } list_of_##TYPE;

#define INIT_LIST_OF(TYPE, NAME, CMP_FUNC) \
  LIST_OF_##TYPE##_INIT(&NAME, CMP_FUNC)

#define DESTROY_LIST_OF(TYPE, NAME) LIST_OF_##TYPE##_DESTROY(&NAME)

/**
 * Initializes a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_INIT_LIST_OF(TYPE) \
  list_status_t LIST_OF_##TYPE##_INIT(      \
      list_of_##TYPE **const self,          \
      bool (*cmp)(TYPE const self, TYPE const data));

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
  TYPE *LIST_OF_##TYPE##_FRONT(list_of_##TYPE const *const self);

/**
 * Returns pointer to the last element in the list
 *
 * @param self The list
 *
 * @return Pointer to the last element
 */
#define DECLARE_FUNCTION_BACK_LIST_OF(TYPE) \
  TYPE *LIST_OF_##TYPE##_BACK(list_of_##TYPE const *const self);

/**
 * Checks if the list has no elements
 *
 * @param self The list
 *
 * @return true if the list is empty, false otherwise
 */
#define DECLARE_FUNCTION_EMPTY_LIST_OF(TYPE) \
  bool LIST_OF_##TYPE##_EMPTY(list_of_##TYPE const *const self);

/**
 * Returns the number of elements in the list
 *
 * @param self The list
 *
 * @return The number of elements in the list
 */
#define DECLARE_FUNCTION_SIZE_LIST_OF(TYPE) \
  size_t LIST_OF_##TYPE##_SIZE(list_of_##TYPE const *const self);

/**
 * Adds an element at the front of the list
 *
 * @param self The list
 * @param data The data to add
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_PUSH_FRONT_LIST_OF(TYPE)                       \
  list_status_t LIST_OF_##TYPE##_PUSH_FRONT(list_of_##TYPE *const self, \
                                            TYPE const data);

/**
 * Adds an element at the back of the list
 *
 * @param self The list
 * @param data The data to add
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_PUSH_BACK_LIST_OF(TYPE)                       \
  list_status_t LIST_OF_##TYPE##_PUSH_BACK(list_of_##TYPE *const self, \
                                           TYPE const data);

/**
 * Removes an element from the front of the list
 *
 * @param self The list
 * @param data The data where to store the element
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_POP_FRONT_LIST_OF(TYPE)                       \
  list_status_t LIST_OF_##TYPE##_POP_FRONT(list_of_##TYPE *const self, \
                                           TYPE *const data);

/**
 * Removes an element from the back of the list
 *
 * @param self The list
 * @param data The data where to store the element
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_POP_BACK_LIST_OF(TYPE)                       \
  list_status_t LIST_OF_##TYPE##_POP_BACK(list_of_##TYPE *const self, \
                                          TYPE *const data);

/**
 * Tells whether an element is in a list or not
 *
 * @param self The list
 * @param data The data to check
 *
 * @return whether the element is in the list or not
 */
#define DECLARE_FUNCTION_CONTAINS_LIST_OF(TYPE) \
  bool LIST_OF_##TYPE##_CONTAINS(list_of_##TYPE *const self, TYPE const data);

#define DECLARE_LIST_OF(TYPE)               \
  DECLARE_STRUCT_LIST_NODE_OF(TYPE)         \
  DECLARE_VTABLE_LIST_OF(TYPE)              \
  DECLARE_STRUCT_LIST_OF(TYPE)              \
  DECLARE_FUNCTION_INIT_LIST_OF(TYPE)       \
  DECLARE_FUNCTION_DESTROY_LIST_OF(TYPE)    \
  DECLARE_FUNCTION_FRONT_LIST_OF(TYPE)      \
  DECLARE_FUNCTION_BACK_LIST_OF(TYPE)       \
  DECLARE_FUNCTION_EMPTY_LIST_OF(TYPE)      \
  DECLARE_FUNCTION_SIZE_LIST_OF(TYPE)       \
  DECLARE_FUNCTION_PUSH_FRONT_LIST_OF(TYPE) \
  DECLARE_FUNCTION_PUSH_BACK_LIST_OF(TYPE)  \
  DECLARE_FUNCTION_POP_FRONT_LIST_OF(TYPE)  \
  DECLARE_FUNCTION_POP_BACK_LIST_OF(TYPE)   \
  DECLARE_FUNCTION_CONTAINS_LIST_OF(TYPE)

#endif  // __COMMON_NETWORK_LIST_H__
