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

    // APRS-IS connection
    val = conf_get_str_or_default(&conf, OPT_IS_HOST, opts->is_host);
    if (opts->is_host[0] == '\0')
        strncpy(opts->is_host, val, sizeof(opts->is_host) - 1);
    val = conf_get_str_or_default(&conf, OPT_IS_FILTER, opts->is_filter);
    if (opts->is_filter[0] == '\0')
        strncpy(opts->is_filter, val, sizeof(opts->is_filter) - 1);
    opts->is_port = conf_get_int_or_default(&conf, OPT_IS_PORT, opts->is_port);
    opts->is_passcode = conf_get_int_or_default(&conf, OPT_IS_PASSCODE, opts->is_passcode);
    opts->is_keepalive = conf_get_int_or_default(&conf, OPT_IS_KEEPALIVE, opts->is_keepalive);

    // UDP injection inputs
    opts->udp_kiss_port = conf_get_int_or_default(&conf, OPT_UDP_KISS_LISTEN, opts->udp_kiss_port);
    opts->udp_tnc2_port = conf_get_int_or_default(&conf, OPT_UDP_TNC2_LISTEN, opts->udp_tnc2_port);

    // Log level
    val = conf_get_str(&conf, OPT_VERBOSE);
    if (val != NULL)
    {
        if (strcmp(val, OPT_VAL_LOG_VERBOSE) == 0)
            opts->log_level = LOG_LEVEL_VERBOSE;
        else if (strcmp(val, OPT_VAL_LOG_DEBUG) == 0)
            opts->log_level = LOG_LEVEL_DEBUG;
    }

    // Directional forwarding
    opts->rf_to_is = conf_get_bool_or_default(&conf, OPT_RF_TO_IS, opts->rf_to_is);
    opts->is_to_rf = conf_get_bool_or_default(&conf, OPT_IS_TO_RF, opts->is_to_rf);
}
