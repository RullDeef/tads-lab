CC = gcc
CFLAGS := -std=gnu99 -Wall -Werror -pedantic -Wextra -ggdb3 -g \
	-Isrc -Ivendor/uki/include -Ivendor/libcmdf/include 
LFLAGS := -Lvendor/uki/lib -luki

.PHONY : clean run unit

clean :
	rm -rf obj *.exe

run : app.exe
	./app.exe

unit : unit.exe
	./unit.exe

unit.exe : \
		obj/unit.o
	$(CC) -o $@ $^ $(LFLAGS)

app.exe : \
		obj/stack_arr.o \
		obj/stack_lst.o \
		obj/main.o
	$(CC) -o $@ $^ $(LFLAGS)

obj/main.o : src/main.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/stack_arr.o : src/stack_arr.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/stack_lst.o : src/stack_lst.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/unit.o : unit_test/unit.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

