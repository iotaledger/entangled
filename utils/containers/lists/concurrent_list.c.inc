/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#define DEFINE_CL_VTABLE(TYPE)                                              \
  concurrent_list_vtable_##TYPE const concurrent_list_vtable_##TYPE##_g = { \
      CL_FRONT_##TYPE,     CL_BACK_##TYPE,       CL_EMPTY_##TYPE,           \
      CL_SIZE_##TYPE,      CL_PUSH_FRONT_##TYPE, CL_PUSH_BACK_##TYPE,       \
      CL_POP_FRONT_##TYPE, CL_POP_BACK_##TYPE,   CL_FIND_##TYPE,            \
      CL_CONTAINS_##TYPE,  CL_REMOVE_##TYPE};

#define DEFINE_CL_INIT(TYPE)                                                  \
  concurrent_list_status_t CL_INIT_##TYPE(                                    \
      concurrent_list_##TYPE **const self,                                    \
      bool (*cmp)(TYPE const *const self, TYPE const *const data)) {          \
    if (self == NULL) {                                                       \
      return CL_NULL_SELF;                                                    \
    }                                                                         \
    if (cmp == NULL) {                                                        \
      return CL_NULL_CMP_FUNC;                                                \
    }                                                                         \
    *self = (concurrent_list_##TYPE *)malloc(sizeof(concurrent_list_##TYPE)); \
    if (*self == NULL) {                                                      \
      return CL_ALLOC_FAILED;                                                 \
    }                                                                         \
    (*self)->vtable = &concurrent_list_vtable_##TYPE##_g;                     \
    (*self)->cmp = cmp;                                                       \
    (*self)->front = NULL;                                                    \
    (*self)->back = NULL;                                                     \
    (*self)->size = 0;                                                        \
    lock_handle_init(&(*self)->lock);                                         \
    return CL_SUCCESS;                                                        \
  }

#define DEFINE_CL_DESTROY(TYPE)               \
  concurrent_list_status_t CL_DESTROY_##TYPE( \
      concurrent_list_##TYPE **const self) {  \
    concurrent_list_node_##TYPE *front, *tmp; \
    if (self == NULL || *self == NULL) {      \
      return CL_NULL_SELF;                    \
    }                                         \
    front = (*self)->front;                   \
    while (front) {                           \
      tmp = front;                            \
      front = front->next;                    \
      free(tmp);                              \
    }                                         \
    lock_handle_destroy(&(*self)->lock);      \
    free(*self);                              \
    *self = NULL;                             \
    return CL_SUCCESS;                        \
  }

#define DEFINE_CL_FRONT(TYPE)                                 \
  TYPE *CL_FRONT_##TYPE(concurrent_list_##TYPE *const self) { \
    TYPE *data = NULL;                                        \
    if (self == NULL) {                                       \
      return NULL;                                            \
    }                                                         \
    lock_handle_lock(&self->lock);                            \
    if (self->front) {                                        \
      data = &self->front->data;                              \
    }                                                         \
    lock_handle_unlock(&self->lock);                          \
    return data;                                              \
  }

#define DEFINE_CL_BACK(TYPE)                                 \
  TYPE *CL_BACK_##TYPE(concurrent_list_##TYPE *const self) { \
    TYPE *data = NULL;                                       \
    if (self == NULL) {                                      \
      return NULL;                                           \
    }                                                        \
    lock_handle_lock(&self->lock);                           \
    if (self->back) {                                        \
      data = &self->back->data;                              \
    }                                                        \
    lock_handle_unlock(&self->lock);                         \
    return data;                                             \
  }

#define DEFINE_CL_EMPTY(TYPE)                                \
  bool CL_EMPTY_##TYPE(concurrent_list_##TYPE *const self) { \
    size_t size;                                             \
    if (self == NULL) {                                      \
      return true;                                           \
    }                                                        \
    lock_handle_lock(&self->lock);                           \
    size = self->size;                                       \
    lock_handle_unlock(&self->lock);                         \
    return size == 0;                                        \
  }

#define DEFINE_CL_SIZE(TYPE)                                  \
  size_t CL_SIZE_##TYPE(concurrent_list_##TYPE *const self) { \
    size_t size;                                              \
    if (self == NULL) {                                       \
      return 0;                                               \
    }                                                         \
    lock_handle_lock(&self->lock);                            \
    size = self->size;                                        \
    lock_handle_unlock(&self->lock);                          \
    return size;                                              \
  }

#define DEFINE_CL_PUSH_FRONT(TYPE)                           \
  concurrent_list_status_t CL_PUSH_FRONT_##TYPE(             \
      concurrent_list_##TYPE *const self, TYPE const data) { \
    concurrent_list_node_##TYPE *node;                       \
    if (self == NULL) {                                      \
      return CL_NULL_SELF;                                   \
    }                                                        \
    node = (concurrent_list_node_##TYPE *)calloc(            \
        1, sizeof(concurrent_list_node_##TYPE));             \
    if (node == NULL) {                                      \
      return CL_ALLOC_FAILED;                                \
    }                                                        \
    node->data = data;                                       \
    lock_handle_lock(&self->lock);                           \
    if (self->size == 0) {                                   \
      self->back = node;                                     \
    } else {                                                 \
      self->front->prev = node;                              \
      node->next = self->front;                              \
    }                                                        \
    self->front = node;                                      \
    self->size++;                                            \
    lock_handle_unlock(&self->lock);                         \
    return CL_SUCCESS;                                       \
  }

#define DEFINE_CL_PUSH_BACK(TYPE)                            \
  concurrent_list_status_t CL_PUSH_BACK_##TYPE(              \
      concurrent_list_##TYPE *const self, TYPE const data) { \
    concurrent_list_node_##TYPE *node;                       \
    if (self == NULL) {                                      \
      return CL_NULL_SELF;                                   \
    }                                                        \
    node = (concurrent_list_node_##TYPE *)calloc(            \
        1, sizeof(concurrent_list_node_##TYPE));             \
    if (node == NULL) {                                      \
      return CL_ALLOC_FAILED;                                \
    }                                                        \
    node->data = data;                                       \
    lock_handle_lock(&self->lock);                           \
    if (self->size == 0) {                                   \
      self->front = node;                                    \
    } else {                                                 \
      self->back->next = node;                               \
      node->prev = self->back;                               \
    }                                                        \
    self->back = node;                                       \
    self->size++;                                            \
    lock_handle_unlock(&self->lock);                         \
    return CL_SUCCESS;                                       \
  }

#define DEFINE_CL_POP_FRONT(TYPE)                             \
  concurrent_list_status_t CL_POP_FRONT_##TYPE(               \
      concurrent_list_##TYPE *const self, TYPE *const data) { \
    concurrent_list_node_##TYPE *tmp;                         \
    if (self == NULL) {                                       \
      return CL_NULL_SELF;                                    \
    }                                                         \
    if (data == NULL) {                                       \
      return CL_NULL_DATA;                                    \
    }                                                         \
    lock_handle_lock(&self->lock);                            \
    if (self->size == 0) {                                    \
      lock_handle_unlock(&self->lock);                        \
      return CL_EMPTY;                                        \
    }                                                         \
    tmp = self->front;                                        \
    *data = tmp->data;                                        \
    self->front = self->front->next;                          \
    if (self->front == NULL) {                                \
      self->back = NULL;                                      \
    }                                                         \
    self->size--;                                             \
    free(tmp);                                                \
    lock_handle_unlock(&self->lock);                          \
    return CL_SUCCESS;                                        \
  }

#define DEFINE_CL_POP_BACK(TYPE)                              \
  concurrent_list_status_t CL_POP_BACK_##TYPE(                \
      concurrent_list_##TYPE *const self, TYPE *const data) { \
    concurrent_list_node_##TYPE *tmp;                         \
    if (self == NULL) {                                       \
      return CL_NULL_SELF;                                    \
    }                                                         \
    if (data == NULL) {                                       \
      return CL_NULL_DATA;                                    \
    }                                                         \
    lock_handle_lock(&self->lock);                            \
    if (self->size == 0) {                                    \
      lock_handle_unlock(&self->lock);                        \
      return CL_EMPTY;                                        \
    }                                                         \
    tmp = self->front;                                        \
    *data = tmp->data;                                        \
    self->back = self->back->prev;                            \
    if (self->back == NULL) {                                 \
      self->front = NULL;                                     \
    }                                                         \
    self->size--;                                             \
    free(tmp);                                                \
    lock_handle_unlock(&self->lock);                          \
    return CL_SUCCESS;                                        \
  }

#define DEFINE_CL_FIND(TYPE)                                                  \
  TYPE *CL_FIND_##TYPE(concurrent_list_##TYPE *const self, TYPE const data) { \
    if (self == NULL) {                                                       \
      return NULL;                                                            \
    }                                                                         \
    concurrent_list_node_##TYPE *iter;                                        \
    lock_handle_lock(&self->lock);                                            \
    iter = self->front;                                                       \
    while (iter) {                                                            \
      if (self->cmp(&iter->data, &data)) {                                    \
        lock_handle_unlock(&self->lock);                                      \
        return &iter->data;                                                   \
      }                                                                       \
      iter = iter->next;                                                      \
    }                                                                         \
    lock_handle_unlock(&self->lock);                                          \
    return NULL;                                                              \
  }

#define DEFINE_CL_CONTAINS(TYPE)                              \
  bool CL_CONTAINS_##TYPE(concurrent_list_##TYPE *const self, \
                          TYPE const data) {                  \
    if (self == NULL) {                                       \
      return false;                                           \
    }                                                         \
    return self->vtable->find(self, data) != NULL;            \
  }

#define DEFINE_CL_REMOVE(TYPE)                                                 \
  bool CL_REMOVE_##TYPE(concurrent_list_##TYPE *const self, TYPE const data) { \
    concurrent_list_node_##TYPE *iter, *tmp;                                   \
    if (self == NULL) {                                                        \
      return CL_NULL_SELF;                                                     \
    }                                                                          \
    lock_handle_lock(&self->lock);                                             \
    iter = self->front;                                                        \
    while (iter) {                                                             \
      tmp = iter->next;                                                        \
      if (self->cmp(&iter->data, &data)) {                                     \
        if (iter->prev) {                                                      \
          iter->prev->next = iter->next;                                       \
        }                                                                      \
        if (iter->next) {                                                      \
          iter->next->prev = iter->prev;                                       \
        }                                                                      \
        if (iter == self->front) {                                             \
          self->front = iter->next;                                            \
        } else if (iter == self->back) {                                       \
          self->back = iter->prev;                                             \
        }                                                                      \
        free(iter);                                                            \
        self->size--;                                                          \
        if (self->size == 0) {                                                 \
          self->front = NULL;                                                  \
          self->back = NULL;                                                   \
        }                                                                      \
      }                                                                        \
      iter = tmp;                                                              \
    }                                                                          \
    lock_handle_unlock(&self->lock);                                           \
    return CL_SUCCESS;                                                         \
  }

#define DEFINE_CL(TYPE)      \
  DEFINE_CL_VTABLE(TYPE)     \
  DEFINE_CL_INIT(TYPE)       \
  DEFINE_CL_DESTROY(TYPE)    \
  DEFINE_CL_FRONT(TYPE)      \
  DEFINE_CL_BACK(TYPE)       \
  DEFINE_CL_EMPTY(TYPE)      \
  DEFINE_CL_SIZE(TYPE)       \
  DEFINE_CL_PUSH_FRONT(TYPE) \
  DEFINE_CL_PUSH_BACK(TYPE)  \
  DEFINE_CL_POP_FRONT(TYPE)  \
  DEFINE_CL_POP_BACK(TYPE)   \
  DEFINE_CL_FIND(TYPE)       \
  DEFINE_CL_CONTAINS(TYPE)   \
  DEFINE_CL_REMOVE(TYPE)
