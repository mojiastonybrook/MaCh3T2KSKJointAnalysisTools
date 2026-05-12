#!/bin/bash
#SBATCH --mail-user=mo.jia@stonybrook.edu
#SBATCH --mail-type=END
#SBATCH --job-name=T2KOnly_smear
#SBATCH --output=T2KOnly_smear.log
#SBATCH --error=T2KOnly_smear.err
#SBATCH --account=rpp-blairt2k
#SBATCH --time=2:30:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=5G

module load root/5.34.36

work_dir=/home/mojia/scratch/smear
cd $work_dir

files=/home/mojia/projects/rpp-blairt2k/mojia/mach3_chains/t2konly/T2KOnly_MegaMerge_reduced_reweighted.root
#files=/project/6008045/skt2kjoint/OA2020chains/MaCh3-OA2020_ALL_data_RCweights_reduced_burnInCut_smearedRevision_withPionSI.root
#/project/6008045/skt2kjoint/reweighted_full/AsimovB_reduced/asimovb_batch3/Output_Iter_3/MaCh3_Job_4_Iter_3_Exec_1_asimovb_float_reduced.root
param="dm23"
#err=3.6e-5
err=1.35e-5

root -l -b -q 'smear_parameter.C("'${files}'","'${param}'", 0.0,'${err}')'


