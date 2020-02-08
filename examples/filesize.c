#include <stdio.h>
#include "strfmt.h"

size_t fmt_file_size(char *str, size_t size, const struct fmt *fmt)
{
    float fsize = fmt->data.u;
    static const char *units[] = { "bytes", "KB", "MB", "GB", "TB" };
    size_t unit = 0;

    while (fsize > 1024 && unit < (sizeof units)/(sizeof units[0])) {
        fsize /= 1024;
        unit++;
    }

    /* Floating-point conversions are not implemented in strfmt so far :( */
    return snprintf(str, size, "%.2f %s", fsize, units[unit]);
}
