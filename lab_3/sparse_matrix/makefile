.PHONY : build_sm clean_sm

build_sm : sparse_matrix/obj/sparse_matrix.a

sparse_matrix/bin/sparse_matrix.a : sparse_matrix/obj/sparse_matrix.o
	mkdir -p sparse_matrix/bin; ar rcs $@ $^

sparse_matrix/obj/sparse_matrix.o : sparse_matrix/src/sparse_matrix.c
	mkdir -p sparse_matrix/obj; $(CC) -Isparse_matrix/include -o $@ -c $^

clean_sm :
	rm -rf sparse_matrix/obj sparse_matrix/bin
