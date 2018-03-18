#!/bin/bash
./analyze_all.sh
./plot_all.sh
if [ ! -d figures ]; then
  mkdir figures
fi
for results in $(ls | grep '_results$'); do
  these_figures=figures/$(echo $results | cut -d '_' -f 1)
  if [ ! -d $these_figures ]; then
    mkdir $these_figures
  fi
  for test_type in $(ls $results); do
    analysis=$results/$test_type/analysis
    if [ ! -d $analysis ] || [ "$(ls $analysis)" == "" ]; then
      echo "Results not yet analyzed, cannot get graphs"
      exit 1
    fi
    complete_put_graphs=$analysis/$(echo $test_type)_complete_put.png
    complete_get_graphs=$analysis/$(echo $test_type)_complete_get.png
    complete_delete_graphs=$analysis/$(echo $test_type)_complete_delete.png
    cp $complete_put_graphs $complete_get_graphs $complete_delete_graphs $these_figures
  done
done


