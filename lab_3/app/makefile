CFLAGS := -std=gnu99 -Wall -Werror -pedantic
LFLAGS :=

.PHONY : build_app clean_app

build_app : app/bin/app.exe

app/bin/app.exe : app/obj/main.o app/obj/conio.o \
		console/bin/console.a \
		sparse_matrix/bin/sparse_matrix.a
	mkdir -p app/bin; $(CC) $(LFLAGS) -o $@ $^

app/obj/main.o : app/src/main.c
	mkdir -p app/obj; $(CC) $(CFLAGS) -Iapp/include -Iconsole/include -Isparse_matrix/include -o $@ -c $^

app/obj/conio.o : app/src/conio.c
	mkdir -p app/obj; $(CC) $(CFLAGS) -Iapp/include -Iconsole/include -Isparse_matrix/include -o $@ -c $^

clean_app :
	rm -rf app/bin app/obj
