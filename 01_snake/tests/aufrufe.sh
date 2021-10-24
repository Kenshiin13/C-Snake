#!/usr/bin/env bash

rm -rf ausgabe diff
mkdir -p ausgabe diff

# format:
# (input or "-" (stdin)) (output or "-" (stdout)) level
./snake < eingabe/level1_1.txt > ausgabe/level1_1.txt
./snake eingabe/level1_2.txt ausgabe/level1_2.txt level/1.txt
./snake - ausgabe/level1_3.txt level/1.txt < eingabe/level1_3.txt
./snake eingabe/level1_4.txt - level/1.txt > ausgabe/level1_4.txt
./snake eingabe/level2_1.txt ausgabe/level2_1.txt level/2.txt eingabe/level3_1.txt ausgabe/level3_1.txt level/3.txt
./snake - ausgabe/level3_2.txt level/3.txt eingabe/level3_3.txt - level/3.txt < eingabe/level3_2.txt > ausgabe/level3_3.txt

for name in 1_1 1_2 1_3 1_4 2_1 3_1 3_2 3_3
do
    input="eingabe/level$name.txt"
    output="ausgabe/level$name.txt"
    expected="erwartete_ausgabe/level$name.txt"
    diff_file="diff/level$name.txt"

    if diff -y "$output" "$expected" > "$diff_file"
    then
        printf '\tOK: %s stimmt mit %s überein.\n' "$expected" "$output"
    else
        printf '\tFEHLER: %s stimmt nicht mit %s überein. Details in %s.\n' "$expected" "$output" "$diff_file"
        exit 1
    fi
done

