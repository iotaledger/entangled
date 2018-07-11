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

#define DEFINE_FUNCTION_INIT_CONCURRENT_QUEUE_OF(TYPE)        \
  int CONCURRENT_QUEUE_OF_##TYPE##_INIT(                      \
      struct concurrent_queue_of_##TYPE##_s **const self) {   \
    if (self == NULL) return CONCURRENT_QUEUE_NULL_SELF;      \
    *self = (concurrent_queue_of_##TYPE *)malloc(             \
        sizeof(concurrent_queue_of_##TYPE));                  \
    if (*self == NULL) return CONCURRENT_QUEUE_ALLOC_FAILED;  \
    (*self)->front = NULL;                                    \
    (*self)->back = NULL;                                     \
    (*self)->vtable = &vtable_concurrent_queue_of_##TYPE##_g; \
    (*self)->size = 0;                                        \
    lock_handle_init(&(*self)->lock);                         \
    cond_handle_init(&(*self)->cond);                         \
    return CONCURRENT_QUEUE_SUCCESS;                          \
  }

#define DEFINE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE)    \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_FRONT(                \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    TYPE *data = NULL;                                     \
    if (self == NULL) return NULL;                         \
    lock_handle_lock(&self->lock);                         \
    if (self->front) data = &self->front->data;            \
    lock_handle_unlock(&self->lock);                       \
    return data;                                           \
  }

#define DEFINE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)     \
  TYPE *CONCURRENT_QUEUE_OF_##TYPE##_BACK(                 \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    TYPE *data = NULL;                                     \
    if (self == NULL) return NULL;                         \
    lock_handle_lock(&self->lock);                         \
    if (self->back) data = &self->back->data;              \
    lock_handle_unlock(&self->lock);                       \
    return data;                                           \
  }

#define DEFINE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE)    \
  bool CONCURRENT_QUEUE_OF_##TYPE##_EMPTY(                 \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    size_t size;                                           \
    if (self == NULL) return true;                         \
    lock_handle_lock(&self->lock);                         \
    size = self->size;                                     \
    lock_handle_unlock(&self->lock);                       \
    return size == 0;                                      \
  }

#define DEFINE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)     \
  size_t CONCURRENT_QUEUE_OF_##TYPE##_SIZE(                \
      struct concurrent_queue_of_##TYPE##_s *const self) { \
    size_t size;                                           \
    if (self == NULL) return 0;                            \
    lock_handle_lock(&self->lock);                         \
    size = self->size;                                     \
    lock_handle_unlock(&self->lock);                       \
    return size;                                           \
  }

#define DEFINE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)                      \
  concurrent_queue_status_t CONCURRENT_QUEUE_OF_##TYPE##_PUSH(              \
      struct concurrent_queue_of_##TYPE##_s *const self, TYPE const data) { \
    concurrent_queue_node_of_##TYPE##_t *node;                              \
    if (self == NULL) return CONCURRENT_QUEUE_NULL_SELF;                    \
    node = calloc(1, sizeof(concurrent_queue_node_of_##TYPE##_t));          \
    if (node == NULL) return CONCURRENT_QUEUE_ALLOC_FAILED;                 \
    node->data = data;                                                      \
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
    return CONCURRENT_QUEUE_SUCCESS;                                        \
  }

#define DEFINE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)                        \
  concurrent_queue_status_t CONCURRENT_QUEUE_OF_##TYPE##_POP(                \
      struct concurrent_queue_of_##TYPE##_s *const self, TYPE *const data) { \
    if (self == NULL) return CONCURRENT_QUEUE_NULL_SELF;                     \
    if (data == NULL) return CONCURRENT_QUEUE_NULL_DATA;                     \
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
    return CONCURRENT_QUEUE_SUCCESS;                                         \
  }

#define DEFINE_CONCURRENT_QUEUE_OF(TYPE)          \
  DEFINE_VTABLE_CONCURRENT_QUEUE_OF(TYPE)         \
  DEFINE_FUNCTION_INIT_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_FRONT_CONCURRENT_QUEUE_OF(TYPE) \
  DEFINE_FUNCTION_BACK_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_EMPTY_CONCURRENT_QUEUE_OF(TYPE) \
  DEFINE_FUNCTION_SIZE_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_PUSH_CONCURRENT_QUEUE_OF(TYPE)  \
  DEFINE_FUNCTION_POP_CONCURRENT_QUEUE_OF(TYPE)
