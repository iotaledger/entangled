/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_CONFIG_H__
#define __CIRI_CONF_CONFIG_H__

#include <stdint.h>

typedef struct ciri_config_s {
    uint16_t tcp_port;
    uint16_t udp_port;
    // add other config variables here
} ciri_config_t;

#endif // __CIRI_CONF_CONFIG_H__
