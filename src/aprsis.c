#include "aprsis.h"

#define SOFTWARE "github.com/xdab/axigate"
#define VERSION "0.0"

void aprsis_build_login(const char *call, int passcode, const char *filter, buffer_t *out_buf)
{
    out_buf->size = snprintf(
        out_buf->data,
        out_buf->capacity,
        "user %s pass %d vers " SOFTWARE " " VERSION,
        call, passcode);

    bool has_filter = (filter != NULL) && (filter[0] != '\0');
    if (has_filter)
        out_buf->size += snprintf(
            out_buf->data + out_buf->size,
            out_buf->capacity - out_buf->size,
            " filter %s",
            filter);

    out_buf->size += snprintf(
        out_buf->data + out_buf->size,
        out_buf->capacity - out_buf->size,
        "\r\n");
}