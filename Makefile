CC=gcc
CFLAGS=`pkg-config --cflags libnotify`
LIBS=`pkg-config --libs libnotify`

uninstall_app: main.c
	$(CC) $(CFLAGS) -o uninstall_app main.c $(LIBS)
