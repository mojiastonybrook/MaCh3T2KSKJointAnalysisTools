#!/bin/bash
g++ -O3 -o ReweightPrior_fast.exe ReweightPrior_fast.cpp $(root-config --cflags --libs)
