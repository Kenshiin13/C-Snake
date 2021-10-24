#!/usr/bin/env bash

logfile="$1"

if grep -E 'definitely lost: [1-9][0-9,]* bytes' "$logfile" ; then
    printf '\tFEHLER: Speicherleck gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Speichertest 1.\n'
fi

if grep -E 'indirectly lost: [1-9][0-9,]* bytes' "$logfile" ; then
    printf '\tFEHLER: Speicherleck gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Speichertest 2.\n'
fi

if grep -E 'possibly lost: [1-9][0-9,]* bytes' "$logfile" ; then
    printf '\tFEHLER: Speicherleck gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Speichertest 3.\n'
fi

if grep -E 'still reachable: [1-9][0-9,]* bytes' "$logfile" ; then
    printf '\tFEHLER: Speicherleck gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Speichertest 4.\n'
fi

if grep -E 'in use at exit: [1-9][0-9,]* bytes' "$logfile" ; then
    printf '\tFEHLER: Speicherleck gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Speichertest 5.\n'
fi

if grep -E 'Invalid free' "$logfile" ; then
    printf '\tFEHLER: Invalid free gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Invalid free test.\n'
fi

if grep -E 'Invalid read of size' "$logfile" ; then
    printf '\tFEHLER: Invalid read gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Invalid read test.\n'
fi

if grep -E 'Invalid write of size' "$logfile" ; then
    printf '\tFEHLER: Invalid write gefunden. Details in Datei %s.\n' "$logfile"
    exit 4
else
    printf '\tOK: Invalid write test.\n'
fi

if grep -E 'depends on uninitialised' "$logfile" ; then
    printf '\tFEHLER: Uninitialisierte Variablen verwendet. Details in Datei %s.\n' "$logfile"
    exit 4
else
   printf '\tOK: Nur initialisierte Variablen verwendet (zumindest laut valgrind).\n'
fi
