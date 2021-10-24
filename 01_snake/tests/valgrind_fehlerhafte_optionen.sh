#!/usr/bin/env bash

rm -rf valgrind
mkdir -p valgrind
mkdir -p ausgabe

input="eingabe/level1_1.txt"
output="ausgabe/test.txt"
level="level/1.txt"
doesnotexist="Diese_Datei_sollte_nicht_existieren.txt"
invalid="Diese_Datei_sollte_nicht_schreibbar_sein.txt"
stderrfile="ausgabe/stderr.txt"
logfile="valgrind/log.txt"

rm -rf "$invalid"
rm -rf "$doesnotexist"
rm -rf "$output"

touch "$invalid"
chmod -rw "$invalid"

function test_params() {
    expected_error_code="$1"

    (set -x; valgrind --leak-check=full --show-leak-kinds=all --log-file=$logfile ./snake "${@:2}" 2> "$stderrfile")
    return_value=$?

    printf '\tStandardfehlerausgabe:\n\t\t'
    cat "$stderrfile"

    if ! tests/valgrind_log_pruefen.sh "$logfile"
    then
        exit 1
    fi

    if [ $return_value -eq "$expected_error_code" ]
    then
        printf '\tOK: Rückgabewert ist %s wie erwartet.\n' "$expected_error_code"
    else
        printf '\tFEHLER: Rückgabewert ist nicht %s, sondern %s.\n\n' "$expected_error_code" "$return_value"
        exit 1
    fi

    if [ -s "$stderrfile" ] ; then
        printf '\tOK: Es wurde etwas auf der Standardfehlerausgabe ausgegeben.\n'
    else
        printf '\tFEHLER: Es wurde keine Fehlermeldung auf der Standardfehlerausgabe ausgegeben.\n'
        exit 1
    fi

    printf '\n'
}

printf 'Prüfe Behandlung von nicht existierender Eingabedateie.\n\n'
test_params '2' "$doesnotexist" "$output" "$level"

printf 'Prüfe Behandlung von nicht existierendem Level.\n\n'
test_params '2' "$input" "$output" "$doesnotexist"

printf "Prüfe Behandlung von nicht-schreibbarer Ausgabedatei $invalid. Dieser Test setzt vorraus, dass Ihr Betriebssystem Schreibrechte respektiert.\n\n"
test_params '2' "$input" "$invalid" "$level"

printf 'Prüfe Behandlung von nicht-lesbarer Eingabedatei "eingabe" (ist ein Ordner).\n\n'
test_params '3' 'eingabe' "$output" "$level"

printf 'Prüfe Behandlung von nicht-lesbarer Level-Datei "eingabe" (ist ein Ordner).\n\n'
test_params '3' "$input" "$output" 'eingabe'

printf "Prüfe Behandlung von falscher Parameteranzahl (1 Parameter, argc = 2).\n\n"
test_params '1' "$input"

printf "Prüfe Behandlung von falscher Parameteranzahl (2 Parameter, argc = 3).\n\n"
test_params '1' "$input" "$output"

printf "Prüfe Behandlung von falscher Parameteranzahl (4 Parameter, argc = 5).\n\n"
test_params '1' "$input" "$output" "$level" "$input"

printf "Prüfe Behandlung von falscher Parameteranzahl (5 Parameter, argc = 6).\n\n"
test_params '1' "$input" "$output" "$level" "$input" "$output"

printf "Prüfe Behandlung von falscher Parameteranzahl (7 Parameter, argc = 8).\n\n"
test_params '1' "$input" "$output" "$level" "$input" "$output" "$level" "$input"
