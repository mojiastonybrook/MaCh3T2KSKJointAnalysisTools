#!/bin/bash

dists="h_dcp h_th13 h_th23 h_dm32"
ordering="BH NH IH"
for dist in $dists; do
  for order in $ordering; do
    name=${dist}_${order}
    root -l -b -q 'CompareRC.cpp("'$name'")'
  done
done
