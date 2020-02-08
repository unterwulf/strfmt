#include <stdio.h>

int main(int argc, const char *argv[])
{
    char buf[1024];
    for (int i = 0; i < 10000000; i++)
        snprintf(buf, sizeof buf, "[%s] %d", __FILE__, __LINE__);
    return 0;
}
