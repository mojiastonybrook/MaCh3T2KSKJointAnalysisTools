#!/bin/bash

#SBATCH --time=0:10:00
#SBATCH --mail-user=mo.jia@stonybrook.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name=PrePostComp
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=5G                     #88
#SBATCH --cpus-per-task=1
#SBATCH --account=rpp-blairt2k

source /home/mojia/setup.sh

workdir=${PWD}

Post_dir=/home/mojia/scratch/analysis_tools/PostFitAnalysis
Post_JF=JFDetShiftNSmear_MaCh3_Merge_PerformPostFitAnalysis_Posteriors.root
#Post_JF=JFDetShiftNSmear_Beluga_MaCh3_PerformPostFitAnalysis_Posteriors.root
#Post_T2K=T2KOnly_MaCh3_PerformPostFitAnalysis_Posteriors.root
#Post_SK=SKOnly_MaCh3_PerformPostFitAnalysis_Posteriors.root
fitType=0
save_dir=/home/mojia/scratch/analysis_tools/PrePostFitComp

cd $workdir

./exe/PrePostFitComp_detSysts ${Post_dir}/${Post_JF} ${fitType} ${save_dir} &> ${save_dir}/run_detSysts_exe.log

