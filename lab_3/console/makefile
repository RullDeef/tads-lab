.PHONY : test build_con clean_con

build_con : console/bin/console.a
test : console/bin/test.exe

console/bin/test.exe: console/bin/console.a console/test/main.c
	mkdir -p console/bin; $(CC) -o $@ $^

console/bin/console.a : console/obj/menu.o
	mkdir -p console/bin; ar rcs $@ $^

console/obj/menu.o : console/src/menu.c
	mkdir -p console/obj; $(CC) -Iconsole/include -o $@ -c $^

clean_con :
	rm -rf console/obj console/bin
