CC = gcc
CFLAGS := -std=gnu99 -Wall -Werror -pedantic -Wextra -ggdb3 -g \
	-Isrc -Ivendor/libcmdf/include 

SRCFILES := $(wildcard src/**/*.c)
OBJFILES := $(subst .c,.o,$(subst src,obj,$(SRCFILES)))

.PHONY : run clean

run : app.exe
	./app.exe

clean :
	rm -rf obj *.exe

app.exe : $(OBJFILES) obj/main.o
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o : src/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^
