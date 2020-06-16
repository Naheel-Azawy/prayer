PREFIX = /usr/bin

c/a.out:
	@cd c && g++ *.c *.cpp

install:
	mkdir -p $(DESTDIR)$(PREFIX)
	cp -f c/a.out $(DESTDIR)$(PREFIX)/theprayer

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/theprayer
