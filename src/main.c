#include <stdio.h>
#include "template.h"
#include "options.h"

int main(int argc, char *argv[])
{
    options_t opts = {0};
    opts_init(&opts);
    opts_parse_args(&opts, argc, argv);
    opts_defaults(&opts);

    if (opts.verbose)
    {
        printf("Verbose mode enabled\n");
        printf("Number: %d\n", opts.number);
        printf("String: '%s'\n", opts.string);
    }

    int value = opts.number ? opts.number : 5;
    int result = template_function(value);
    printf("The square of %d is %d\n", value, result);

    return 0;
}
