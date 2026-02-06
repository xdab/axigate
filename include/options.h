#pragma once

#include <stdbool.h>

#define OPT_VERBOSE "verbose"
#define OPT_NUMBER "number"
#define OPT_STRING "string"

#define OPT_SHORT_VERBOSE 'v'
#define OPT_SHORT_NUMBER 'n'
#define OPT_SHORT_STRING 's'

#define OPT_STR_SIZE 256

typedef struct options
{
    bool verbose;
    int number;
    char string[OPT_STR_SIZE];
} options_t;

// Clears out options_t setting null/zero values
void opts_init(options_t *opts);

// Parses command line arguments to options_t, OVERWRITING ALL VALUES
void opts_parse_args(options_t *opts, int argc, char *argv[]);

// Applies default values, overwriting only null/zero/empty values
void opts_defaults(options_t *opts);
