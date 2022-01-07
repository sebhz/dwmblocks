#include <stdio.h>
#include <time.h>
#include <string.h>

#define fmt "%m/%d %H:%M WW%V"

void
datetime (char *buf, int len)
{
    time_t rawtime;
    struct tm *timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (buf, len, fmt, timeinfo);
}
