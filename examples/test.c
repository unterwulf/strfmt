#include <stdio.h>
#include <stdint.h>
#include "strfmt.h"

int main(int argc, const char *argv[])
{
    char buf[2048];
    struct sha1 hash = { { 0x1, 0x2, 0x3, [19] = 0xFF } };
#if 1
    strfmt(buf, sizeof buf,
           "Filename: %#\n"
           "First letter: %#\n"
           "Line number: [%#]\n"
           "Line number: [%#]\n"
           "Line number: [%#]\n"
           "Line number: [%#]\n"
           "Line number: [%#]\n",
           f(string __FILE__),
           f(leftadj w(10) char __FILE__[0]),
           f(w(10) octal __LINE__),
           f(w(10) hex __LINE__),
           f(w(10) leftadj decimal __LINE__),
           f(zeropad explplus w(10) decimal __LINE__),
           f(explplus w(10) decimal 0)
          );
    puts(buf);
    printf("Filename: %s\n"
           "Line number: [%10o]\n"
           "Line number: [%10x]\n"
           "Line number: [%-10d]\n"
           "Line number: [%+010d]\n"
           "Line number: [%+10d]\n",
           __FILE__,
           __LINE__,
           __LINE__,
           __LINE__,
           __LINE__,
           0
          );
#else
    strfmt(buf, sizeof buf,
           "Filename: %#\nLine number    : [%#]\nLine number oct: [%#]\nLine number hex: [%#]\nHash: %#\nFilesize: %#\n",
           f(string __FILE__),
           f(decimal __LINE__, 10),
           f(octal __LINE__, 10),
           f(alt hex __LINE__, 10),
           f(sha1 &hash),
           f(file_size 1024*1024+1));
    puts(buf);
    strfmt(buf, sizeof buf, "Filename: [%#]\n", f(string __FILE__, 10, 4));
    puts(buf);
    strfmt(buf, sizeof buf, "Filename: [%#]\n", f(leftadjusted string __FILE__, 10, 5));
    puts(buf);
    printf("Filename: [%10.4s]\n", __FILE__);
    strfmt(buf, sizeof buf, "IP: [%#]\n", f(ipv4 0xc0a80001));
    puts(buf);
#endif
#if 0
	strfmt("New connection from %# port %#",
	       F(addr, remote), F(port, remote));
#endif
    return 0;
}
