CC = gcc
CFLAGS := -std=gnu99 -Wall -Werror -pedantic -Wextra -ggdb3 -g \
	-Isrc -Ivendor/uki/include -Ivendor/libcmdf/include 
# LFLAGS := -Lvendor/uki/lib -luki

SRCFILES := $(wildcard src/**/*.c)
OBJFILES := $(subst .c,.o,$(subst src,obj,$(SRCFILES)))

.PHONY : clean run unit

clean :
	rm -rf obj *.exe

run : app.exe
	./app.exe

unit : unit.exe
	./unit.exe

unit.exe : $(OBJFILES) obj/unit.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

app.exe : $(OBJFILES) obj/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

obj/%.o : src/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^

obj/unit.o : unit_test/unit.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^
