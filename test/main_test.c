#include "test.h"
#include "test_template.h"

int main(void)
{
    begin_suite();

    begin_module("Template");
    test_template_function();
    end_module();

    int failed = end_suite();

    return failed ? 1 : 0;
}