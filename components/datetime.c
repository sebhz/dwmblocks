#include <stdio.h>
#include <time.h>
#include <string.h>

#define fmt "%m/%d %H:%M WW%V"
#define SECS_PER_DAY 86400
#define tm_to_secs(tm) ((tm)->tm_sec + (tm)->tm_min*60.0 + (tm)->tm_hour*3600)

static const char *const duodecimal_chars[12] =
    { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "↊", "↋" };
static const char *const hour_names[13] =
    { "midnight", "one", "two", "three", "four", "five", "six", "seven",
    "eight", "nine", "ten", "eleven", "noon"
};
static const char *const minute_names[7] =
    { "", "five", "ten", "a quarter", "twenty", "twenty five", "half" };

void
dozenaltime (char *buf, int len)
{
    int hours_per_day = 12, is_semidiurnal = 0;
    double decimal_secs;
    int dozenal_secs, dz_sec, dz_min, dz_hr;
    time_t rawtime;
    struct tm *timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    if (is_semidiurnal)
        hours_per_day = 24;
    decimal_secs = tm_to_secs (timeinfo);
    dozenal_secs =
        (int) (decimal_secs / SECS_PER_DAY * hours_per_day * 144 * 144);
    dz_sec = dozenal_secs % 144;
    dz_min = (int) (dozenal_secs / 144.0) % 144;
    dz_hr = (int) (dozenal_secs / 144.0 / 144.0);

    snprintf (buf, len, "%s%s%s%s:%s%s",
              is_semidiurnal ? duodecimal_chars[dz_hr / 12] : "",
              duodecimal_chars[dz_hr % 12], duodecimal_chars[dz_min / 12],
              duodecimal_chars[dz_min % 12], duodecimal_chars[dz_sec / 12],
              duodecimal_chars[dz_sec % 12]);
}

void
internettime (char *buf, int len)
{
    time_t secs = (time (NULL) + 3600) % SECS_PER_DAY;  /* BMT is UTC + 1 */
    snprintf (buf, len, "@%3.2f", (float) (secs) / SECS_PER_DAY * 1000);
}

void
fuzzytime (char *buf, int len)
{
    const char *hour_name, *min_name;
    int round_min;
    time_t rawtime;
    struct tm *timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    /* Powerslave. 1984 */
    if ((timeinfo->tm_min == 58) && (timeinfo->tm_hour == 23)) {
        snprintf (buf, len, "two minutes to midnight");
        return;
    }

    round_min = ((timeinfo->tm_min + 2) % 60) / 5;
    if (timeinfo->tm_min <= 32) {       /* Minutes past hour */
        hour_name =
            timeinfo->tm_hour ==
            12 ? hour_names[12] : hour_names[timeinfo->tm_hour % 12];
        min_name = minute_names[round_min];
    }
    else {                      /* Minutes to next hour */
        hour_name =
            timeinfo->tm_hour ==
            11 ? hour_names[12] : hour_names[(timeinfo->tm_hour + 1) % 12];
        min_name = minute_names[(12 - round_min) % 12];
    }

    snprintf (buf, len, "%s%s%s%s%s",
              timeinfo->tm_min % 5 ? "about " : "",
              min_name,
              (round_min == 0) ? "" : (timeinfo->tm_min <=
                                       32) ? " past " : " to ", hour_name,
              (round_min == 0)
              && (timeinfo->tm_hour % 12 != 0) ? " o'clock" : "");
}

void
hextime (char *buf, int len)
{
    time_t rawtime;
    struct tm *timeinfo;
    int decimal_secs, hexa_secs;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    decimal_secs = tm_to_secs (timeinfo);
    hexa_secs = (int) ((float) decimal_secs / SECS_PER_DAY * 0x10000);
    snprintf (buf, len, "%X_%02X_%X",
              hexa_secs >> 12, (hexa_secs >> 4) & 0xFF, hexa_secs & 0xF);
}

/* Alien code binary time - original idea taken from
   https://github.com/raboof/notion/blob/main/contrib/statusd/statusd_binclock.lua */
//#define ALIEN_SHOW_SECS
#define BIT(x, b) (((x) >> (b)) & 0x1)
void
alientime (char *buf, int len)
{
    time_t rawtime;
    struct tm *timeinfo;
    int i, alien_index;
    const char alien_codes[4] = { '_', '.', '\'', ':' };
    // const char alien_codes[4] = {'-', ',', '^', '='};
#ifdef ALIEN_SHOW_SECS
    const int max_len = 14;
    const char separator = '/';
#else
    const int max_len = 7;
#endif
    if (len < max_len)
        return;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    for (i = 0; i < 6; i++) {
        alien_index =
            (BIT (timeinfo->tm_hour, 5 - i) << 1) + BIT (timeinfo->tm_min,
                                                         5 - i);
        buf[i] = alien_codes[alien_index];
#ifdef ALIEN_SHOW_SECS
        buf[i + 7] = alien_codes[BIT (timeinfo->tm_sec, 5 - i)];
#endif
    }
#ifdef ALIEN_SHOW_SECS
    buf[6] = separator;
#endif
    buf[max_len - 1] = '\0';
}

void
datetime (char *buf, int len)
{
    time_t rawtime;
    struct tm *timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (buf, len, fmt, timeinfo);
}
