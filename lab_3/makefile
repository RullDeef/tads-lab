CFLAGS := -std=gnu99 -Wall -Werror -pedantic
LFLAGS :=

.PHONY : build clean

build : bin/app.exe
	bin/app.exe

clean :
	rm -rf bin/* obj/* **/bin/* **/obj/*

include tests/makefile

bin/app.exe : obj/main.o obj/conio.o obj/sparse_matrix.o
	$(CC) $(LFLAGS) -o $@ $^

obj/main.o : src/main.c
	$(CC) $(CFLAGS) -o $@ -c $^

obj/conio.o : src/conio.c
	$(CC) $(CFLAGS) -o $@ -c $^

obj/sparse_matrix.o : src/sparse_matrix.c
	$(CC) $(CFLAGS) -o $@ -c $^
