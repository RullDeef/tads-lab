CFLAGS := -std=c99 -Wall -Werror -pedantic -Wextra -Ivendor/uki/include
LFLAGS := -Lvendor/uki/lib -luki

.PHONY : build clean run make_dataset

make_dataset : app.exe
	eff_test/makedata.sh; py eff_test/graph.py

build : app.exe

run : app.exe
	./app.exe

app.exe : \
		obj/conio.o \
		obj/sparse_matrix.o \
		obj/main.o
	$(CC) -o $@ $^ $(LFLAGS)

obj/sparse_matrix.o : src/sparse_matrix.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/conio.o : src/conio.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/main.o : src/main.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

clean :
	rm -rf obj
