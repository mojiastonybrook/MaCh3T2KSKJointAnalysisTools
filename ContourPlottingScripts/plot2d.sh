#!/bin/bash

#files=$(find new_binning/ -maxdepth 1 -name "t2kskJF*2D*rebin.root" -type f)
#files=$(find /home/clarence/work/mach3/skt2kjoint/asimovs -name "T2KSK_JointFit_Asimov*Contours*2D*.root" -type f)
#files=$(find ./ -name "Contours_2D_*_BurnIn_80000_skonly_float.root" -type f)
files=$(find ./ -name "Contours_2D_*RC_BurnIn_50000_skonly_StatOnlyHPD_float.root" -type f)
#Contours_2D_woRC_BurnIn_0_asimovb_float.root
dists="h_dcp_th13 h_dcp_th23 h_dcp_dm32 h_th13_dcp h_th13_th23 h_th13_dm32 h_th23_dcp h_th23_dm32 h_dm32_dcp h_dm32_th13 h_dm32_th23"
hiers="BH IH NH"

for file in $files; do
  for dist in $dists; do
    for hier in $hiers; do
      plot=${dist}_${hier}
      echo $plot
      root -l -b -q 'Draw2D.cpp("'$file'", "'$plot'")' &
      wait
    done
  done
done
