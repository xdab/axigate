#include "test.h"
#include "test_rxigate.h"

int main(void)
{
    begin_suite();

    begin_module("RX-iGate");
    test_prepare_for_rxigate_basic();
    test_prepare_for_rxigate_with_relays();
    test_prepare_for_rxigate_max_path();
    test_prepare_for_rxigate_empty_path();
    end_module();

    int failed = end_suite();

    return failed ? 1 : 0;
}
