#include "options.h"
#include "common.h"
#include "conf.h"
#include <string.h>

void opts_parse_conf_file(options_t *opts, const char *filename)
{
    nonnull(opts, "opts");
    if (NULL == filename || filename[0] == '\0')
        return;

    conf_t conf;
    conf_error_e err = conf_load(&conf, filename);
    EXITIF(err != CONF_SUCCESS, -1, "failed to load config file: %s (error %d)", filename, err);

    // TNC connection (TCP or Unix socket)
    const char *val;
    val = conf_get_str_or_default(&conf, OPT_SOCKET, opts->socket);
    if (opts->socket[0] == '\0')
        strncpy(opts->socket, val, sizeof(opts->socket) - 1);

    val = conf_get_str_or_default(&conf, OPT_HOST, opts->host);
    if (opts->host[0] == '\0')
        strncpy(opts->host, val, sizeof(opts->host) - 1);
    opts->port = conf_get_int_or_default(&conf, OPT_PORT, opts->port);

    // Gateway identity
    val = conf_get_str_or_default(&conf, OPT_CALL, opts->call);
    if (opts->call[0] == '\0')
        strncpy(opts->call, val, sizeof(opts->call) - 1);
    opts->ssid = conf_get_int_or_default(&conf, OPT_SSID, opts->ssid);

    // Log level
    val = conf_get_str(&conf, OPT_VERBOSE);
    if (val != NULL)
    {
        if (strcmp(val, OPT_VAL_LOG_VERBOSE) == 0)
            opts->log_level = LOG_LEVEL_VERBOSE;
        else if (strcmp(val, OPT_VAL_LOG_DEBUG) == 0)
            opts->log_level = LOG_LEVEL_DEBUG;
    }

    // Dry run
    opts->dry_run = conf_get_bool_or_default(&conf, OPT_DRY_RUN, opts->dry_run);
}
