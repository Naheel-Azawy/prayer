PREFIX = /usr/bin

HDR_CORE = core/prayertimes.h
SRC_CORE = core/prayertimes.c
SRC_CLI  = ui_cli/main.c
SRC_INO  = ui_ino/prayer.ino
SRC_WEB  = ui_web/index.html ui_web/main.js ui_web/pt_iface.js ui_web/ripple.css ui_web/ripple.js ui_web/style.css

WASM_FUNCS="[ \
'_pt_full',   \
'_next_time', \
'_remaining_to']"

all: build/prayer
ino: build/prayer.ino
web: build/web/index.html

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
	rm -r ui_android/Prayer/app/src/main/assets/*
	cp build/web/* ui_android/Prayer/app/src/main/assets/

install:
	mkdir -p $(DESTDIR)$(PREFIX)
	cp -f build/prayer $(DESTDIR)$(PREFIX)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/prayer

clean:
	rm -r build
	rm -r ui_web/node_modules ui_web/package-lock.json

.PHONY: install uninstall clean
