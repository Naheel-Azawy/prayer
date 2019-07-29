#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "prayertimes.h"
#include "prayerstrings.hpp"

bool next_only = false;

prayer_time *pt;
prayer_strings *pts;
short time_now;
byte nxt;
byte real_nxt;

struct tm *ts;
#define year()  (ts->tm_year + 1900)
#define month() (ts->tm_mon + 1)
#define day()   (ts->tm_mday)

void update() {
  time_t t = time(NULL);
  ts = localtime(&t);
  time_now = ts->tm_hour * 60 + ts->tm_min;

  if (time_now == 0) { // if new day
    calculate_for(pt, year(), month(), day());
  }

  real_nxt = get_next_time(pt, time_now);
  nxt = closest_index(real_nxt);

  if (next_only) {
    printf("%s\n",
           get_one_line_small_next_remaining(pts, time_now, real_nxt).c_str());
    return;
  }

  FOR_PRAYER(p, SIMPLE_TIMES, {
      printf("%s\t\t%s\n",
             pts->times_names[p].c_str(),
             get_string_time(pts, p).c_str());
      if (p == nxt) {
        printf("%s\n",
               get_remaining_time_string(pts, time_now,
                                         (p == nxt ? -1 : p), real_nxt).c_str());
      }
    });

}

void setup() {

  time_t t = time(NULL);
  ts = localtime(&t);

  pt = prayer_time_new();

  pts = prayer_strings_new(pt);
  pts->time_format = TIME_12;
  pts->is_ar       = false;

  pt->opts.calc_method      = int_to_method(QATAR);
  pt->opts.asr_juristic     = SHAFII;
  pt->opts.adjust_high_lats = ANGLE_BASED;
  pt->opts.lat              = 25.2899589;
  pt->opts.lng              = 51.4974742;
  pt->opts.time_zone        = 3;

  init_strings(pts);
  calculate_for(pt, year(), month(), day());

  update();

}

int main(int argc, char **argv) {
  if (argc > 1) {
    next_only = strcmp(argv[1], "n") == 0;
  }
  setup();
  return 0;
}
