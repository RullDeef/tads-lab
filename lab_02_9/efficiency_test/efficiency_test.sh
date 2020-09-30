#!/bin/bash

cd efficiency_test
rm -rf a_slow.txt a_fast.txt b_slow.txt b_fast.txt

for ((N = 0; N <= 80; N += 1))
do
    sed -n "1,${N}p;16483q" test_data.txt > temp_test_data.txt

    ../app.exe < ./input_a_slow.txt 1>/dev/null 2>> a_slow.txt
    ../app.exe < ./input_b_slow.txt 1>/dev/null 2>> b_slow.txt
    ../app.exe < ./input_a_fast.txt 1>/dev/null 2>> a_fast.txt
    ../app.exe < ./input_b_fast.txt 1>/dev/null 2>> b_fast.txt
done

rm -rf temp_test_data.txt
