#include "rxigate.h"
#include <common.h>

int send_to_aprsis(tcp_client_t *aprsis, ax25_packet_t *packet, buffer_t *buf, options_t *opts)
{
    if (tnc2_packet_to_string(packet, buf) <= 0)
        return -1;

    char tx_indicator = opts->dry_run ? 't' : 'T';
    LOG("%c %.*s\n", tx_indicator, (int)buf->size, buf->data);

    if (opts->dry_run)
        return 0;

    if (buf->size + 2 > buf->capacity)
        return -1;

    buf->data[buf->size++] = '\r';
    buf->data[buf->size++] = '\n';
    tcp_client_send(aprsis, buf);

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
