#!/bin/bash

# This script runs analyze_results on all results CSVs which are in every sub folder of all *_results folders

num_results=$(ls | grep '_results$')
if [ "${num_results[0]}" == "" ]; then
  echo "please run RocksBench to generate results first"
  exit 1
fi

for results in $(ls | grep '_results$'); do
  for test_type in $(ls $results); do
    analysis=$results/$test_type/analysis
    if [ ! -d $analysis ] || [ "$(ls $analysis)" == "" ]; then
      echo "Results not yet analyzed, cannot plot"
      exit 1
    fi
    tests=$(ls $results/$test_type)
    for test in $tests; do
      ext=$(echo $test | cut -d '.' -f 2)
      if [ ! "$ext" = "csv" ]; then
        continue
      fi
    done
    complete_put_results=$analysis/$(echo $test_type)_complete_put.csv
    complete_get_results=$analysis/$(echo $test_type)_complete_get.csv
    complete_delete_results=$analysis/$(echo $test_type)_complete_delete.csv
    python ./plot_results.py $complete_put_results
    python ./plot_results.py $complete_get_results
    python ./plot_results.py $complete_delete_results
  done
done

echo "Finished analyzing results"
