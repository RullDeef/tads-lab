CFLAGS := -std=c99 -Wall -Werror -pedantic -Wextra -Ivendor/uki/include -ggdb3 -g -Isrc
LFLAGS := -Lvendor/uki/lib -luki

.PHONY : build clean run unit make_dataset

make_dataset : app.exe
	eff_test/makedata.sh; py eff_test/graph.py

build : app.exe

run : app.exe
	./app.exe

unit : unit.exe
	./unit.exe

unit.exe : \
		obj/conio.o \
		obj/dense_matrix.o \
		obj/sparse_matrix.o \
		obj/converter.o \
		obj/matrix_input.o \
		obj/unit.o
	$(CC) -o $@ $^ $(LFLAGS)

app.exe : \
		obj/conio.o \
		obj/dense_matrix.o \
		obj/sparse_matrix.o \
		obj/converter.o \
		obj/matrix_input.o \
		obj/main.o
	$(CC) -o $@ $^ $(LFLAGS)

obj/dense_matrix.o : src/dense_matrix.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/sparse_matrix.o : src/sparse_matrix.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/converter.o : src/converter.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/conio.o : src/conio.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/matrix_input.o : src/matrix_input.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/main.o : src/main.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/unit.o : unit_test/unit.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

clean :
	rm -rf obj
