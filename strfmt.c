/*
 * Copyright 2020 Vitaly Sinilin <vs@kp4.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "strfmt.h"
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#ifdef FMT_USER_TYPES
#define X(name, handler) extern fmt_user_type_handler_t handler;
FMT_USER_TYPES
#undef X
#endif

static fmt_user_type_handler_t *fmt_user_handlers[] = {
#ifdef FMT_USER_TYPES
#define X(name, handler) &handler,
FMT_USER_TYPES
#undef X
#endif
};

size_t fmt_padded(char *dst, size_t size, const char *src, size_t len,
                  const struct fmt *fmt)
{
    size_t width = (fmt->width > 0 && len < (size_t)fmt->width)
                   ? (size_t)fmt->width : len;
    size_t padding = width - len;
    size_t towrite = (width > size) ? size : width;

    if (towrite && padding && !fmt->leftadj) {
        int zero = fmt->zeropad && !fmt->explprecn;
        if (padding > towrite)
            padding = towrite;
        memset(dst, zero ? '0' : ' ', padding);
        dst += padding;
        towrite -= padding;
    }

    if (towrite) {
        if (len > towrite)
            len = towrite;
        memcpy(dst, src, len);
        dst += len;
        towrite -= len;
    }

    if (towrite && padding && fmt->leftadj) {
        if (padding > towrite)
            padding = towrite;
        memset(dst, ' ', padding);
    }

    return width;
}

static inline size_t fmt_integer(char *str, size_t size, const struct fmt *fmt)
{
    char buf[33] = "";
    char *ptr = buf + sizeof(buf) - 1;
    size_t len = 0;
    int minus = 0;
    fmt_uintmax_t value;
    unsigned int base = 10;
    char sign = '\0';

    if (fmt->type == FT_DEC) {
        if (fmt->data.i < 0) {
            minus = 1;
            value = (fmt_uintmax_t)-fmt->data.i;
        } else {
            value = fmt->data.u;
        }
    } else {
        switch (fmt->type) {
            case FT_OCT: base = 8; break;
            case FT_HEX: base = 16; break;
        }
        value = fmt->data.u;
    }

    do {
        int digit = value % base;
        *--ptr = (digit < 10 ? '0' : 'A' - 10) + digit;
        len++;
    } while ((value /= base) != 0);

    if (fmt->type == FT_DEC) {
        if (minus)
            sign = '-';
        else if (fmt->explplus) /* should override blnkplus */
            sign = '+';
        else if (fmt->blnkplus)
            sign = ' ';
    } else if (fmt->alt && (len > 1 || *ptr != '0')) {
       if (fmt->type == FT_OCT) {
            *--ptr = '0';
            len++;
       } else if (fmt->type == FT_HEX) {
            *--ptr = 'x';
            *--ptr = '0';
            len += 2;
       }
    }

    if (sign) {
        if (fmt->leftadj || !fmt->zeropad) {
            *--ptr = sign;
            len++;
            sign = '\0';
        }
    }

    int ret = fmt_padded(str, size, ptr, len, fmt);

    if (sign && size)
        *str = sign;

    return ret;
}

static inline size_t fmt_char(char *str, size_t size, const struct fmt *fmt)
{
    return fmt_padded(str, size, (const char *)&fmt->data.uc, 1, fmt);
}

static inline size_t fmt_str(char *str, size_t size, const struct fmt *fmt)
{
    size_t len = strlen(fmt->data.ptr);

    if (fmt->precision > 0 && len > (size_t)fmt->precision)
        len = fmt->precision;

    return fmt_padded(str, size, fmt->data.ptr, len, fmt);
}

int vstrfmt(char *str, size_t size, const char *fmt, va_list ap)
{
    const char *pos;
    const char *lastspec = NULL;
    int is_empty_str = (size == 0);
    size_t ret = size;
    enum { st_escape, st_normal, st_spec } state = st_normal;

#define append_char(c) { if (size > 0) { *str++ = c; size--; } else ret++; }

    for (pos = fmt; *pos; pos++) {
        switch (state) {
            case st_normal:
                switch (*pos) {
                    case '\\': state = st_escape; break;
                    case '%':  lastspec = pos;
                               state = st_spec;
                               break;
                    default:   append_char(*pos);
                }
                break;

            case st_escape:
                switch (*pos) {
                    case 'n': append_char('\n'); break;
                    case 'r': append_char('\r'); break;
                    case 't': append_char('\t'); break;
                    case '\\': append_char('\\'); break;
                    /* process it again */
                    default:  append_char('\\'); pos--;
                }
                state = st_normal;
                break;

            case st_spec:
                switch (*pos) {
                    case '#': {
                        struct fmt f = va_arg(ap, struct fmt);
                        if (f.type < FT_END) {
                            size_t towrite = 0;

                            switch (f.type) {
                                case FT_OCT:
                                case FT_HEX:
                                case FT_UNSIGNED:
                                case FT_DEC:
                                    towrite = fmt_integer(str, size, &f);
                                    break;
                                case FT_CHAR:
                                    towrite = fmt_char(str, size, &f);
                                    break;
                                case FT_STR:
                                    towrite = fmt_str(str, size, &f);
                                    break;
                                default:
                                    towrite = fmt_user_handlers[f.type - UFT_BASE](str, size, &f);
                            }

                            if (towrite > size) {
                                ret += towrite - size;
                                towrite = size;
                            }

                            if (towrite) {
                                str += towrite;
                                size -= towrite;
                            }
                        }
                        break;
                    }
                    default:
                        append_char('%');
                        pos = lastspec + 1;
                        break;
                }

                state = st_normal;
                break;
        }
    }

    if (size > 0)
        *str = '\0';
    else if (!is_empty_str)
        *(str-1) = '\0';

    return ret - size;
}

size_t strfmt(char *str, size_t size, const char *fmt, ...)
{
    size_t ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vstrfmt(str, size, fmt, ap);
    va_end(ap);
    return ret;
}
