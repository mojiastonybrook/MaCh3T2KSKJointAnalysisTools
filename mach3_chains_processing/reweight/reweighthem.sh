#!/bin/bash

# Last iteration of Summit, the beluga files, the graham p100 files, the graham t4 files
#files="/vols/t2k/users/cvw09/data/skt2kjoint/Summit/DataChains_Summit/raw_iter_8/reduced/MaCh3_MCMC_chain_Iter_8_reduced_merge.root /vols/t2k/users/cvw09/data/skt2kjoint/ComputeCanada/chains_beluga/reduced/JF2023_Beluga_Data_reduced_merged.root /vols/t2k/users/cvw09/data/skt2kjoint/ComputeCanada/chains_graham/JF2023_datafit/gpu_p100/reduced/MaCh3_Job_graham_p100_reduced_merge.root /vols/t2k/users/cvw09/data/skt2kjoint/ComputeCanada/chains_graham/JF2023_datafit/gpu_t4/reduced/MaCh3_Job_graham_t4_reduced_merge.root"
#file_loc=/project/6008045/skt2kjoint
#file_loc=/home/mojia/scratch/AsimovB/AsimovB_reduced
#file_loc=/project/6008045/skt2kjoint/reweighted_full/AsimovB_reduced
#file_loc=/home/mojia/projects/rpp-blairt2k/mojia/mach3_chains/skonly/reduced_files
#file_loc=/home/mojia/scratch/jointfit/BANFFFit/06102024/reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonly/09142024/StatOnlyHPD/reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonlyAsimov_PHAon/reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1/reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonly/10302024/coarse31x26PHAon_bat1/reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonly/11052024/coarse31x26_PHAon_bat2/reduced_files
#file_loc=/home/mojia/scratch/jointfit/JointFit/12132024/reduced_files
#file_loc=/home/mojia/scratch/jointfit/JointFit/12162024/reduced_files
file_loc=/home/mojia/scratch/jointfit/JointFit_extra/reduced_files
#file_loc=/home/mojia/scratch/jointfit/SKonly/09042024/SKOnly_PHAon_batch1/reduced_files
#beluga_files=$(find ${file_loc}/chains_beluga -maxdepth 1 -name "*subbatch*.root" -type f)
#p100_files=$(find ${file_loc}/chains_graham/JF2023_datafit/gpu_p100/merged_stage1 -maxdepth 1 -name "*Exec_[0-9]*.root" -type f)
#t4_files=$(find ${file_loc}/chains_graham/JF2023_datafit/gpu_t4/merged_stage1 -maxdepth 1 -name "*Exec_[0-9]*.root" -type f)
#summit_files=$(find ${file_loc}/DataChains_Summit/raw -maxdepth 1 -name "MaCh3_MCMC_chain_[0-9]*_Iter*0_7.root" -type f)
#summit_files_iter8=$(find ${file_loc}/DataChains_Summit/raw_iter_8 -maxdepth 1 -name "MaCh3_MCMC_chain_[0-9]*_Iter_8.root" -type f)
#summit_asimov_files=$(find ${file_loc}/AsimovChains_Summit/raw -maxdepth 1 -name "MaCh3_MCMC_chain_[0-9]*.root" -type f)
#root_files=$(find ${file_loc}/batch_* -name "*_float_reduced.root" -type f)
#root_files=$(find ${file_loc}/Output_Iter_[1] -name "*_float_reduced.root" -type f)
#root_files=$(find ${file_loc}/JFBatch_* -name "*_float_reduced.root" -type f)
#root_files=$(find ${file_loc}/JF_CorrEs_batch_* -name "*_float_reduced.root" -type f)
root_files=$(find ${file_loc}/*/JFBatch_* -name "*_float_reduced.root" -type f)

echo "Root files:"
echo ${root_files} | wc -w
# Check each of them 
#echo ${beluga_files} | wc -w
#echo ${p100_files} | wc -w
#echo ${t4_files} | wc -w
#echo ${summit_files} | wc -w
#echo ${summit_files_iter8} | wc -w
#echo ${summit_asimov_files} | wc -w

# Overall save directory
# Some of the files are in directories that can not be written to
#save_dir=/home/mojia/scratch/jointfit/SKonly/09142024/StatOnlyHPD/RCreweight_reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonlyAsimov_PHAon/RCreweight_reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1/RCreweight_reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/10302024/coarse31x26PHAon_bat1/RCreweight_reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/09042024/SKOnly_PHAon_batch1/RCreweight_reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/11052024/coarse31x26_PHAon_bat2/RCreweight_reduced_files
#save_dir=/home/mojia/scratch/jointfit/JointFit/12132024/RCreweight_reduced_files
save_dir=/home/mojia/scratch/jointfit/JointFit_extra/RCreweight_reduced_files
mkdir -pv $save_dir

files=${root_files}
#files="${beluga_files} ${p100_files} ${t4_files} ${summit_files} ${summit_files_iter8} ${summit_asimov_files}"
#files="${summit_asimov_files}"
#files="${summit_files} ${summit_asimov_files}"

# Five minutes wall time
NTHREADS=1
WALLTIME_CC=02:00:00
RAMMB=4000
# Submission script
subopt="--account=rpp-blairt2k --time=${WALLTIME_CC} --cpus-per-task=${NTHREADS} --mem=${RAMMB}M"
submit_sys="sbatch"

nfiles=0
for file in $files; do
  echo "Running on: $file"

  # make the directory that this file belongs to
  # The output file
  output=${save_dir}/${file##${file_loc}/}
  output=${output%%.root}_flat_rc_reweighted.root

  # The output directory (needs making)
  save_dir_long=${output%\/*}
  mkdir -p ${save_dir_long}

  basefilename=$(basename $file)

  # Also different way of specifiying output
  stdout="--output ${save_dir_long}/${basefilename}.out"
  stderr="--error ${save_dir_long}/${basefilename}.err"
  ScriptFileName="./reweight_wrapper.sh $file $output"

  # Check if output file exists
  if [[ -f $output ]]; then
    echo "Found file $output already. Skipping..."
    continue
  fi
  nfiles=$(($nfiles+1))

  echo "Writing to: $output"

  submit="${submit_sys} ${subopt} ${stdout} ${stderr} ${ScriptFileName}"
  eval $submit
  
  #if [[ $nfiles -gt 0 ]]; then
    #break
  #fi
done

echo $nfiles files
