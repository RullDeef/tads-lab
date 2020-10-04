CFLAGS := -std=gnu99 -Wall -Werror -pedantic -I.
LFLAGS :=

.PHONY : test unittest

test : unittest
unittest : tests/bin/unit.exe
	# drmemory -brief -batch -warn_null_ptr -ignore_kernel -possible_leaks -check_leaks -count_leaks --
	tests/bin/unit.exe

tests/bin/unit.exe : obj/sparse_matrix.o tests/obj/main.o
	$(CC) $(LFLAGS) -o $@ $^

tests/obj/main.o : tests/unit/main.c
	$(CC) $(CFLAGS) -o $@ -c $^
