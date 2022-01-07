#include <stdio.h>
#include <time.h>

void uptime(char *buf, int len) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  int d = t.tv_sec/86400;
  int h = (t.tv_sec % 86400)/3600;
  int m = (t.tv_sec % 3600)/60;
  snprintf(buf, len, "%dd %02dh %02dm\n", d, h, m);
}

