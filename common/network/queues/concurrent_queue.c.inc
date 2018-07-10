/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#define DEFINE_VTABLE_CONCURRENT_QUEUE_OF(TYPE) \
  vtable_concurrent_queue_of_##TYPE##_t const   \
      vtable_concurrent_queue_of_##TYPE##_g =   \
          {CONCURRENT_QUEUE_OF_##TYPE##_FRONT,  \
           CONCURRENT_QUEUE_OF_##TYPE##_BACK,   \
           CONCURRENT_QUEUE_OF_##TYPE##_EMPTY,  \
           CONCURRENT_QUEUE_OF_##TYPE##_SIZE,   \
           CONCURRENT_QUEUE_OF_##TYPE##_PUSH,   \
           CONCURRENT_QUEUE_OF_##TYPE##_POP};

#define DEFINE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE)    \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_FRONT(                \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    if (self == NULL) return NULL;                         \
    lock_handle_lock(&self->lock);                         \
    if (self->front == NULL) {                             \
      lock_handle_unlock(&self->lock);                     \
      return NULL;                                         \
    }                                                      \
    TYPE *data = &self->front->data;                       \
    lock_handle_unlock(&self->lock);                       \
    return data;                                           \
  }

#define DEFINE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)     \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_BACK(                 \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    if (self == NULL) return NULL;                         \
    lock_handle_lock(&self->lock);                         \
    if (self->back == NULL) {                              \
      lock_handle_unlock(&self->lock);                     \
      return NULL;                                         \
    }                                                      \
    TYPE *data = &self->back->data;                        \
    lock_handle_unlock(&self->lock);                       \
    return data;                                           \
  }

#define DEFINE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE)    \
  int CONCURRENT_QUEUE_OF_##TYPE##_EMPTY(                  \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    if (self == NULL) return 1;                            \
    lock_handle_lock(&self->lock);                         \
    size_t size = self->size;                              \
    lock_handle_unlock(&self->lock);                       \
    return size == 0;                                      \
  }

#define DEFINE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)     \
  size_t CONCURRENT_QUEUE_OF_##TYPE##_SIZE(                \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    if (self == NULL) return 0;                            \
    lock_handle_lock(&self->lock);                         \
    size_t size = self->size;                              \
    lock_handle_unlock(&self->lock);                       \
    return size;                                           \
  }

#define DEFINE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)                      \
  int CONCURRENT_QUEUE_OF_##TYPE##_PUSH(                                    \
      struct concurrent_queue_of_##TYPE##_s *const self, TYPE const data) { \
    if (self == NULL) return 1;                                             \
    concurrent_queue_node_of_##TYPE##_t *node = malloc(sizeof(*node));      \
    if (node == NULL) return 1;                                             \
    node->data = data;                                                      \
    node->next = NULL;                                                      \
    lock_handle_lock(&self->lock);                                          \
    if (self->front == NULL) {                                              \
      self->front = node;                                                   \
      self->back = node;                                                    \
    } else {                                                                \
      self->back->next = node;                                              \
      self->back = node;                                                    \
    }                                                                       \
    self->size++;                                                           \
    lock_handle_unlock(&self->lock);                                        \
    cond_handle_signal(&self->cond);                                        \
    return 0;                                                               \
  }

#define DEFINE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)                        \
  int CONCURRENT_QUEUE_OF_##TYPE##_POP(                                      \
      struct concurrent_queue_of_##TYPE##_s *const self, TYPE *const data) { \
    if (self == NULL || data == NULL) return 1;                              \
    lock_handle_lock(&self->lock);                                           \
    while (self->size == 0) {                                                \
      cond_handle_wait(&self->cond, &self->lock);                            \
    }                                                                        \
    concurrent_queue_node_of_##TYPE##_t *tmp = self->front;                  \
    *data = tmp->data;                                                       \
    self->front = self->front->next;                                         \
    if (self->front == NULL) self->back = NULL;                              \
    self->size--;                                                            \
    free(tmp);                                                               \
    lock_handle_unlock(&self->lock);                                         \
    return 0;                                                                \
  }

#define DEFINE_CONCURRENT_QUEUE_OF(TYPE)          \
  DEFINE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)         \
  DEFINE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE) \
  DEFINE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  DEFINE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)
