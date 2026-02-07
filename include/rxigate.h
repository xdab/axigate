#pragma once
#include <ax25.h>
#include <tcp.h>

int prepare_for_rx_igate(ax25_packet_t *packet);

int send_to_is(tcp_client_t *aprsis, ax25_packet_t *packet);
