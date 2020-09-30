#!/bin/bash

printf "Сортировка без использования дополнительных массивов и таблиц:\n\n"
time -p $(./app_a.exe < ./efficiency_test/input_commands.txt > out_a.txt)

echo

printf "Сортировка с использованием дополнительной таблицы ключей:\n\n"
time -p $(./app_b.exe < ./efficiency_test/input_commands.txt > out_b.txt)
