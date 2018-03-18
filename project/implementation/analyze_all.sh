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
    if [ ! -d $analysis ]; then
      mkdir $analysis
    fi
    complete_put_results=$analysis/$(echo $test_type)_complete_put.csv
    complete_get_results=$analysis/$(echo $test_type)_complete_get.csv
    complete_delete_results=$analysis/$(echo $test_type)_complete_delete.csv
    echo "size,mean,variance," > $complete_put_results
    echo "size,mean,variance," > $complete_get_results
    echo "size,mean,variance," > $complete_delete_results
    tests=$(ls $results/$test_type)
    for test in $tests; do
      base_test=$(echo $test | cut -d '.' -f 1)
      ext=$(echo $test | cut -d '.' -f 2)
      if [ ! "$ext" = "csv" ]; then
        continue
      fi
      ./analyze_results.sh $results/$test_type/$test
      put_results=$(echo $base_test)_put.txt
      get_results=$(echo $base_test)_get.txt
      delete_results=$(echo $base_test)_delete.txt
      size=$(echo $test | cut -d '.' -f 1 | cut -d '_' -f 4)
      mean_put=$(cat $results/$test_type/$put_results | grep mean | \
        cut -d ' ' -f 2)
      variance_put=$(cat $results/$test_type/$put_results | grep variance | \
        cut -d ' ' -f 2)
      mean_get=$(cat $results/$test_type/$get_results | grep mean | \
        cut -d ' ' -f 2)
      variance_get=$(cat $results/$test_type/$get_results | grep variance | \
        cut -d ' ' -f 2)
      mean_delete=$(cat $results/$test_type/$delete_results | grep mean | \
        cut -d ' ' -f 2)
      variance_delete=$(cat $results/$test_type/$delete_results | grep variance | \
        cut -d ' ' -f 2)
      echo "$size,$mean_put,$variance_put," >> \
        $complete_put_results
      echo "$size,$mean_get,$variance_get," >> \
        $complete_delete_results
      echo "$size,$mean_delete,$variance_delete," >> \
        $complete_delete_results
    done
  done
done

echo "Finished analyzing results"
