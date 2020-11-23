for x in $(find ./src -name "*.c")
do
    obj=$(echo ${x%/*} | sed -e "s;./src;;")
    mkdir -p "./gcov$obj"
    gcov -o ./obj/$obj ${x##*/}
    mv ./${x##*/}.gcov ./gcov$(echo $x | sed -e "s;./src;;").gcov
done

rm -rf ./*.h.gcov
