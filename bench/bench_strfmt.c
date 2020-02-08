#include "strfmt.h"

int main(int argc, const char *argv[])
{
    char buf[1024];
    for (int i = 0; i < 10000000; i++)
        strfmt(buf, sizeof buf, "[%#] %#", fg(__FILE__), fg(__LINE__));
    return 0;
}
