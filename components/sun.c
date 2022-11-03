#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "meeus.h"

void
get_sun_coord (double phi, double L, double *A, double *h)
{
    double jd, jde, H;
    double alpha, delta;

    dt_get_current_jd (0, &jd);
    jde = dy_ut_to_dt (jd);
    /* Get sun apparent coordinates. Low accuracy is more than enough */
    sun_apparent_equatorial_coord (jde, &alpha, &delta, 0);

    /* We now have the sun geocentric coordinates.
       Let's convert them to horizontal coordinates
       at the observer's latitude and longitude */
    coo_get_local_hour_angle (jd, L, alpha, &H, 1);
    coo_equ_to_hor (H, delta, phi, A, h);
    /* and correct altitude for the atmospheric refraction
       as we want to get the "measured" coordinates */
    *h += ref_refraction_true_to_apparent (*h, 1) / 60;
}

void sun(char *buf, int len)
{
    double A, h;
    get_sun_coord(43.56, -7.12, &A, &h);
    snprintf (buf, len, "α=%.2f, γ=%.2f", h, A);
}
