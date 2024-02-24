CC=gcc
CFLAGS=$(shell pkg-config --cflags libnotify)
LIBS=$(shell pkg-config --libs libnotify)
PREFIX=$(HOME)/.local/bin

uninstall_appimage: main.c
	$(CC) $(CFLAGS) -o uninstall_appimage main.c $(LIBS)

install:
	@mkdir -p $(PREFIX)
	@cp -p uninstall_appimage $(PREFIX)/uninstall_appimage
	@echo "uninstall_appimage installed to $(PREFIX)"

uninstall:
	@rm -f $(PREFIX)/uninstall_appimage
	@echo "uninstall_appimage removed from $(PREFIX)"
