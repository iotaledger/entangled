/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

static unsigned int const concurrent_queue_pop_timeout_g = 2;

#define DEFINE_CQ_VTABLE(TYPE)                                                \
  concurrent_queue_vtable_##TYPE const concurrent_queue_vtable_##TYPE##_g = { \
      CQ_FRONT_##TYPE, CQ_BACK_##TYPE, CQ_EMPTY_##TYPE,                       \
      CQ_SIZE_##TYPE,  CQ_PUSH_##TYPE, CQ_POP_##TYPE};

#define DEFINE_CQ_INIT(TYPE)                                                \
  concurrent_queue_status_t CQ_INIT_##TYPE(                                 \
      concurrent_queue_##TYPE **const self) {                               \
    if (self == NULL) {                                                     \
      return CQ_NULL_SELF;                                                  \
    }                                                                       \
    *self =                                                                 \
        (concurrent_queue_##TYPE *)malloc(sizeof(concurrent_queue_##TYPE)); \
    if (*self == NULL) {                                                    \
      return CQ_ALLOC_FAILED;                                               \
    }                                                                       \
    (*self)->vtable = &concurrent_queue_vtable_##TYPE##_g;                  \
    (*self)->front = NULL;                                                  \
    (*self)->back = NULL;                                                   \
    (*self)->size = 0;                                                      \
    lock_handle_init(&(*self)->lock);                                       \
    cond_handle_init(&(*self)->cond);                                       \
    return CQ_SUCCESS;                                                      \
  }

#define DEFINE_CQ_DESTROY(TYPE)                \
  concurrent_queue_status_t CQ_DESTROY_##TYPE( \
      concurrent_queue_##TYPE **const self) {  \
    concurrent_queue_node_##TYPE *front, *tmp; \
    if (self == NULL || *self == NULL) {       \
      return CQ_NULL_SELF;                     \
    }                                          \
    front = (*self)->front;                    \
    while (front) {                            \
      tmp = front;                             \
      front = front->next;                     \
      free(tmp);                               \
    }                                          \
    lock_handle_destroy(&(*self)->lock);       \
    cond_handle_destroy(&(*self)->cond);       \
    free(*self);                               \
    *self = NULL;                              \
    return CQ_SUCCESS;                         \
  }

#define DEFINE_CQ_FRONT(TYPE)                                  \
  TYPE *CQ_FRONT_##TYPE(concurrent_queue_##TYPE *const self) { \
    TYPE *data = NULL;                                         \
    if (self == NULL) {                                        \
      return NULL;                                             \
    }                                                          \
    lock_handle_lock(&self->lock);                             \
    if (self->front) {                                         \
      data = &self->front->data;                               \
    }                                                          \
    lock_handle_unlock(&self->lock);                           \
    return data;                                               \
  }

#define DEFINE_CQ_BACK(TYPE)                                  \
  TYPE *CQ_BACK_##TYPE(concurrent_queue_##TYPE *const self) { \
    TYPE *data = NULL;                                        \
    if (self == NULL) {                                       \
      return NULL;                                            \
    }                                                         \
    lock_handle_lock(&self->lock);                            \
    if (self->back) {                                         \
      data = &self->back->data;                               \
    }                                                         \
    lock_handle_unlock(&self->lock);                          \
    return data;                                              \
  }

#define DEFINE_CQ_EMPTY(TYPE)                                 \
  bool CQ_EMPTY_##TYPE(concurrent_queue_##TYPE *const self) { \
    size_t size;                                              \
    if (self == NULL) {                                       \
      return true;                                            \
    }                                                         \
    lock_handle_lock(&self->lock);                            \
    size = self->size;                                        \
    lock_handle_unlock(&self->lock);                          \
    return size == 0;                                         \
  }

#define DEFINE_CQ_SIZE(TYPE)                                   \
  size_t CQ_SIZE_##TYPE(concurrent_queue_##TYPE *const self) { \
    size_t size;                                               \
    if (self == NULL) {                                        \
      return 0;                                                \
    }                                                          \
    lock_handle_lock(&self->lock);                             \
    size = self->size;                                         \
    lock_handle_unlock(&self->lock);                           \
    return size;                                               \
  }

#define DEFINE_CQ_PUSH(TYPE)                                  \
  concurrent_queue_status_t CQ_PUSH_##TYPE(                   \
      concurrent_queue_##TYPE *const self, TYPE const data) { \
    concurrent_queue_node_##TYPE *node;                       \
    if (self == NULL) {                                       \
      return CQ_NULL_SELF;                                    \
    }                                                         \
    node = (concurrent_queue_node_##TYPE *)calloc(            \
        1, sizeof(concurrent_queue_node_##TYPE));             \
    if (node == NULL) {                                       \
      return CQ_ALLOC_FAILED;                                 \
    }                                                         \
    node->data = data;                                        \
    lock_handle_lock(&self->lock);                            \
    if (self->front == NULL) {                                \
      self->front = node;                                     \
      self->back = node;                                      \
    } else {                                                  \
      self->back->next = node;                                \
      self->back = node;                                      \
    }                                                         \
    self->size++;                                             \
    lock_handle_unlock(&self->lock);                          \
    cond_handle_signal(&self->cond);                          \
    return CQ_SUCCESS;                                        \
  }

#define DEFINE_CQ_POP(TYPE)                                                    \
  concurrent_queue_status_t CQ_POP_##TYPE(concurrent_queue_##TYPE *const self, \
                                          TYPE *const data) {                  \
    if (self == NULL) {                                                        \
      return CQ_NULL_SELF;                                                     \
    }                                                                          \
    if (data == NULL) {                                                        \
      return CQ_NULL_DATA;                                                     \
    }                                                                          \
    lock_handle_lock(&self->lock);                                             \
    while (self->size == 0) {                                                  \
      if (cond_handle_timedwait(&self->cond, &self->lock,                      \
                                concurrent_queue_pop_timeout_g) != 0) {        \
        lock_handle_unlock(&self->lock);                                       \
        return CQ_TIMEOUT;                                                     \
      }                                                                        \
    }                                                                          \
    concurrent_queue_node_##TYPE *tmp = self->front;                           \
    *data = tmp->data;                                                         \
    self->front = self->front->next;                                           \
    if (self->front == NULL) {                                                 \
      self->back = NULL;                                                       \
    }                                                                          \
    self->size--;                                                              \
    free(tmp);                                                                 \
    lock_handle_unlock(&self->lock);                                           \
    return CQ_SUCCESS;                                                         \
  }

#define DEFINE_CQ(TYPE)   \
  DEFINE_CQ_VTABLE(TYPE)  \
  DEFINE_CQ_INIT(TYPE)    \
  DEFINE_CQ_DESTROY(TYPE) \
  DEFINE_CQ_FRONT(TYPE)   \
  DEFINE_CQ_BACK(TYPE)    \
  DEFINE_CQ_EMPTY(TYPE)   \
  DEFINE_CQ_SIZE(TYPE)    \
  DEFINE_CQ_PUSH(TYPE)    \
  DEFINE_CQ_POP(TYPE)
