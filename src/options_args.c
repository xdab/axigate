#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include "options.h"

static struct argp_option options[] = {
    {OPT_VERBOSE, OPT_SHORT_VERBOSE, 0, 0, "Enable verbose output", 0},
    {OPT_NUMBER, OPT_SHORT_NUMBER, "NUM", 0, "A number (default: 42)", 0},
    {OPT_STRING, OPT_SHORT_STRING, "STR", 0, "A string", 0},
    {0, 0, 0, 0, 0, 0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    options_t *opts = state->input;

    switch (key)
    {
    case OPT_SHORT_VERBOSE:
        opts->verbose = true;
        break;
    case OPT_SHORT_NUMBER:
        opts->number = atoi(arg);
        break;
    case OPT_SHORT_STRING:
        strncpy(opts->string, arg, OPT_STR_SIZE - 1);
        opts->string[OPT_STR_SIZE - 1] = '\0';
        break;
    case ARGP_KEY_NO_ARGS:
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {
    options,
    parse_opt,
    "",
    "Template project with CLI argument parsing"};

void opts_parse_args(options_t *opts, int argc, char *argv[])
{
    if (!opts || argc == 0 || !argv)
        return;

    argp_parse(&argp, argc, argv, 0, 0, opts);
}
