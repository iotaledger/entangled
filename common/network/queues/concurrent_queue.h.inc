/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdbool.h>
#include <stdlib.h>

#include "common/cond_handle.h"
#include "common/lock_handle.h"

typedef enum concurrent_queue_status_e {
  CONCURRENT_QUEUE_SUCCESS,
  CONCURRENT_QUEUE_NULL_SELF,
  CONCURRENT_QUEUE_NULL_DATA,
  CONCURRENT_QUEUE_ALLOC_FAILED
} concurrent_queue_status_t;

#define DECLARE_STRUCT_CONCURRENT_QUEUE_NODE_OF(TYPE)  \
  typedef struct concurrent_queue_node_of_##TYPE##_s { \
    TYPE data;                                         \
    struct concurrent_queue_node_of_##TYPE##_s *next;  \
  } concurrent_queue_node_of_##TYPE##_t;

#define DECLARE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)                              \
  struct concurrent_queue_of_##TYPE##_s;                                      \
  typedef struct vtable_concurrent_queue_of_##TYPE##_s {                      \
    TYPE *(*front)(struct concurrent_queue_of_##TYPE##_s *const self);        \
    TYPE *(*back)(struct concurrent_queue_of_##TYPE##_s *const self);         \
    bool (*empty)(struct concurrent_queue_of_##TYPE##_s *const self);         \
    size_t (*size)(struct concurrent_queue_of_##TYPE##_s *const self);        \
    concurrent_queue_status_t (*push)(                                        \
        struct concurrent_queue_of_##TYPE##_s *const self, TYPE const data);  \
    concurrent_queue_status_t (*pop)(                                         \
        struct concurrent_queue_of_##TYPE##_s *const self, TYPE *const data); \
  } vtable_concurrent_queue_of_##TYPE##_t;                                    \
  extern vtable_concurrent_queue_of_##TYPE##_t const                          \
      vtable_concurrent_queue_of_##TYPE##_g;

#define DECLARE_STRUCT_CONCURRENT_QUEUE_OF(TYPE)         \
  typedef struct concurrent_queue_of_##TYPE##_s {        \
    concurrent_queue_node_of_##TYPE##_t *front;          \
    concurrent_queue_node_of_##TYPE##_t *back;           \
    vtable_concurrent_queue_of_##TYPE##_t const *vtable; \
    size_t size;                                         \
    lock_handle_t lock;                                  \
    cond_handle_t cond;                                  \
  } concurrent_queue_of_##TYPE;

#define INIT_CONCURRENT_QUEUE_OF(TYPE, NAME)               \
  NAME = (concurrent_queue_of_##TYPE *)malloc(             \
      sizeof(concurrent_queue_of_##TYPE));                 \
  if (NAME != NULL) {                                      \
    NAME->front = NULL;                                    \
    NAME->back = NULL;                                     \
    NAME->vtable = &vtable_concurrent_queue_of_##TYPE##_g; \
    NAME->size = 0;                                        \
    lock_handle_init(&NAME->lock);                         \
    cond_handle_init(&NAME->cond);                         \
  }

/**
 * Returns pointer to the first element in the queue. This element will be the
 * first element to be removed on a call to pop()
 *
 * @param self The queue
 *
 * @return Pointer to the first element
 */
#define DECLARE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE) \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_FRONT(              \
      struct concurrent_queue_of_##TYPE##_s *const self);

/**
 * Returns pointer to the last element in the queue. This is the most recently
 * pushed element
 *
 * @param self The queue
 *
 * @return Pointer to the last element
 */
#define DECLARE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE) \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_BACK(              \
      struct concurrent_queue_of_##TYPE##_s *const self);

/**
 * Checks if the underlying container has no elements
 *
 * @param self The queue
 *
 * @return true if the queue is empty, false otherwise
 */
#define DECLARE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  bool CONCURRENT_QUEUE_OF_##TYPE##_EMPTY(               \
      struct concurrent_queue_of_##TYPE##_s *const self);

/**
 * Returns the number of elements in the queue
 *
 * @param self The queue
 *
 * @return The number of elements in the queue
 */
#define DECLARE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE) \
  size_t CONCURRENT_QUEUE_OF_##TYPE##_SIZE(             \
      struct concurrent_queue_of_##TYPE##_s *const self);

/**
 * Pushes the given element value to the end of the queue
 *
 * @param self The queue
 * @param data The data to be pushed
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)        \
  concurrent_queue_status_t CONCURRENT_QUEUE_OF_##TYPE##_PUSH( \
      struct concurrent_queue_of_##TYPE##_s *const self, TYPE const data);

/**
 * Removes an element from the front of the queue
 *
 * @param self The queue
 * @param data The popped data
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)        \
  concurrent_queue_status_t CONCURRENT_QUEUE_OF_##TYPE##_POP( \
      struct concurrent_queue_of_##TYPE##_s *const self, TYPE *const data);

#define DECLARE_CONCURRENT_QUEUE_OF(TYPE)          \
  DECLARE_STRUCT_CONCURRENT_QUEUE_NODE_OF(TYPE)    \
  DECLARE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)         \
  DECLARE_STRUCT_CONCURRENT_QUEUE_OF(TYPE)         \
  DECLARE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE) \
  DECLARE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)  \
  DECLARE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  DECLARE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)  \
  DECLARE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)  \
  DECLARE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)\
