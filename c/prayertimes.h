#ifndef PRAYERTIMES_H_
#define PRAYERTIMES_H_

#include <stdbool.h>

/* DEFINITIONS -------------------------------------------------- */

/* Juristic Methods --- */
#define SHAFII 0 /* Shafii (standard) */
#define HANAFI 1 /* Hanafi */

/* Adjusting Methods for Higher Latitudes --- */
#define NONE        0 /* No adjustment */
#define MIDNIGHT    1 /* middle of night */
#define ONE_SEVENTH 2 /* 1/7th of night */
#define ANGLE_BASED 3 /* angle/60th of night */

/* Time Formats --- */
#define TIME_24    0 /* 24-hour format */
#define TIME_12    1 /* 12-hour format */
#define TIME_12_NS 2 /* 12-hour format with no suffix */

/* Times Indices --- */
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

/* Calculation Methods --- */
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

/* Other Stuff --- */
#define PT_TIMES_CALC_LEN 7
#define PT_TIMES_ALL_LEN 12
#define PT_TIMES_IQAMA_LEN 5

/* Looping --- */
#define PRAY_TIMES_ONLY_LEN 5
#define PRAY_TIMES_ONLY { FAJR, DUHR, ASR, MAGRIB, ISHA }

#define SIMPLE_TIMES_LEN 6
#define SIMPLE_TIMES { FAJR, SUNRISE, DUHR, ASR, MAGRIB, ISHA }

#define SIMPLE_TIMES_WITH_IQAMA_LEN 11
#define SIMPLE_TIMES_WITH_IQAMA { FAJR, FAJR_IQAMA, SUNRISE, DUHR, DUHR_IQAMA, ASR, ASR_IQAMA, MAGRIB, MAGRIB_IQAMA, ISHA, ISHA_IQAMA }

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

/* STRUCTS ------------------------------------------------------ */

typedef struct {
  float fajr_angle;
  bool magrib_is_minuets; // true = minutes after sunset, false = angle
  float magrib_val;
  bool isha_is_minuets; // true = minutes after sunset, false = angle
  float isha_val;
} method;

typedef struct {

  method calc_method; // calculation method
  byte asr_juristic; // Juristic method for Asr
  byte dhuhr_minutes; // minutes after mid-day for Dhuhr
  byte adjust_high_lats; // adjusting method for higher latitudes

  double lat; // latitude
  double lng; // longitude
  double time_zone; // time-zone

  short offsets[PT_TIMES_CALC_LEN];
  short iqama[PT_TIMES_ALL_LEN]; // iqama after {fajr, duhr, asr, magrib, isha} minutes

} prayer_opts;

typedef struct {

  /* public -------------------- */
  prayer_opts opts;
  short times[PT_TIMES_ALL_LEN];

  /* private ------------------- */
  double j_date; // Julian date
  double equation_of_time;
  double sun_declination;

} prayer_time;

/* PROTOTYPES --------------------------------------------------- */

prayer_time* prayer_time_new();
method       int_to_method(int i);
void         calculate_for(prayer_time* self, int year, int month, int day);
byte         closest_index(byte i);
byte         iqama_index(byte i);
byte         get_next_time(prayer_time* self, short now);
bool         is_praying_time(prayer_time* self, short now, bool or_iqama);
short        create_time(int h, int m);
short        get_short_next_remaining(prayer_time* self, short now, byte next);
short        get_time(prayer_time* self, byte i);
void         set_opts(prayer_time*, byte, float, bool, float, bool, float, byte, byte, byte, double, double, double, short, short, short, short, short, short, short, short, short, short, short, short);

#endif /* PRAYERTIMES_H_ */
