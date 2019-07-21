#include "prayerstrings.hpp"
#include "prayertimes.h"

prayer_strings *prayer_strings_new(prayer_time *pt) {
  prayer_strings *n = (prayer_strings *) malloc(sizeof(prayer_strings));
  n->pt = pt;
  n->time_format = TIME_12;
  n->is_ar = false;
  return n;
}

void init_strings(prayer_strings *self) {
  bool jumuaa = false; // TODO
  if (self->is_ar) {
    self->times_names[0] = "الفجر";
    self->times_names[1] = "إقامة الفجر";
    self->times_names[2] = "الشروق";
    self->times_names[3] = jumuaa ? "الجمعة" : "الظهر";
    self->times_names[4] = jumuaa ? "إقامة الجمعة" : "إقامة الظهر";
    self->times_names[5] = "العصر";
    self->times_names[6] = "إقامة العصر";
    self->times_names[7] = "المغرب";
    self->times_names[8] = "المغرب";
    self->times_names[9] = "إقامة المغرب";
    self->times_names[10] = "العشاء";
    self->times_names[11] = "إقامة العشاء";
    self->str_remaining = "المتبقي";
    self->str_passed = "مرت";
    self->str_to_pray = "لصلاة";
    self->str_iqama = "الإقامة";
    self->str_to_iqama = "للإقامة";
    self->str_from_iqama = "من الإقامة";
    self->str_its_time_for = "حان وقت";
    self->str_hour = "ساعة";
    self->str_hours = "ساعات";
    self->str_2_hours = "ساعتان";
    self->str_minute = "دقيقة";
    self->str_minutes = "دقائق";
    self->str_2_minutes = "دقيقتان";
    self->str_and = " و ";
  } else {
    self->times_names[0] = "Fajr";
    self->times_names[1] = "Fajr iqama";
    self->times_names[2] = "Sunrise";
    self->times_names[3] = jumuaa ? "Jumuaa" : "Duhr";
    self->times_names[4] = jumuaa ? "Jumuaa iqama" : "Duhr iqama";
    self->times_names[5] = "Asr";
    self->times_names[6] = "Asr iqama";
    self->times_names[7] = "Magrib";
    self->times_names[8] = "Magrib";
    self->times_names[9] = "Magrib iqama";
    self->times_names[10] = "Isha";
    self->times_names[11] = "Isha iqama";
    self->str_remaining = "Remaining";
    self->str_passed = "Passed";
    self->str_to_pray = "to";
    self->str_iqama = "iqama";
    self->str_to_iqama = "to iqama";
    self->str_from_iqama = "from iqama";
    self->str_its_time_for = "It's time for";
    self->str_hour = "hr";
    self->str_hours = self->str_hour;
    self->str_2_hours = self->str_hour;
    self->str_minute = "min";
    self->str_minutes = self->str_minute;
    self->str_2_minutes = self->str_minute;
    self->str_and = " and ";
  }
}

string get_times_name_simple_lower_case(int i) {
  switch (i) {
  case 0:
    return "fajr";
  case 1:
    return "sunrise";
  case 2:
    return "duhr";
  case 3:
    return "asr";
  case 4:
    return "magrib";
  case 5:
    return "isha";
  }
  return "error";
}

string time_to_string_full(short hours, short minutes, byte time_format, bool is_ar) {
  string result;
  string suffix;
  switch (time_format) {
  case TIME_24:
    if ((hours >= 0 && hours <= 9) && (minutes >= 0 && minutes <= 9)) {
      result = "0" + to_string(hours) + ":0" + to_string(minutes);
    } else if ((hours >= 0 && hours <= 9)) {
      result = "0" + to_string(hours) + ":" + to_string(minutes);
    } else if ((minutes >= 0 && minutes <= 9)) {
      result = to_string(hours) + ":0" + to_string(minutes);
    } else {
      result = to_string(hours) + ":" + to_string(minutes);
    }
    break;
  case TIME_12:
    if (is_ar) {
      suffix = hours >= 12 ? " م " : "ص";
    } else {
      suffix = hours >= 12 ? "PM" : "AM";
    }
    hours = (short) ((((hours + 12) - 1) % (12)) + 1);
    if ((hours >= 0 && hours <= 9) && (minutes >= 0 && minutes <= 9)) {
      result = "0" + to_string(hours) + ":0" + to_string(minutes) + " " + suffix;
    } else if ((hours >= 0 && hours <= 9)) {
      result = "0" + to_string(hours) + ":" + to_string(minutes) + " " + suffix;
    } else if ((minutes >= 0 && minutes <= 9)) {
      result = to_string(hours) + ":0" + to_string(minutes) + " " + suffix;
    } else {
      result = to_string(hours) + ":" + to_string(minutes) + " " + suffix;
    }
    break;
  case TIME_12_NS:
    hours = (short) ((((hours + 12) - 1) % (12)) + 1);
    if ((hours >= 0 && hours <= 9) && (minutes >= 0 && minutes <= 9)) {
      result = "0" + to_string(hours) + ":0" + to_string(minutes);
    } else if ((hours >= 0 && hours <= 9)) {
      result = "0" + to_string(hours) + ":" + to_string(minutes);
    } else if ((minutes >= 0 && minutes <= 9)) {
      result = to_string(hours) + ":0" + to_string(minutes);
    } else {
      result = to_string(hours) + ":" + to_string(minutes);
    }
    break;
  default:
    return "";
  }
  return result;
}

string time_to_string_full_short_time(short time, byte time_format, bool is_ar) {
  return time_to_string_full((short) (time / 60), (short) (time % 60), time_format, is_ar);
}

string time_to_string_short_time(prayer_strings *self, short time) {
  return time_to_string_full_short_time(time, self->time_format, self->is_ar);
}

string time_to_string_short_time_format(prayer_strings *self, short time, byte time_format) {
  return time_to_string_full_short_time(time, time_format, self->is_ar);
}

void get_string_times(prayer_strings *self, string *out) {
  for (int i = 0; i < PT_TIMES_ALL_LEN; ++i) {
    out[i] = time_to_string_short_time(self, self->pt->times[i]);
  }
}

string get_string_time(prayer_strings *self, byte i) {
  return time_to_string_short_time(self, self->pt->times[i]);
}

string get_remaining_time_string(prayer_strings *self, short now, byte i, byte next) {
  if (i == ((byte) -1)) i = next;
  short time = self->pt->times[i];
  short diff = (short) (time - now);
  bool is_now = diff == 0;
  if (is_now) {
    return self->str_its_time_for + " " + self->times_names[i];
  }
  string s;
  bool passed = diff < 0;
  if (passed) {
    diff = (short) -diff;
    if (i == FAJR && next == FAJR) {
      diff = (short) (time + (24 * 60) - now);
      s = self->str_remaining;
    } else {
      s = self->str_passed;
    }
  } else {
    s = self->str_remaining;
  }

  short h = (short) (diff / 60);
  short m = (short) (diff % 60);
  string hS = to_string(h);
  string mS = to_string(m);
  string t;
  if (self->is_ar) {
    switch (h) {
    case 1:
      hS = self->str_hour;
      break;
    case 2:
      hS = self->str_2_hours;
      break;
    default:
      hS += " ";
      if (h > 10)
        hS += self->str_hour;
      else
        hS += self->str_hours;
    }
    switch (m) {
    case 1:
      mS = self->str_minute;
      break;
    case 2:
      mS = self->str_2_minutes;
      break;
    default:
      mS += " ";
      if (m > 10)
        mS += self->str_minute;
      else
        mS += self->str_minutes;
    }
    if (h == 0) {
      t = mS;
    } else if (m == 0) {
      t = hS;
    } else {
      t = hS + self->str_and + mS;
    }
  } else {
    if (h == 0) {
      t = mS + " " + self->str_minutes;
    } else if (m == 0) {
      t = hS + " " + self->str_hours;
    } else {
      t = hS + " " + self->str_hours + self->str_and + mS + " " + self->str_minutes;
    }
  }

  s += " " + t;
  if (i == FAJR_IQAMA || i == DUHR_IQAMA || i == ASR_IQAMA ||
      i == MAGRIB_IQAMA || i == ISHA_IQAMA) {
    s += " " + (passed ? self->str_from_iqama : self->str_to_iqama);
  }
  return s;
}

string get_next_with_remaining_string(prayer_strings *self) {
  byte n = get_next_time(self->pt, self->pt->time_now);
  byte n2 = closest_index(n);
  return self->times_names[n2] + "   " + time_to_string_short_time(self, self->pt->times[n2]) + "\n" + get_remaining_time_string(self, self->pt->time_now, (byte) -1, n);
}

void get_next_with_remaining_two_string(prayer_strings *self, string *out) {
  byte n = get_next_time(self->pt, self->pt->time_now);
  byte n2 = closest_index(n);
  out[0] = self->times_names[n2] + "   " + time_to_string_short_time(self, self->pt->times[n2]);
  out[1] = get_remaining_time_string(self, self->pt->time_now, (byte) -1, n);
}

void get_next_with_remaining_two_string(prayer_strings *self, string *out, short now, byte next) {
  byte n2 = closest_index(next);
  out[0] = self->times_names[n2] + "   " + time_to_string_short_time(self, self->pt->times[n2]);
  out[1] = get_remaining_time_string(self, now, (byte) -1, next);
}

string get_small_next_remaining_full(prayer_strings *self, short now, byte next, char sep) {
  short time = self->pt->times[next];
  short diff = (short) (time - now);
  bool is_now = diff == 0;
  if (is_now) {
    return sep == ' ' ? self->str_its_time_for + " " + self->times_names[next] : self->times_names[next];
  }
  string s;
  if (diff < 0) {
    diff = (short) -diff;
    if (next == FAJR) {
      diff = (short) (time + (24 * 60) - now);
      s = self->str_remaining;
    } else {
      s = self->str_passed;
    }
  } else {
    s = self->str_remaining;
  }
  if (next == FAJR_IQAMA || next == DUHR_IQAMA || next == ASR_IQAMA ||
      next == MAGRIB_IQAMA || next == ISHA_IQAMA) {
    s = self->str_iqama;
  }
  return s + sep + time_to_string_short_time_format(self, diff, TIME_24);
}

string get_small_next_remaining(prayer_strings *self, short now, byte next) {
  return get_small_next_remaining_full(self, now, next, '\n');
}

string get_remaining_time_string(prayer_strings *self) {
  return get_remaining_time_string(self, self->pt->time_now, (byte) -1, get_next_time(self->pt, self->pt->time_now));
}

string get_remaining_time_string(prayer_strings *self, short now) {
  return get_remaining_time_string(self, now, (byte) -1, get_next_time(self->pt, now));
}

string get_remaining_time_string(prayer_strings *self, short now, byte i) {
  return get_remaining_time_string(self, now, i, get_next_time(self->pt, now));
}

string get_small_next_remaining_now(prayer_strings *self) {
  return get_small_next_remaining(self, self->pt->time_now, get_next_time(self->pt, self->pt->time_now));
}

string get_one_line_small_next_remaining(prayer_strings *self, short now, byte next) {
  short time = self->pt->times[next];
  short diff = (short) (time - now);
  bool is_now = diff == 0;
  if (is_now) {
    return self->times_names[next];
  }
  if (diff < 0) {
    diff = (short) -diff;
    if (next == FAJR) {
      diff = (short) (time + (24 * 60) - now);
    }
  }
  return time_to_string_short_time_format(self, diff, TIME_24);
}
