CC = gcc
CFLAGS := -std=gnu99 -Wall -Werror -Wextra -Wuninitialized \
	-Isrc -Ivendor/callocs/src -ggdb3 -g -Wno-unused-parameter
LFLAGS :=

SRCFILES := $(shell find src \( -name *.c ! -name "main.c" \))
OBJFILES := $(subst .c,.o,$(subst src,obj,$(SRCFILES)))

.PHONY : clean run auto gendata

clean :
	rm -rf obj *.exe

auto : app.exe
	./app.exe < stdin.txt

run : app.exe
	./app.exe

gendata :
	py gendata.py 20

app.exe : $(OBJFILES) obj/main.o vendor/callocs/src/callocs.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

obj/%.o : src/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^
