#!/bin/bash

# Last iteration of Summit, the beluga files, the graham p100 files, the graham t4 files
#files="/vols/t2k/users/cvw09/data/skt2kjoint/Summit/DataChains_Summit/raw_iter_8/reduced/MaCh3_MCMC_chain_Iter_8_reduced_merge.root /vols/t2k/users/cvw09/data/skt2kjoint/ComputeCanada/chains_beluga/reduced/JF2023_Beluga_Data_reduced_merged.root /vols/t2k/users/cvw09/data/skt2kjoint/ComputeCanada/chains_graham/JF2023_datafit/gpu_p100/reduced/MaCh3_Job_graham_p100_reduced_merge.root /vols/t2k/users/cvw09/data/skt2kjoint/ComputeCanada/chains_graham/JF2023_datafit/gpu_t4/reduced/MaCh3_Job_graham_t4_reduced_merge.root"
#file_loc=/project/6008045/skt2kjoint/reweighted_full/output_flat
#file_loc=/project/6008045/skt2kjoint/AsimovBChains_Graham/08022023
#file_loc=/home/mojia/projects/rpp-blairt2k/mojia/mach3_chains/skonly
#file_loc=/home/mojia/scratch/jointfit/SKonly/05012024
#file_loc=/home/mojia/scratch/jointfit/BANFFFit/06102024
#file_loc=/home/mojia/scratch/jointfit/SKonly/08202024
#file_loc=/home/mojia/scratch/jointfit/SKonly/09142024/StatOnlyHPD
#file_loc=/home/mojia/scratch/jointfit/SKonlyAsimov_PHAon
#file_loc=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1
#file_loc=/home/mojia/scratch/jointfit/SKonly/09042024/SKOnly_PHAon_batch1
#file_loc=/home/mojia/scratch/jointfit/SKonly/10302024/coarse31x26PHAon_bat1
#file_loc=/home/mojia/scratch/jointfit/SKonly/11052024/coarse31x26_PHAon_bat2
#file_loc=/home/mojia/scratch/jointfit/JointFit/12132024 
#file_loc=/home/mojia/scratch/jointfit/JointFit/12162024
file_loc=/home/mojia/scratch/jointfit/JointFit_extra 
#beluga_files=$(find ${file_loc}/chains_beluga -maxdepth 1 -name "*subbatch*.root" -type f)
#p100_files=$(find ${file_loc}/chains_graham/JF2023_datafit/gpu_p100/merged_stage1 -maxdepth 1 -name "*Exec_[0-9]*.root" -type f)
#t4_files=$(find ${file_loc}/chains_graham/JF2023_datafit/gpu_t4/merged_stage1 -maxdepth 1 -name "*Exec_[0-9]*.root" -type f)
#summit_files=$(find ${file_loc}/DataChains_Summit/raw -maxdepth 1 -name "MaCh3_MCMC_chain_[0-9]*_Iter*0_7_flat*.root" -type f)
#summit_files_iter8=$(find ${file_loc}/DataChains_Summit/raw_iter_8 -maxdepth 1 -name "MaCh3_MCMC_chain_[0-9]*_Iter_8_flat*.root" -type f)
#summit_asimov_files=$(find ${file_loc}/AsimovChains_Summit/raw -maxdepth 1 -name "MaCh3_MCMC_chain_[0-9]*.root" -type f)

#root_files=$(find ${file_loc}/batch_* -name "MaCh3_Job_*.root" -type f)
#root_files=$(find ${file_loc}/Output_Iter_[1] -name "MaCh3_Job_*.root" -type f)
#root_files=$(find ${file_loc}/JFBatch_* -name "MaCh3_Job_*.root" -type f)
#root_files=$(find ${file_loc}/JF_CorrEs_batch_* -name "MaCh3_Job_*.root" -type f)
root_files=$(find ${file_loc}/*/JFBatch_* -name "MaCh3_Job_*.root" -type f)
echo "root files:"
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
#save_dir=/home/mojia/projects/rpp-blairt2k/mojia/mach3_chains/skonly/reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/09142024/StatOnlyHPD/reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonlyAsimov_PHAon/reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/10102024/coarse31x26_PHAoff_batch1/reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/09042024/SKOnly_PHAon_batch1/reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/10302024/coarse31x26PHAon_bat1/reduced_files
#save_dir=/home/mojia/scratch/jointfit/SKonly/11052024/coarse31x26_PHAon_bat2/reduced_files
#save_dir=/home/mojia/scratch/jointfit/JointFit/12132024/reduced_files
save_dir=/home/mojia/scratch/jointfit/JointFit_extra/reduced_files
mkdir -pv $save_dir

files=${root_files}
#files="${beluga_files} ${p100_files} ${t4_files} ${summit_files} ${summit_files_iter8} ${summit_asimov_files}"
#files="${summit_asimov_files}"
#files="${summit_files} ${summit_asimov_files}"

# Five minutes wall time
NTHREADS=1
WALLTIME_CC=00:59:00
RAMMB=2000
# Submission script
subopt="--account=rpp-blairt2k --time=${WALLTIME_CC} --cpus-per-task=${NTHREADS} --mem=${RAMMB}M"
submit_sys="sbatch"

nfiles=0
for file in $files; do
  echo $file

  # make the directory that this file belongs to
  # The output file
  output=${save_dir}/${file##${file_loc}/}
  output=${output%%.root}_float_reduced.root

  # The output directory (needs making)
  save_dir_long=${output%\/*}
  mkdir -p ${save_dir_long}

  basefilename=$(basename $file)

  # Also different way of specifiying output
  stdout="--output ${save_dir_long}/${basefilename}.out"
  stderr="--error ${save_dir_long}/${basefilename}.err"
  ScriptFileName="./reduce_wrapper.sh $file $output"

  # Check if output file exists
  if [[ -f $output ]]; then
    echo "Found file $output already. Skipping..."
    continue
  fi
  nfiles=$(($nfiles+1))

  submit="${submit_sys} ${subopt} ${stdout} ${stderr} ${ScriptFileName}"
  eval $submit
  
  #if [[ $nfiles -gt 0 ]]; then
    #break
  #fi
done

echo $nfiles files
