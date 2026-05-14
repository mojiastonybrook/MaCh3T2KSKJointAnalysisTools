#!/bin/bash

fileloc=/home/clarence/work/mach3/skt2kjoint/asimovs

for hier in 0 1 2; do
  root -l -q -b 'DrawTrianglePlot.C("'$fileloc'/1d/wRC/T2KSK_JointFit_Asimov_Reduced_Floats_reweighted_smeared_0.0000360.root_Contours_1D_wRC_BurnIn_80000.root", "'$fileloc'/2d/wRC/T2KSK_JointFit_Asimov_Reduced_Floats_reweighted_smeared_0.0000360.root_Contours_2D_wRC_BurnIn_80000.root", 1, '$hier')'
  root -l -q -b 'DrawTrianglePlot.C("'$fileloc'/1d/woRC/T2KSK_JointFit_Asimov_Reduced_Floats_reweighted_smeared_0.0000360.root_Contours_1D_woRC_BurnIn_80000.root", "'$fileloc'/2d/woRC/T2KSK_JointFit_Asimov_Reduced_Floats_reweighted_smeared_0.0000360.root_Contours_2D_woRC_BurnIn_80000.root", 0, '$hier')'
done
