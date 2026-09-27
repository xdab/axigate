#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "options.h"

static struct argp_option options[] = {
    {OPT_CONFIG, OPT_SHORT_CONFIG, "FILE", 0, "Configuration file", 0},
    {OPT_HOST, OPT_SHORT_HOST, "HOST", 0, "TNC host address", 1},
    {OPT_PORT, OPT_SHORT_PORT, "PORT", 0, "TNC port (default: 8144)", 1},
    {OPT_SOCKET, OPT_SHORT_SOCKET, "PATH", 0, "Unix socket path", 1},

    {OPT_IS_HOST, OPT_SHORT_IS_HOST, "HOST", 0, "APRS-IS host (default: rotate.aprs2.net)", 1},
    {OPT_IS_PORT, OPT_SHORT_IS_PORT, "PORT", 0, "APRS-IS port (default: 14580)", 1},
    {OPT_IS_FILTER, OPT_SHORT_IS_FILTER, "FILTER", 0, "APRS-IS filter string", 1},
    {OPT_IS_PASSCODE, OPT_SHORT_IS_PASSCODE, "PASSCDE", 0, "APRS-IS passcode (default: -1)", 1},
    {OPT_IS_KEEPALIVE, OPT_SHORT_IS_KEEPALIVE, "SECONDS", 0, "Minimum seconds between APRS-IS keepalives (default: 300, 0 = off)", 1},

    {OPT_UDP_KISS_LISTEN, OPT_SHORT_UDP_KISS_LISTEN, "PORT", 0, "UDP port listening for KISS packets to gate to APRS-IS", 1},
    {OPT_UDP_TNC2_LISTEN, OPT_SHORT_UDP_TNC2_LISTEN, "PORT", 0, "UDP port listening for TNC2 packets to gate to APRS-IS", 1},

    {OPT_CALL, OPT_SHORT_CALL, "CALL", 0, "Digipeater callsign", 2},
    {OPT_SSID, OPT_SHORT_SSID, "SSID", 0, "Digipeater SSID", 2},

    {OPT_VERBOSE, OPT_SHORT_VERBOSE, 0, 0, "Verbose logs", 4},
    {OPT_DEBUG, OPT_SHORT_DEBUG, 0, 0, "Debug logs (very verbose)", 4},

    {OPT_RF_TO_IS, OPT_SHORT_RF_TO_IS, 0, 0, "Enable RF to APRS-IS forwarding", 5},
    {OPT_IS_TO_RF, OPT_SHORT_IS_TO_RF, 0, 0, "Enable APRS-IS to RF forwarding", 5},

    {0, 0, 0, 0, 0, 0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    options_t *opts = state->input;
    switch (key)
    {
    case OPT_SHORT_CONFIG:
        strncpy(opts->config_file, arg, sizeof(opts->config_file) - 1);
        break;
    case OPT_SHORT_HOST:
        strncpy(opts->host, arg, sizeof(opts->host) - 1);
        break;
    case OPT_SHORT_PORT:
        opts->port = atoi(arg);
        break;
    case OPT_SHORT_SOCKET:
        strncpy(opts->socket, arg, sizeof(opts->socket) - 1);
        break;
    case OPT_SHORT_IS_HOST:
        strncpy(opts->is_host, arg, sizeof(opts->is_host) - 1);
        break;
    case OPT_SHORT_IS_PORT:
        opts->is_port = atoi(arg);
        break;
    case OPT_SHORT_IS_FILTER:
        strncpy(opts->is_filter, arg, sizeof(opts->is_filter) - 1);
        break;
    case OPT_SHORT_IS_PASSCODE:
        opts->is_passcode = atoi(arg);
        break;
    case OPT_SHORT_IS_KEEPALIVE:
        opts->is_keepalive = atoi(arg);
        break;
    case OPT_SHORT_UDP_KISS_LISTEN:
        opts->udp_kiss_port = atoi(arg);
        break;
    case OPT_SHORT_UDP_TNC2_LISTEN:
        opts->udp_tnc2_port = atoi(arg);
        break;
    case OPT_SHORT_CALL:
        strncpy(opts->call, arg, sizeof(opts->call) - 1);
        break;
    case OPT_SHORT_SSID:
        opts->ssid = atoi(arg);
        break;
    case OPT_SHORT_VERBOSE:
        opts->log_level = LOG_LEVEL_VERBOSE;
        break;
    case OPT_SHORT_DEBUG:
        opts->log_level = LOG_LEVEL_DEBUG;
        break;
    case OPT_SHORT_RF_TO_IS:
        opts->rf_to_is = true;
        break;
    case OPT_SHORT_IS_TO_RF:
        opts->is_to_rf = true;
        break;
    case ARGP_KEY_NO_ARGS:
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct argp argp = {
    options,
    parse_opt,
    "",
    "AX.25 <> APRS-IS bidirectional gateway"};

void opts_parse_args(options_t *opts, int argc, char *argv[])
{
    nonnull(opts, "opts");
    nonzero(argc, "argc");
    nonnull(argv, "argv");

    argp_parse(&argp, argc, argv, 0, 0, opts);
}
