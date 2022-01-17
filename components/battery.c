/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include "util.h"

const char *bat = "BAT0";

int battery_perc(void)
{
    int perc;
    char path[PATH_MAX];

    if (esnprintf(path, sizeof(path),
                  "/sys/class/power_supply/%s/capacity", bat) < 0) {
        return -1;
    }
    if (pscanf(path, "%d", &perc) != 1) {
        return -1;
    }

    return perc;
}

char *
battery_state(void)
{
    static struct {
        char *state;
        char *symbol;
    } map[] = {
        { "Charging",    "+" },
        { "Discharging", "-" },
        { "Full",        "o" },
    };
    size_t i;
    char path[PATH_MAX], state[12];

    if (esnprintf(path, sizeof(path),
                  "/sys/class/power_supply/%s/status", bat) < 0) {
        return NULL;
    }
    if (pscanf(path, "%12s", state) != 1) {
        return NULL;
    }

    for (i = 0; i < LEN(map); i++) {
        if (!strcmp(map[i].state, state)) {
            break;
        }
    }
    return (i == LEN(map)) ? "?" : map[i].symbol;
}

void battery(char *buf, int len)
{
    int perc = battery_perc();
    char *state = battery_state();
    snprintf(buf, len, "%-3d%% %s", perc, state);
}
