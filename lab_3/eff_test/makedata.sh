exit 0

rm -rf eff_test/stats.txt

for ((per = 5; per <= 100; per += 5))
do
    printf "4\n$per\n\n6\n" >.temp
    ./app.exe <.temp
done

rm -rf .temp
