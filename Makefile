CC=gcc
CFLAGS=`pkg-config --cflags libnotify`
LIBS=`pkg-config --libs libnotify`

uninstall_appimage: main.c
	$(CC) $(CFLAGS) -o uninstall_appimage main.c $(LIBS)
