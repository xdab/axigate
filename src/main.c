#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <common.h>
#include <buffer.h>
#include <tcp.h>
#include <udp.h>
#include <poller.h>
#include <line.h>
#include <tnc2.h>
#include "aprsis.h"
#include "connection.h"
#include "packet.h"
#include "options.h"
#include "rxigate.h"
#include "txigate.h"

#define READ_BUF_SIZE 2048

static volatile sig_atomic_t g_shutdown_requested = 0;
static connection_t *g_tnc = NULL;
static tcp_client_t *g_aprsis = NULL;
static bool g_aprsis_logged_in = false;
static time_t g_last_keepalive = 0;
ax25_addr_t g_igate_call;
options_t g_opts;

static void signal_handler(int sig)
{
    (void)sig;
    g_shutdown_requested = 1;
}

static void aprsis_send_login(void)
{
    char login_str[256];
    buffer_t login_buf = {
        .data = login_str,
        .capacity = sizeof(login_str),
        .size = 0};

    aprsis_build_login(g_opts.call, g_opts.is_passcode, g_opts.is_filter, &login_buf);
    tcp_client_send(g_aprsis, &login_buf);
    g_aprsis_logged_in = true;
    g_last_keepalive = time(NULL);

    LOG("T %.*s", login_buf.size - 2, login_buf.data);
}

static void aprsis_keepalive(void)
{
    if (!g_aprsis_logged_in || g_opts.is_keepalive <= 0)
        return;

    time_t now = time(NULL);
    if (now - g_last_keepalive < g_opts.is_keepalive)
        return;

    LOGV("sending keepalive after %d s", (int)(now - g_last_keepalive));
    aprsis_send_login();
}

static void aprsis_line_callback(const buffer_t *line_buf)
{
    if (line_buf->size == 0)
        return;

    if (line_buf->data[0] == '#')
    {
        if (!g_aprsis_logged_in)
            aprsis_send_login();
        return;
    }

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

    send_to_is(g_aprsis, packet);
}

static void udp_packet_callback(ax25_packet_t *packet)
{
    packet_log("U", packet);

    if (prepare_for_rx_igate(packet) < 0)
    {
        LOGV("failed to prepare injected packet");
        return;
    }

    send_to_is(g_aprsis, packet);
}

static void udp_line_callback(const buffer_t *line_buf)
{
    if (line_buf->size == 0)
        return;

    ax25_packet_t packet;
    if (tnc2_string_to_packet(&packet, line_buf) != 0)
    {
        LOGV("invalid packet injected via UDP");
        return;
    }

    udp_packet_callback(&packet);
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
    udp_server_t udp_kiss_server;
    udp_server_t udp_tnc2_server;
    bool udp_kiss_enabled = false;
    bool udp_tnc2_enabled = false;

    char buf_data[READ_BUF_SIZE];
    buffer_t buf = {
        .data = buf_data,
        .capacity = READ_BUF_SIZE,
        .size = 0};

    line_reader_t aprsis_line_reader;
    line_reader_init(&aprsis_line_reader, aprsis_line_callback);

    line_reader_t udp_line_reader;
    line_reader_init(&udp_line_reader, udp_line_callback);

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

    (void)tcp_client_set_nodelay(&aprsis, true);

    g_aprsis = &aprsis;
    g_tnc = &conn;

    int tnc_fd = (conn.type == CONNECTION_TCP) ? conn.client.tcp.fd : conn.client.uds.fd;
    socket_poller_add(&poller, tnc_fd, POLLER_EV_IN);
    socket_poller_add(&poller, aprsis.fd, POLLER_EV_IN);

    if (g_opts.udp_kiss_port > 0)
    {
        EXITIF(udp_server_init(&udp_kiss_server, g_opts.udp_kiss_port, 0) < 0, -1,
               "failed to listen for UDP KISS input on port %d", g_opts.udp_kiss_port);
        socket_poller_add(&poller, udp_kiss_server.fd, POLLER_EV_IN);
        udp_kiss_enabled = true;
    }

    if (g_opts.udp_tnc2_port > 0)
    {
        EXITIF(udp_server_init(&udp_tnc2_server, g_opts.udp_tnc2_port, 0) < 0, -1,
               "failed to listen for UDP TNC2 input on port %d", g_opts.udp_tnc2_port);
        socket_poller_add(&poller, udp_tnc2_server.fd, POLLER_EV_IN);
        udp_tnc2_enabled = true;
    }

    LOG("connected, waiting for data...");

    while (!g_shutdown_requested)
    {
        aprsis_keepalive();

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

        if (udp_kiss_enabled && socket_poller_is_ready(&poller, udp_kiss_server.fd))
        {
            int len = udp_server_listen(&udp_kiss_server, &buf);
            for (int i = 0; i < len; i++)
                if (packet_decode(&decoder, buf_data[i], &packet))
                    udp_packet_callback(&packet);
        }

        if (udp_tnc2_enabled && socket_poller_is_ready(&poller, udp_tnc2_server.fd))
        {
            int len = udp_server_listen(&udp_tnc2_server, &buf);
            for (int i = 0; i < len; i++)
                line_reader_process(&udp_line_reader, buf_data[i]);
        }
    }

SHUTDOWN_ALL:
    LOG("shutting down...");
    tcp_client_free(&aprsis);

    if (udp_kiss_enabled)
        udp_server_free(&udp_kiss_server);
    if (udp_tnc2_enabled)
        udp_server_free(&udp_tnc2_server);

SHUTDOWN_TNC:
    connection_free(&conn);

SHUTDOWN:
    socket_poller_free(&poller);

    return 0;
}
