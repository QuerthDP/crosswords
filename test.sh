#!/bin/bash

for i in *.out
do
    DIR=${i%.out}
    mkdir $DIR
    cp $i ./$DIR/$i
    cp ${i%.out}.cc ./$DIR/${i%.out}.cc
    cp crosswords.cc ./$DIR/crosswords.cc
    cp crosswords.h ./$DIR/crosswords.h
    cd ./$DIR
    g++ -Wall -Wextra -O2 -std=c++20 *.cc

    echo -e "${i%.out} >>"

    if (valgrind --leak-check=full -q --error-exitcode=1 ./a.out >result.out)
    then
        echo -e "\tASSERT \033[0;32m"OK"\033[0;0m"
    else
        echo -e "\tASSERT \033[0;31m"NOT OK"\033[0;0m"
    fi

    if (diff result.out $i &>/dev/null)
    then
        echo -e "\tRESULT \033[0;32m"OK"\033[0;0m"
    else
        echo -e "\tRESULT \033[0;31m"NOT OK"\033[0;0m"
    fi

    cd ..
    rm -r ./$DIR 2>/dev/null
done