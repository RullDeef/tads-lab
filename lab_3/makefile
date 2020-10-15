.PHONY : build clean run

include sparse_matrix/makefile
include console/makefile
include app/makefile

build : build_app
clean : clean_app clean_con clean_sm

run : app/bin/app.exe
	app/bin/app.exe