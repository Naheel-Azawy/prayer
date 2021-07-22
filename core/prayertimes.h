#ifndef PRAYERTIMES_H_
#define PRAYERTIMES_H_

#include <stdbool.h>

/* DEFINITIONS -------------------------------------------------- */

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

#define IS_IQAMA(i)                                     \
  (i == FAJR_IQAMA || i == DUHR_IQAMA || i == ASR_IQAMA \
   || i == MAGRIB_IQAMA || i == ISHA_IQAMA)

/* Time Formats --- */
#define TIME_24    0 /* 24-hour format */
#define TIME_12    1 /* 12-hour format */
#define TIME_12_NS 2 /* 12-hour format with no suffix */

/* Calculation Methods --- */
#define CUSTOM              0 /* Custom Setting */
#define MAKKAH              1 /* Umm al-Qura, Makkah */
#define EGYPT               2 /* Egyptian General Authority of Survey */
#define QATAR               3 /* Qatar Calendar House */
#define KARACHI             4 /* University of Islamic Sciences, Karachi */
#define ISNA                5 /* Islamic Society of North America (ISNA) */
#define MWL                 6 /* Muslim World League (MWL) */
#define ALGERIA             7 /* Algeria, Ministry of Religious Affairs and Endowments */
#define JORDAN              8 /* Jordan, Ministry of Awqaf, Islamic Affairs and Holy Places */
#define KUWAIT              9 /* Kuwait, Ministry of Awqaf and Islamic Affairs */
#define ENGLAND_BIRMINGHAM 10 /* United Kingdom, Birmingham Central Mosque */
#define ENGLAND_LONDON     11 /* United Kingdom, London Central Mosque */
#define GERMANY_MUNCHEN    12 /* Germany, Islamic Center of Munich */
#define GERMANY_AACHEN     13 /* Germany, Islamic Center of Aachen */

/* Juristic Methods --- */
#define SHAFII 0 /* Shafii (standard) */
#define HANAFI 1 /* Hanafi */

/* Adjusting Methods for Higher Latitudes --- */
#define NONE        0 /* No adjustment */
#define MIDNIGHT    1 /* middle of night */
#define ONE_SEVENTH 2 /* 1/7th of night */
#define ANGLE_BASED 3 /* angle/60th of night */

/* Other Stuff --- */
#define PT_TIMES_CALC_LEN   7
#define PT_TIMES_ALL_LEN   12
#define PT_TIMES_IQAMA_LEN  5

/* Looping --- */
#define TIMES_LEN 6
#define TIMES { FAJR, SUNRISE, DUHR, ASR, MAGRIB, ISHA }

#define TIMES_WITH_IQAMA_LEN 11
#define TIMES_WITH_IQAMA { FAJR, FAJR_IQAMA, SUNRISE, DUHR, DUHR_IQAMA, ASR, ASR_IQAMA, MAGRIB, MAGRIB_IQAMA, ISHA, ISHA_IQAMA }

#define FOR_PRAYER(var, type, body) {           \
    static byte __arr[] = type;                 \
    byte __count;                               \
    byte var;                                   \
    for (__count = 0, var = __arr[0];           \
         __count < type##_LEN;                  \
         ++__count, var = __arr[__count])       \
      body                                      \
        }

/* STRUCTS ------------------------------------------------------ */

typedef unsigned char byte;

typedef struct {
  float fajr_angle;
  bool magrib_is_minuets; // true = minutes after sunset, false = angle
  float magrib_val;
  bool isha_is_minuets; // true = minutes after sunset, false = angle
  float isha_val;
} method;

typedef struct {
  method calc_method;    // calculation method
  byte asr_juristic;     // Juristic method for Asr
  byte duhr_minutes;     // minutes after mid-day for Duhr
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

/* UTILS -------------------------------------------------------- */

#define TIME(h, m) ((short) (h * 60 + m))

extern method METHODS[];

/* PROTOTYPES --------------------------------------------------- */

void pt_init(prayer_time *pt);
void pt_full(short *times, int year, int month, int day,
             byte, float, bool, float, bool, float, byte, byte, byte,
             double, double, double, short, short, short, short, short,
             short, short, short, short, short, short, short);

void         calculate_for(prayer_time* self, int year, int month, int day);
byte         closest_index(byte i);
byte         next_time(short* times, short now);
short        remaining_to(short* times, short now, byte next);

#endif /* PRAYERTIMES_H_ */
