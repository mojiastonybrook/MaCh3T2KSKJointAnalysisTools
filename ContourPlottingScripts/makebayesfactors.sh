#!/bin/bash

#files="../T2KSK_JointFit_Data_Reduced_Floats_reweighted.root"
#files="../t2kskJF_summitlast_beluga_p100_t4_summitlarge_reduced_float_reweighted_smeared_0.0000360.root"
#files=/home/clarence/work/mach3/skt2kjoint/burn_in_checks_asimov/T2KSK_JointFit_Asimov_Reduced_Floats_reweighted_smeared_0.0000360.root
#files=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1/SKOnly_coarse31x26_PHAoff_MegaMerge_float_reduced_reweighted.root
#files=/home/mojia/scratch/jointfit/SKonly/09042024/SKOnly_PHAon_batch1/SKOnly_PHAon_MegaMerge_float_reduced_reweighted.root
#files=/home/mojia/scratch/jointfit/SKonly/11292023/SKOnly_MegaMerge_float_reduced_reweighted.root
files=/home/mojia/scratch/jointfit/SKonly/10302024/coarse31x26PHAon_bat1/SKOnly_coarse31x26PHAon_MegaMerge_float_reduced_reweighted.root

RCs="true false"
burnin=50000

# Loop over files
for file in $files; do
  # Loop over reactor constraint (yes, no)
  for RC in $RCs; do
    #root -l -b -q 'MakeContours1D.C("'$file'", '$burnin', '$RC', 0, true)' &
    #root -l -b -q 'MakeContours2D.C("'$file'", '$burnin', '$RC', 0, true)' &
    ( root -l -b -q 'CalculateBayesFactor.C("'$file'", '$burnin', '$RC')' &> $(basename $file)_${RC}_bayesfactors.out & )
  done
done

# Then finally draw the triangle plot 
#root -l -b -q 'DrawTrianglePlot.C("'$file1d'", '$file2d', '$RC')'
