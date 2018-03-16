#!/bin/bash

# This script runs analyze_results on all results CSVs which are in every sub folder of all *_results folders

num_results=$(ls | grep '_results$')
if [ "${num_results[0]}" == "" ]; then
  echo "please run RocksBench to generate results first"
  exit 1
fi

for results in $(ls | grep '_results$'); do
  for test_type in $(ls $results); do
    tests=$(ls $results/$test_type)
    for test in $tests; do
      ext=$(echo $test | cut -d '.' -f 2)
      if [ ! "$ext" = "csv" ]; then
        continue
      fi
      ./plot_results.sh $results/$test_type/$(echo $test | cut -d '.' -f 1)
    done
  done
done

echo "Finished analyzing results"
