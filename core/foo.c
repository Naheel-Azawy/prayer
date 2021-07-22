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
    printf("%d\n", times[i]);
  }
  return 0;
}
