CC=gcc
CFLAGS=`pkg-config --cflags libnotify`
LIBS=`pkg-config --libs libnotify`

uninstall_appimage: main.c
	$(CC) $(CFLAGS) -o uninstall_appimage main.c $(LIBS)

PREFIX=$(HOME)/.local/bin

install:
    @mkdir -p $(PREFIX)
    @cp -p uninstall_appimage $(PREFIX)

uninstall:
    @rm -f $(PREFIX)/uninstall_appimage
