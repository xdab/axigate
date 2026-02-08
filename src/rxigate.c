#include "rxigate.h"
#include "options.h"
#include <common.h>
#include <tnc2.h>
#include <string.h>

extern ax25_addr_t g_igate_call;
extern options_t g_opts;

int send_to_is(tcp_client_t *aprsis, ax25_packet_t *packet)
{
    if (!g_opts.rf_to_is)
        return 0;

    unsigned char buf[512];
    buffer_t tnc2_buf = {
        .data = buf,
        .capacity = sizeof(buf),
        .size = 0};

    if (tnc2_packet_to_string(packet, &tnc2_buf) < 0)
        return -1;

    LOG("R %.*s", (int)tnc2_buf.size, tnc2_buf.data);

    tnc2_buf.data[tnc2_buf.size++] = '\r';
    tnc2_buf.data[tnc2_buf.size++] = '\n';
    tcp_client_send(aprsis, &tnc2_buf);

    return 0;
}

int prepare_for_rx_igate(ax25_packet_t *packet)
{
    if (packet->info_len <= 0)
        return -1; // Pointless to forward

    if (packet->info[0] == '}')
        return -1; // Encapsulated packet from a TX-IGate, ignore to prevent loops

    if (packet->path_len >= AX25_MAX_PATH_LEN)
        return -2; // No room to add IGate path

    for (int i = 0; i < packet->path_len; i++)
        if (strncmp(packet->path[i].callsign, "RFONLY", 6) == 0 ||
            strncmp(packet->path[i].callsign, "NOGATE", 6) == 0)
            return -3;

    if (packet->path_len > 0)
        packet->path[packet->path_len - 1].last = false;

    ax25_addr_t qar;
    ax25_addr_init_with(&qar, "qAR", 0, false);
    packet->path[packet->path_len] = qar;
    packet->path[packet->path_len++].last = false;

    packet->path[packet->path_len] = g_igate_call;
    packet->path[packet->path_len++].last = true;

    // Some stations send packets like this, which is not accepted by APRS-IS
    packet->source.repeated = false;
    packet->destination.repeated = false;

    return 0;
}
