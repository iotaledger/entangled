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
  CQ_SUCCESS,
  CQ_NULL_SELF,
  CQ_NULL_DATA,
  CQ_ALLOC_FAILED,
  CQ_TIMEOUT
} concurrent_queue_status_t;

#define DECLARE_CQ_STRUCT_NODE(TYPE)                \
  typedef struct concurrent_queue_node_##TYPE##_s { \
    TYPE data;                                      \
    struct concurrent_queue_node_##TYPE##_s *next;  \
  } concurrent_queue_node_##TYPE;

#define DECLARE_CQ_VTABLE(TYPE)                                            \
  typedef struct concurrent_queue_##TYPE##_s concurrent_queue_##TYPE;      \
  typedef struct concurrent_queue_vtable_##TYPE##_s {                      \
    TYPE *(*front)(concurrent_queue_##TYPE *const self);                   \
    TYPE *(*back)(concurrent_queue_##TYPE *const self);                    \
    bool (*empty)(concurrent_queue_##TYPE *const self);                    \
    size_t (*size)(concurrent_queue_##TYPE *const self);                   \
    concurrent_queue_status_t (*push)(concurrent_queue_##TYPE *const self, \
                                      TYPE const data);                    \
    concurrent_queue_status_t (*pop)(concurrent_queue_##TYPE *const self,  \
                                     TYPE *const data);                    \
  } concurrent_queue_vtable_##TYPE;                                        \
  extern concurrent_queue_vtable_##TYPE const                              \
      concurrent_queue_vtable_##TYPE##_g;

#define DECLARE_CQ_STRUCT(TYPE)                   \
  typedef struct concurrent_queue_##TYPE##_s {    \
    concurrent_queue_vtable_##TYPE const *vtable; \
    concurrent_queue_node_##TYPE *front;          \
    concurrent_queue_node_##TYPE *back;           \
    size_t size;                                  \
    lock_handle_t lock;                           \
    cond_handle_t cond;                           \
  } concurrent_queue_##TYPE;

/**
 * Initializes a concurrent queue
 *
 * @param self The queue
 *
 * @return an exit status
 */
#define DECLARE_CQ_INIT(TYPE)               \
  concurrent_queue_status_t CQ_INIT_##TYPE( \
      concurrent_queue_##TYPE **const self);
#define CQ_INIT(TYPE, NAME) CQ_INIT_##TYPE(&NAME)

/**
 * Destroys a concurrent queue
 * /!\ This function is not thread safe
 *
 * @param self The queue
 *
 * @return an exit status
 */
#define DECLARE_CQ_DESTROY(TYPE)               \
  concurrent_queue_status_t CQ_DESTROY_##TYPE( \
      concurrent_queue_##TYPE **const self);
#define CQ_DESTROY(TYPE, NAME) CQ_DESTROY_##TYPE(&NAME)

/**
 * Returns pointer to the first element in the queue. This element will be the
 * first element to be removed on a call to pop()
 *
 * @param self The queue
 *
 * @return Pointer to the first element
 */
#define DECLARE_CQ_FRONT(TYPE) \
  TYPE *CQ_FRONT_##TYPE(concurrent_queue_##TYPE *const self);
#define CQ_FRONT(queue) (queue)->vtable->front((queue))

/**
 * Returns pointer to the last element in the queue. This is the most recently
 * pushed element
 *
 * @param self The queue
 *
 * @return Pointer to the last element
 */
#define DECLARE_CQ_BACK(TYPE) \
  TYPE *CQ_BACK_##TYPE(concurrent_queue_##TYPE *const self);
#define CQ_BACK(queue) (queue)->vtable->back((queue))

/**
 * Checks if the queue has no elements
 *
 * @param self The queue
 *
 * @return true if the queue is empty, false otherwise
 */
#define DECLARE_CQ_EMPTY(TYPE) \
  bool CQ_EMPTY_##TYPE(concurrent_queue_##TYPE *const self);
#define CQ_EMPTY(queue) (queue)->vtable->empty((queue))

/**
 * Returns the number of elements in the queue
 *
 * @param self The queue
 *
 * @return The number of elements in the queue
 */
#define DECLARE_CQ_SIZE(TYPE) \
  size_t CQ_SIZE_##TYPE(concurrent_queue_##TYPE *const self);
#define CQ_SIZE(queue) (queue)->vtable->size((queue))

/**
 * Pushes the given element value to the end of the queue
 *
 * @param self The queue
 * @param data The data to be pushed
 *
 * @return an exit status
 */
#define DECLARE_CQ_PUSH(TYPE)               \
  concurrent_queue_status_t CQ_PUSH_##TYPE( \
      concurrent_queue_##TYPE *const self, TYPE const data);
#define CQ_PUSH(queue, data) (queue)->vtable->push((queue), (data))

/**
 * Removes an element from the front of the queue
 *
 * @param self The queue
 * @param data The popped data
 *
 * @return an exit status
 */
#define DECLARE_CQ_POP(TYPE)                                                   \
  concurrent_queue_status_t CQ_POP_##TYPE(concurrent_queue_##TYPE *const self, \
                                          TYPE *const data);
#define CQ_POP(queue, data) (queue)->vtable->pop((queue), (data))

#define DECLARE_CQ(TYPE)       \
  DECLARE_CQ_STRUCT_NODE(TYPE) \
  DECLARE_CQ_VTABLE(TYPE)      \
  DECLARE_CQ_STRUCT(TYPE)      \
  DECLARE_CQ_INIT(TYPE)        \
  DECLARE_CQ_DESTROY(TYPE)     \
  DECLARE_CQ_FRONT(TYPE)       \
  DECLARE_CQ_BACK(TYPE)        \
  DECLARE_CQ_EMPTY(TYPE)       \
  DECLARE_CQ_SIZE(TYPE)        \
  DECLARE_CQ_PUSH(TYPE)        \
  DECLARE_CQ_POP(TYPE)

#endif  // __UTILS_CONTAINERS_QUEUES_CONCURRENT_QUEUE_H__
