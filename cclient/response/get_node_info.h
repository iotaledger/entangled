/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup response
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_RESPONSE_GET_NODE_INFO_H
#define CCLIENT_RESPONSE_GET_NODE_INFO_H

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utarray.h"
#include "utils/char_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get node info response.
 *
 */
typedef struct get_node_info_res_s {
  /**
   * Name of the IOTA software you're currently using.
   */
  char_buffer_t* app_name;
  /**
   * The version of the IOTA software you're currently running.
   */
  char_buffer_t* app_version;
  /**
   * Current UNIX timestamp.
   */
  uint64_t time;
  /**
   * Index of the latest milestone.
   */
  uint32_t latest_milestone_index;
  /**
   * Index of the latest solid subtangle.
   */
  uint32_t latest_solid_subtangle_milestone_index;
  /**
   * The start index of the milestones.
   * This index is encoded in each milestone transaction by the coordinator
   */
  uint32_t milestone_start_index;
  /**
   * Number of tips in the network.
   */
  uint32_t tips;
  /**
   * Transactions to request during syncing process.
   */
  uint32_t transactions_to_request;
  /**
   * Number of neighbors you are directly connected with.
   */
  uint16_t neighbors;
  /**
   * Packets which are currently queued up.
   */
  uint16_t packets_queue_size;
  /**
   * The hash of the latest transaction that was signed off by the coordinator.
   */
  flex_trit_t latest_milestone[FLEX_TRIT_SIZE_243];
  /**
   * The hash of the latest transaction which is solid and is used for sending transactions. For a milestone to become
   * solid, your local node must approve the subtangle of coordinator-approved transactions, and have a consistent view
   * of all referenced transactions.
   */
  flex_trit_t latest_solid_subtangle_milestone[FLEX_TRIT_SIZE_243];
  /*
   * Enabled features
   */
  UT_array* features;
  /**
   * The address of the coordinator being followed by this node.
   */
  flex_trit_t coordinator_address[FLEX_TRIT_SIZE_243];
} get_node_info_res_t;

/**
 * @brief Allocates a get node info response object.
 *
 * @return A pointer to the response object.
 */
get_node_info_res_t* get_node_info_res_new();

/**
 * @brief Frees a get node info response object.
 *
 * @param[in] res The response object.
 */
void get_node_info_res_free(get_node_info_res_t** res);

/**
 * @brief Sets application name to the response.
 *
 * @param[in] res The response object.
 * @param[in] name A string of the application name.
 * @return #retcode_t
 */
static inline retcode_t get_node_info_res_app_name_set(get_node_info_res_t* const res, char const* const name) {
  if (!res || !name) {
    return RC_NULL_PARAM;
  }
  return char_buffer_set(res->app_name, name);
}

/**
 * @brief Gets the application name
 *
 * @param[in] res The response object.
 * @return A pointer to the application name.
 */
static inline char const* get_node_info_res_app_name(get_node_info_res_t const* const res) {
  if (!res) {
    return NULL;
  }

  if (!res->app_name) {
    return NULL;
  }

  return res->app_name->data;
}

/**
 * @brief Sets the application version
 *
 * @param[in] res The response object.
 * @param[in] version  A string of the application version.
 * @return #retcode_t
 */
static inline retcode_t get_node_info_res_app_version_set(get_node_info_res_t* const res, char const* const version) {
  if (!res || !version) {
    return RC_NULL_PARAM;
  }

  return char_buffer_set(res->app_version, version);
}

/**
 * @brief Gets the application version.
 *
 * @param[in] res The response object.
 * @return A pointer to the application version.
 */
static inline char const* get_node_info_res_app_version(get_node_info_res_t const* const res) {
  if (!res) {
    return NULL;
  }

  if (!res->app_version) {
    return NULL;
  }
  return res->app_version->data;
}

/**
 * @brief Sets the latest milestone hash.
 *
 * @param[in] res The response object
 * @param[in] hash A milestone hash.
 * @return #retcode_t
 */
static inline retcode_t get_node_info_res_lm_set(get_node_info_res_t* const res, flex_trit_t const* const hash) {
  if (!res || !hash) {
    return RC_NULL_PARAM;
  }
  memcpy(res->latest_milestone, hash, FLEX_TRIT_SIZE_243);
  return RC_OK;
}

/**
 * @brief Gets the latest milestone hash.
 *
 * @param[in] res The response object.
 * @return A pointer to the hash of latest milestone.
 */
static inline flex_trit_t const* get_node_info_res_lm(get_node_info_res_t const* const res) {
  if (!res) {
    return NULL;
  }
  return res->latest_milestone;
}

/**
 * @brief Sets the latest solid subtangle milestone hash.
 *
 * @param[in] res The response object.
 * @param[in] hash A pointer to the hash of latest solid subtangle milestone.
 * @return #retcode_t
 */
static inline retcode_t get_node_info_res_lssm_set(get_node_info_res_t* const res, flex_trit_t const* const hash) {
  if (!res || !hash) {
    return RC_NULL_PARAM;
  }
  memcpy(res->latest_solid_subtangle_milestone, hash, FLEX_TRIT_SIZE_243);
  return RC_OK;
}

/**
 * @brief Gets the latest solid subtangle milestone hash.
 *
 * @param[in] res The response object.
 * @return A pointer to the hash of latest solid subtangle milestone.
 */
static inline flex_trit_t const* get_node_info_res_lssm(get_node_info_res_t const* const res) {
  if (!res) {
    return NULL;
  }
  return res->latest_solid_subtangle_milestone;
}

/**
 * @brief Sets the coordinator address
 *
 * @param[in] res The response object.
 * @param[in] hash A coordinator hash.
 * @return #retcode_t
 */
static inline retcode_t get_node_info_res_coordinator_address_set(get_node_info_res_t* const res,
                                                                  flex_trit_t const* const hash) {
  if (!res || !hash) {
    return RC_NULL_PARAM;
  }
  memcpy(res->coordinator_address, hash, FLEX_TRIT_SIZE_243);
  return RC_OK;
}
static inline flex_trit_t const* get_node_info_res_coordinator_address(get_node_info_res_t const* const res) {
  if (!res) {
    return NULL;
  }
  return res->coordinator_address;
}
static inline char const* get_node_info_res_features_at(get_node_info_res_t* res, size_t idx) {
  if (!res) {
    return NULL;
  }
  return *(const char**)utarray_eltptr(res->features, idx);
}
static inline size_t get_node_info_req_features_len(get_node_info_res_t* res) {
  if (!res) {
    return 0;
  }
  return utarray_len(res->features);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NODE_INFO_H

/** @} */
