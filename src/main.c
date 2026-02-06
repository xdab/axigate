#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <common.h>
#include <buffer.h>
#include "connection.h"
#include "packet.h"
#include "options.h"

#define READ_BUF_SIZE 2048

static volatile sig_atomic_t g_shutdown_requested = 0;

static void signal_handler(int sig)
{
    (void)sig;
    g_shutdown_requested = 1;
}

int main(int argc, char *argv[])
{
    options_t opts = {0};
    opts_init(&opts);
    opts_parse_args(&opts, argc, argv);
    opts_parse_conf_file(&opts, opts.config_file);
    opts_defaults(&opts);

    _log_level = opts.log_level;

    connection_t conn;
    kiss_decoder_t decoder;
    ax25_packet_t packet;

    char buf_data[READ_BUF_SIZE];
    buffer_t buf = {
        .data = buf_data,
        .capacity = READ_BUF_SIZE,
        .size = 0};

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    kiss_decoder_init(&decoder);

    if (opts.socket[0] != '\0')
        LOG("connecting to TNC via Unix socket at %s...", opts.socket);
    else
        LOG("connecting to TNC via TCP at %s:%d...", opts.host, opts.port);

    if (connection_init(&conn, opts.host, opts.port, opts.socket) < 0)
    {
        LOG("failed to connect to TNC");
        goto SHUTDOWN;
    }

    LOG("connected, waiting for data...");

    while (!g_shutdown_requested)
    {
        int len = connection_listen(&conn, &buf);
        if (len < 0)
        {
            LOG("connection lost");
            goto SHUTDOWN;
        }

        if (len == 0)
            continue;

        for (int i = 0; i < len; i++)
        {
            if (!packet_decode(&decoder, buf_data[i], &packet))
                continue;

            packet_log("<", &packet);
        }
    }

SHUTDOWN:
    LOG("shutting down...");
    connection_free(&conn);

    return 0;
}
