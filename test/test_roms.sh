#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Syntax: $0 <test path>"
    exit
fi
FILES=$(find $1 -type f -name "*.gb")
OUT_FILE=results-$(git rev-parse --short=8 HEAD).txt
for file in $FILES
do
    ./gusgb $file
    echo "Type the result [ OK | NOK ] and hit ENTER"
    read result
    echo "$file $result" >> "$OUT_FILE"
done
