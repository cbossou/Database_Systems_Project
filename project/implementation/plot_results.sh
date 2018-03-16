#!/bin/bash

# This script takes in a filename (excluding extension) and then uses the pilot generated results to plot info about it

if [ ! $# -eq 1 ]; then
  echo "Please provide a file to plot"
  exit 1
fi

files=$(ls $1_*)
if [ "${files[0]}" == "" ]; then
  echo "Analysis results for $1.csv don't exist"
  exit 1
fi

echo "Couldn't plot $1 because system is not yet ready"
