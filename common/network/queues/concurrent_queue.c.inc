/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#define DEFINE_VTABLE_CONCURRENT_QUEUE_OF(TYPE) \
  vtable_concurrent_queue_of_##TYPE##_t const   \
      vtable_concurrent_queue_of_##TYPE##_g =   \
          {CONCURRENT_QUEUE_OF_##TYPE##_FRONT,  \
           CONCURRENT_QUEUE_OF_##TYPE##_BACK,   \
           CONCURRENT_QUEUE_OF_##TYPE##_EMPTY,  \
           CONCURRENT_QUEUE_OF_##TYPE##_SIZE,   \
           CONCURRENT_QUEUE_OF_##TYPE##_PUSH,   \
           CONCURRENT_QUEUE_OF_##TYPE##_POP};

#define DEFINE_NEW_CONCURRENT_QUEUE_OF(TYPE)                                \
  struct concurrent_queue_of_##TYPE##_s *NEW_CONCURRENT_QUEUE_OF_##TYPE() { \
    struct concurrent_queue_of_##TYPE##_s *queue = malloc(sizeof(*queue));  \
    if (queue == NULL) return NULL;                                         \
    queue->front = NULL;                                                    \
    queue->back = NULL;                                                     \
    queue->vtable = &vtable_concurrent_queue_of_##TYPE##_g;                 \
    queue->size = 0;                                                        \
    return queue;                                                           \
  }

// TODO lock_handle
// TODO cond_handle
// TODO return ptr or cpy ?

#define DEFINE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE)   \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_FRONT(               \
                                                          \
      struct concurrent_queue_of_##TYPE##_s *self) {      \
    if (self == NULL || self->front == NULL) return NULL; \
    return &self->front->data;                            \
  }

#define DEFINE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)   \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_BACK(               \
      struct concurrent_queue_of_##TYPE##_s *self) {     \
    if (self == NULL || self->back == NULL) return NULL; \
    return &self->back->data;                            \
  }

#define DEFINE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  int CONCURRENT_QUEUE_OF_##TYPE##_EMPTY(               \
      struct concurrent_queue_of_##TYPE##_s *self) {    \
    if (self == NULL || self->size == 0) return 1;      \
    return 0;                                           \
  }

#define DEFINE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE) \
  size_t CONCURRENT_QUEUE_OF_##TYPE##_SIZE(            \
      struct concurrent_queue_of_##TYPE##_s *self) {   \
    if (self == NULL) return 0;                        \
    return self->size;                                 \
  }

#define DEFINE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)                 \
  void CONCURRENT_QUEUE_OF_##TYPE##_PUSH(                              \
      struct concurrent_queue_of_##TYPE##_s *self, TYPE data) {        \
    if (self == NULL) return;                                          \
    concurrent_queue_node_of_##TYPE##_t *node = malloc(sizeof(*node)); \
    if (node == NULL) return;                                          \
    node->data = data;                                                 \
    node->next = NULL;                                                 \
    self->size++;                                                      \
    if (self->front == NULL) {                                         \
      self->front = node;                                              \
      self->back = node;                                               \
      return;                                                          \
    }                                                                  \
    self->back->next = node;                                           \
    self->back = node;                                                 \
    return;                                                            \
  }

#define DEFINE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)       \
  void CONCURRENT_QUEUE_OF_##TYPE##_POP(                    \
      struct concurrent_queue_of_##TYPE##_s *self) {        \
    if (self == NULL || self->front == NULL) return;        \
    concurrent_queue_node_of_##TYPE##_t *tmp = self->front; \
    self->front = self->front->next;                        \
    if (self->front == NULL) self->back = NULL;             \
    self->size--;                                           \
    free(tmp);                                              \
  }

#define DEFINE_CONCURRENT_QUEUE_OF(TYPE)          \
  DEFINE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)         \
  DEFINE_NEW_CONCURRENT_QUEUE_OF(TYPE)            \
  DEFINE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE) \
  DEFINE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  DEFINE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)
