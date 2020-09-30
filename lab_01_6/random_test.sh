# build first
gcc --std=c99 -Wall -Werror -pedantic -DDIGIT_PACKS_AMOUNT=3 -I./src/ ./src/big_float/big_float.c ./tests/random_test.c -o ./bin/app-random.exe

# test
./bin/app-random.exe
