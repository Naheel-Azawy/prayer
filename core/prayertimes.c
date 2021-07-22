#include <math.h>
#include "prayertimes.h"

method METHODS[] = {{18,   true, 0, false, 17},   /*  0 CUSTOM             */
                    {18.5, true, 0, true,  90},   /*  1 MAKKAH             */
                    {19.5, true, 0, false, 17.5}, /*  2 EGYPT              */
                    {18,   true, 0, true,  90},   /*  3 QATAR              */
                    {18,   true, 0, false, 18},   /*  4 KARACHI            */
                    {15,   true, 0, false, 15},   /*  5 ISNA               */
                    {18,   true, 0, false, 17},   /*  6 MWL                */
                    {18,   true, 3, false, 17},   /*  7 ALGERIA            */
                    {18,   true, 0, false, 18},   /*  8 JORDAN             */
                    {18,   true, 0, false, 17.5}, /*  9 KUWAIT             */
                    {18,   true, 0, false, 17},   /* 10 ENGLAND_BIRMINGHAM */
                    {18,   true, 0, false, 17},   /* 11 ENGLAND_LONDON     */
                    {18,   true, 0, false, 17},   /* 12 GERMANY_MUNCHEN    */
                    {18,   true, 0, false, 17}};  /* 13 GERMANY_AACHEN     */

#define NUM_ITERATIONS 1 /* number of iterations needed to compute times */

void pt_init(prayer_time *pt) {
  pt->opts.calc_method = METHODS[MAKKAH];
  pt->opts.asr_juristic = SHAFII;
  pt->opts.duhr_minutes = 0;
  pt->opts.adjust_high_lats = ANGLE_BASED;

  pt->opts.offsets[0] = 0;
  pt->opts.offsets[1] = 0;
  pt->opts.offsets[2] = 0;
  pt->opts.offsets[3] = 0;
  pt->opts.offsets[4] = 0;
  pt->opts.offsets[5] = 0;
  pt->opts.offsets[6] = 0;

  pt->opts.iqama[0] = 25;
  pt->opts.iqama[1] = 20;
  pt->opts.iqama[2] = 25;
  pt->opts.iqama[3] = 10;
  pt->opts.iqama[4] = 20;
}

void pt_full
(
 short *times, int year, int month, int day,

 byte  calc_method, // calculation method
 float fajr_angle,
 bool  magrib_is_minuets, // true = minutes after sunset, false = angle
 float magrib_val,
 bool  isha_is_minuets, // true = minutes after sunset, false = angle
 float isha_val,

 byte asr_juristic, // Juristic method for Asr
 byte duhr_minutes, // minutes after mid-day for Duhr
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
 short iqama_isha
 ) {
  prayer_time pt;

  if (calc_method != (byte) -1) {
    pt.opts.calc_method = METHODS[calc_method];
  } else {
    pt.opts.calc_method.fajr_angle = fajr_angle;
    pt.opts.calc_method.magrib_is_minuets = magrib_is_minuets;
    pt.opts.calc_method.magrib_val = magrib_val;
    pt.opts.calc_method.isha_is_minuets = isha_is_minuets;
    pt.opts.calc_method.isha_val = isha_val;
  }
  pt.opts.asr_juristic = asr_juristic;
  pt.opts.duhr_minutes = duhr_minutes;
  pt.opts.adjust_high_lats = adjust_high_lats;
  pt.opts.lat = lat;
  pt.opts.lng = lng;
  pt.opts.time_zone = time_zone;
  pt.opts.offsets[0] = offset_fajr;
  pt.opts.offsets[1] = offset_sunrise;
  pt.opts.offsets[2] = offset_duhr;
  pt.opts.offsets[3] = offset_asr;
  pt.opts.offsets[4] = offset_sunset;
  pt.opts.offsets[5] = offset_magrib;
  pt.opts.offsets[6] = offset_isha;
  pt.opts.iqama[0] = iqama_fajr;
  pt.opts.iqama[1] = iqama_duhr;
  pt.opts.iqama[2] = iqama_asr;
  pt.opts.iqama[3] = iqama_magrib;
  pt.opts.iqama[4] = iqama_isha;

  calculate_for(&pt, year, month, day);

  for (int i = 0; i < PT_TIMES_ALL_LEN; ++i) {
    times[i] = pt.times[i];
  }
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
  return floor(365.25 * (year + 4716)) +
    floor(30.6001 * (month + 1)) + day + B - 1524.5;
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
  double e = 23.439 - (0.00000036 * D);
  double d = darcsin(dsin(e) * dsin(L));
  double RA = darctan2((dcos(e) * dsin(L)), (dcos(L))) / 15.0;
  RA = fixhour(RA);
  self->equation_of_time = q / 15.0 - RA;
  self->sun_declination = d;
}

// compute mid-day (Duhr, Zawal) time
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
  double fajr = compute_time(self, 180 - self->opts.calc_method.fajr_angle, times[0]);
  double sunrise = compute_time(self, 180 - 0.833, times[1]);
  calc_sun_position(self, self->j_date + times[2]);
  double duhr = compute_mid_day(self);
  double asr = compute_asr(self, 1 + self->opts.asr_juristic, times[3]);
  double sunset = compute_time(self, 0.833, times[4]);
  double maghrib = compute_time(self, self->opts.calc_method.magrib_val, times[5]);
  double isha = compute_time(self, self->opts.calc_method.isha_val, times[6]);

  times[0] = fajr;
  times[1] = sunrise;
  times[2] = duhr;
  times[3] = asr;
  times[4] = sunset;
  times[5] = maghrib;
  times[6] = isha;
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

  double fajr_diff = night_portion(self, self->opts.calc_method.fajr_angle) * night_time;

  if (isnan(times[0]) || time_diff(times[0], times[1]) > fajr_diff) {
    times[0] = times[1] - fajr_diff;
  }

  // Adjust Isha
  double isha_angle = (self->opts.calc_method.isha_is_minuets) ? self->opts.calc_method.isha_val : 18;
  double isha_diff = night_portion(self, isha_angle) * night_time;
  if (isnan(times[6]) || time_diff(times[4], times[6]) > isha_diff) {
    times[6] = times[4] + isha_diff;
  }

  // Adjust Maghrib
  double maghrib_angle = (self->opts.calc_method.magrib_is_minuets) ? self->opts.calc_method.magrib_val : 4;
  double maghrib_diff = night_portion(self, maghrib_angle) * night_time;
  if (isnan(times[5]) || time_diff(times[4], times[5]) > maghrib_diff) {
    times[5] = times[4] + maghrib_diff;
  }
}

// adjust times in a prayer time array
static void adjust_times(prayer_time *self, double *times) {
  for (int i = 0; i < PT_TIMES_CALC_LEN; i++) {
    times[i] += self->opts.time_zone - self->opts.lng / 15;
  }

  times[2] += self->opts.duhr_minutes / 60; // Duhr
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

byte next_time(short *times, short now) {
  FOR_PRAYER(p, TIMES_WITH_IQAMA, {
      // no idea why but needed in arduino...
      // otherwise it will go beyond array
      if (p > ISHA_IQAMA)
        break;
      if (times[p] >= now)
        return p;
    });
  return FAJR;
}

bool is_praying_time(short *times, short now, bool or_iqama) {
  byte next = next_time(times, now);
  if (!or_iqama)
    next = closest_index(next);
  short time = times[next];
  short diff = (short) (time - now);
  return diff == 0;
}

// returns the remaining time in minutes short time
short remaining_to(short *times, short now, byte next) {
  short time = times[next];
  short diff = (short) (time - now);
  if (diff < 0) {
    if (next == FAJR) {
      diff = (short) (time + (24 * 60) - now) * -1;
    }
  }
  return diff;
}
