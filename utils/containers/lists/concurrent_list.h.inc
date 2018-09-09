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
  CL_SUCCESS,
  CL_NULL_SELF,
  CL_NULL_DATA,
  CL_NULL_CMP_FUNC,
  CL_ALLOC_FAILED,
  CL_EMPTY
} concurrent_list_status_t;

#define DECLARE_CL_STRUCT_NODE(TYPE)               \
  typedef struct concurrent_list_node_##TYPE##_s { \
    TYPE data;                                     \
    struct concurrent_list_node_##TYPE##_s *prev;  \
    struct concurrent_list_node_##TYPE##_s *next;  \
  } concurrent_list_node_##TYPE;

#define DECLARE_CL_VTABLE(TYPE)                                                \
  typedef struct concurrent_list_##TYPE##_s concurrent_list_##TYPE;            \
  typedef struct concurrent_list_vtable_##TYPE##_s {                           \
    TYPE *(*front)(concurrent_list_##TYPE *const self);                        \
    TYPE *(*back)(concurrent_list_##TYPE *const self);                         \
    bool (*empty)(concurrent_list_##TYPE *const self);                         \
    size_t (*size)(concurrent_list_##TYPE *const self);                        \
    concurrent_list_status_t (*push_front)(concurrent_list_##TYPE *const self, \
                                           TYPE const data);                   \
    concurrent_list_status_t (*push_back)(concurrent_list_##TYPE *const self,  \
                                          TYPE const data);                    \
    concurrent_list_status_t (*pop_front)(concurrent_list_##TYPE *const self,  \
                                          TYPE *const data);                   \
    concurrent_list_status_t (*pop_back)(concurrent_list_##TYPE *const self,   \
                                         TYPE *const data);                    \
    TYPE *(*find)(concurrent_list_##TYPE *const self, TYPE const data);        \
    bool (*contains)(concurrent_list_##TYPE *const self, TYPE const data);     \
    bool (*remove)(concurrent_list_##TYPE *const self, TYPE const data);       \
  } concurrent_list_vtable_##TYPE;                                             \
  extern concurrent_list_vtable_##TYPE const concurrent_list_vtable_##TYPE##_g;

#define DECLARE_CL_STRUCT(TYPE)                                  \
  typedef struct concurrent_list_##TYPE##_s {                    \
    concurrent_list_vtable_##TYPE const *vtable;                 \
    bool (*cmp)(TYPE const *const self, TYPE const *const data); \
    concurrent_list_node_##TYPE *front;                          \
    concurrent_list_node_##TYPE *back;                           \
    size_t size;                                                 \
    lock_handle_t lock;                                          \
  } concurrent_list_##TYPE;

/**
 * Initializes a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_CL_INIT(TYPE)              \
  concurrent_list_status_t CL_INIT_##TYPE( \
      concurrent_list_##TYPE **const self, \
      bool (*cmp)(TYPE const *const self, TYPE const *const data));
#define CL_INIT(TYPE, NAME, CMP_FUNC) CL_INIT_##TYPE(&NAME, CMP_FUNC)

/**
 * Destroys a list
 *
 * @param self The list
 *
 * @return an exit status
 */
#define DECLARE_CL_DESTROY(TYPE)              \
  concurrent_list_status_t CL_DESTROY_##TYPE( \
      concurrent_list_##TYPE **const self);
#define CL_DESTROY(TYPE, NAME) CL_DESTROY_##TYPE(&NAME)

/**
 * Returns pointer to the first element in the list
 *
 * @param self The list
 *
 * @return Pointer to the first element
 */
#define DECLARE_CL_FRONT(TYPE) \
  TYPE *CL_FRONT_##TYPE(concurrent_list_##TYPE *const self);
#define CL_FRONT(list) (list)->vtable->front((list))

/**
 * Returns pointer to the last element in the list
 *
 * @param self The list
 *
 * @return Pointer to the last element
 */
#define DECLARE_CL_BACK(TYPE) \
  TYPE *CL_BACK_##TYPE(concurrent_list_##TYPE *const self);
#define CL_BACK(list) (list)->vtable->back((list))

/**
 * Checks if the list has no elements
 *
 * @param self The list
 *
 * @return true if the list is empty, false otherwise
 */
#define DECLARE_CL_EMPTY(TYPE) \
  bool CL_EMPTY_##TYPE(concurrent_list_##TYPE *const self);
#define CL_EMPTY(list) (list)->vtable->empty((list))

/**
 * Returns the number of elements in the list
 *
 * @param self The list
 *
 * @return The number of elements in the list
 */
#define DECLARE_CL_SIZE(TYPE) \
  size_t CL_SIZE_##TYPE(concurrent_list_##TYPE *const self);
#define CL_SIZE(list) (list)->vtable->size((list))

/**
 * Adds an element at the front of the list
 *
 * @param self The list
 * @param data The data to add
 *
 * @return an exit status
 */
#define DECLARE_CL_PUSH_FRONT(TYPE)              \
  concurrent_list_status_t CL_PUSH_FRONT_##TYPE( \
      concurrent_list_##TYPE *const self, TYPE const data);
#define CL_PUSH_FRONT(list, data) (list)->vtable->push_front((list), (data))

/**
 * Adds an element at the back of the list
 *
 * @param self The list
 * @param data The data to add
 *
 * @return an exit status
 */
#define DECLARE_CL_PUSH_BACK(TYPE)              \
  concurrent_list_status_t CL_PUSH_BACK_##TYPE( \
      concurrent_list_##TYPE *const self, TYPE const data);
#define CL_PUSH_BACK(list, data) (list)->vtable->push_back((list), (data))

/**
 * Removes an element from the front of the list
 *
 * @param self The list
 * @param data The data where to store the element
 *
 * @return an exit status
 */
#define DECLARE_CL_POP_FRONT(TYPE)              \
  concurrent_list_status_t CL_POP_FRONT_##TYPE( \
      concurrent_list_##TYPE *const self, TYPE *const data);
#define CL_POP_FRONT(list, data) (list)->vtable->pop_front((list), (data))

/**
 * Removes an element from the back of the list
 *
 * @param self The list
 * @param data The data where to store the element
 *
 * @return an exit status
 */
#define DECLARE_CL_POP_BACK(TYPE)              \
  concurrent_list_status_t CL_POP_BACK_##TYPE( \
      concurrent_list_##TYPE *const self, TYPE *const data);
#define CL_POP_BACK(list, data) (list)->vtable->pop_back((list), (data))

/**
 * Find an element in a list
 *
 * @param self The list
 * @param data The data to find
 *
 * @return null or a pointer to the data
 */
#define DECLARE_CL_FIND(TYPE) \
  TYPE *CL_FIND_##TYPE(concurrent_list_##TYPE *const self, TYPE const data);
#define CL_FIND(list, data) (list)->vtable->find((list), (data))

/**
 * Checks whether an element is in a list or not
 *
 * @param self The list
 * @param data The data to check
 *
 * @return whether the element is in the list or not
 */
#define DECLARE_CL_CONTAINS(TYPE) \
  bool CL_CONTAINS_##TYPE(concurrent_list_##TYPE *const self, TYPE const data);
#define CL_CONTAINS(list, data) (list)->vtable->contains((list), (data))

/**
 * Removes elements from a list
 *
 * @param self The list
 * @param data The data to remove
 *
 * @return whether elements were removed
 */
#define DECLARE_CL_REMOVE(TYPE) \
  bool CL_REMOVE_##TYPE(concurrent_list_##TYPE *const self, TYPE const data);
#define CL_REMOVE(list, data) (list)->vtable->remove((list), (data))

#define DECLARE_CL(TYPE)       \
  DECLARE_CL_STRUCT_NODE(TYPE) \
  DECLARE_CL_VTABLE(TYPE)      \
  DECLARE_CL_STRUCT(TYPE)      \
  DECLARE_CL_INIT(TYPE)        \
  DECLARE_CL_DESTROY(TYPE)     \
  DECLARE_CL_FRONT(TYPE)       \
  DECLARE_CL_BACK(TYPE)        \
  DECLARE_CL_EMPTY(TYPE)       \
  DECLARE_CL_SIZE(TYPE)        \
  DECLARE_CL_PUSH_FRONT(TYPE)  \
  DECLARE_CL_PUSH_BACK(TYPE)   \
  DECLARE_CL_POP_FRONT(TYPE)   \
  DECLARE_CL_POP_BACK(TYPE)    \
  DECLARE_CL_FIND(TYPE)        \
  DECLARE_CL_CONTAINS(TYPE)    \
  DECLARE_CL_REMOVE(TYPE)

#endif  // __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_H__
