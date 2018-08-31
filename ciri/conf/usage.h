/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_CONF_ARGS_H__
#define __CIRI_CONF_CONF_ARGS_H__

#include <stdlib.h>

typedef enum ciri_arg_requirement_e {
  NO_ARG,
  REQUIRED_ARG,
  OPTIONAL_ARG
} ciri_arg_requirement_t;

static struct ciri_argument_s {
  char* name;
  char val;
  char* desc;
  ciri_arg_requirement_t has_arg;
} ciri_arguments_g[] = {
    {"help", 'h', "Displays this usage", NO_ARG},
    {"port", 'p',
     "This is a mandatory option that defines the port to be used to send API "
     "commands to your node",
     REQUIRED_ARG},
    {"neighbors", 'n',
     "Neighbors that you are connected with will be added via this option",
     REQUIRED_ARG},
    {"config", 'c',
     "Config INI file that can be used instead of CLI options. See more "
     "below",
     REQUIRED_ARG},
    {"udp-receiver-port", 'u', "UDP receiver port", REQUIRED_ARG},
    {"tcp-receiver-port", 't', "TCP receiver port", REQUIRED_ARG},
    {"testnet", 'e', "Makes it possible to run IRI with the IOTA testnet",
     NO_ARG},
    {"remote", 'r', "Remotely access your node and send API commands", NO_ARG},
    {"remote-auth", 'a',
     "Requires authentication password for accessing remotely. Requires a "
     "correct username:hashedpassword combination",
     REQUIRED_ARG},
    {"remote-limit-api", 'l',
     "Excludes certain API calls from being able to be accessed remotely",
     REQUIRED_ARG},
    {"send-limit", 's',
     "Limits the outbound bandwidth consumption. Limit is set to mbit/s",
     REQUIRED_ARG},
    {"max-peers", 'm',
     "Limits the number of max accepted peers. Default is set to 0 (mutual "
     "tethering)",
     REQUIRED_ARG},
    {"dns-resolution-false", 'd', "Ignores DNS resolution refreshing", NO_ARG},
    {NULL, 0, NULL, NO_ARG}};

static char* short_options = "hp:n:c:u:t:era:l:s:m:d";

#ifdef __cplusplus
extern "C" {
#endif

void iota_usage();

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_CONF_CONF_ARGS_H__
