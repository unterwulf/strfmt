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
#ifndef STRFMT_H
#define STRFMT_H

#include <stddef.h>
#include <stdint.h>
#include "usrfmt.h"

/* Unlike printf family, we have only one integer type. Application is free
 * to decide which one to use. The only limitation is that the unsigned one
 * must be able to represent all positive values of the signed one. */
#ifndef FMT_INTMAX_DEFINED
typedef unsigned long long fmt_uintmax_t;
typedef long long fmt_intmax_t;
#endif

/* Expansion helper macros. They are not meant to be used directly. */
#define __FMT_EMPTY
#define __FMT_DEFER(...) __VA_ARGS__ __FMT_EMPTY
#define __FMT_EXPAND(...) __VA_ARGS__

/* Maximum number of attributes is 9 + type, so is the number of expansions. */
#define f(...) \
/* 1 */ __FMT_EXPAND( \
/* 2 */ __FMT_EXPAND( \
/* 3 */ __FMT_EXPAND( \
/* 4 */ __FMT_EXPAND( \
/* 6 */ __FMT_EXPAND( \
/* 7 */ __FMT_EXPAND( \
/* 8 */ __FMT_EXPAND( \
/* 9 */ __FMT_EXPAND( \
/* 10*/ __FMT_EXPAND(__f(__VA_ARGS__))))))))))

#define __f(...) (struct fmt){ __FMT_SPEC_attr(__VA_ARGS__) }

#define __FMT_SPEC(pos, v, ...) v, __FMT_SPEC_##pos(__VA_ARGS__)
#define __FMT_SPEC_id()         __FMT_SPEC

#define __FMT_SPEC_attr(...)    __FMT_DEFER(__FMT_SPEC_id)()( __FMT_SPEC_##__VA_ARGS__ )
#define __FMT_SPEC_type(f, v)   .data = { .f = (v) }

#define __FMT_SPEC_alt          attr, .alt = 1,
#define __FMT_SPEC_leftadj      attr, .leftadj = 1,
#define __FMT_SPEC_blnkplus     attr, .blnkplus = 1,
#define __FMT_SPEC_explplus     attr, .explplus = 1,
#define __FMT_SPEC_zeropad      attr, .zeropad = 1,

#define __FMT_SPEC_w(v)         attr, .width = (v),
#define __FMT_SPEC_p(v)         attr, .precision = (v), explprecn
#define __FMT_SPEC_explprecn    attr, .explprecn = 1,

#define FMT_TYPE_SPEC(id, member) type, .type = (id), member,

#define __FMT_SPEC_decimal      FMT_TYPE_SPEC(FT_DEC,      i)
#define __FMT_SPEC_octal        FMT_TYPE_SPEC(FT_OCT,      u)
#define __FMT_SPEC_hex          FMT_TYPE_SPEC(FT_HEX,      u)
#define __FMT_SPEC_unsigned     FMT_TYPE_SPEC(FT_UNSIGNED, u)
#define __FMT_SPEC_char         FMT_TYPE_SPEC(FT_CHAR,     uc)
#define __FMT_SPEC_string       FMT_TYPE_SPEC(FT_STR,      ptr)

struct fmt {
    unsigned int type:8;
    unsigned int alt:1;
    unsigned int zeropad:1;
    unsigned int leftadj:1;
    unsigned int blnkplus:1;
    unsigned int explplus:1;
    unsigned int thousand:1;
    unsigned int explprecn:1; /* to distinguish default from explicit 0 */
    unsigned int reserved:1;
    union {
        void *ptr;
        fmt_uintmax_t u;
        fmt_intmax_t i;
        unsigned char uc;
    } data;
    unsigned int width;
    unsigned int precision;
} fmt;

typedef size_t fmt_user_type_handler_t(char *str, size_t size,
                                       const struct fmt *fmt);

enum {
    FT_DEC,
    FT_OCT,
    FT_HEX,
    FT_UNSIGNED,
    FT_CHAR,
    FT_STR,
#define UFT_BASE (FT_STR+1)

#ifdef FMT_USER_TYPES
#define X(name, handler) UFT_##name,
FMT_USER_TYPES
#undef X
#endif

    FT_END
};

#if __STDC_VERSION__ >= 201112L
static inline struct fmt fg_int(int v) {
    return f(decimal v);
}

static inline struct fmt fg_str(char *v) {
    return f(string v);
}

#define fg(v) _Generic((v), \
        int: fg_int, \
        char *: fg_str \
        )(v)
#endif

size_t strfmt(char *str, size_t size, const char *fmt, ...);

#endif
