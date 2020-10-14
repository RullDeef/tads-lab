.PHONY : build clean

build : bin/app.exe

bin/app.exe: src/main.c src/menu.c
	mkdir -p bin; $(CC) -std=c99 -o $@ $^
	# /usr/lib/libpanel.dll.a /usr/lib/libncurses.a

clean :
	rm -rf bin
