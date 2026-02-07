#ifndef TEST_RXIGATE_H
#define TEST_RXIGATE_H

#include "test.h"
#include <ax25.h>
#include <buffer.h>
#include "rxigate.h"

ax25_addr_t g_igate_call;

void test_prepare_for_rxigate_basic()
{
    ax25_packet_t packet;
    ax25_packet_init(&packet);

    ax25_addr_init_with(&packet.destination, "APRS", 0, false);
    ax25_addr_init_with(&packet.source, "N0CALL", 0, false);
    packet.path_len = 0;

    prepare_for_rx_igate(&packet);

    assert_equal_int(packet.path_len, 2, "path_len should be 2 after prepare_for_rxigate");
    assert_true(packet.path[0].last == false, "qAR should not be last");
    assert_true(packet.path[1].last == true, "g_rxigate_addr should be last");
}

void test_prepare_for_rxigate_with_relays()
{
    ax25_packet_t packet;
    ax25_packet_init(&packet);

    ax25_addr_init_with(&packet.destination, "APRS", 0, false);
    ax25_addr_init_with(&packet.source, "N0CALL", 0, false);
    ax25_addr_init_with(&packet.path[0], "W1AW", 4, false);
    packet.path[0].last = true;
    packet.path_len = 1;

    prepare_for_rx_igate(&packet);

    assert_equal_int(packet.path_len, 3, "path_len should be 3 after adding qAR and g_rxigate_addr");
    assert_true(packet.path[0].last == false, "W1AW-4 last flag should be cleared");
    assert_true(packet.path[1].last == false, "qAR should not be last");
    assert_true(packet.path[2].last == true, "g_rxigate_addr should be last");
}

void test_prepare_for_rxigate_max_path()
{
    ax25_packet_t packet;
    ax25_packet_init(&packet);

    ax25_addr_init_with(&packet.destination, "APRS", 0, false);
    ax25_addr_init_with(&packet.source, "N0CALL", 0, false);

    for (int i = 0; i < AX25_MAX_PATH_LEN; i++)
    {
        ax25_addr_init_with(&packet.path[i], "TEST", i, false);
        packet.path[i].last = (i == AX25_MAX_PATH_LEN - 1);
    }
    packet.path_len = AX25_MAX_PATH_LEN;

    prepare_for_rx_igate(&packet);

    assert_equal_int(packet.path_len, AX25_MAX_PATH_LEN, "path_len should not change when at max");
    assert_true(packet.path[AX25_MAX_PATH_LEN - 1].last == true, "last flag unchanged at max path");
}

void test_prepare_for_rxigate_empty_path()
{
    ax25_packet_t packet;
    ax25_packet_init(&packet);

    ax25_addr_init_with(&packet.destination, "APRS", 0, false);
    ax25_addr_init_with(&packet.source, "N0CALL", 0, false);
    packet.path_len = 0;

    prepare_for_rx_igate(&packet);

    assert_equal_int(packet.path_len, 2, "path_len should be 2 with qAR and g_rxigate_addr");
    assert_true(packet.path[0].last == false, "qAR should not be last");
    assert_true(packet.path[1].last == true, "g_rxigate_addr should be last");
}

#endif
