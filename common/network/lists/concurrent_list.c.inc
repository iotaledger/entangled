/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#define DEFINE_VTABLE_CONCURRENT_LIST_OF(TYPE)    \
  vtable_concurrent_list_of_##TYPE##_t const      \
      vtable_concurrent_list_of_##TYPE##_g = {    \
          CONCURRENT_LIST_OF_##TYPE##_FRONT,      \
          CONCURRENT_LIST_OF_##TYPE##_BACK,       \
          CONCURRENT_LIST_OF_##TYPE##_EMPTY,      \
          CONCURRENT_LIST_OF_##TYPE##_SIZE,       \
          CONCURRENT_LIST_OF_##TYPE##_PUSH_FRONT, \
          CONCURRENT_LIST_OF_##TYPE##_PUSH_BACK,  \
          CONCURRENT_LIST_OF_##TYPE##_POP_FRONT,  \
          CONCURRENT_LIST_OF_##TYPE##_POP_BACK,   \
          CONCURRENT_LIST_OF_##TYPE##_CONTAINS,   \
          CONCURRENT_LIST_OF_##TYPE##_REMOVE};

#define DEFINE_FUNCTION_INIT_CONCURRENT_LIST_OF(TYPE)                \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_INIT(         \
      concurrent_list_of_##TYPE **const self,                        \
      bool (*cmp)(TYPE const *const self, TYPE const *const data)) { \
    if (self == NULL) {                                              \
      return CONCURRENT_LIST_NULL_SELF;                              \
    }                                                                \
    if (cmp == NULL) {                                               \
      return CONCURRENT_LIST_NULL_CMP_FUNC;                          \
    }                                                                \
    *self = (concurrent_list_of_##TYPE *)malloc(                     \
        sizeof(concurrent_list_of_##TYPE));                          \
    if (*self == NULL) {                                             \
      return CONCURRENT_LIST_ALLOC_FAILED;                           \
    }                                                                \
    (*self)->vtable = &vtable_concurrent_list_of_##TYPE##_g;         \
    (*self)->cmp = cmp;                                              \
    (*self)->front = NULL;                                           \
    (*self)->back = NULL;                                            \
    (*self)->size = 0;                                               \
    lock_handle_init(&(*self)->lock);                                \
    return CONCURRENT_LIST_SUCCESS;                                  \
  }

#define DEFINE_FUNCTION_DESTROY_CONCURRENT_LIST_OF(TYPE)        \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_DESTROY( \
      concurrent_list_of_##TYPE **const self) {                 \
    concurrent_list_node_of_##TYPE##_t *front, *tmp;            \
    if (self == NULL || *self == NULL) {                        \
      return CONCURRENT_LIST_NULL_SELF;                         \
    }                                                           \
    front = (*self)->front;                                     \
    while (front) {                                             \
      tmp = front;                                              \
      front = front->next;                                      \
      free(tmp);                                                \
    }                                                           \
    lock_handle_destroy(&(*self)->lock);                        \
    free(*self);                                                \
    *self = NULL;                                               \
    return CONCURRENT_LIST_SUCCESS;                             \
  }

#define DEFINE_FUNCTION_FRONT_CONCURRENT_LIST_OF(TYPE) \
  TYPE *CONCURRENT_LIST_OF_##TYPE##_FRONT(             \
      concurrent_list_of_##TYPE *const self) {         \
    TYPE *data = NULL;                                 \
    if (self == NULL) {                                \
      return NULL;                                     \
    }                                                  \
    lock_handle_lock(&self->lock);                     \
    if (self->front) {                                 \
      data = &self->front->data;                       \
    }                                                  \
    lock_handle_unlock(&self->lock);                   \
    return data;                                       \
  }

#define DEFINE_FUNCTION_BACK_CONCURRENT_LIST_OF(TYPE) \
  TYPE *CONCURRENT_LIST_OF_##TYPE##_BACK(             \
      concurrent_list_of_##TYPE *const self) {        \
    TYPE *data = NULL;                                \
    if (self == NULL) {                               \
      return NULL;                                    \
    }                                                 \
    lock_handle_lock(&self->lock);                    \
    if (self->back) {                                 \
      data = &self->back->data;                       \
    }                                                 \
    lock_handle_unlock(&self->lock);                  \
    return data;                                      \
  }

#define DEFINE_FUNCTION_EMPTY_CONCURRENT_LIST_OF(TYPE) \
  bool CONCURRENT_LIST_OF_##TYPE##_EMPTY(              \
      concurrent_list_of_##TYPE *const self) {         \
    size_t size;                                       \
    if (self == NULL) {                                \
      return true;                                     \
    }                                                  \
    lock_handle_lock(&self->lock);                     \
    size = self->size;                                 \
    lock_handle_unlock(&self->lock);                   \
    return size == 0;                                  \
  }

#define DEFINE_FUNCTION_SIZE_CONCURRENT_LIST_OF(TYPE) \
  size_t CONCURRENT_LIST_OF_##TYPE##_SIZE(            \
      concurrent_list_of_##TYPE *const self) {        \
    size_t size;                                      \
    if (self == NULL) {                               \
      return 0;                                       \
    }                                                 \
    lock_handle_lock(&self->lock);                    \
    size = self->size;                                \
    lock_handle_unlock(&self->lock);                  \
    return size;                                      \
  }

#define DEFINE_FUNCTION_PUSH_FRONT_CONCURRENT_LIST_OF(TYPE)        \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_PUSH_FRONT( \
      concurrent_list_of_##TYPE *const self, TYPE const data) {    \
    concurrent_list_node_of_##TYPE##_t *node;                      \
    if (self == NULL) {                                            \
      return CONCURRENT_LIST_NULL_SELF;                            \
    }                                                              \
    node = (concurrent_list_node_of_##TYPE##_t *)calloc(           \
        1, sizeof(concurrent_list_node_of_##TYPE##_t));            \
    if (node == NULL) {                                            \
      return CONCURRENT_LIST_ALLOC_FAILED;                         \
    }                                                              \
    node->data = data;                                             \
    lock_handle_lock(&self->lock);                                 \
    if (self->size == 0) {                                         \
      self->back = node;                                           \
    } else {                                                       \
      self->front->prev = node;                                    \
      node->next = self->front;                                    \
    }                                                              \
    self->front = node;                                            \
    self->size++;                                                  \
    lock_handle_unlock(&self->lock);                               \
    return CONCURRENT_LIST_SUCCESS;                                \
  }

#define DEFINE_FUNCTION_PUSH_BACK_CONCURRENT_LIST_OF(TYPE)        \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_PUSH_BACK( \
      concurrent_list_of_##TYPE *const self, TYPE const data) {   \
    concurrent_list_node_of_##TYPE##_t *node;                     \
    if (self == NULL) {                                           \
      return CONCURRENT_LIST_NULL_SELF;                           \
    }                                                             \
    node = (concurrent_list_node_of_##TYPE##_t *)calloc(          \
        1, sizeof(concurrent_list_node_of_##TYPE##_t));           \
    if (node == NULL) {                                           \
      return CONCURRENT_LIST_ALLOC_FAILED;                        \
    }                                                             \
    node->data = data;                                            \
    lock_handle_lock(&self->lock);                                \
    if (self->size == 0) {                                        \
      self->front = node;                                         \
    } else {                                                      \
      self->back->next = node;                                    \
      node->prev = self->back;                                    \
    }                                                             \
    self->back = node;                                            \
    self->size++;                                                 \
    lock_handle_unlock(&self->lock);                              \
    return CONCURRENT_LIST_SUCCESS;                               \
  }

#define DEFINE_FUNCTION_POP_FRONT_CONCURRENT_LIST_OF(TYPE)        \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_POP_FRONT( \
      concurrent_list_of_##TYPE *const self, TYPE *const data) {  \
    concurrent_list_node_of_##TYPE##_t *tmp;                      \
    if (self == NULL) {                                           \
      return CONCURRENT_LIST_NULL_SELF;                           \
    }                                                             \
    if (data == NULL) {                                           \
      return CONCURRENT_LIST_NULL_DATA;                           \
    }                                                             \
    lock_handle_lock(&self->lock);                                \
    if (self->size == 0) {                                        \
      lock_handle_unlock(&self->lock);                            \
      return CONCURRENT_LIST_EMPTY;                               \
    }                                                             \
    tmp = self->front;                                            \
    *data = tmp->data;                                            \
    self->front = self->front->next;                              \
    if (self->front == NULL) {                                    \
      self->back = NULL;                                          \
    }                                                             \
    self->size--;                                                 \
    free(tmp);                                                    \
    lock_handle_unlock(&self->lock);                              \
    return CONCURRENT_LIST_SUCCESS;                               \
  }

#define DEFINE_FUNCTION_POP_BACK_CONCURRENT_LIST_OF(TYPE)        \
  concurrent_list_status_t CONCURRENT_LIST_OF_##TYPE##_POP_BACK( \
      concurrent_list_of_##TYPE *const self, TYPE *const data) { \
    concurrent_list_node_of_##TYPE##_t *tmp;                     \
    if (self == NULL) {                                          \
      return CONCURRENT_LIST_NULL_SELF;                          \
    }                                                            \
    if (data == NULL) {                                          \
      return CONCURRENT_LIST_NULL_DATA;                          \
    }                                                            \
    lock_handle_lock(&self->lock);                               \
    if (self->size == 0) {                                       \
      lock_handle_unlock(&self->lock);                           \
      return CONCURRENT_LIST_EMPTY;                              \
    }                                                            \
    tmp = self->front;                                           \
    *data = tmp->data;                                           \
    self->back = self->back->prev;                               \
    if (self->back == NULL) {                                    \
      self->front = NULL;                                        \
    }                                                            \
    self->size--;                                                \
    free(tmp);                                                   \
    lock_handle_unlock(&self->lock);                             \
    return CONCURRENT_LIST_SUCCESS;                              \
  }

#define DEFINE_FUNCTION_CONTAINS_CONCURRENT_LIST_OF(TYPE)       \
  bool CONCURRENT_LIST_OF_##TYPE##_CONTAINS(                    \
      concurrent_list_of_##TYPE *const self, TYPE const data) { \
    if (self == NULL) {                                         \
      return false;                                             \
    }                                                           \
    concurrent_list_node_of_##TYPE##_t *iter;                   \
    lock_handle_lock(&self->lock);                              \
    iter = self->front;                                         \
    while (iter) {                                              \
      if (self->cmp(&iter->data, &data)) {                      \
        lock_handle_unlock(&self->lock);                        \
        return true;                                            \
      }                                                         \
      iter = iter->next;                                        \
    }                                                           \
    lock_handle_unlock(&self->lock);                            \
    return false;                                               \
  }

#define DEFINE_FUNCTION_REMOVE_CONCURRENT_LIST_OF(TYPE)         \
  bool CONCURRENT_LIST_OF_##TYPE##_REMOVE(                      \
      concurrent_list_of_##TYPE *const self, TYPE const data) { \
    concurrent_list_node_of_##TYPE##_t *iter, *tmp;             \
    if (self == NULL) {                                         \
      return CONCURRENT_LIST_NULL_SELF;                         \
    }                                                           \
    lock_handle_lock(&self->lock);                              \
    iter = self->front;                                         \
    while (iter) {                                              \
      tmp = iter->next;                                         \
      if (self->cmp(&iter->data, &data)) {                      \
        if (iter->prev) {                                       \
          iter->prev->next = iter->next;                        \
        }                                                       \
        if (iter->next) {                                       \
          iter->next->prev = iter->prev;                        \
        }                                                       \
        if (iter == self->front) {                              \
          self->front = iter->next;                             \
        } else if (iter == self->back) {                        \
          self->back = iter->prev;                              \
        }                                                       \
        free(iter);                                             \
        self->size--;                                           \
        if (self->size == 0) {                                  \
          self->front = NULL;                                   \
          self->back = NULL;                                    \
        }                                                       \
      }                                                         \
      iter = tmp;                                               \
    }                                                           \
    lock_handle_unlock(&self->lock);                            \
    return CONCURRENT_LIST_SUCCESS;                             \
  }

#define DEFINE_CONCURRENT_LIST_OF(TYPE)               \
  DEFINE_VTABLE_CONCURRENT_LIST_OF(TYPE)              \
  DEFINE_FUNCTION_INIT_CONCURRENT_LIST_OF(TYPE)       \
  DEFINE_FUNCTION_DESTROY_CONCURRENT_LIST_OF(TYPE)    \
  DEFINE_FUNCTION_FRONT_CONCURRENT_LIST_OF(TYPE)      \
  DEFINE_FUNCTION_BACK_CONCURRENT_LIST_OF(TYPE)       \
  DEFINE_FUNCTION_EMPTY_CONCURRENT_LIST_OF(TYPE)      \
  DEFINE_FUNCTION_SIZE_CONCURRENT_LIST_OF(TYPE)       \
  DEFINE_FUNCTION_PUSH_FRONT_CONCURRENT_LIST_OF(TYPE) \
  DEFINE_FUNCTION_PUSH_BACK_CONCURRENT_LIST_OF(TYPE)  \
  DEFINE_FUNCTION_POP_FRONT_CONCURRENT_LIST_OF(TYPE)  \
  DEFINE_FUNCTION_POP_BACK_CONCURRENT_LIST_OF(TYPE)   \
  DEFINE_FUNCTION_CONTAINS_CONCURRENT_LIST_OF(TYPE)   \
  DEFINE_FUNCTION_REMOVE_CONCURRENT_LIST_OF(TYPE)
