#ifndef PRAYERTIMES_H_
#define PRAYERTIMES_H_

#include <stdbool.h>

/* DEFINITIONS -------------------------------------------------- */

/* Juristic Methods */
#define SHAFII 0 /* Shafii (standard) */
#define HANAFI 1 /* Hanafi */

/* Adjusting Methods for Higher Latitudes */
#define NONE        0 /* No adjustment */
#define MIDNIGHT    1 /* middle of night */
#define ONE_SEVENTH 2 /* 1/7th of night */
#define ANGLE_BASED 3 /* angle/60th of night */

/* Time Formats */
#define TIME_24    0 /* 24-hour format */
#define TIME_12    1 /* 12-hour format */
#define TIME_12_NS 2 /* 12-hour format with no suffix */

/* times indices */
#define FAJR          0
#define FAJR_IQAMA    1
#define SUNRISE       2
#define DUHR          3
#define DUHR_IQAMA    4
#define ASR           5
#define ASR_IQAMA     6
#define SUNSET        7
#define MAGRIB        8
#define MAGRIB_IQAMA  9
#define ISHA         10
#define ISHA_IQAMA   11

/* Calculation Methods */
#define KARACHI             0 /* University of Islamic Sciences, Karachi */
#define ISNA                1 /* Islamic Society of North America (ISNA) */
#define MWL                 2 /* Muslim World League (MWL) */
#define MAKKAH              3 /* Umm al-Qura, Makkah */
#define EGYPT               4 /* Egyptian General Authority of Survey */
#define CUSTOM              5 /* Custom Setting */
#define QATAR               6 /* Qatar Calendar House */
#define ALGERIA             7 /* Algeria, Ministry of Religious Affairs and Endowments */
#define JORDAN              8 /* Jordan, Ministry of Awqaf, Islamic Affairs and Holy Places */
#define KUWAIT              9 /* Kuwait, Ministry of Awqaf and Islamic Affairs */
#define ENGLAND_BIRMINGHAM 10 /* United Kingdom, Birmingham Central Mosque */
#define ENGLAND_LONDON     11 /* United Kingdom, London Central Mosque */
#define GERMANY_MUNCHEN    12 /* Germany, Islamic Center of Munich */
#define GERMANY_AACHEN     13 /* Germany, Islamic Center of Aachen */

/* Other Stuff */
#define PT_TIMES_CALC_LEN 7
#define PT_TIMES_ALL_LEN 12
#define PT_TIMES_IQAMA_LEN 5

/* Looping */
#define PRAY_TIMES_ONLY_LEN 5
#define PRAY_TIMES_ONLY { FAJR, DUHR, ASR, MAGRIB, ISHA }

#define SIMPLE_TIMES_LEN 6
#define SIMPLE_TIMES { FAJR, SUNRISE, DUHR, ASR, MAGRIB, ISHA }

#define SIMPLE_TIMES_WITH_IQAMA_LEN 11
#define SIMPLE_TIMES_WITH_IQAMA { FAJR, FAJR_IQAMA, SUNRISE, DUHR, DUHR_IQAMA, ASR, \
                                  ASR_IQAMA, MAGRIB, MAGRIB_IQAMA, ISHA, ISHA_IQAMA }

#define FOR_PRAYER(var, type, body) {           \
    static byte __arr[] = type;                 \
    byte __count;                               \
    byte var;                                   \
    for (__count = 0, var = __arr[0];           \
         __count < type##_LEN;                  \
         ++__count, var = __arr[__count])       \
      body                                      \
        }

typedef unsigned char byte;

/* METHOD STRUCT ------------------------------------------------ */

typedef struct {
  float fajr_angle;
  bool magrib_is_minuets; // true = minutes after sunset, false = angle
  float magrib_val;
  bool isha_is_minuets; // true = minutes after sunset, false = angle
  float isha_val;
} method;

/* MAIN STRUCT -------------------------------------------------- */

typedef struct {

  /* public -------------------- */
  method *calc_method; // calculation method
  byte asr_juristic; // Juristic method for Asr
  byte dhuhr_minutes; // minutes after mid-day for Dhuhr
  byte adjust_high_lats; // adjusting method for higher latitudes

  double lat; // latitude
  double lng; // longitude
  double time_zone; // time-zone

  short offsets[PT_TIMES_CALC_LEN];
  short iqama[PT_TIMES_ALL_LEN]; // iqama after {fajr, duhr, asr, magrib, isha} minutes

  /* private ------------------- */
  double j_date; // Julian date
  short times[PT_TIMES_ALL_LEN]; // cached times
  double equation_of_time;
  double sun_declination;
  short time_now;

} prayer_time;

/* PROTOTYPES --------------------------------------------------- */

void         init_methods();
prayer_time *prayer_time_new();
method      *int_to_method(int i);
void         calculate_for_full(prayer_time *self, int year, int month, int day, double latitude, double longitude, double t_zone);
void         calculate_for_date(prayer_time *self, int year, int month, int day);
byte         closest_index(byte i);
byte         iqama_index(byte i);
byte         get_next_time(prayer_time *self, short now);
bool         is_praying_time(prayer_time *self, short now, bool or_iqama);
void         tune(prayer_time *self, int fajr, int sunrise, int duhr, int asr, int sunset, int magrib, int isha);
void         set_custom_method(prayer_time *self, method *m);
void         set_fajr_angle(prayer_time *self, float angle);
void         set_maghrib_angle(prayer_time *self, float angle);
void         set_isha_angle(prayer_time *self, float angle);
void         set_maghrib_minutes(prayer_time *self, float minutes);
void         set_isha_minutes(prayer_time *self, float minutes);
short        create_time(int h, int m);
short        get_short_next_remaining(prayer_time *self, short now, byte next);

#endif /* PRAYERTIMES_H_ */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>


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

method *int_to_method(int i) {
  return &METHODS[i];
}

prayer_time *prayer_time_new() {
  prayer_time *n = (prayer_time *) malloc(sizeof(prayer_time));

  n->calc_method = int_to_method(MAKKAH);
  n->asr_juristic = SHAFII;
  n->dhuhr_minutes = 0;
  n->adjust_high_lats = ANGLE_BASED;

  n->iqama[0] = 25;
  n->iqama[1] = 20;
  n->iqama[2] = 25;
  n->iqama[3] = 10;
  n->iqama[4] = 20;

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
  double Beg = -dsin(G) - dsin(D) * dsin(self->lat);
  double Mid = dcos(D) * dcos(self->lat);
  double V = darccos(Beg / Mid) / 15.0;
  return Z + (G > 90 ? -V : V);
}

// compute the time of Asr
// Shafii: step=1, Hanafi: step=2
static double compute_asr(prayer_time *self, double step, double t) {
  calc_sun_position(self, self->j_date + t);
  double D = self->sun_declination;
  double G = -darccot(step + dtan(fabs(self->lat - D)));
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
  double Fajr = compute_time(self, 180 - self->calc_method->fajr_angle, times[0]);
  double Sunrise = compute_time(self, 180 - 0.833, times[1]);
  calc_sun_position(self, self->j_date + times[2]);
  double Dhuhr = compute_mid_day(self);
  double Asr = compute_asr(self, 1 + self->asr_juristic, times[3]);
  double Sunset = compute_time(self, 0.833, times[4]);
  double Maghrib = compute_time(self, self->calc_method->magrib_val, times[5]);
  double Isha = compute_time(self, self->calc_method->isha_val, times[6]);

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
  if (self->adjust_high_lats == ANGLE_BASED)
    calc = (angle) / 60.0;
  else if (self->adjust_high_lats == MIDNIGHT)
    calc = 0.5;
  else if (self->adjust_high_lats == ONE_SEVENTH)
    calc = 0.14286;
  return calc;
}

// adjust Fajr, Isha and Maghrib for locations in higher latitudes
static void adjust_high_lat_times(prayer_time *self, double *times) {
  double night_time = time_diff(times[4], times[1]); // sunset to sunrise

  double FajrDiff = night_portion(self, self->calc_method->fajr_angle) * night_time;

  if (isnan(times[0]) || time_diff(times[0], times[1]) > FajrDiff) {
    times[0] = times[1] - FajrDiff;
  }

  // Adjust Isha
  double IshaAngle = (self->calc_method->isha_is_minuets) ? self->calc_method->isha_val : 18;
  double IshaDiff = night_portion(self, IshaAngle) * night_time;
  if (isnan(times[6]) || time_diff(times[4], times[6]) > IshaDiff) {
    times[6] = times[4] + IshaDiff;
  }

  // Adjust Maghrib
  double MaghribAngle = (self->calc_method->magrib_is_minuets) ? self->calc_method->magrib_val : 4;
  double MaghribDiff = night_portion(self, MaghribAngle) * night_time;
  if (isnan(times[5]) || time_diff(times[4], times[5]) > MaghribDiff) {
    times[5] = times[4] + MaghribDiff;
  }
}

// adjust times in a prayer time array
static void adjust_times(prayer_time *self, double *times) {
  for (int i = 0; i < PT_TIMES_CALC_LEN; i++) {
    times[i] += self->time_zone - self->lng / 15;
  }

  times[2] += self->dhuhr_minutes / 60; // Dhuhr
  if (self->calc_method->magrib_is_minuets) // Maghrib
    times[5] = times[4] + self->calc_method->magrib_val / 60;
  if (self->calc_method->isha_is_minuets) // Isha
    times[6] = times[5] + self->calc_method->isha_val / 60;

  if (self->adjust_high_lats != NONE)
    adjust_high_lat_times(self, times);
}

static void tune_times(prayer_time *self, double *times) {
  for (int i = 0; i < PT_TIMES_CALC_LEN; i++) {
    times[i] = times[i] + self->offsets[i] / 60.0;
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
  self->times[FAJR_IQAMA] = (short) (self->times[FAJR] + self->iqama[0]);
  self->times[SUNRISE] = float_to_minutes_short(times[1]);
  self->times[DUHR] = float_to_minutes_short(times[2]);
  self->times[DUHR_IQAMA] = (short) (self->times[DUHR] + self->iqama[1]);
  self->times[ASR] = float_to_minutes_short(times[3]);
  self->times[ASR_IQAMA] = (short) (self->times[ASR] + self->iqama[2]);
  self->times[SUNSET] = float_to_minutes_short(times[4]);
  self->times[MAGRIB] = float_to_minutes_short(times[5]);
  self->times[MAGRIB_IQAMA] = (short) (self->times[MAGRIB] + self->iqama[3]);
  self->times[ISHA] = float_to_minutes_short(times[6]);
  self->times[ISHA_IQAMA] = (short) (self->times[ISHA] + self->iqama[4]);
}

/* -------------------- Interface Functions -------------------- */

// setup prayer times
void calculate_for_full(prayer_time *self, int year, int month, int day, double latitude, double longitude, double t_zone) {
  self->lat = latitude;
  self->lng = longitude;
  self->time_zone = t_zone;
  self->j_date = julian_date(year, month, day);
  double lon_diff = longitude / (15.0 * 24.0);
  self->j_date = self->j_date - lon_diff;
  compute_day_times(self);
}

// setup prayer times without changing the location
void calculate_for_date(prayer_time *self, int year, int month, int day) {
  calculate_for_full(self, year, month, day, self->lat, self->lng, self->time_zone);
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

// set time offsets
void tune(prayer_time *self, int fajr, int sunrise, int duhr, int asr, int sunset, int magrib, int isha) {
  self->offsets[0] = (short) fajr;
  self->offsets[1] = (short) sunrise;
  self->offsets[2] = (short) duhr;
  self->offsets[3] = (short) asr;
  self->offsets[4] = (short) sunset;
  self->offsets[5] = (short) magrib;
  self->offsets[6] = (short) isha;
}

// set the angle for calculating Fajr
void set_fajr_angle(prayer_time *self, float angle) {
  self->calc_method = int_to_method(CUSTOM);
  METHODS[CUSTOM].fajr_angle = angle;
}

// set the angle for calculating Maghrib
void set_maghrib_angle(prayer_time *self, float angle) {
  self->calc_method = int_to_method(CUSTOM);
  METHODS[CUSTOM].magrib_is_minuets = false;
  METHODS[CUSTOM].magrib_val = angle;
}

// set the angle for calculating Isha
void set_isha_angle(prayer_time *self, float angle) {
  self->calc_method = int_to_method(CUSTOM);
  METHODS[CUSTOM].isha_is_minuets = false;
  METHODS[CUSTOM].isha_val = angle;
}

// set the minutes after Sunset for calculating Maghrib
void set_maghrib_minutes(prayer_time *self, float minutes) {
  self->calc_method = int_to_method(CUSTOM);
  METHODS[CUSTOM].magrib_is_minuets = true;
  METHODS[CUSTOM].magrib_val = minutes;
}

// set the minutes after Maghrib for calculating Isha
void set_isha_minutes(prayer_time *self, float minutes) {
  self->calc_method = int_to_method(CUSTOM);
  METHODS[CUSTOM].isha_is_minuets = true;
  METHODS[CUSTOM].isha_val = minutes;
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
#include <LiquidCrystal.h>
#include <Time.h>

#define TIME_FORMAT      TIME_12
#define CALC_METHOD      QATAR
#define ASR_JURISTIC     SHAFII
#define ADJUST_HIGH_LATS ANGLE_BASED
#define LAT              25.2899589
#define LNG              51.4974742
#define TIMEZONE         3

#define START_YEAR 2019
#define START_MONTH 7
#define START_DAY 22
#define START_HOUR 22
#define START_MINUTE 17

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define PRINT  Serial.print
#define PRINT2 Serial.print

prayer_time *pt;
short pt_times[PT_TIMES_ALL_LEN];
byte nxt;
byte real_nxt;

const char *TIMES_NAMES[] = { "FAJR",
                              "FAJR_IQAMA",
                              "SUNRISE",
                              "DUHR",
                              "DUHR_IQAMA",
                              "ASR",
                              "ASR_IQAMA",
                              "SUNSET",
                              "MAGRIB",
                              "MAGRIB_IQAMA",
                              "ISHA",
                              "ISHA_IQAMA" };

void update_time() {
  pt->time_now = hour() * 60 + minute();
}

void setup_time() {
  setTime(START_HOUR, START_MINUTE, 60, START_DAY, START_MONTH, START_YEAR);
  update_time();
}

#define ARDUINO_PRINT_TIME(s, t, format) {      \
    short _h = t / 60, _m = t % 60, _p = 0;     \
    if (format == TIME_12 && _h >= 12)          \
      _p = 1, _h -= 12;                         \
    if (_h < 10) s.print('0');                  \
    s.print(_h);                                \
    s.print(':');                               \
    if (_m < 10) s.print('0');                  \
    s.print(_m);                                \
    if (format == TIME_12)                      \
      s.print(_p ? "PM" : "AM");                \
  }

void update() {
  update_time();

  if (pt->time_now == 0) { // if new day
    calculate_for_date(pt, year(), month(), day());
  }

  real_nxt = get_next_time(pt, pt->time_now);
  nxt = closest_index(real_nxt);

#define PRINT_BOTH(s) Serial.print(s); lcd.print(s);

  Serial.print("TIME\t\t");
  ARDUINO_PRINT_TIME(Serial, pt->time_now, TIME_FORMAT);
  Serial.print(" ");
  Serial.print(year());
  Serial.print("-");
  Serial.print(month());
  Serial.print("-");
  Serial.print(day());
  Serial.println('\n');

  lcd.setCursor(0, 0);
  ARDUINO_PRINT_TIME(lcd, pt->time_now, TIME_FORMAT);
  lcd.print(' ');

  short nxt_time;
  FOR_PRAYER(p, SIMPLE_TIMES, {
      Serial.print(TIMES_NAMES[p]);
      Serial.print("\t\t");
      ARDUINO_PRINT_TIME(Serial, pt->times[p], TIME_FORMAT);
      Serial.println();
      if (p == nxt) {
        lcd.print(TIMES_NAMES[p][0]);
        ARDUINO_PRINT_TIME(lcd, pt->times[p], TIME_FORMAT);
        lcd.setCursor(0, 1);
        nxt_time = get_short_next_remaining(pt, pt->time_now, real_nxt);
        if (nxt_time / 60 == 30) {
          PRINT_BOTH(TIMES_NAMES[nxt_time % 60]);
          PRINT_BOTH(" TIME");
        } else if (nxt_time / 60 == 31) {
          PRINT_BOTH("IQAMA ");
          ARDUINO_PRINT_TIME(Serial, nxt_time % 60, TIME_24);
          ARDUINO_PRINT_TIME(lcd, nxt_time % 60, TIME_24);
        } else {
          PRINT_BOTH("REMAINING ");
          ARDUINO_PRINT_TIME(Serial, nxt_time, TIME_24);
          ARDUINO_PRINT_TIME(lcd, nxt_time, TIME_24);
        }
        Serial.println();
        for (int i = 0; i < 16; ++i)
          lcd.print(' ');
      }
    });

  Serial.println();

}

void setup() {

  Serial.begin(9600);
  lcd.begin(16, 2);

  pt = prayer_time_new();
  setup_time();

  pt->calc_method = int_to_method(CALC_METHOD);
  pt->asr_juristic = ASR_JURISTIC;
  pt->adjust_high_lats = ADJUST_HIGH_LATS;
  tune(pt, 0, 0, 0, 0, 0, 0, 0);

  update_time();

  calculate_for_full(pt, year(), month(), day(),
                     LAT, LNG, TIMEZONE);

  update();

}

void loop() {
  if (millis() % 60000 == 0) {
    update();
  }
}
