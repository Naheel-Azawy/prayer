#define TIME_FORMAT      TIME_12
#define CALC_METHOD      QATAR
#define ASR_JURISTIC     SHAFII
#define ADJUST_HIGH_LATS ANGLE_BASED
#define LAT              25.2899589
#define LNG              51.4974742
#define TIMEZONE         3

#define START_YEAR   2019
#define START_MONTH  7
#define START_DAY    21
#define START_HOUR   1
#define START_MINUTE 48

#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define PRINT  Serial.print
#define PRINT2 Serial.print

prayer_time *pt;
short pt_times[PT_TIMES_ALL_LEN];
byte nxt;
byte real_nxt;

int y, m, d;
int h, min, h_base, min_base;

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

void setup_time() {
  y = START_YEAR;
  m = START_MONTH;
  d = START_DAY;
  h = h_base = START_HOUR;
  min = min_base = START_MINUTE;
  pt->time_now = h * 60 + min;
}

void update_time() {
  h = pt->time_now / 60;
  min = min_base + millis() / 60000;
  if (min == 60) {
    min = 0;
    ++h;
    if (h == 25) {
      h = 0;
    }
  }
  pt->time_now = h * 60 + min;
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
    calculate_for_date(pt, y, m, d);
  }

  real_nxt = get_next_time(pt, pt->time_now);
  nxt = closest_index(real_nxt);

#define PRINT_BOTH(s) Serial.print(s); lcd.print(s);

  Serial.print("TIME\t\t");
  ARDUINO_PRINT_TIME(Serial, pt->time_now, TIME_FORMAT);
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

  calculate_for_full(pt, y, m, d, LAT, LNG, TIMEZONE);

  update();

}

void loop() {
  if (millis() % 60000 == 0) {
    update();
  }
}
