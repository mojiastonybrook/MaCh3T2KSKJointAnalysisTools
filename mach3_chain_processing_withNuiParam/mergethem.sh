#!/bin/bash
#source /home/mojia/scratch/AsimovB/setup.sh

#file_loc=/home/mojia/scratch/jointfit/SKT2KJointFit_Data/nui_osc_RCreweighted
file_loc=/home/mojia/scratch/jointfit/JointFit/12162024/nui_osc_RCreweighted

#save_dir=/home/mojia/scratch/jointfit/SKT2KJointFit_Data
save_dir=/home/mojia/scratch/jointfit/JointFit/12162024

#file_loc=/home/mojia/scratch/jointfit/postBANFFFit/nui_osc_RCreweighted/batch_001_01
#save_dir=/home/mojia/scratch/jointfit/postBANFFFit

#files=$(find ${file_loc} -name "MaCh3_MCMC_chain_11*_Iter_0_7_float_osc_nui.root" -type f)
#files=$(find ${file_loc} -name "MaCh3_MCMC_chain_[1-4][0-9]_Iter_0_7_float_osc_nui_flat_rc_reweighted.root" -type f)
files=$(find ${file_loc} -name "MaCh3_Job_*_Iter_*_Exec_*_float_osc_nui_flat_rc_reweighted.root" -type f)

nfiles=$(echo $files | wc -w)
echo "Found $nfiles files"

#hadd ${save_dir}/JF2023_Data_Summit_MegaMerge_float_osc_nui_reweighted.root ${files}
#hadd ${save_dir}/JF2023_Data_Summit_40chains_Merge_float_osc_nui_reweighted.root ${files}
#hadd ${save_dir}/PostBANFFFit_batch1_MegaMerge_float_osc_nui_reweighted.root ${files}
hadd ${save_dir}/JF2023_Data_OscProbCalModified_CorrEs_batch1Iter0_Merge_float_osc_nui_reweighted.root ${files}
