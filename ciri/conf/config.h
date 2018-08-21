/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONFIGURATION__
#define __COMMON_CONFIGURATION__
#include <stdint.h>

typedef struct {
    uint16_t tcp_port_g;
    uint16_t udp_port_g;
    // add other config variables here
} ciri_config_t;

#endif // __COMMON_CONFIGURATION__
