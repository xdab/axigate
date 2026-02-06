#include "options.h"

void opts_init(options_t *opts)
{
    if (!opts)
        return;

    opts->verbose = false;
    opts->number = 0;
    opts->string[0] = '\0';
}

void opts_defaults(options_t *opts)
{
    if (!opts)
        return;

    if (opts->number == 0)
        opts->number = 42;
}
