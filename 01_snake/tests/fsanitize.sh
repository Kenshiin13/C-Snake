#!/usr/bin/env bash

program='./snake_fsanitize'

rm -rf ausgabe diff
mkdir -p ausgabe diff

for name in 1_1 1_2 1_3 1_4 1_5 1_6 1_7 1_8 1_9 1_10 2_1 3_1 3_2 3_3 3_4 4_1 5_1
do
    level="level/$(echo $name | cut -d_ -f1).txt"
    input="eingabe/level$name.txt"
    output="ausgabe/level$name.txt"
    expected="erwartete_ausgabe/level$name.txt"
    diff_file="diff/level$name.txt"

    (set -x ; "$program" "$input" "$output" "$level")

    return_value="$?"

    if [ $return_value -eq 0 ]
    then
        printf '\tOK: Rückgabewert ist 0.\n'
    else
        printf '\tFEHLER: Rückgabewert ist nicht 0, sondern %s.\n' "$return_value"
        exit 6
    fi

    if diff -y "$output" "$expected" > "$diff_file"
    then
        printf '\tOK: %s stimmt mit %s überein.\n' "$expected" "$output"
    else
        printf '\tFEHLER: %s stimmt nicht mit %s überein. Details in %s.\n' "$expected" "$output" "$diff_file"
        exit 7
    fi

    printf '\n'
done
