#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "prayertimes.h"

method METHODS[] = {{18,   true, 0, false, 18},   /*  1 KARACHI            */
                    {15,   true, 0, false, 15},   /*  2 ISNA               */
                    {18,   true, 0, false, 17},   /*  3 MWL                */
                    {18.5, true, 0, true,  90},   /*  4 MAKKAH             */
                    {19.5, true, 0, false, 17.5}, /*  5 EGYPT              */
                    {18,   true, 0, false, 17},   /*  7 CUSTOM             */
                    {18,   true, 0, true,  90},   /*  8 QATAR              */
                    {18,   true, 3, false, 17},   /* 11 ALGERIA            */
                    {18,   true, 0, false, 18},   /* 18 JORDAN             */
                    {18,   true, 0, false, 17.5}, /* 19 KUWAIT             */
                    {18,   true, 0, false, 17},   /* 22 ENGLAND_BIRMINGHAM */
                    {18,   true, 0, false, 17},   /* 23 ENGLAND_LONDON     */
                    {18,   true, 0, false, 17},   /* 24 GERMANY_MUNCHEN    */
                    {18,   true, 0, false, 17}};  /* 25 GERMANY_AACHEN     */

#define is_iqama(i)                                     \
  (i == FAJR_IQAMA || i == DUHR_IQAMA || i == ASR_IQAMA \
   || i == MAGRIB_IQAMA || i == ISHA_IQAMA)

#define NUM_ITERATIONS 1 /* number of iterations needed to compute times */

method int_to_method(int i) {
  return METHODS[i];
}

prayer_time *prayer_time_new() {
  prayer_time *n = (prayer_time *) malloc(sizeof(prayer_time));

  n->opts.calc_method = int_to_method(MAKKAH);
  n->opts.asr_juristic = SHAFII;
  n->opts.dhuhr_minutes = 0;
  n->opts.adjust_high_lats = ANGLE_BASED;

  n->opts.offsets[0] = 0;
  n->opts.offsets[1] = 0;
  n->opts.offsets[2] = 0;
  n->opts.offsets[3] = 0;
  n->opts.offsets[4] = 0;
  n->opts.offsets[5] = 0;
  n->opts.offsets[6] = 0;

  n->opts.iqama[0] = 25;
  n->opts.iqama[1] = 20;
  n->opts.iqama[2] = 25;
  n->opts.iqama[3] = 10;
  n->opts.iqama[4] = 20;

  return n;
}

/* ---------------------- Trigonometric Functions ----------------------- */

// range reduce angle in degrees.
static double fixangle(double a) {
  a = a - (360 * (floor(a / 360.0)));
  a = a < 0 ? (a + 360) : a;
  return a;
}

// range reduce hours to 0..23
static double fixhour(double a) {
  a = a - 24.0 * floor(a / 24.0);
  a = a < 0 ? (a + 24) : a;
  return a;
}

// radian to degree
static double radians_to_degrees(double alpha) {
  return ((alpha * 180.0) / M_PI);
}

// deree to radian
static double DegreesToRadians(double alpha) {
  return ((alpha * M_PI) / 180.0);
}

// degree sin
static double dsin(double d) {
  return (sin(DegreesToRadians(d)));
}

// degree cos
static double dcos(double d) {
  return (cos(DegreesToRadians(d)));
}

// degree tan
static double dtan(double d) {
  return (tan(DegreesToRadians(d)));
}

// degree arcsin
static double darcsin(double x) {
  double val = asin(x);
  return radians_to_degrees(val);
}

// degree arccos
static double darccos(double x) {
  double val = acos(x);
  return radians_to_degrees(val);
}

// degree arctan2
static double darctan2(double y, double x) {
  double val = atan2(y, x);
  return radians_to_degrees(val);
}

// degree arccot
static double darccot(double x) {
  double val = atan2(1.0, x);
  return radians_to_degrees(val);
}

/* ---------------------- Julian Date Functions ----------------------- */

// calculate julian date from a calendar date
static double julian_date(int year, int month, int day) {
  if (month <= 2) {
    year -= 1;
    month += 12;
  }
  double A = floor(year / 100.0);
  double B = 2 - A + floor(A / 4.0);
  return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + B - 1524.5;
}

/* ---------------------- Calculation Functions ----------------------- */

/*
  References:
  http://www.ummah.net/astronomy/saltime
  http://aa.usno.navy.mil/faq/docs/SunApprox.html
  compute declination angle of sun and equation of time
*/
static void calc_sun_position(prayer_time *self, double jd) {
  double D = jd - 2451545;
  double g = fixangle(357.529 + 0.98560028 * D);
  double q = fixangle(280.459 + 0.98564736 * D);
  double L = fixangle(q + (1.915 * dsin(g)) + (0.020 * dsin(2 * g)));
  // double R = 1.00014 - 0.01671 * [self dcos:g] - 0.00014 * [self dcos: (2*g)];
  double e = 23.439 - (0.00000036 * D);
  double d = darcsin(dsin(e) * dsin(L));
  double RA = (darctan2((dcos(e) * dsin(L)), (dcos(L)))) / 15.0;
  RA = fixhour(RA);
  self->equation_of_time = q / 15.0 - RA;
  self->sun_declination = d;
}

// compute mid-day (Dhuhr, Zawal) time
// call `calc_sun_position(self, self->j_date + t);` to function correctly
static double compute_mid_day(prayer_time *self) {
  double T = self->equation_of_time;
  return fixhour(12 - T);
}

// compute time for a given angle G
static double compute_time(prayer_time *self, double G, double t) {
  calc_sun_position(self, self->j_date + t);
  double D = self->sun_declination;
  double Z = compute_mid_day(self);
  double Beg = -dsin(G) - dsin(D) * dsin(self->opts.lat);
  double Mid = dcos(D) * dcos(self->opts.lat);
  double V = darccos(Beg / Mid) / 15.0;
  return Z + (G > 90 ? -V : V);
}

// compute the time of Asr
// Shafii: step=1, Hanafi: step=2
static double compute_asr(prayer_time *self, double step, double t) {
  calc_sun_position(self, self->j_date + t);
  double D = self->sun_declination;
  double G = -darccot(step + dtan(fabs(self->opts.lat - D)));
  return compute_time(self, G, t);
}

/* ---------------------- Misc Functions ----------------------- */

// compute the difference between two times
static double time_diff(double time1, double time2) {
  return fixhour(time2 - time1);
}

/* ---------------------- Compute Prayer Times ----------------------- */

// convert hours to day portions
static void day_portion(double *times) {
  for (int i = 0; i < PT_TIMES_CALC_LEN; i++)
    times[i] /= 24;
}

// compute prayer times at given julian date
static void compute_times(prayer_time *self, double *times) {
  day_portion(times);
  double Fajr = compute_time(self, 180 - self->opts.calc_method.fajr_angle, times[0]);
  double Sunrise = compute_time(self, 180 - 0.833, times[1]);
  calc_sun_position(self, self->j_date + times[2]);
  double Dhuhr = compute_mid_day(self);
  double Asr = compute_asr(self, 1 + self->opts.asr_juristic, times[3]);
  double Sunset = compute_time(self, 0.833, times[4]);
  double Maghrib = compute_time(self, self->opts.calc_method.magrib_val, times[5]);
  double Isha = compute_time(self, self->opts.calc_method.isha_val, times[6]);

  times[0] = Fajr;
  times[1] = Sunrise;
  times[2] = Dhuhr;
  times[3] = Asr;
  times[4] = Sunset;
  times[5] = Maghrib;
  times[6] = Isha;

}

// convert double to short
static short float_to_minutes_short(double time) {
  if (isnan(time)) {
    return -1;
  }
  time = fixhour(time + 0.5 / 60.0); // add 0.5 minutes to round
  short hours = (short) floor(time);
  short minutes = (short) floor((time - hours) * 60.0);
  return (short) (minutes + hours * 60);
}

// the night portion used for adjusting times in higher latitudes
static double night_portion(prayer_time *self, double angle) {
  double calc = 0;
  if (self->opts.adjust_high_lats == ANGLE_BASED)
    calc = (angle) / 60.0;
  else if (self->opts.adjust_high_lats == MIDNIGHT)
    calc = 0.5;
  else if (self->opts.adjust_high_lats == ONE_SEVENTH)
    calc = 0.14286;
  return calc;
}

// adjust Fajr, Isha and Maghrib for locations in higher latitudes
static void adjust_high_lat_times(prayer_time *self, double *times) {
  double night_time = time_diff(times[4], times[1]); // sunset to sunrise

  double FajrDiff = night_portion(self, self->opts.calc_method.fajr_angle) * night_time;

  if (isnan(times[0]) || time_diff(times[0], times[1]) > FajrDiff) {
    times[0] = times[1] - FajrDiff;
  }

  // Adjust Isha
  double IshaAngle = (self->opts.calc_method.isha_is_minuets) ? self->opts.calc_method.isha_val : 18;
  double IshaDiff = night_portion(self, IshaAngle) * night_time;
  if (isnan(times[6]) || time_diff(times[4], times[6]) > IshaDiff) {
    times[6] = times[4] + IshaDiff;
  }

  // Adjust Maghrib
  double MaghribAngle = (self->opts.calc_method.magrib_is_minuets) ? self->opts.calc_method.magrib_val : 4;
  double MaghribDiff = night_portion(self, MaghribAngle) * night_time;
  if (isnan(times[5]) || time_diff(times[4], times[5]) > MaghribDiff) {
    times[5] = times[4] + MaghribDiff;
  }
}

// adjust times in a prayer time array
static void adjust_times(prayer_time *self, double *times) {
  for (int i = 0; i < PT_TIMES_CALC_LEN; i++) {
    times[i] += self->opts.time_zone - self->opts.lng / 15;
  }

  times[2] += self->opts.dhuhr_minutes / 60; // Dhuhr
  if (self->opts.calc_method.magrib_is_minuets) // Maghrib
    times[5] = times[4] + self->opts.calc_method.magrib_val / 60;
  if (self->opts.calc_method.isha_is_minuets) // Isha
    times[6] = times[5] + self->opts.calc_method.isha_val / 60;

  if (self->opts.adjust_high_lats != NONE)
    adjust_high_lat_times(self, times);
}

static void tune_times(prayer_time *self, double *times) {
  for (int i = 0; i < PT_TIMES_CALC_LEN; i++) {
    times[i] = times[i] + self->opts.offsets[i] / 60.0;
  }
}

// compute prayer times at given julian date
static void compute_day_times(prayer_time *self) {
  double times[] = { 5, 6, 12, 13, 18, 18, 18 }; // default times

  for (int i = 1; i <= NUM_ITERATIONS; i++) {
    compute_times(self, times);
  }

  adjust_times(self, times);
  tune_times(self, times);

  self->times[FAJR] = float_to_minutes_short(times[0]);
  self->times[FAJR_IQAMA] = (short) (self->times[FAJR] + self->opts.iqama[0]);
  self->times[SUNRISE] = float_to_minutes_short(times[1]);
  self->times[DUHR] = float_to_minutes_short(times[2]);
  self->times[DUHR_IQAMA] = (short) (self->times[DUHR] + self->opts.iqama[1]);
  self->times[ASR] = float_to_minutes_short(times[3]);
  self->times[ASR_IQAMA] = (short) (self->times[ASR] + self->opts.iqama[2]);
  self->times[SUNSET] = float_to_minutes_short(times[4]);
  self->times[MAGRIB] = float_to_minutes_short(times[5]);
  self->times[MAGRIB_IQAMA] = (short) (self->times[MAGRIB] + self->opts.iqama[3]);
  self->times[ISHA] = float_to_minutes_short(times[6]);
  self->times[ISHA_IQAMA] = (short) (self->times[ISHA] + self->opts.iqama[4]);
}

/* -------------------- Interface Functions -------------------- */

// setup prayer times
void calculate_for(prayer_time *self, int year, int month, int day) {
  self->j_date = julian_date(year, month, day);
  double lon_diff = self->opts.lng / (15.0 * 24.0);
  self->j_date = self->j_date - lon_diff;
  compute_day_times(self);
}

byte closest_index(byte i) {
  switch (i) {
  case FAJR:
  case FAJR_IQAMA:
    return FAJR;
  case SUNRISE:
    return SUNRISE;
  case DUHR:
  case DUHR_IQAMA:
    return DUHR;
  case ASR:
  case ASR_IQAMA:
    return ASR;
  case SUNSET:
  case MAGRIB:
  case MAGRIB_IQAMA:
    return MAGRIB;
  case ISHA:
  case ISHA_IQAMA:
    return ISHA;
  default:
    return -1;
  }
}

byte iqama_index(byte i) {
  switch (i) {
  case FAJR:
  case FAJR_IQAMA:
    return 0;
  case SUNRISE:
  case DUHR:
  case DUHR_IQAMA:
    return 1;
  case ASR:
  case ASR_IQAMA:
    return 2;
  case SUNSET:
  case MAGRIB:
  case MAGRIB_IQAMA:
    return 3;
  case ISHA:
  case ISHA_IQAMA:
    return 4;
  default:
    return -1;
  }
}

byte get_next_time(prayer_time *self, short now) {
  FOR_PRAYER(p, SIMPLE_TIMES_WITH_IQAMA, {
      // no idea why but needed in arduino...
      // otherwise it will go beyond array
      if (p > ISHA_IQAMA)
        break;
      if (self->times[p] >= now)
        return p;
    });
  return FAJR;
}

bool is_praying_time(prayer_time *self, short now, bool or_iqama) {
  byte next = get_next_time(self, now);
  if (!or_iqama)
    next = closest_index(next);
  short time = self->times[next];
  short diff = (short) (time - now);
  return diff == 0;
}

short create_time(int h, int m) {
  return (short) (h * 60 + m);
}

// returns the remaining time in minutes short time
// if is now, then hours is 30
// if next is iqama time, hours is 31
short get_short_next_remaining(prayer_time *self, short now, byte next) {
  short time = self->times[next];
  short diff = (short) (time - now);
  bool is_now = diff == 0;
  if (is_now) {
    return 30 * 60 + next;
  }
  if (diff < 0) {
    diff = (short) -diff;
    if (next == FAJR) {
      diff = (short) (time + (24 * 60) - now);
    }
  }
  if (is_iqama(next)) {
    diff += 31 * 60;
  }
  return diff;
}

short get_time(prayer_time *self, byte i) {
  return self->times[i];
}

void set_opts(prayer_time *self,

              byte calc_method, // calculation method
              float fajr_angle,
              bool magrib_is_minuets, // true = minutes after sunset, false = angle
              float magrib_val,
              bool isha_is_minuets, // true = minutes after sunset, false = angle
              float isha_val,

              byte asr_juristic, // Juristic method for Asr
              byte dhuhr_minutes, // minutes after mid-day for Dhuhr
              byte adjust_high_lats, // adjusting method for higher latitudes

              double lat, // latitude
              double lng, // longitude
              double time_zone, // time-zone

              short offset_fajr,
              short offset_sunrise,
              short offset_duhr,
              short offset_asr,
              short offset_sunset,
              short offset_magrib,
              short offset_isha,

              short iqama_fajr,
              short iqama_duhr,
              short iqama_asr,
              short iqama_magrib,
              short iqama_isha) {

  if (calc_method != (byte) -1) {
    self->opts.calc_method = METHODS[calc_method];
  } else {
    self->opts.calc_method.fajr_angle = fajr_angle;
    self->opts.calc_method.magrib_is_minuets = magrib_is_minuets;
    self->opts.calc_method.magrib_val = magrib_val;
    self->opts.calc_method.isha_is_minuets = isha_is_minuets;
    self->opts.calc_method.isha_val = isha_val;
  }
  self->opts.asr_juristic = asr_juristic;
  self->opts.dhuhr_minutes = dhuhr_minutes;
  self->opts.adjust_high_lats = adjust_high_lats;
  self->opts.lat = lat;
  self->opts.lng = lng;
  self->opts.time_zone = time_zone;
  self->opts.offsets[0] = offset_fajr;
  self->opts.offsets[1] = offset_sunrise;
  self->opts.offsets[2] = offset_duhr;
  self->opts.offsets[3] = offset_asr;
  self->opts.offsets[4] = offset_sunset;
  self->opts.offsets[5] = offset_magrib;
  self->opts.offsets[6] = offset_isha;
  self->opts.iqama[0] = iqama_fajr;
  self->opts.iqama[1] = iqama_duhr;
  self->opts.iqama[2] = iqama_asr;
  self->opts.iqama[3] = iqama_magrib;
  self->opts.iqama[4] = iqama_isha;

}
