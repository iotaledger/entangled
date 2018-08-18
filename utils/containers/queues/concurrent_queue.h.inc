/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_H__
#define __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_H__

#include <stdbool.h>

#include "utils/handles/cond.h"
#include "utils/handles/lock.h"

typedef enum concurrent_queue_status_e {
  CONCURRENT_QUEUE_SUCCESS,
  CONCURRENT_QUEUE_NULL_SELF,
  CONCURRENT_QUEUE_NULL_DATA,
  CONCURRENT_QUEUE_ALLOC_FAILED,
  CONCURRENT_QUEUE_TIMEOUT
} concurrent_queue_status_t;

#define DECLARE_STRUCT_CONCURRENT_QUEUE_NODE_OF(TYPE)  \
  typedef struct concurrent_queue_node_of_##TYPE##_s { \
    TYPE data;                                         \
    struct concurrent_queue_node_of_##TYPE##_s *next;  \
  } concurrent_queue_node_of_##TYPE;

#define DECLARE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)                              \
  typedef struct concurrent_queue_of_##TYPE##_s concurrent_queue_of_##TYPE;   \
  typedef struct vtable_concurrent_queue_of_##TYPE##_s {                      \
    TYPE *(*front)(concurrent_queue_of_##TYPE *const self);                   \
    TYPE *(*back)(concurrent_queue_of_##TYPE *const self);                    \
    bool (*empty)(concurrent_queue_of_##TYPE *const self);                    \
    size_t (*size)(concurrent_queue_of_##TYPE *const self);                   \
    concurrent_queue_status_t (*push)(concurrent_queue_of_##TYPE *const self, \
                                      TYPE const data);                       \
    concurrent_queue_status_t (*pop)(concurrent_queue_of_##TYPE *const self,  \
                                     TYPE *const data);                       \
  } vtable_concurrent_queue_of_##TYPE##_t;                                    \
  extern vtable_concurrent_queue_of_##TYPE##_t const                          \
      vtable_concurrent_queue_of_##TYPE##_g;

#define DECLARE_STRUCT_CONCURRENT_QUEUE_OF(TYPE)         \
  typedef struct concurrent_queue_of_##TYPE##_s {        \
    vtable_concurrent_queue_of_##TYPE##_t const *vtable; \
    concurrent_queue_node_of_##TYPE *front;              \
    concurrent_queue_node_of_##TYPE *back;               \
    size_t size;                                         \
    lock_handle_t lock;                                  \
    cond_handle_t cond;                                  \
  } concurrent_queue_of_##TYPE;

#define INIT_CONCURRENT_QUEUE_OF(TYPE, NAME) \
  CONCURRENT_QUEUE_OF_##TYPE##_INIT(&NAME)

#define DESTROY_CONCURRENT_QUEUE_OF(TYPE, NAME) \
  CONCURRENT_QUEUE_OF_##TYPE##_DESTROY(&NAME)

/**
 * Initializes a concurrent queue
 *
 * @param self The queue
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_INIT_CONCURRENT_QUEUE_OF(TYPE)        \
  concurrent_queue_status_t CONCURRENT_QUEUE_OF_##TYPE##_INIT( \
      concurrent_queue_of_##TYPE **const self);

/**
 * Destroys a concurrent queue
 * /!\ This function is not thread safe
 *
 * @param self The queue
 *
 * @return an exit status
 */
#define DECLARE_FUNCTION_DESTROY_CONCURRENT_QUEUE_OF(TYPE)        \
  concurrent_queue_status_t CONCURRENT_QUEUE_OF_##TYPE##_DESTROY( \
      concurrent_queue_of_##TYPE **const self);

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
      concurrent_queue_of_##TYPE *const self);
#define CQ_FRONT(queue) (queue)->vtable->front((queue))

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
      concurrent_queue_of_##TYPE *const self);
#define CQ_BACK(queue) (queue)->vtable->back((queue))

/**
 * Checks if the queue has no elements
 *
 * @param self The queue
 *
 * @return true if the queue is empty, false otherwise
 */
#define DECLARE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  bool CONCURRENT_QUEUE_OF_##TYPE##_EMPTY(               \
      concurrent_queue_of_##TYPE *const self);
#define CQ_EMPTY(queue) (queue)->vtable->empty((queue))

/**
 * Returns the number of elements in the queue
 *
 * @param self The queue
 *
 * @return The number of elements in the queue
 */
#define DECLARE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE) \
  size_t CONCURRENT_QUEUE_OF_##TYPE##_SIZE(             \
      concurrent_queue_of_##TYPE *const self);
#define CQ_SIZE(queue) (queue)->vtable->size((queue))

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
      concurrent_queue_of_##TYPE *const self, TYPE const data);
#define CQ_PUSH(queue, data) (queue)->vtable->push((queue), (data))

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
      concurrent_queue_of_##TYPE *const self, TYPE *const data);
#define CQ_POP(queue, data) (queue)->vtable->pop((queue), (data))

#define DECLARE_CONCURRENT_QUEUE_OF(TYPE)            \
  DECLARE_STRUCT_CONCURRENT_QUEUE_NODE_OF(TYPE)      \
  DECLARE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)           \
  DECLARE_STRUCT_CONCURRENT_QUEUE_OF(TYPE)           \
  DECLARE_FUNCTION_INIT_CONCURRENT_QUEUE_OF(TYPE)    \
  DECLARE_FUNCTION_DESTROY_CONCURRENT_QUEUE_OF(TYPE) \
  DECLARE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE)   \
  DECLARE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)    \
  DECLARE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE)   \
  DECLARE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)    \
  DECLARE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)    \
  DECLARE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)

#endif  // __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_H__
