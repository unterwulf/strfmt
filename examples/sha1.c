#include <stdint.h>
#include "strfmt.h"

size_t fmt_sha1(char *str, size_t size, const struct fmt *fmt)
{
    const struct sha1 *hash = fmt->data.ptr;
    size /= 2;
    if (size > SHA1_SIZE)
        size = SHA1_SIZE;
    for (size_t i = 0; i < size; i++, str += 2)
        strfmt(str, 3, "%#", f(zeropad w(2) hex hash->value[i]));
    return 2*SHA1_SIZE;
}
