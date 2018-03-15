#!/bin/bash
if [ ! $# -eq 1 ]; then
  echo "Please provide a file to analyze"
  exit 1
fi

if [ ! -e "$1" ]; then
  echo "File $1 doesn't exist"
  exit 1
fi

cat $1 | pilot analyze -i 1 -f 0 - > $(ls $1 | cut -d '.' -f 1)_stdput.txt
cat $1 | pilot analyze -i 1 -f 1 - > $(ls $1 | cut -d '.' -f 1)_stdget.txt
