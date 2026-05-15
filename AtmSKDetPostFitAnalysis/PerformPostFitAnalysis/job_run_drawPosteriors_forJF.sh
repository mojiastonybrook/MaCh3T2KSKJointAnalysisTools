#!/bin/bash
#SBATCH --mail-user=mo.jia@stonybrook.edu
#SBATCH --mail-type=END,FAIL
#SBATCH --job-name=PerformPostFitAnalysis_shiNSmeJF
#SBATCH --account=def-blairt2k
#SBATCH --time=24:00:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=38G

work_dir=/home/mojia/scratch/analysis_tools/PostFitAnalysis
output_file=${work_dir}/JFDetShiftNSmear_MaCh3_Merge_PerformPostFitAnalysis_Posteriors.root
burn_in=80000
#file_loc=/home/mojia/scratch/jointfit/SKonly/11292023/batch_0

#files=$(find ${file_loc} -name "MaCh3_batch_*_merged_raw.root" -type f)
#files=/home/mojia/scratch/jointfit/SKT2KJointFit_Data/JF2023_Data_Summit_MegaMerge_float_osc_nui_reweighted.root
#files=/home/mojia/scratch/jointfit/JointFit_detShiSme/beluga/JF_ShiNSme_Merge_float_reduced_reweighted_wNui.root
#files=/project/6008045/skt2kjoint/Sensitivity_AsimovA/JF2023_Data_OscProbCalModified_CorrEs_batch1Iter0_Merge_float_osc_nui_reweighted.root
files=/home/mojia/scratch/jointfit/JointFit_detShiSme/processed/JF_ShiNSme_Merge_float_reduced_reweighted_wNui.root

cd $work_dir

./PerformPostFitAnalysis_jf2023_drawPosteriors ${output_file} ${burn_in} ${files} &> ${work_dir}/run_postfitanalysis_forShiftNSmearJF.log

