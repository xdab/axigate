#pragma once

#include "common.h"
#include <stdbool.h>

#define OPT_CONFIG "config"
#define OPT_SHORT_CONFIG 'c'

#define OPT_HOST "host"
#define OPT_PORT "port"
#define OPT_SOCKET "socket"
#define OPT_IS_HOST "is-host"
#define OPT_IS_PORT "is-port"
#define OPT_IS_FILTER "is-filter"
#define OPT_IS_PASSCODE "is-passcode"
#define OPT_DEFAULT_IS_PASSCODE (-1)
#define OPT_IS_KEEPALIVE "is-keepalive"
#define OPT_DEFAULT_IS_KEEPALIVE 300
#define OPT_IS_KEEPALIVE_UNSET (-1)
#define OPT_UDP_KISS_LISTEN "udp-kiss-listen"
#define OPT_UDP_TNC2_LISTEN "udp-tnc2-listen"
#define OPT_CALL "call"
#define OPT_SSID "ssid"
#define OPT_VERBOSE "verbose"
#define OPT_DEBUG "debug"

#define OPT_VAL_LOG_STANDARD "standard"
#define OPT_VAL_LOG_VERBOSE "verbose"
#define OPT_VAL_LOG_DEBUG "debug"

#define OPT_RF_TO_IS "rf-to-is"
#define OPT_SHORT_RF_TO_IS 'r'
#define OPT_IS_TO_RF "is-to-rf"
#define OPT_SHORT_IS_TO_RF 'i'

#define OPT_SHORT_HOST 'h'
#define OPT_SHORT_PORT 'p'
#define OPT_SHORT_SOCKET 'x'
#define OPT_SHORT_CALL 'C'
#define OPT_SHORT_SSID 's'
#define OPT_SHORT_VERBOSE 'v'
#define OPT_SHORT_DEBUG 'V'
#define OPT_SHORT_IS_HOST 1001
#define OPT_SHORT_IS_PORT 1002
#define OPT_SHORT_IS_FILTER 1003
#define OPT_SHORT_IS_PASSCODE 1004
#define OPT_SHORT_UDP_KISS_LISTEN 1005
#define OPT_SHORT_UDP_TNC2_LISTEN 1006
#define OPT_SHORT_IS_KEEPALIVE 1007

typedef struct
{
    char name[8];
    int hops;
    bool traced;
} opts_alias_t;

typedef struct options
{
    char config_file[256];
    char host[64];
    int port;
    char socket[256];

    char is_host[64];
    int is_port;
    char is_filter[64];
    int is_passcode;
    int is_keepalive;

    int udp_kiss_port;
    int udp_tnc2_port;

    char call[8];
    int ssid;

    log_level_e log_level;
    bool rf_to_is;
    bool is_to_rf;
} options_t;

// Clears out options_t setting null/zero values
void opts_init(options_t *opts);

// Parses command line arguments to options_t, OVERWRITING ALL VALUES
void opts_parse_args(options_t *opts, int argc, char *argv[]);

// Parses file to options_t, overwriting only null/zero/empty values
void opts_parse_conf_file(options_t *opts, const char *filename);

// Applies default values, overwriting only null/zero/empty values
void opts_defaults(options_t *opts);
