/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONFIGURATION_ARGS__
#define __COMMON_CONFIGURATION_ARGS__

char* short_options = "u:t:";
struct option long_options[] = {                                                                    
    {"udp-receiver-port", required_argument, 0, 'u'},
    {"tcp-receiver-port", required_argument, 0, 't'},
};

#endif // __COMMON_CONFIGURATION_ARGS__
