#!/bin/bash

#files=Contours_1D_woRC_BurnIn_80000_updateOscStepsize_skonly_float.root
#files=Contours_1D_woRC_BurnIn_80000_banff_float.root
#files=Contours_1D_wRC_BurnIn_80000_skonly_fixedEvtMirParam_float.root
files=Contours_1D_woRC_BurnIn_80000_skonly_Asimov_float.root
#Contours_1D_woRC_BurnIn_0_asimovb_float.root
dists="h_dcp h_th13 h_th23 h_dm32"
hiers="BH IH NH"

for file in $files; do
  for dist in $dists; do
    for hier in $hiers; do
      plot=${dist}_${hier}
      root -l -b -q 'Draw1D.cpp("'$file'", "'$plot'")' &
      wait
    done
  done
done
