#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "prayertimes.h"
#include "prayerstrings.hpp"

bool next_only = false;

prayer_time *pt;
prayer_strings *pts;
byte nxt;
byte real_nxt;

int y, m, d;
int h, min, h_base, min_base;

void update_time() {
  time_t t = time(NULL);
  struct tm *ts = localtime(&t);
  y = ts->tm_year + 1900;
  m = ts->tm_mon + 1;
  d = ts->tm_mday;
  pt->time_now = ts->tm_hour * 60 + ts->tm_min;
}

void update() {
  update_time();

  if (pt->time_now == 0) { // if new day
    calculate_for_date(pt, y, m, d);
  }

  real_nxt = get_next_time(pt, pt->time_now);
  nxt = closest_index(real_nxt);

  if (next_only) {
    printf("%s\n",
           get_one_line_small_next_remaining(pts, pt->time_now, real_nxt).c_str());
    return;
  }

  FOR_PRAYER(p, SIMPLE_TIMES, {
      printf("%s\t\t%s\n",
             pts->times_names[p].c_str(),
             get_string_time(pts, p).c_str());
      if (p == nxt) {
        printf("%s\n",
               get_remaining_time_string(pts, pt->time_now,
                                         (p == nxt ? -1 : p), real_nxt).c_str());
      }
    });

}

void setup() {

  pt = prayer_time_new();
  pts = prayer_strings_new(pt);

  pts->time_format = TIME_12;
  pts->is_ar = false;
  pt->calc_method = int_to_method(QATAR);
  pt->asr_juristic = SHAFII;
  pt->adjust_high_lats = ANGLE_BASED;
  tune(pt, 0, 0, 0, 0, 0, 0, 0);

  update_time();

  init_strings(pts);
  calculate_for_full(pt, y, m, d, 25.2899589, 51.4974742, 3);

  update();

}

int main(int argc, char **argv) {
  if (argc > 1) {
    next_only = strcmp(argv[1], "n") == 0;
  }
  setup();
  return 0;
}
