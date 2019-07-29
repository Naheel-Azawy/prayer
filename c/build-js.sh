#!/bin/sh
rm -rf js
mkdir js
node build-js-h.js
emcc prayertimes.c -o js/prayertimes.js -s EXPORTED_FUNCTIONS="$(cat js/cc-exported)" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
