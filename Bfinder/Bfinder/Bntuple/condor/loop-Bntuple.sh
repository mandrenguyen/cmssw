#!/bin/bash

if [[ $# -ne 6 ]]; then
    echo "usage: ./loop-Bntuple.sh [input file] [output dir] [output filename] [is real] [if skim] [proxy]"
    exit 1
fi

INFILE=$1
DESTINATION=$2
OUTFILE=$3
REAL=$4
SKIM=$5
export X509_USER_PROXY=${PWD}/$6

SRM_PREFIX="/mnt/hadoop/"
SRM_PATH=${DESTINATION#${SRM_PREFIX}}

set -x
./loop.exe $INFILE $OUTFILE $REAL $SKIM
set +x

if [[ $? -eq 0 ]]; then
    gfal-copy file://$PWD/${OUTFILE} gsiftp://se01.cmsaf.mit.edu:2811/${SRM_PATH}/${OUTFILE}
fi

rm $OUTFILE