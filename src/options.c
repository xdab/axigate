#include "options.h"

void opts_init(options_t *opts)
{
    nonnull(opts, "opts");

    opts->config_file[0] = '\0';
    opts->host[0] = '\0';
    opts->port = 0;
    opts->socket[0] = '\0';

    opts->call[0] = '\0';
    opts->ssid = 0;

    opts->log_level = LOG_LEVEL_STANDARD;
    opts->dry_run = false;
}

void opts_defaults(options_t *opts)
{
    nonnull(opts, "opts");

    REPLACE_IF_a_WITH_b(opts->port, 0, 8144);
}
