#!/bin/bash

module load nixpkgs/16.09
module load gcc/5.4.0
module load gsl/1.16

module load root/5.34.36

source /home/mojia/T2KSKJointFit_software/CMT/setup.sh
module load cuda/8.0.44
export CUDAPATH=${CUDA_HOME}
