CC := gcc
CFLAGS := -std=c99 -g3 -ggdb -O0 -Wall -Werror -pedantic -Wextra -Ivendor/uki/include
LFLAGS := -Lvendor/uki/lib -luki

.PHONY : run compare clean

run : app.exe
	./app.exe

compare : CFLAGS += -D_PERF_TEST
compare : app.exe
	./efficiency_test/efficiency_test.sh

clean :
	rm -rf *.exe *.stackdump obj

app.exe : \
		obj/flat.o \
		obj/flat_table.o \
		obj/conio.o \
		obj/sorters.o \
		obj/table_sorts.o \
		obj/sort_menu.o \
		obj/main_menu.o \
		obj/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

obj/flat.o : src/flat.c \
		src/flat.h
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<

obj/flat_table.o : src/flat_table.c \
		src/flat.h \
		src/flat_table.h
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<

obj/conio.o : src/conio.c \
		src/conio.h
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<

obj/sorters.o : src/sorters.c \
		src/sorters.h
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<

obj/table_sorts.o : src/table_sorts.c \
		src/table_sorts.h
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<

obj/sort_menu.o : src/sort_menu.c \
		src/sort_menu.h
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<

obj/main_menu.o : src/main_menu.c \
		src/main_menu.h
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<

obj/main.o : src/main.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $<
