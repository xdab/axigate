#pragma once

#include <ax25.h>
#include "connection.h"

#define DEST_APN001 "APN001"
#define TXIGATE_TCPIP "TCPIP"

int prepare_for_tx_igate(ax25_packet_t *packet);

int send_to_tnc(connection_t *conn, ax25_packet_t *packet);
