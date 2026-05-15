#!/bin/bash

src=PerformPostFitAnalysis_jf2023
#src=PerformPostFitAnalysis_AtmSKDet_Pi0
#suffix=drawPosteriors
suffix=xsec_osc_cov
#suffix=2DPost

g++ $(root-config --cflags) -g -o ${src}_${suffix} ${src}.cpp -I$(root-config --incdir) $(root-config --glibs --libs)

echo "${src}_${suffix} compiled!"
