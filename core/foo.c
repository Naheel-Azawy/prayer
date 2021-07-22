#include <stdio.h>
#include "./prayertimes.h"

int main() {
  short times[11];

  pt_full(times, 2021, 7, 21,
          QATAR, -1, false, -1, false, -1,
          SHAFII, 0, ANGLE_BASED,
          25.411720, 51.503878, 3.0,
          0, 0, 0, 0, 0, 0, 0,
          25, 20, 25, 10, 20);

  for (int i = 0; i < 11; ++i) {
    printf("%02d:%02d\n", times[i] / 60, times[i] % 60);
  }

  short now = TIME(20, 36);
  byte next = next_time(times, now);
  short rem = remaining_to(times, now, next);
  printf("nxt: %d, rem: %02d:%02d\n", next, rem / 60, rem % 60);

  return 0;
}
