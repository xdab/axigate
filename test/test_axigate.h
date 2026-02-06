#ifndef TEST_AXIGATE_H
#define TEST_AXIGATE_H

#include "test.h"
#include "axigate.h"

void test_axigate_function()
{
    // Example test case for axigate_function
    int input = 4;
    int expected_output = 16; // 4 * 4
    int actual_output = axigate_function(input);
    assert_equal_int(actual_output, expected_output, "axigate_function should return the square of the input");
}

#endif
