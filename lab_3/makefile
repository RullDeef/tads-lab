CC := gcc
CFLAGS := -std=gnu99 -Wall -Werror -pedantic -Wextra \
	-Ivendor/uki/include -Ivendor/libcmdf/include  -ggdb3 -g -Isrc
LFLAGS := -Lvendor/uki/lib -luki

SRCFILES := $(shell find src -name *.c)
OBJFILES := $(subst .c,.o,$(subst src,obj,$(SRCFILES)))

.PHONY : build clean run make_dataset

run : app.exe
	./app.exe

make_dataset : app.exe
	eff_test/makedata.sh; py eff_test/graph.py

build : app.exe

app.exe : $(OBJFILES)
	$(CC) -o $@ $^ $(LFLAGS)

$(OBJFILES) : obj/%.o : src/%.c
	mkdir -p $(dir $@); $(CC) $(CFLAGS) -o $@ -c $^

clean :
	rm -rf obj *.exe
