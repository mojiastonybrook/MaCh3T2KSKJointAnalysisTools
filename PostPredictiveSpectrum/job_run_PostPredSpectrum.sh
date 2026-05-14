#!/bin/bash

#SBATCH --time=24:00:00
#SBATCH --mail-user=mo.jia@stonybrook.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name=PPS_SKOnly
#SBATCH --output=PPS_SKOnly_n3000.log
#SBATCH --error=PPS_SKOnly_n3000.err
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=20G
#SBATCH --cpus-per-task=4
#SBATCH --gres=gpu:v100l:1
#SBATCH --account=rpp-blairt2k

#source /home/mojia/setup.sh
cd /home/mojia/MaCh3_2023/MaCh3 
source setup.sh
#export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK                                                                                                                   
export OMP_NUM_THREADS=8
./AtmJointFit_Bin/AtmPredictive_JF2023_SKOnly /home/mojia/projects/rpp-blairt2k/mojia/PostPredictiveSpectrum/AtmConfig_SKonly_PostPredSpec.cfg 222222 N3000 80000 /home/mojia/scratch/jointfit/SKonly/11292023   



