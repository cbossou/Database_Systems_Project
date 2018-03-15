#!/bin/bash
if [ ! -e results ]; then
  echo "please run RocksBench to generate results first"
  exit 1
fi

results=$(ls results)
for test_type in $results; do
  tests=$(ls results/$test_type)
  for test in $tests; do
    ext=$(echo $test | cut -d '.' -f 2)
    if [ ! "$ext" = "csv" ]; then
      continue
    fi
    ./analyze_results.sh results/$test_type/$test
  done
done

echo "Finished analyzing results"
