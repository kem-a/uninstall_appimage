CC=gcc
CFLAGS=$(shell pkg-config --cflags libnotify)
LIBS=$(shell pkg-config --libs libnotify)

uninstall_appimage: main.c
    $(CC) $(CFLAGS) -o uninstall_appimage main.c $(LIBS)
