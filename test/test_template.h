#ifndef TEST_TEMPLATE_H
#define TEST_TEMPLATE_H

#include "test.h"
#include "template.h"

void test_template_function()
{
    // Example test case for template_function
    int input = 4;
    int expected_output = 16; // 4 * 4
    int actual_output = template_function(input);
    assert_equal_int(actual_output, expected_output, "template_function should return the square of the input");
}

#endif
