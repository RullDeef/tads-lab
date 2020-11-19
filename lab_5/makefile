CC = gcc
CFLAGS := -std=gnu99 -Wall -Werror -pedantic -Wextra \
	-Wuninitialized \
	-Isrc -Ivendor/uki/include -Ivendor/libcmdf/include \
	-ggdb3 -g
# LFLAGS := -Lvendor/uki/lib -luki

SRCFILES := $(shell find src \( -name *.c ! -name "main.c" \))
OBJFILES := $(subst .c,.o,$(subst src,obj,$(SRCFILES)))

UNIT_SRCFILES := $(shell find unit_test -name *.c)
UNIT_OBJFILES := $(subst .c,.o,$(subst unit_test,obj/unit_test,$(UNIT_SRCFILES)))

.PHONY : clean run unit

clean :
	rm -rf obj *.exe

run : app.exe
	./app.exe

unit : unit.exe
	./unit.exe

unit.exe : $(OBJFILES) $(UNIT_OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

app.exe : $(OBJFILES) obj/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

obj/%.o : src/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^

obj/unit_test/%.o : unit_test/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^
