CC = gcc
CFLAGS = -std=c99 -g3 -O0 -Wall -Werror -pedantic

SRC := src/flat.c src/flat_table.c src/main.c src/operations.c
OBJ := obj/flat.o obj/flat_table.o obj/main.o obj/operations.o

.PHONY : clean

app.exe : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRC)

obj/flat.o : src/flat.h src/flat.c
	$(CC) $(CFLAGS) -o $@ -c src/flat.c

obj/flat_table.o : src/flat.h src/flat_table.h src/flat_table.c
	$(CC) $(CFLAGS) -o $@ -c src/flat_table.c

obj/operations.o : src/flat.h src/flat_table.h src/operations.h src/operations.c
	$(CC) $(CFLAGS) -o $@ -c src/operations.c

obj/main.o : src/operations.h src/main.c
	$(CC) $(CFLAGS) -o $@ -c src/main.c

compare : CFLAGS += -D_PERF_TEST=100000
compare : app.exe
	./efficiency_test/efficiency_test.sh

clean :
	rm -rf *.exe *.stackdump obj
