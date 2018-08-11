// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_RESPONSE_GET_NEIGHBORS_H
#define CCLIENT_RESPONSE_GET_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * IP Address of your peer.
   */
  char* address;
  /**
   * Number of all transactions sent (invalid, valid, already-seen).
   */
  int numberOfAllTransactions;
  /**
   * Invalid transactions your peer has sent you. These are transactions with
   * invalid signatures or overall schema.
   */
  int numberOfInvalidTransactions;
  /**
   * Number of newly transmitted transactions.
   */
  int numberOfNewTransactions;
} neighbor_t;

typedef struct {
  neighbor_t* const neighbors;
  size_t size;
} get_neighbors_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NEIGHBORS_H
