CC = gcc
CFLAGS = -std=c99 -g3 -O0 -Wall -Werror -pedantic
 
.PHONY : clean

app.exe : obj/flat.o obj/flat_table.o obj/main.o obj/operations.o obj/conio.o
	$(CC) $(CFLAGS) -o $@ $^

obj/flat.o : src/flat.h src/flat.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c src/flat.c

obj/flat_table.o : src/flat.h src/flat_table.h src/flat_table.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c src/flat_table.c

obj/conio.o : src/conio.h src/conio.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c src/conio.c

obj/operations.o : src/flat.h src/flat_table.h src/operations.h src/operations.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c src/operations.c

obj/main.o : src/operations.h src/main.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c src/main.c

compare : CFLAGS += -D_PERF_TEST=100
compare : app.exe
	./efficiency_test/efficiency_test.sh

clean :
	rm -rf *.exe *.stackdump obj
