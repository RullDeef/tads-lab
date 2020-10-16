.PHONY : build clean run

build : app.exe

run : app.exe
	./app.exe

app.exe : \
		obj/con_menu.o \
		obj/conio.o \
		obj/sparse_matrix.o \
		obj/main.o
	$(CC) $(LFLAGS) -o $@ $^

obj/con_menu.o : src/con_menu.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/conio.o : src/conio.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/sparse_matrix.o : src/sparse_matrix.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

obj/main.o : src/main.c
	mkdir -p obj; $(CC) $(CFLAGS) -o $@ -c $^

clean :
	rm -rf obj
