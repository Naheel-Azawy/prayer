#!/bin/bash
rm -rf arduino
mkdir -p arduino
cat prayertimes.h \
    <(sed 's/#include "prayertimes.h"//' prayertimes.c) \
    <(sed -E "
    s/#define START_YEAR.*/#define START_YEAR $(date '+%Y')/;
    s/#define START_MONTH.*/#define START_MONTH $(date '+%-m')/;
    s/#define START_DAY.*/#define START_DAY $(date '+%-d')/;
    s/#define START_HOUR.*/#define START_HOUR $(date '+%-H')/;
    s/#define START_MINUTE.*/#define START_MINUTE $(date '+%-M')/" arduino.ino) \
    > arduino/arduino.ino
cd arduino
arduino-cli compile --fqbn arduino:avr:nano
if [ "$1" = run ]; then
    sudo arduino-cli upload --fqbn arduino:avr:nano -p /dev/ttyUSB0
fi
