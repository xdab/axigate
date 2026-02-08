#include "txigate.h"
#include "packet.h"
#include "options.h"
#include <common.h>
#include <tnc2.h>

extern ax25_addr_t g_igate_call;
extern options_t g_opts;

int prepare_for_tx_igate(ax25_packet_t *packet)
{
    nonnull(packet, "packet");

    // Replace original path with TCPIP network marker and IGate address
    packet->destination.last = false;
    ax25_addr_t tcpip;
    ax25_addr_init_with(&tcpip, TXIGATE_TCPIP, 0, false);
    packet->path[0] = tcpip;
    packet->path[0].last = false;
    packet->path[1] = g_igate_call;
    packet->path[1].repeated = true;
    packet->path[1].last = true;
    packet->path_len = 2;

    // Build encapsulated info field with original packet as TNC2 string
    char new_info[AX25_MAX_INFO_LEN] = {'}'};
    buffer_t new_info_buf = {
        .data = new_info + 1,
        .capacity = AX25_MAX_INFO_LEN - 1,
        .size = 0};

    int new_info_len = tnc2_packet_to_string(packet, &new_info_buf);
    if (new_info_len < 0)
        return -1;
    new_info_len++; // account for leading '}' byte

    packet->source = g_igate_call;
    ax25_addr_init_with(&packet->destination, DEST_APN001, 0, false);
    packet->destination.last = true;
    packet->path_len = 0;
    packet->control = 0x03;
    packet->protocol = 0xF0;
    memcpy(packet->info, new_info, new_info_len);
    packet->info_len = new_info_len;

    return 0;
}

int send_to_tnc(connection_t *conn, ax25_packet_t *packet)
{
    nonnull(conn, "conn");
    nonnull(packet, "packet");

    unsigned char kiss_buf[512];
    buffer_t kiss_out = {
        .data = kiss_buf,
        .capacity = sizeof(kiss_buf),
        .size = 0};

    if (!packet_encode(packet, &kiss_out))
    {
        LOG("failed to encode TX packet");
        return -1;
    }

    unsigned char tnc2_buf_data[512];
    buffer_t tnc2_buf = {
        .data = tnc2_buf_data,
        .capacity = sizeof(tnc2_buf_data),
        .size = 0};

    if (tnc2_packet_to_string(packet, &tnc2_buf) < 0)
    {
        LOG("failed to convert packet to TNC2 string");
        return -1;
    }

    char tx_indicator = g_opts.is_to_rf ? '>' : 'D';
    LOG("%c %.*s", tx_indicator, (int)(tnc2_buf.size), tnc2_buf_data);

    if (!g_opts.is_to_rf)
        return 0;

    return connection_send(conn, &kiss_out);
}
