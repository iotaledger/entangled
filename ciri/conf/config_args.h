/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_ARGS_H__
#define __CIRI_CONF_ARGS_H__

char* short_options = "u:t:";
struct option long_options[] = {                                                                    
    {"udp-receiver-port", required_argument, 0, 'u'},
    {"tcp-receiver-port", required_argument, 0, 't'},
};

#endif // __CIRI_CONF_ARGS_H__
