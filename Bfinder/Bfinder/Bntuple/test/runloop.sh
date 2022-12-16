#!/bin/bash

set -x
cd ../
g++ loop.C $(root-config --libs --cflags) -g -o loop.exe && { mv loop.exe test/ ; }
cd test/
[[ -f loop.exe ]] && ./loop.exe HiForestAOD_20.root ntuple.root 0 1 -1 
rm loop.exe
set +x