/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_TEST_JSON_H
#define CCLIENT_TEST_JSON_H

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "serializer/json/json_serializer.h"

#define GET_BALANCES_SERIALIZE_ADDRESS                                         \
  "HBBYKAKTILIPVUKFOTSLHGENPTXYBNKXZFQFR9VQFWNBMTQNRVOUKPVPRNBSZVVILMAFBKOTBL" \
  "GLWLOHQ"
#define GET_BALANCES_DESERIALIZE_BALANCE "114544444"
#define GET_BALANCES_DESERIALIZE_REFERENCE                                     \
  "INRTUYSZCWBHGFGGXXPWRWBZACYAFGVRRP9VYEQJOHYD9URMELKWAFYFMNTSP9MCHLXRGAFMBO" \
  "ZPZ9999"
#define GET_BALANCES_DESERIALIZE_MILESTONEINDEX "128"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_TEST_JSON_H
