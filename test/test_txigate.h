#ifndef TEST_TXIGATE_H
#define TEST_TXIGATE_H

#include "test.h"
#include <ax25.h>
#include <buffer.h>
#include <tnc2.h>
#include "txigate.h"

void test_prepare_for_tx_igate_basic()
{
    ax25_packet_t packet;
    ax25_packet_init(&packet);

    ax25_addr_init_with(&packet.source, "SRC", 0, false);
    ax25_addr_init_with(&packet.destination, "DST", 0, false);
    packet.path_len = 0;
    memcpy(packet.info, "test", 4);
    packet.info_len = 4;

    int result = prepare_for_tx_igate(&packet);
    assert_equal_int(result, 0, "prepare_for_tx_igate should return 0");

    assert_true(strncmp(packet.source.callsign, "MYCALL", 6) == 0, "source call");
    assert_equal_int(packet.source.ssid, 2, "source ssid");
    assert_true(strncmp(packet.destination.callsign, "APN001", 6) == 0, "dest call");
    assert_equal_int(packet.destination.ssid, 0, "dest ssid");
    assert_equal_int(packet.control, 0x03, "control");
    assert_equal_int(packet.protocol, 0xF0, "protocol");
}

#endif
