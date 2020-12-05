CC = gcc
CFLAGS := -std=gnu99 -Wall -Werror -pedantic -Wextra -Wuninitialized \
	-Isrc -Ivendor/uki -Ivendor/callocs/src -ggdb3 -g
LFLAGS :=

SRCFILES := $(shell find src \( -name *.c ! -name "main.c" \))
OBJFILES := $(subst .c,.o,$(subst src,obj,$(SRCFILES)))

.PHONY : clean run gendata

clean :
	rm -rf obj *.exe

run : app.exe
	./app.exe < stdin.txt

gendata :
	py gendata.py 20

app.exe : $(OBJFILES) obj/main.o vendor/callocs/src/callocs.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

obj/%.o : src/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^
