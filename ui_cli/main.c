#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../core/prayertimes.h"

typedef struct {
  int  time_format;
  bool mini;
} ui_opts;

const char *TIMES_NAMES[] = {
  "Fajr",
  "Fajr iqama",
  "Sunrise",
  "Duhr",
  "Duhr iqama",
  "Asr",
  "Asr iqama",
  "Sunset",
  "Magrib",
  "Magrib iqama",
  "Isha",
  "Isha iqama"
};

void help() {
  printf("usage: prayer [OPTION...] [ARG...]\n");
  printf("  --tz     <TIMEZONE>    timezone\n");
  printf("  --lat    <LATITUDE>    latitude\n");
  printf("  --lng    <LONGITUDE>   longitude\n");
  printf("  --method <METHOD>      fajr and isha calculation method\n");
  printf("  --asr    <ASR>         asr juristic calculation method\n");
  printf("  --highlat <HIGHLAT>    high latitude adjustment method\n");
  printf("  --mini                 only show the remaining time\n");
  printf("  --help                 show this help\n");
  printf("\n");

  printf("METHOD:\n");
  printf("  makkah\n");
  printf("  egypt\n");
  printf("  qatar\n");
  printf("  karachi\n");
  printf("  isna\n");
  printf("  mwl\n");
  printf("  algeria\n");
  printf("  jordan\n");
  printf("  kuwait\n");
  printf("  england_birmingham\n");
  printf("  england_london\n");
  printf("  germany_munchen\n");
  printf("  germany_aachen\n");
  printf("\n");

  printf("ASR:\n");
  printf("  shafii\n");
  printf("  hanafi\n");
  printf("\n");

  printf("HIGHLAT:\n");
  printf("  none\n");
  printf("  midnight\n");
  printf("  one_seventh\n");
  printf("  angle_based\n");
}

byte str2method(char *str) {
  if      (strcmp(str, "custom")             == 0) { return CUSTOM; }
  else if (strcmp(str, "makkah")             == 0) { return MAKKAH; }
  else if (strcmp(str, "egypt")              == 0) { return EGYPT; }
  else if (strcmp(str, "qatar")              == 0) { return QATAR; }
  else if (strcmp(str, "karachi")            == 0) { return KARACHI; }
  else if (strcmp(str, "isna")               == 0) { return ISNA; }
  else if (strcmp(str, "mwl")                == 0) { return MWL; }
  else if (strcmp(str, "algeria")            == 0) { return ALGERIA; }
  else if (strcmp(str, "jordan")             == 0) { return JORDAN; }
  else if (strcmp(str, "kuwait")             == 0) { return KUWAIT; }
  else if (strcmp(str, "england_birmingham") == 0) { return ENGLAND_BIRMINGHAM; }
  else if (strcmp(str, "england_london")     == 0) { return ENGLAND_LONDON; }
  else if (strcmp(str, "germany_munchen")    == 0) { return GERMANY_MUNCHEN; }
  else if (strcmp(str, "germany_aachen")     == 0) { return GERMANY_AACHEN; }
  else {
    printf("ERROR: unknown method '%s'\n", str);
    help();
    exit(1);
  }
}

byte str2asr(char *str) {
  if      (strcmp(str, "shafii") == 0) { return SHAFII; }
  else if (strcmp(str, "hanafi") == 0) { return HANAFI; }
  else {
    printf("ERROR: unknown asr method '%s'\n", str);
    help();
    exit(1);
  }
}

byte str2highlat(char *str) {
  if      (strcmp(str, "none")        == 0) { return NONE; }
  else if (strcmp(str, "midnight")    == 0) { return MIDNIGHT; }
  else if (strcmp(str, "one_seventh") == 0) { return ONE_SEVENTH; }
  else if (strcmp(str, "angle_based") == 0) { return ANGLE_BASED; }
  else {
    printf("ERROR: unknown high latitude method '%s'\n", str);
    help();
    exit(1);
  }
}

void print_time(short t, int format) {
  short h = t / 60,
    m = t % 60,
    p = 0;
    if (format == TIME_12 && h > 12)
      p = 1, h -= 12;
    printf("%02d:%02d", h, m);
    if (format == TIME_12)
      printf(p ? " PM" : " AM");
}

// TODO: save as a config file
void parse_args(int argc, char **argv, prayer_time *pt, ui_opts *opts) {
  for (int i = 1; i < argc; ++i) {
    if (strcmp("--help", argv[i]) == 0) {
      help();
      exit(0);

    } else if (strcmp("--mini", argv[i]) == 0) {
      opts->mini = true;

    } else if (strcmp("--tz", argv[i]) == 0) {
      pt->opts.time_zone = atof(argv[++i]);

    } else if (strcmp("--lat", argv[i]) == 0) {
      pt->opts.lat = atof(argv[++i]);

    } else if (strcmp("--lng", argv[i]) == 0) {
      pt->opts.lng = atof(argv[++i]);

    } else if (strcmp("--method", argv[i]) == 0) {
      pt->opts.calc_method = METHODS[str2method(argv[++i])];

    } else if (strcmp("--asr", argv[i]) == 0) {
      pt->opts.asr_juristic = str2asr(argv[++i]);

    } else if (strcmp("--highlat", argv[i]) == 0) {
      pt->opts.adjust_high_lats = str2highlat(argv[++i]);
    }
  }
}

int main(int argc, char **argv) {
  time_t t_t = time(NULL);
  struct tm *t = localtime(&t_t);
  prayer_time pt;
  ui_opts opts;
  byte nxt;
  byte real_nxt;
  short rem;

  pt_init(&pt);
  pt.opts.calc_method      = METHODS[QATAR];
  pt.opts.asr_juristic     = SHAFII;
  pt.opts.adjust_high_lats = ANGLE_BASED;
  pt.opts.lat              = 25.2899589;
  pt.opts.lng              = 51.4974742;
  pt.opts.time_zone        = 3;

  opts.time_format = TIME_12;
  opts.mini        = false;

  int year = 1900 + t->tm_year;
  int month = 1+ t->tm_mon;
  int day = t->tm_mday;
  short time_now = t->tm_hour * 60 + t->tm_min;

  parse_args(argc, argv, &pt, &opts);

  calculate_for(&pt, year, month, day);

  real_nxt = next_time(pt.times, time_now);
  nxt = closest_index(real_nxt);

  // print_time(time_now, opts.time_format);
  // printf(" %04d.%02d.%02d\n\n", year, month, day);

  if (!opts.mini) {
    FOR_PRAYER(p, TIMES, {
        printf("%s\t\t", TIMES_NAMES[p]);
        print_time(pt.times[p], opts.time_format);
        printf("\n");
        if (p == nxt) {
          rem = remaining_to(pt.times, time_now, real_nxt);
          printf("    ");
          if (rem == 0) {
            printf("time for %s", TIMES_NAMES[real_nxt]);
          } else if (IS_IQAMA(real_nxt)) {
            print_time(rem, TIME_24);
            printf(" to iqama");
          } else {
            print_time(rem, TIME_24);
            printf(" remaining");
          }
          printf("\n");
        }
      });

  } else {
    rem = remaining_to(pt.times, time_now, real_nxt);
    if (rem == 0) {
      printf("%s", TIMES_NAMES[real_nxt]);
    } else if (IS_IQAMA(real_nxt)) {
      print_time(rem, TIME_24);
      printf("i");
    } else {
      print_time(rem, TIME_24);
    }
    printf("\n");
  }

  return 0;
}
