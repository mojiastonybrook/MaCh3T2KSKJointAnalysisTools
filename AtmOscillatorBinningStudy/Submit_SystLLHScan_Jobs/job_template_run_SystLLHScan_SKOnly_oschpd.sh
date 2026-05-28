#!/bin/bash

#SBATCH --time=4:00:00
#SBATCH --mail-user=mo.jia@stonybrook.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name=SystLLHScan_SKOnly_HPD_osc
#SBATCH --output=OUTPUTLOG/SystLLHScan_SKOnly_HPD_osc_PHAoff_coarseCOARBIN_fineFINEBIN.log
#SBATCH --error=OUTPUTLOG/SystLLHScan_SKOnly_HPD_osc_PHAoff_coarseCOARBIN_fineFINEBIN.err
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=5G
#SBATCH --cpus-per-task=4
#SBATCH --gres=gpu:v100l:1
#SBATCH --account=rpp-blairt2k

source /home/mojia/setup.sh
cd /home/mojia/MaCh3_2023/MaCh3 
source setup.sh
                                                                                                                  
export OMP_NUM_THREADS=4
./AtmJointFit_Bin/SystLLHScan_HPD ATMCONFIG 



