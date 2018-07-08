/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/cond_handle.h"
#include "common/lock_handle.h"

#define DECLARE_STRUCT_CONCURRENT_QUEUE_NODE_OF(TYPE)  \
  typedef struct concurrent_queue_node_of_##TYPE##_s { \
    TYPE data;                                         \
    struct concurrent_queue_node_of_##TYPE##_s *next;  \
  } concurrent_queue_node_of_##TYPE##_t;

#define DECLARE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)                           \
  struct concurrent_queue_of_##TYPE##_s;                                   \
  typedef struct vtable_concurrent_queue_of_##TYPE##_s {                   \
    TYPE *(*front)(struct concurrent_queue_of_##TYPE##_s * self);          \
    TYPE *(*back)(struct concurrent_queue_of_##TYPE##_s * self);           \
    int (*empty)(struct concurrent_queue_of_##TYPE##_s * self);            \
    size_t (*size)(struct concurrent_queue_of_##TYPE##_s * self);          \
    void (*push)(struct concurrent_queue_of_##TYPE##_s * self, TYPE data); \
    void (*pop)(struct concurrent_queue_of_##TYPE##_s * self);             \
  } vtable_concurrent_queue_of_##TYPE##_t;                                 \
  extern vtable_concurrent_queue_of_##TYPE##_t const                       \
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

#define DECLARE_NEW_CONCURRENT_QUEUE_OF(TYPE) \
  struct concurrent_queue_of_##TYPE##_s *NEW_CONCURRENT_QUEUE_OF_##TYPE();

/**
 * Returns pointer to the first element in the queue. This element will be the
 * first element to be removed on a call to pop()
 *
 * @return Pointer to the first element
 */
#define DECLARE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE) \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_FRONT(              \
      struct concurrent_queue_of_##TYPE##_s *self);

/**
 * Returns pointer to the last element in the queue. This is the most recently
 * pushed element
 *
 * @return Pointer to the last element
 */
#define DECLARE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE) \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_BACK(              \
      struct concurrent_queue_of_##TYPE##_s *self);

/**
 * Checks if the underlying container has no elements
 *
 * @return true if the queue is empty, false otherwise
 */
#define DECLARE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  int CONCURRENT_QUEUE_OF_##TYPE##_EMPTY(                \
      struct concurrent_queue_of_##TYPE##_s *self);

/**
 * Returns the number of elements in the queue
 *
 * @return The number of elements in the queue
 */
#define DECLARE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE) \
  size_t CONCURRENT_QUEUE_OF_##TYPE##_SIZE(             \
      struct concurrent_queue_of_##TYPE##_s *self);

/**
 * Pushes the given element value to the end of the queue
 */
#define DECLARE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE) \
  void CONCURRENT_QUEUE_OF_##TYPE##_PUSH(               \
      struct concurrent_queue_of_##TYPE##_s *self, TYPE data);

/**
 * Removes an element from the front of the queue
 */
#define DECLARE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE) \
  void CONCURRENT_QUEUE_OF_##TYPE##_POP(               \
      struct concurrent_queue_of_##TYPE##_s *self);

#define DECLARE_CONCURRENT_QUEUE_OF(TYPE)          \
  DECLARE_STRUCT_CONCURRENT_QUEUE_NODE_OF(TYPE)    \
  DECLARE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)         \
  DECLARE_STRUCT_CONCURRENT_QUEUE_OF(TYPE)         \
  DECLARE_NEW_CONCURRENT_QUEUE_OF(TYPE)            \
  DECLARE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE) \
  DECLARE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)  \
  DECLARE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  DECLARE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)  \
  DECLARE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)  \
  DECLARE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)\
