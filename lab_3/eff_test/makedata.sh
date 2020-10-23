exit 0

rm -rf eff_test/stats.txt

for ((per = 0; per <= 50; per += 1))
do
    printf "4\n$per\n\n6\n" >.temp
    ./app.exe <.temp
done

rm -rf .temp
