/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file list.h
\brief List template.
*/
#ifndef __MAM_V2_LIST_H__
#define __MAM_V2_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#define def_mam_list_node(info_type, node_name) \
  typedef struct _##node_name {                 \
    info_type info;                             \
    struct _##node_name *prev;                  \
    struct _##node_name *next;                  \
  } node_name

#define def_mam_list(node_name, list_name) \
  typedef struct _##list_name {            \
    node_name *begin, *end;                \
  } list_name

#define mam_list_node_insert_next(_this, _new) \
  do {                                         \
    MAM2_ASSERT(_this);                        \
    MAM2_ASSERT(_new);                         \
    _new->next = _this->next;                  \
    _new->prev = _this;                        \
    if (_this->next) _this->next->prev = _new; \
    _this->next = _new;                        \
  } while (0)

#define mam_list_node_insert_prev(_this, _new) \
  do {                                         \
    MAM2_ASSERT(_this);                        \
    MAM2_ASSERT(_new);                         \
    _new->prev = _this->prev;                  \
    _new->next = _this;                        \
    if (_this->prev) _this->prev->next = _new; \
    _this->prev = _new;                        \
  } while (0)

#define mam_list_node_remove_this(_this)              \
  do {                                                \
    MAM2_ASSERT(_this);                               \
    if (_this->next) _this->next->prev = _this->prev; \
    if (_this->prev) _this->prev->next = _this->next; \
  } while (0)

#define mam_list_insert_end(list, node)          \
  do {                                           \
    if (0 == list.begin) {                       \
      MAM2_ASSERT(0 == list.end);                \
      list.begin = node;                         \
    } else {                                     \
      MAM2_ASSERT(0 != list.end);                \
      mam_list_node_insert_next(list.end, node); \
    }                                            \
    list.end = node;                             \
  } while (0)

#define mam_list_remove(list, node)       \
  do {                                    \
    MAM2_ASSERT(node);                    \
    if (node == list.begin) {             \
      MAM2_ASSERT(0 == node->prev);       \
      list.begin = node->next;            \
    }                                     \
    if (node == list.end) {               \
      MAM2_ASSERT(0 == node->next);       \
      list.end = node->prev;              \
    }                                     \
    mam / v2_list_node_remove_this(node); \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_LIST_H__
