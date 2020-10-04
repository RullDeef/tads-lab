CFLAGS := -std=gnu99 -Wall -Werror -pedantic
LFLAGS :=

.PHONY : build clean

build : app.exe

clean :
	rm -rf bin/* obj/* **/bin/* **/obj/*

include tests/makefile

app.exe : obj/main.o obj/sparse_matrix.o
	$(CC) $(LFLAGS) -o $@ $^

obj/main.o : src/main.c
	$(CC) $(CFLAGS) -o $@ -c $^

obj/sparse_matrix.o : src/sparse_matrix.c
	$(CC) $(CFLAGS) -o $@ -c $^
