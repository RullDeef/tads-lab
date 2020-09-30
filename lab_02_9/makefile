CC = gcc
CFLAGS = -std=c99 -g3 -O0 -Wall -Werror -pedantic

VAR_A_SRC := src/flat.c src/flat_table_a.c
VAR_B_SRC := src/flat.c src/flat_table_b.c

BUILD_SRC := src/main.c src/operations.c

.PHONY : clean

build : app_a.exe app_b.exe

app_a.exe : BUILD_SRC += $(VAR_A_SRC)
app_a.exe : CFLAGS += -D_VAR_A
app_a.exe : $(BUILD_SRC)
	$(CC) $(CFLAGS) -o $@ $(BUILD_SRC)

app_b.exe : BUILD_SRC += $(VAR_B_SRC)
app_b.exe : CFLAGS += -D_VAR_B
app_b.exe : $(BUILD_SRC)
	$(CC) $(CFLAGS) -o $@ $(BUILD_SRC)

compare : CFLAGS += -D_PERF_TEST
compare : app_a.exe app_b.exe
	./efficiency_test/efficiency_test.sh

clean :
	rm -rf app_*.exe *.stackdump
