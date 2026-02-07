#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <common.h>
#include <buffer.h>
#include <tcp.h>
#include <poller.h>
#include <line.h>
#include <tnc2.h>
#include "connection.h"
#include "packet.h"
#include "options.h"
#include "rxigate.h"
#include "txigate.h"

#define READ_BUF_SIZE 2048

#define SOFTWARE "axigate"
#define VERSION "1.0"

static volatile sig_atomic_t g_shutdown_requested = 0;
static connection_t *g_tnc = NULL;
static tcp_client_t *g_aprsis = NULL;
static bool g_aprsis_logged_in = false;
ax25_addr_t g_igate_call;
options_t g_opts;

static void signal_handler(int sig)
{
    (void)sig;
    g_shutdown_requested = 1;
}

static void aprsis_send_login(void)
{
    char login_buf[256];

    int len = snprintf(
        login_buf, sizeof(login_buf),
        "user %s pass %d vers " SOFTWARE " " VERSION,
        g_opts.call,
        g_opts.is_passcode);

    bool has_filter = g_opts.is_filter[0] != '\0';
    if (has_filter)
    {
        len += snprintf(
            login_buf + len, sizeof(login_buf) - len,
            " filter %s",
            g_opts.is_filter);
    }

    len += snprintf(login_buf + len, sizeof(login_buf) - len, "\r\n");

    buffer_t send_buf = {
        .data = (unsigned char *)login_buf,
        .size = len,
        .capacity = (int)sizeof(login_buf)};
    tcp_client_send(g_aprsis, &send_buf);
    g_aprsis_logged_in = true;

    LOG("T %.*s", (int)send_buf.size - 2, send_buf.data);
}

static void aprsis_line_callback(const buffer_t *line_buf)
{
    if (line_buf->size == 0)
        return;

    if (line_buf->data[0] == '#')
    {
        LOG("%.*s", (int)line_buf->size, line_buf->data);

        if (!g_aprsis_logged_in)
            aprsis_send_login();

        return;
    }

    LOG("r %.*s", (int)line_buf->size, line_buf->data);

    ax25_packet_t packet;
    if (tnc2_string_to_packet(&packet, line_buf) != 0)
    {
        LOGV("invalid packet from APRS-IS");
        return;
    }

    if (prepare_for_tx_igate(&packet) < 0)
    {
        LOGV("failed to prepare TX packet");
        return;
    }

    send_to_tnc(g_tnc, &packet);
}

static void tnc_packet_callback(ax25_packet_t *packet)
{
    packet_log("<", packet);

    if (prepare_for_rx_igate(packet) < 0)
    {
        LOGV("failed to prepare RX packet");
        return;
    }

    if (prepare_for_rx_igate(packet) < 0)
    {
        LOGV("failed to prepare RX packet");
        return;
    }

    send_to_is(g_aprsis, packet);
}

int main(int argc, char *argv[])
{
    opts_init(&g_opts);
    opts_parse_args(&g_opts, argc, argv);
    opts_parse_conf_file(&g_opts, g_opts.config_file);
    opts_defaults(&g_opts);

    ax25_addr_init_with(&g_igate_call, g_opts.call, g_opts.ssid, false);

    _log_level = g_opts.log_level;

    connection_t conn;
    tcp_client_t aprsis;
    kiss_decoder_t decoder;
    ax25_packet_t packet;

    char buf_data[READ_BUF_SIZE];
    buffer_t buf = {
        .data = buf_data,
        .capacity = READ_BUF_SIZE,
        .size = 0};

    line_reader_t aprsis_line_reader;
    line_reader_init(&aprsis_line_reader, aprsis_line_callback);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    socket_poller_t poller;
    socket_poller_init(&poller);

    kiss_decoder_init(&decoder);

    if (g_opts.socket[0] != '\0')
        LOG("connecting to TNC via Unix socket at %s...", g_opts.socket);
    else
        LOG("connecting to TNC via TCP at %s:%d...", g_opts.host, g_opts.port);

    if (connection_init(&conn, g_opts.host, g_opts.port, g_opts.socket) < 0)
    {
        LOG("failed to connect to TNC");
        goto SHUTDOWN;
    }

    LOG("connecting to APRS-IS at %s:%d...", g_opts.is_host, g_opts.is_port);

    if (tcp_client_init(&aprsis, g_opts.is_host, g_opts.is_port, TCP_DEF_TIMEOUT_MS) < 0)
    {
        LOG("failed to connect to APRS-IS");
        goto SHUTDOWN_TNC;
    }

    g_aprsis = &aprsis;
    g_tnc = &conn;

    int tnc_fd = (conn.type == CONNECTION_TCP) ? conn.client.tcp.fd : conn.client.uds.fd;
    socket_poller_add(&poller, tnc_fd, POLLER_EV_IN);
    socket_poller_add(&poller, aprsis.fd, POLLER_EV_IN);

    LOG("connected, waiting for data...");

    while (!g_shutdown_requested)
    {
        int ready = socket_poller_wait(&poller, 1000);
        if (ready < 0)
        {
            LOG("poll error");
            goto SHUTDOWN_ALL;
        }

        if (ready == 0)
            continue;

        if (socket_poller_is_ready(&poller, tnc_fd))
        {
            int len = connection_listen(&conn, &buf);
            if (len < 0)
            {
                LOG("TNC connection lost");
                goto SHUTDOWN_ALL;
            }

            for (int i = 0; i < len; i++)
                if (packet_decode(&decoder, buf_data[i], &packet))
                    tnc_packet_callback(&packet);
        }

        if (socket_poller_is_ready(&poller, aprsis.fd))
        {
            int len = tcp_client_listen(&aprsis, &buf);
            if (len < 0)
            {
                LOG("APRS-IS connection lost");
                goto SHUTDOWN_ALL;
            }

            for (int i = 0; i < len; i++)
                line_reader_process(&aprsis_line_reader, buf_data[i]);
        }
    }

SHUTDOWN_ALL:
    LOG("shutting down...");
    tcp_client_free(&aprsis);

SHUTDOWN_TNC:
    connection_free(&conn);

SHUTDOWN:
    socket_poller_free(&poller);

    return 0;
}
