#!/bin/bash

cd efficiency_test
rm -rf a_slow.txt a_fast.txt b_slow.txt b_fast.txt

for ((N = 1; N <= 300; N += 1))
do
    sed -n "1,${N}{p;b}" test_data.txt > temp_test_data.txt
    ../app.exe < ./input.txt 1>/dev/null 2>> res.txt
done

rm -rf temp_test_data.txt
