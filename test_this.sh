#!/bin/bash

if [ -f foo ]
then
    echo this script requies use of a file foo
    exit 1
fi

if [ -f bar ]
then
    echo this script requies use of a file bar
    exit 1
fi

for X in *
do
    if [ -f "$X" ]
    then
        echo checking $X
        ./sample -cm -i $X -o foo
        ./sample -dm -i foo -o bar
        diff $X bar
        rm foo
        rm bar
    fi
done

exit 0
