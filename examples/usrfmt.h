#ifndef USRFMT_H
#define USRFMT_H

#include <stdint.h>

#define FMT_USER_TYPES \
    X(SHA1,      fmt_sha1) \
    X(IPV4,      fmt_ipv4) \
    X(FILE_SIZE, fmt_file_size)

#define __FMT_SPEC_ipv4      FMT_TYPE_SPEC(UFT_IPV4, u)
#define __FMT_SPEC_sha1      FMT_TYPE_SPEC(UFT_SHA1, ptr)
#define __FMT_SPEC_file_size FMT_TYPE_SPEC(UFT_FILE_SIZE, u)

#define SHA1_SIZE 20

struct sha1 {
    uint8_t value[SHA1_SIZE];
};

#endif
