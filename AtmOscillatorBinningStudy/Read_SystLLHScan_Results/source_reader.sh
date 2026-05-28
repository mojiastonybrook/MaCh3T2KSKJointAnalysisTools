#!/bin/bash

coarse_bin=$1
fine_bin=$2

save_dir=/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit
file=${save_dir}/coarse${coarse_bin}_fine${fine_bin}/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse${coarse_bin}_fine${fine_bin}_SystLLHScan.root

echo $file

if [ ! -f $file ]; then
  echo "NO SUCH FILE"
fi

root -l -b 'read_SystLLHScan_file.C("'${file}'")'
