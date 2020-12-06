CC = gcc
CFLAGS := -std=gnu99 -Wall -Werror -pedantic -Wextra -Wuninitialized \
	-Isrc -Ivendor/uki -Ivendor/callocs/src -ggdb3 -g
LFLAGS :=

SRCFILES := $(shell find src \( -name *.c ! -name "main.c" \))
OBJFILES := $(subst .c,.o,$(subst src,obj,$(SRCFILES)))

.PHONY : clean run auto gendata func

clean :
	rm -rf obj *.exe

auto : app.exe
	./app.exe < stdin.txt

run : app.exe
	./app.exe

gendata :
	py gendata.py 20

func : func.exe
	py func_test/test.py

func.exe : $(OBJFILES) obj/main.o vendor/callocs/src/callocs.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

app.exe : $(OBJFILES) obj/main.o vendor/callocs/src/callocs.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

obj/%.o : src/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^
