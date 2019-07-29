#include <LiquidCrystal.h>
#include <Time.h>

#define START_YEAR   2019
#define START_MONTH  7
#define START_DAY    21
#define START_HOUR   1
#define START_MINUTE 48

#define TIME_FORMAT TIME_12

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define PRINT  Serial.print
#define PRINT2 Serial.print

prayer_time *pt;
short pt_times[PT_TIMES_ALL_LEN];
short time_now;
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

#define ARDUINO_PRINT_TIME(s, t, format) {      \
    short _h = t / 60, _m = t % 60, _p = 0;     \
    if (format == TIME_12 && _h > 12)           \
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
  time_now = hour() * 60 + minute();

  if (time_now == 0) { // if new day
    calculate_for(pt, year(), month(), day());
  }

  real_nxt = get_next_time(pt, time_now);
  nxt = closest_index(real_nxt);

#define PRINT_BOTH(s) Serial.print(s); lcd.print(s);

  Serial.print("TIME\t\t");
  ARDUINO_PRINT_TIME(Serial, time_now, TIME_FORMAT);
  Serial.print(" ");
  Serial.print(year());
  Serial.print("-");
  Serial.print(month());
  Serial.print("-");
  Serial.print(day());
  Serial.println('\n');

  lcd.setCursor(0, 0);
  ARDUINO_PRINT_TIME(lcd, time_now, TIME_FORMAT);
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
        nxt_time = get_short_next_remaining(pt, time_now, real_nxt);
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
  setTime(START_HOUR, START_MINUTE, 60, START_DAY, START_MONTH, START_YEAR);

  pt = prayer_time_new();

  pt->opts.calc_method      = int_to_method(QATAR);
  pt->opts.asr_juristic     = SHAFII;
  pt->opts.adjust_high_lats = ANGLE_BASED;
  pt->opts.lat              = 25.2899589;
  pt->opts.lng              = 51.4974742;
  pt->opts.time_zone        = 3;

  calculate_for(pt, year(), month(), day());

  update();

}

void loop() {
  if (millis() % 60000 == 0) {
    update();
  }
}
