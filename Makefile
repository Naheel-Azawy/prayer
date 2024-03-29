PREFIX = /usr/local

HDR_CORE  = core/prayertimes.h
SRC_CORE  = core/prayertimes.c
SRC_CLI   = ui_cli/main.c
SRC_INO   = ui_ino/prayer.ino
SRC_WEB   = ui_web/index.html ui_web/main.js ui_web/pt_iface.js ui_web/ripple.css ui_web/ripple.js ui_web/style.css
SRC_DROID = $(shell find ui_android/Prayer/app -type f -name '*.java' -or -name '*.xml')

WASM_FUNCS="[ \
'_pt_full',   \
'_next_time', \
'_remaining_to']"

all:   build/prayer
ino:   build/prayer.ino
web:   build/web/index.html
droid: build/prayer.apk

build/prayer: $(HDR_CORE) $(SRC_CORE) $(SRC_CLI)
	mkdir -p build
	gcc -lm $(SRC_CORE) $(SRC_CLI) -o build/prayer

build/prayer.ino: $(HDR_CORE) $(SRC_CORE) $(SRC_INO)
	mkdir -p build
	cat $(HDR_CORE) $(SRC_CORE) $(SRC_INO) > build/prayer.ino
	sed -Ei "s/#include \"prayertimes.h\"//" build/prayer.ino

build/prayertimes.wasm: $(HDR_CORE) $(SRC_CORE)
	mkdir -p build/web
	emcc -Os                                \
		-s STANDALONE_WASM                  \
		-s EXPORTED_FUNCTIONS=$(WASM_FUNCS) \
		-Wl,--no-entry                      \
		core/prayertimes.c -o build/prayertimes.wasm

build/prayertimes.wasm.js: build/prayertimes.wasm
	mkdir -p build/web
	echo 'export const pt_asm = atob(`' > build/prayertimes.wasm.js
	base64 build/prayertimes.wasm      >> build/prayertimes.wasm.js
	echo '`);'                         >> build/prayertimes.wasm.js

ui_web/node_modules:
	cd ui_web && npm i

build/web/index.html: ui_web/node_modules build/prayertimes.wasm.js $(SRC_WEB)
	cd ui_web && npm run build:prod
	mkdir -p ui_android/Prayer/app/src/main/assets
	rm -rf ui_android/Prayer/app/src/main/assets/*
	cp build/web/* ui_android/Prayer/app/src/main/assets/

web-watch: ui_web/node_modules build/prayertimes.wasm.js $(SRC_WEB)
	cd ui_web && npm run watch

build/prayer.apk: build/web/index.html $(SRC_DROID)
	mkdir -p ui_android/Prayer/app/src/main/assets
	rm -rf ui_android/Prayer/app/src/main/assets/*
	cp build/web/* ui_android/Prayer/app/src/main/assets/
	cd ui_android/Prayer && ./gradlew assembleDebug
	cp ui_android/Prayer/app/build/outputs/apk/debug/app-debug.apk build/prayer.apk

droid-install: build/prayer.apk
	adb install -r build/prayer.apk
	adb shell am start -n xyz.naheel.prayer/.MainActivity

install: all
	mkdir -p $(PREFIX)/bin
	cp -f build/prayer $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/prayer

clean:
	rm -r build
#	rm -r ui_web/node_modules ui_web/package-lock.json

.PHONY: all ino web droid droid-install install uninstall clean
