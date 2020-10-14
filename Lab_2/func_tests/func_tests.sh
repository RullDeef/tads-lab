# rebuild app first
./build.sh

# regenerate func tests data
py ./tests/func_tests_generator.py

# run over all positives

declare -i pos_total=$(ls -1 ./tests/pos_*_in.txt | wc -l)
declare -i pos_passed=0

for ((i = 1; i <= $pos_total; i++))
do
    n=$(printf "%02d" $i)
    in="./tests/pos_${n}_in.txt"
    out=".temp"
    actual="./tests/pos_${n}_out.txt"

    # run program
    ./bin/app-test.exe >$out <$in
    status=$?

    # check results
    if [[ $status != 0 ]]
    then
        echo "Positive test #$n: Failed with status $status"
    else
        diff -q $out $actual >/dev/null
        if [[ $? -eq 0 ]]
        then
            echo "Positive test #$n: Passed"
            pos_passed=$(($pos_passed + 1))
        else
            echo "Positive test #$n: Failed"
            echo "Expected output:"
            cat $actual
            echo "Got:"
            cat $out
        fi
    fi
done

echo

declare -i neg_total=$(ls -1 ./tests/neg_*_in.txt | wc -l)
declare -i neg_passed=0

for ((i = 1; i <= $neg_total; i++))
do
    n=$(printf "%02d" $i)
    in="./tests/neg_${n}_in.txt"
    out=".temp"

    # run program
    ./bin/app-test.exe >$out <$in
    status=$?

    # check results
    if [[ $status == 0 ]]
    then
        echo "Negative test #$n: Failed with status $status"
    else
        echo "Negative test #$n: Passed"
        neg_passed=$(($neg_passed + 1))
    fi
done

#rm -rf ./.temp
