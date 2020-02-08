#include <stddef.h>
#include "strfmt.h"

#define OCT(addr, i) (((addr) >> ((3 - (i))*8)) & 0xFF)

size_t fmt_ipv4(char *str, size_t size, const struct fmt *fmt)
{
    return strfmt(str, size, "%#.%#.%#.%#",
                  f(decimal OCT(fmt->data.u, 0)),
                  f(decimal OCT(fmt->data.u, 1)),
                  f(decimal OCT(fmt->data.u, 2)),
                  f(decimal OCT(fmt->data.u, 3)));
}
