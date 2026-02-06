#pragma once

#include "common.h"
#include <stdbool.h>

#define OPT_CONFIG "config"
#define OPT_SHORT_CONFIG 'c'

#define OPT_HOST "host"
#define OPT_PORT "port"
#define OPT_SOCKET "socket"
#define OPT_CALL "call"
#define OPT_SSID "ssid"
#define OPT_VERBOSE "verbose"

#define OPT_VAL_LOG_STANDARD "standard"
#define OPT_VAL_LOG_VERBOSE "verbose"
#define OPT_VAL_LOG_DEBUG "debug"

#define OPT_DRY_RUN "dry-run"
#define OPT_SHORT_DRY_RUN 'n'

#define OPT_SHORT_HOST 'h'
#define OPT_SHORT_PORT 'p'
#define OPT_SHORT_SOCKET 'x'
#define OPT_SHORT_CALL 'C'
#define OPT_SHORT_SSID 's'
#define OPT_SHORT_VERBOSE 'v'
#define OPT_SHORT_DEBUG 'V'

#define OPT_STR_SIZE 256
#define OPT_MAX_ALIASES 32

typedef struct
{
    char name[8];
    int hops;
    bool traced;
} opts_alias_t;

typedef struct options
{
    char config_file[OPT_STR_SIZE];
    char host[64];
    int port;
    char socket[OPT_STR_SIZE];

    char call[8];
    int ssid;

    log_level_e log_level;
    bool dry_run;
} options_t;

// Clears out options_t setting null/zero values
void opts_init(options_t *opts);

// Parses command line arguments to options_t, OVERWRITING ALL VALUES
void opts_parse_args(options_t *opts, int argc, char *argv[]);

// Parses file to options_t, overwriting only null/zero/empty values
void opts_parse_conf_file(options_t *opts, const char *filename);

// Applies default values, overwriting only null/zero/empty values
void opts_defaults(options_t *opts);
