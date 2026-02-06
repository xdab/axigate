#include <stdio.h>
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

    return 0;
}
