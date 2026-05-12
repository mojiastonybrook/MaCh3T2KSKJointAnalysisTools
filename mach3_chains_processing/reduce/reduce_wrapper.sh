#!/bin/bash

if [[ $# -ne 2 ]]; then
  echo "$0 needs two arguments: input file and output file"
  echo "You gave $#"
  exit
fi

module load nixpkgs/16.09
module load gcc/5.4.0
module load gsl/1.16
#export PATH=/home/cwret/procmail-3.22/new:${PATH}
module load root/5.34.36

source /home/mojia/T2KSKJointFit_software/CMT/setup.sh
module load cuda/8.0.44
export CUDAPATH=${CUDA_HOME}

echo "Running on $1, writing to $2..."

./reduceDataSet.exe $1 $2

echo "Done!"
