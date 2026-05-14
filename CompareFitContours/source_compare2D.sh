#!/bin/bash

#ROOT6 should be used for contour comparisons
#module load gcc/7.3.0
module unload root
module load root/6.14.04

cd /home/mojia/projects/rpp-blairt2k/mojia/BANFFFitContours/comparison_JF2023 
############################
#  1D/2D plots comparison  #
############################
#CompareContours functions can take in 2 or 3 output files produced from MakeContours1D/2D, mostly used one is the one with 7 arguments (compare 2 files)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#CompareContours function with 7 arguments
#argv 1: dimension (1: 1D; 2: 2D)
#     2: interval representation (0: credible interval,default, no need to change; 1: confidence interval)
#     3: input file 1 name
#     4: input file 1 legend name
#     5: input file 2 name
#     6: input file 2 legend name
#     7: additional string for output files naming
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#CompareContours function with 9 arguments
#argv 1: dimension (1: 1D; 2: 2D)
#     2: interval representation (0: credible interval,default, no need to change; 1: confidence interval)
#     3: input file 1 name
#     4: input file 1 legend name
#     5: input file 2 name
#     6: input file 2 legend name
#     7: input file 3 name
#     8: input file 3 legend name
#     9: additional string for output files naming

#2D comparison
#ONEDFILENAME1=\"/home/mojia/projects/rpp-blairt2k/mojia/BANFFFitContours/comparison_JF2023/Contours_2D_wRC_BurnIn_80000_JF2023_float.root\"
#ONEDLEG1=\"JF2023_DataFit_wRC\"
#ONEDFILENAME2=\"/home/mojia/projects/rpp-blairt2k/mojia/BANFFFitContours/Contours_2D_wRC_BurnIn_80000_banffFit_float.root\"
#ONEDLEG2=\"postBANFF_DataFit_wRC\"
#ONEDADDLABEL=\"JFpostBANFF_DataFit_wRC\"
ONEDFILENAME1=\"/home/mojia/projects/rpp-blairt2k/mojia/SoloExpFitsContours/ContourPlottingScripts/Contours_2D_wRC_BurnIn_80000_skonly_float.root\"
ONEDLEG1=\"SKOnly_wRC\"
ONEDFILENAME2=\"/home/mojia/projects/rpp-blairt2k/mojia/SoloExpFitsContours/ContourPlottingScripts/Contours_2D_wRC_BurnIn_80000_skonly_postBANFFFit_float.root\"
ONEDLEG2=\"SKOnly_postBANFFFit_wRC\"
ONEDADDLABEL=\"SKOnly_postBANFFFit_wRC\"

root -b -q "CompareContours.C(2,0,$ONEDFILENAME1,$ONEDLEG1,$ONEDFILENAME2,$ONEDLEG2,$ONEDADDLABEL)" &
wait


