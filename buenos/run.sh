#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 <program>"
    exit 1
fi

make
make -C tests/
rm store.file
util/tfstool create store.file 2048 disk1

for file in $(find tests -type f -executable -printf "%f\n")
do
    util/tfstool write store.file tests/$file $file
done
yams buenos initprog=[disk1]$1
