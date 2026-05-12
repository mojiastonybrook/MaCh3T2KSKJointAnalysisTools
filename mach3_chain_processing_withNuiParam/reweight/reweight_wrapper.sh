#!/bin/bash

if [[ $# -ne 2 ]]; then
  echo "$0 needs two arguments: input file and output file"
  echo "You gave $#"
  exit
fi

# load up ROOT
#module load StdEnv/2020
#module load gcc/9.3.0
#module load root/6.26.06

# Check that directory exists
#outputdir=${2%\/*}
#mkdir -p ${outputdir}
#echo "Made output directory ${outputdir}"

module load nixpkgs/16.09
module load gcc/5.4.0
module load gsl/1.16
#export PATH=/home/cwret/procmail-3.22/new:${PATH}
#source /home/cwret/CMT/setup.sh
module load root/5.34.36

#module load cuda/8.0.44
#export CUDAPATH=${CUDA_HOME}

echo "Running on $1, writing to $2..."

#root -b -q -l 'ReweightPrior_fast.C("'$1'", 0, "'$2'")'
./ReweightPrior_fast.exe $1 0 $2

echo "Done!"
