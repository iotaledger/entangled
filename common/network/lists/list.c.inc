/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#define DEFINE_VTABLE_LIST_OF(TYPE)                             \
  vtable_list_of_##TYPE##_t const vtable_list_of_##TYPE##_g = { \
      LIST_OF_##TYPE##_FRONT,      LIST_OF_##TYPE##_BACK,       \
      LIST_OF_##TYPE##_EMPTY,      LIST_OF_##TYPE##_SIZE,       \
      LIST_OF_##TYPE##_PUSH_FRONT, LIST_OF_##TYPE##_PUSH_BACK,  \
      LIST_OF_##TYPE##_POP_FRONT,  LIST_OF_##TYPE##_POP_BACK};

#define DEFINE_FUNCTION_INIT_LIST_OF(TYPE)                           \
  list_status_t LIST_OF_##TYPE##_INIT(list_of_##TYPE **const self) { \
    if (self == NULL) return LIST_NULL_SELF;                         \
    *self = (list_of_##TYPE *)malloc(sizeof(list_of_##TYPE));        \
    if (*self == NULL) return LIST_ALLOC_FAILED;                     \
    (*self)->vtable = &vtable_list_of_##TYPE##_g;                    \
    (*self)->front = NULL;                                           \
    (*self)->back = NULL;                                            \
    (*self)->size = 0;                                               \
    return LIST_SUCCESS;                                             \
  }

#define DEFINE_FUNCTION_DESTROY_LIST_OF(TYPE)                           \
  list_status_t LIST_OF_##TYPE##_DESTROY(list_of_##TYPE **const self) { \
    list_node_of_##TYPE##_t *front, *tmp;                               \
    if (self == NULL || *self == NULL) return LIST_NULL_SELF;           \
    front = (*self)->front;                                             \
    while (front) {                                                     \
      tmp = front;                                                      \
      front = front->next;                                              \
      free(tmp);                                                        \
    }                                                                   \
    free(*self);                                                        \
    *self = NULL;                                                       \
    return LIST_SUCCESS;                                                \
  }

#define DEFINE_FUNCTION_FRONT_LIST_OF(TYPE)                        \
  TYPE *LIST_OF_##TYPE##_FRONT(list_of_##TYPE const *const self) { \
    TYPE *data = NULL;                                             \
    if (self == NULL) return NULL;                                 \
    if (self->front) data = &self->front->data;                    \
    return data;                                                   \
  }

#define DEFINE_FUNCTION_BACK_LIST_OF(TYPE)                        \
  TYPE *LIST_OF_##TYPE##_BACK(list_of_##TYPE const *const self) { \
    TYPE *data = NULL;                                            \
    if (self == NULL) return NULL;                                \
    if (self->back) data = &self->back->data;                     \
    return data;                                                  \
  }

#define DEFINE_FUNCTION_EMPTY_LIST_OF(TYPE)                       \
  bool LIST_OF_##TYPE##_EMPTY(list_of_##TYPE const *const self) { \
    if (self == NULL) return true;                                \
    return self->size == 0;                                       \
  }

#define DEFINE_FUNCTION_SIZE_LIST_OF(TYPE)                         \
  size_t LIST_OF_##TYPE##_SIZE(list_of_##TYPE const *const self) { \
    if (self == NULL) return 0;                                    \
    return self->size;                                             \
  }

#define DEFINE_FUNCTION_PUSH_FRONT_LIST_OF(TYPE)                               \
  list_status_t LIST_OF_##TYPE##_PUSH_FRONT(list_of_##TYPE *const self,        \
                                            TYPE const data) {                 \
    list_node_of_##TYPE##_t *node;                                             \
    if (self == NULL) {                                                        \
      return LIST_NULL_SELF;                                                   \
    }                                                                          \
    node =                                                                     \
        (list_node_of_##TYPE##_t *)calloc(1, sizeof(list_node_of_##TYPE##_t)); \
    if (node == NULL) {                                                        \
      return LIST_ALLOC_FAILED;                                                \
    }                                                                          \
    node->data = data;                                                         \
    if (self->size == 0) {                                                     \
      self->back = node;                                                       \
    } else {                                                                   \
      self->front->prev = node;                                                \
      node->next = self->front;                                                \
    }                                                                          \
    self->front = node;                                                        \
    self->size++;                                                              \
    return LIST_SUCCESS;                                                       \
  }

#define DEFINE_FUNCTION_PUSH_BACK_LIST_OF(TYPE)                                \
  list_status_t LIST_OF_##TYPE##_PUSH_BACK(list_of_##TYPE *const self,         \
                                           TYPE const data) {                  \
    list_node_of_##TYPE##_t *node;                                             \
    if (self == NULL) {                                                        \
      return LIST_NULL_SELF;                                                   \
    }                                                                          \
    node =                                                                     \
        (list_node_of_##TYPE##_t *)calloc(1, sizeof(list_node_of_##TYPE##_t)); \
    if (node == NULL) {                                                        \
      return LIST_ALLOC_FAILED;                                                \
    }                                                                          \
    node->data = data;                                                         \
    if (self->size == 0) {                                                     \
      self->front = node;                                                      \
    } else {                                                                   \
      self->back->next = node;                                                 \
      node->prev = self->back;                                                 \
    }                                                                          \
    self->back = node;                                                         \
    self->size++;                                                              \
    return LIST_SUCCESS;                                                       \
  }

#define DEFINE_FUNCTION_POP_FRONT_LIST_OF(TYPE)                        \
  list_status_t LIST_OF_##TYPE##_POP_FRONT(list_of_##TYPE *const self, \
                                           TYPE *const data) {         \
    list_node_of_##TYPE##_t *tmp;                                      \
    if (self == NULL) {                                                \
      return LIST_NULL_SELF;                                           \
    }                                                                  \
    if (data == NULL) {                                                \
      return LIST_NULL_DATA;                                           \
    }                                                                  \
    if (self->size == 0) {                                             \
      return LIST_EMPTY;                                               \
    }                                                                  \
    tmp = self->front;                                                 \
    *data = tmp->data;                                                 \
    self->front = self->front->next;                                   \
    if (self->front == NULL) self->back = NULL;                        \
    self->size--;                                                      \
    free(tmp);                                                         \
    return LIST_SUCCESS;                                               \
  }

#define DEFINE_FUNCTION_POP_BACK_LIST_OF(TYPE)                        \
  list_status_t LIST_OF_##TYPE##_POP_BACK(list_of_##TYPE *const self, \
                                          TYPE *const data) {         \
    list_node_of_##TYPE##_t *tmp;                                     \
    if (self == NULL) {                                               \
      return LIST_NULL_SELF;                                          \
    }                                                                 \
    if (data == NULL) {                                               \
      return LIST_NULL_DATA;                                          \
    }                                                                 \
    if (self->size == 0) {                                            \
      return LIST_EMPTY;                                              \
    }                                                                 \
    tmp = self->front;                                                \
    *data = tmp->data;                                                \
    self->back = self->back->prev;                                    \
    if (self->back == NULL) self->front = NULL;                       \
    self->size--;                                                     \
    free(tmp);                                                        \
    return LIST_SUCCESS;                                              \
  }

#define DEFINE_LIST_OF(TYPE)               \
  DEFINE_VTABLE_LIST_OF(TYPE)              \
  DEFINE_FUNCTION_INIT_LIST_OF(TYPE)       \
  DEFINE_FUNCTION_DESTROY_LIST_OF(TYPE)    \
  DEFINE_FUNCTION_FRONT_LIST_OF(TYPE)      \
  DEFINE_FUNCTION_BACK_LIST_OF(TYPE)       \
  DEFINE_FUNCTION_EMPTY_LIST_OF(TYPE)      \
  DEFINE_FUNCTION_SIZE_LIST_OF(TYPE)       \
  DEFINE_FUNCTION_PUSH_FRONT_LIST_OF(TYPE) \
  DEFINE_FUNCTION_PUSH_BACK_LIST_OF(TYPE)  \
  DEFINE_FUNCTION_POP_FRONT_LIST_OF(TYPE)  \
  DEFINE_FUNCTION_POP_BACK_LIST_OF(TYPE)
