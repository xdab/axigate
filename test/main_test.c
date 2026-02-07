#include "test.h"
#include "test_rxigate.h"
#include "test_txigate.h"
#include "options.h"

ax25_addr_t g_igate_call;
options_t g_opts;

int main(void)
{
    ax25_addr_init_with(&g_igate_call, "MYCALL", 2, false);

    begin_suite();

    begin_module("RX-iGate");
    test_prepare_for_rxigate_basic();
    test_prepare_for_rxigate_with_relays();
    test_prepare_for_rxigate_max_path();
    test_prepare_for_rxigate_empty_path();
    end_module();

    begin_module("TX-iGate");
    test_prepare_for_tx_igate_basic();
    end_module();

    int failed = end_suite();

    return failed ? 1 : 0;
}
