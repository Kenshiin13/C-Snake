#!/usr/bin/env bash

# Für mehr Informationen zu valgrind:
# https://valgrind.org/docs/manual/mc-manual.html

rm -rf ausgabe
rm -rf diff
rm -rf valgrind
mkdir -p ausgabe
mkdir -p diff
mkdir -p valgrind

outputfile='valgrind/temporary_output.txt'

if sha256sum -c 'tests/pruefsummen.txt' ; then
    printf '\tOK: Testdateien wurden nicht modifiziert.\n\n'
else
    printf '\./tFEHLER: Eine oder mehrere Testdateien wurden modifiziert.\n\n'
    exit 1
fi

for name in 1_1 1_2 1_3 1_4 1_5 1_6 1_7 1_8 1_9 1_10 2_1 3_1 3_2 3_3 3_4 4_1 5_1
do
    level="level/$(echo $name | cut -d_ -f1).txt"
    input="eingabe/level$name.txt"
    output="ausgabe/level$name.txt"
    expected="erwartete_ausgabe/level$name.txt"
    logfile="valgrind/$name.txt"
    diff_file="diff/level$name.txt"

    (set -x ; valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --log-file="$logfile" \
        ./snake "$input" "$output" "$level" )

    return_value="$?"

    if [ $return_value -eq 0 ]; then
        printf '\tOK: Rückgabewert ist 0.\n'
    else
        printf '\tFEHLER: Rückgabewert ist nicht 0, sondern %s.\n' "$return_value"
        exit 5
    fi

    if ! tests/valgrind_log_pruefen.sh "$logfile" ; then
        exit 1
    fi

    if diff -y "$output" "$expected" > "$diff_file"
    then
        printf '\tOK: %s stimmt mit %s überein.\n' "$expected" "$output"
    else
        printf '\tFEHLER: %s stimmt nicht mit %s überein. Details in %s.\n' "$expected" "$output" "$diff_file"
        exit 1
    fi

    printf '\n'
done
