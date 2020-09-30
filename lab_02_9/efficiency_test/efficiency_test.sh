#!/bin/bash

cd efficiency_test

rm -rf a_slow.txt a_fast.txt b_slow.txt b_fast.txt

# printf "Сортировка без использования дополнительных массивов и таблиц:\n\n"
../app.exe < ./input_a_slow.txt 1>/dev/null 2>> a_slow.txt
# printf "Сортировка с использованием дополнительной таблицы ключей:\n\n"
# ../app.exe < ./input_a_fast.txt 1>/dev/null 2> a_fast.txt
