#!/bin/bash
#SBATCH --mail-user=mo.jia@stonybrook.edu
#SBATCH --mail-type=END
#SBATCH --job-name=SKonlyCoarseBinSmear_2D
#SBATCH --output=SKonlyCoarseBinSmear_2D.log
#SBATCH --error=SKonlyCoarseBinSmear_2D.err
#SBATCH --account=rpp-blairt2k
#SBATCH --time=4:00:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=5G

#ROOT6 should be used for contour comparisons
#module load gcc/7.3.0
#module load root/6.14.04
module load root/5.34.36

#cd $MACH3/AtmJointFit_Utils/code_for_Junjie
cd /home/mojia/projects/rpp-blairt2k/mojia/SoloExpFitsContours/ContourPlottingScripts

#files=/project/6008045/skt2kjoint/reweighted_full/AsimovB_RCreweight_reduced/AsimovB_MegaMerge_reduced_reweighted_float.root
#files=/home/mojia/projects/rpp-blairt2k/mojia/mach3_chains/t2konly/T2KOnly_MegaMerge_reduced_reweighted_smeared_OA2020.root
#files=/project/6008045/skt2kjoint/OA2020chains/MaCh3-OA2020_ALL_data_RCweights_reduced_burnInCut_smearedRevision_withPionSI.root
#/project/6008045/skt2kjoint/reweighted_full/AsimovB_reduced/asimovb_batch3/Output_Iter_3/MaCh3_Job_4_Iter_3_Exec_1_asimovb_float_reduced.root
#files=/home/mojia/scratch/jointfit/SKT2KJointFit_Data/RCreweight_reduced_files/JF2023_summit_MegaMerge_float_reduced_reweighted.root
#files=/home/mojia/scratch/jointfit/SKonly/08122024/SKOnly_PostBANFFConstrained_MegaMerge_float_reduced_reweighted.root
#files=/home/mojia/scratch/jointfit/SKonly/09142024/StatOnlyHPD/SKOnly_StatOnlyHPD_MegaMerge_float_reduced_reweighted.root
files=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1/SKOnly_coarse31x26_PHAoff_MegaMerge_float_reduced_reweighted.root

RCs="false true"
burnin=50000
addinfo="skonly_coarse31x26"

# Loop over files
for file in $files; do
  # Loop over reactor constraint (yes, no)
  for RC in $RCs; do
    #JJ: you can run all of them together or separately; run time estimation: 1D~10min, 2D~1.5hr; to save you some time, 1D contour output and 2D contour output files are ready and included in the same dir
    #root -l -b -q 'MakeContours1D.C("'$file'", '$burnin', '$RC', 0, false)' &
    #wait
    root -l -b -q 'MakeContours2D.C("'$file'", '$burnin', '$RC', 0, false,"'$addinfo'")' &
    wait
    #JJ: haven't try this yet
    #root -l -b -q 'CalculatesBayesFactor.C("'$file'", '$burnin', '$RC')'
  done
done

# Then finally draw the triangle plot
#JJ: haven't try this yet
#root -l -b -q 'DrawTrianglePlot.C("'$file1d'", '$file2d', '$RC')'
