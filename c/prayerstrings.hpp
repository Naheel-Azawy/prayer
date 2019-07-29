#ifndef PRAYER_STRINGS_H_
#define PRAYER_STRINGS_H_

#include <iostream>
#include "prayertimes.h"

using namespace std;

typedef struct {
  prayer_time *pt;

  int time_format; // time format
  bool is_ar; // is Arabic

  string times_names[12];
  string str_remaining;
  string str_passed;
  string str_to_pray;
  string str_iqama;
  string str_to_iqama;
  string str_from_iqama;
  string str_its_time_for;
  string str_hour;
  string str_hours;
  string str_2_hours;
  string str_minute;
  string str_minutes;
  string str_2_minutes;
  string str_and;
} prayer_strings;

prayer_strings* prayer_strings_new(prayer_time *pt);
void            init_strings(prayer_strings *self);
string          get_times_name_simple_lower_case(int i);
string          time_to_string_full(short hours, short minutes, byte time_format, bool is_ar);
string          time_to_string_full_short_time(short time, byte time_format, bool is_ar);
string          time_to_string_short_time(prayer_strings *self, short time);
string          time_to_string_short_time_format(prayer_strings *self, short time, byte time_format);
void            get_string_times(prayer_strings *self, string *out);
string          get_string_time(prayer_strings *self, byte i);
string          get_remaining_time_string(prayer_strings *self, short now, byte i, byte next);
string          get_next_with_remaining_string(prayer_strings *self, short now);
void            get_next_with_remaining_two_string(prayer_strings *self, short now, string *out);
void            get_next_with_remaining_two_string(prayer_strings *self, string *out, short now, byte next);
string          get_small_next_remaining_full(prayer_strings *self, short now, byte next, char sep);
string          get_small_next_remaining(prayer_strings *self, short now, byte next);
string          get_remaining_time_string(prayer_strings *self, short now);
string          get_remaining_time_string(prayer_strings *self, short now, byte i);
string          get_small_next_remaining(prayer_strings *self, short now);
string          get_one_line_small_next_remaining(prayer_strings *self, short now, byte next);

#endif /* PRAYER_STRINGS_H_ */
