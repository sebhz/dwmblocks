#include <stdio.h>
#include <time.h>
#include <string.h>

#define fmt "%m/%d %H:%M:%S WW%V"

void lctime(char *buf, int len) {
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  strftime(buf, len, fmt, timeinfo);
}

void uptime(char *buf, int len) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  int d = t.tv_sec/86400;
  int h = (t.tv_sec % 86400)/3600;
  int m = (t.tv_sec % 3600)/60;
  snprintf(buf, len, "%dd %02dh %02dm\n", d, h, m);
}

