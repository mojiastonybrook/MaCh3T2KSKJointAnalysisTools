#!/bin/bash

#module load StdEnv/2020
#module load gcc/9.3.0
#module load root/6.26.06
#
# Doesnt' work
#module load nixpkgs/16.09
#module load gcc/7.3.0
#module load root/6.08.02

module load nixpkgs/16.09
module load gcc/5.4.0
module load gsl/1.16
#export PATH=/home/cwret/procmail-3.22/new:${PATH}
module load root/5.34.36

source /home/mojia/T2KSKJointFit_software/CMT/setup.sh
module load cuda/8.0.44
export CUDAPATH=${CUDA_HOME}
g++ -o reduceRawDataSet.exe reduceRawDataSet.cpp $(root-config --cflags --libs)
