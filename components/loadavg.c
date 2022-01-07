#include <stdio.h>
#include <stdlib.h>

void
loadavg (char *buf, int len)
{
    double avgs[3];

    if (getloadavg (avgs, 3) < 0) {
        return;
    }

    snprintf (buf, len, "%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}
