/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_CONF_ARGS_H__
#define __CIRI_CONF_CONF_ARGS_H__

typedef enum ciri_arg_requirement_e {
  NO_ARG,
  REQUIRED_ARG,
  OPTIONAL_ARG
} ciri_arg_requirement_t;

static struct ciri_argument_s {
  char* long_option;
  char short_option;
  char* desc;
  ciri_arg_requirement_t status;
} ciri_arguments_g[] = {
    {"--port", 'p',
     "This is a mandatory option that defines the port to be used to send API "
     "commands to your node",
     REQUIRED_ARG},
    {"--neighbors", 'n',
     "Neighbors that you are connected with will be added via this option",
     REQUIRED_ARG},
    {"--config", 'c',
     "Config INI file that can be used instead of CLI options. See more "
     "below",
     REQUIRED_ARG},
    {"--udp-receiver-port", 'u', "UDP receiver port", REQUIRED_ARG},
    {"--tcp-receiver-port", 't', "TCP receiver port", REQUIRED_ARG},
    {"--testnet", 'e', "Makes it possible to run IRI with the IOTA testnet",
     NO_ARG},
    {"--remote", 'r', "Remotely access your node and send API commands",
     NO_ARG},
    {"--remote-auth", 'a',
     "Require authentication password for accessing remotely. Requires a "
     "correct username:hashedpassword combination",
     REQUIRED_ARG},
    {"--remote-limit-api", 'l',
     "Exclude certain API calls from being able to be accessed remotely",
     REQUIRED_ARG},
    {"--send-limit", 's',
     "Limit the outbound bandwidth consumption. Limit is set to mbit/s",
     REQUIRED_ARG},
    {"--max-peers", 'm',
     " Limit the number of max accepted peers. Default is set to 0 (mutual "
     "tethering)",
     REQUIRED_ARG},
    {"--dns-resolution-false", 'd', "Ignores DNS resolution refreshing",
     NO_ARG},
    {NULL, 0, NULL, NO_ARG}};

static char* short_options = "u:t:";
static struct option long_options[] = {
    {"udp-receiver-port", required_argument, 0, 'u'},
    {"tcp-receiver-port", required_argument, 0, 't'},
};

#endif  // __CIRI_CONF_CONF_ARGS_H__
