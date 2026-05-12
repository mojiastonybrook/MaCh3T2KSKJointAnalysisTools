#!/bin/bash
#source /home/mojia/scratch/AsimovB/setup.sh

#file_loc=/home/mojia/projects/rpp-blairt2k/mojia/mach3_chains/skonly/RCreweight_reduced_files
#save_dir=/home/mojia/projects/rpp-blairt2k/mojia/mach3_chains/skonly
#file_loc=/home/mojia/scratch/jointfit/BANFFFit/06102024/RCreweight_reduced_files
#save_dir=/home/mojia/scratch/jointfit/BANFFFit/06102024
#file_loc=/home/mojia/scratch/jointfit/SKT2KJointFit_Data/RCreweight_reduced_files/reweighted
#save_dir=/home/mojia/scratch/jointfit/SKT2KJointFit_Data/RCreweight_reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonly/08202024/reduced_files/RCreweight_reduced_files/batch_0/Output_Iter_0
#file_loc=/home/mojia/scratch/jointfit/SKonly/09142024/StatOnlyHPD/RCreweight_reduced_files/Output_Iter_0
#file_loc=/home/mojia/scratch/jointfit/SKonlyAsimov_PHAon/RCreweight_reduced_files/Output_Iter_0
#file_loc=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1/RCreweight_reduced_files/Output_Iter_1
#file_loc=/home/mojia/scratch/jointfit/SKonly/10302024/coarse31x26PHAon_bat1/RCreweight_reduced_files/Output_Iter_[1-2]
#file_loc=/home/mojia/scratch/jointfit/SKonly/11052024/coarse31x26_PHAon_bat2/RCreweight_reduced_files/Output_Iter_1
#file_loc=/home/mojia/scratch/jointfit/JointFit/12132024/RCreweight_reduced_files
#file_loc=/home/mojia/scratch/jointfit/JointFit/12162024/RCreweight_reduced_files
file_loc=/home/mojia/scratch/jointfit/JointFit_extra/RCreweight_reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonly/09042024/SKOnly_PHAon_batch1/RCreweight_reduced_files/Output_Iter_1
#save_dir=/home/mojia/scratch/jointfit/SKonly/09142024/StatOnlyHPD
#save_dir=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1
#save_dir=/home/mojia/scratch/jointfit/SKonly/09042024/SKOnly_PHAon_batch1
#save_dir=/home/mojia/scratch/jointfit/SKonly/10302024/coarse31x26PHAon_bat1
#save_dir=/home/mojia/scratch/jointfit/JointFit/12132024
save_dir=/home/mojia/scratch/jointfit/JointFit_extra

#files=$(find ${file_loc}/JFBatch_* -name "*_float_reduced_flat_rc_reweighted.root" -type f)
files=$(find ${file_loc}/*/JFBatch_* -name "*_float_reduced_flat_rc_reweighted.root" -type f)

nfiles=$(echo $files | wc -w)
echo "Found $nfiles files"

hadd ${save_dir}/JF_OscProbCalModified_Merge_float_reduced_reweighted.root ${files}
