#ifndef __MERKLE_H__
#define __MERKLE_H__

#include "common/curl-p/trit.h"

#define NULL_SIBLING 1
#define NULL_TREE 2
#define LEAF_INDEX_OUT_OF_BOUNDS 3
#define DEPTH_OUT_OF_BOUNDS 4

size_t merkle_size(size_t leaf_count);
size_t merkle_depth(size_t node_count);
size_t merkle_node_index(size_t depth, size_t width, size_t tree_depth);

int merkle_create(trit_t *const tree, trit_t *const seed, int64_t offset,
                  const size_t base_size, size_t security, Curl *const c);
int merkle_branch(trit_t *sibling, trit_t *const merkle_tree,
                  size_t tree_length, size_t tree_depth, size_t leaf_index);
int merkle_root(trit_t *hash, trit_t *siblings, size_t siblings_number,
                size_t index, Curl *c);

#endif  //__MERKLE_H__
