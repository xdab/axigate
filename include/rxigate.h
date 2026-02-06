#pragma once
#include <ax25.h>
#include <buffer.h>
#include <tcp.h>
#include <tnc2.h>
#include "options.h"

extern ax25_addr_t g_rxigate_addr;

void prepare_for_rxigate(ax25_packet_t *packet);

int send_to_aprsis(tcp_client_t *aprsis, ax25_packet_t *packet, buffer_t *buf, options_t *opts);
