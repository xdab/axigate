#include "test.h"
#include "test_axigate.h"

int main(void)
{
    begin_suite();

    begin_module("Axigate");
    test_axigate_function();
    end_module();

    int failed = end_suite();

    return failed ? 1 : 0;
}