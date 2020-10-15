.PHONY : test build_con clean_con

build_con : console/bin/console.a
test : console/bin/test.exe

console/bin/test.exe: console/bin/console.a console/test/main.c
	mkdir -p console/bin; $(CC) -o $@ $^

console/bin/console.a : \
		console/obj/con_menu.o \
		console/obj/utf8.o
	mkdir -p console/bin; ar rcs $@ $^

console/obj/con_menu.o : console/src/con_menu.c
	mkdir -p console/obj; $(CC) -Iconsole/include -o $@ -c $^

console/obj/utf8.o : console/src/utf8.c
	mkdir -p console/obj; $(CC) -Iconsole/include -o $@ -c $^

clean_con :
	rm -rf console/obj console/bin
