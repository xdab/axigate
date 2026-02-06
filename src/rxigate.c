#include "rxigate.h"
#include <common.h>
#include <tnc2.h>

int send_to_aprsis(tcp_client_t *aprsis, ax25_packet_t *packet, options_t *opts)
{
    unsigned char buf[512];
    buffer_t tnc2_buf = {
        .data = buf,
        .capacity = sizeof(buf),
        .size = 0};

    if (tnc2_packet_to_string(packet, &tnc2_buf) <= 0)
        return -1;

    char tx_indicator = opts->dry_run ? 't' : 'T';
    LOG("%c %.*s\n", tx_indicator, (int)tnc2_buf.size, tnc2_buf.data);

    if (opts->dry_run)
        return 0;

    tnc2_buf.data[tnc2_buf.size++] = '\r';
    tnc2_buf.data[tnc2_buf.size++] = '\n';
    tcp_client_send(aprsis, &tnc2_buf);

    return 0;
}

void prepare_for_rxigate(ax25_packet_t *packet)
{
    if (packet->path_len >= AX25_MAX_PATH_LEN)
        return;

    if (packet->path_len > 0)
        packet->path[packet->path_len - 1].last = false;

    ax25_addr_t qar;
    ax25_addr_init_with(&qar, "qAR", 0, false);
    packet->path[packet->path_len++] = qar;

    packet->path[packet->path_len] = g_rxigate_addr;
    packet->path[packet->path_len++].last = true;
}
