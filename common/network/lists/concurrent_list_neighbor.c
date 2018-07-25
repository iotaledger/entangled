#include <string.h>

#include "common/network/lists/concurrent_list_neighbor.h"

DEFINE_LIST_OF(neighbor_t);

bool cmp_neighbor(neighbor_t const *const lhs, neighbor_t const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }
  return strcmp(lhs->endpoint.host, rhs->endpoint.host) == 0 &&
         lhs->endpoint.port == rhs->endpoint.port;
}

bool add_neighbor(neighbors_list_t *const neighbors, char const *const host,
                  uint16_t const port) {
  neighbor_t neighbor;
  strcpy(neighbor.endpoint.host, host);
  neighbor.endpoint.port = port;
  if (neighbors->vtable->contain(neighbors, neighbor) == true) {
    return false;
  }
  return neighbors->vtable->push_back(neighbors, neighbor) ==
         CONCURRENT_LIST_SUCCESS;
}

bool remove_neighbor(neighbors_list_t *const neighbors, char const *const host,
                     uint16_t const port) {
  neighbor_t neighbor;
  strcpy(neighbor.endpoint.host, host);
  neighbor.endpoint.port = port;
  return neighbors->vtable->remove(neighbors, neighbor) ==
         CONCURRENT_LIST_SUCCESS;
}
