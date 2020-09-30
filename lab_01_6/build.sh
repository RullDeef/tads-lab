mkdir -p ./bin/
gcc -std=c99 -Wall -Werror -pedantic -O0 -g -ggdb -DTEST -I./src $(find ./src -name *.c) -o ./bin/app-test.exe
gcc -std=c99 -Wall -Werror -pedantic -O0 -g -ggdb -I./src $(find ./src -name *.c) -o ./bin/app-debug.exe
gcc -std=c99 -DNDEBUG -O3 -I./src $(find ./src -name *.c) -o ./bin/app.exe
