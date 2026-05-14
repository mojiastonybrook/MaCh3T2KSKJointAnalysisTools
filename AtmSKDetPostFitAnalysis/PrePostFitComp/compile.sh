#!/bin/bash

src=PrePostFitComp_detSysts

#suffix=xsec_osc_cov


g++ $(root-config --cflags) -std=c++11 -g -o ${src} ${src}.cpp -I$(root-config --incdir) $(root-config --glibs --libs)

echo "${src} compiled!"
