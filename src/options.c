#include <string.h>
#include "options.h"

void opts_init(options_t *opts)
{
    nonnull(opts, "opts");

    opts->config_file[0] = '\0';
    opts->host[0] = '\0';
    opts->port = 0;
    opts->socket[0] = '\0';

    opts->is_host[0] = '\0';
    opts->is_port = 0;
    opts->is_filter[0] = '\0';

    opts->call[0] = '\0';
    opts->ssid = 0;

    opts->log_level = LOG_LEVEL_STANDARD;
    opts->dry_run = false;
}

void opts_defaults(options_t *opts)
{
    nonnull(opts, "opts");

    REPLACE_IF_a_WITH_b(opts->port, 0, 8144);
    REPLACE_IF_a_WITH_b(opts->is_port, 0, 14580);
    REPLACE_IF_a_WITH_b(opts->is_passcode, 0, OPT_DEFAULT_IS_PASSCODE);

    if (opts->is_host[0] == '\0')
        strncpy(opts->is_host, "rotate.aprs2.net", sizeof(opts->is_host) - 1);
}
