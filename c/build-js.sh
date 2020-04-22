#!/bin/sh
rm -rf js
mkdir js
node build-js-h.js
emcc prayertimes.c -o js/prayertimescore.js -s EXPORTED_FUNCTIONS="$(cat js/cc-exported)" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
cat js/prayertimescore.js js/prayertimes.h.js prayertimes.js > js/prayertimes.js
