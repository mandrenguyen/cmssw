#!/bin/bash

if [[ $# -ne 6 ]]; then
    echo "usage: ./loop-condor-checkfile.sh [input dir] [output dir] [is real] [if skim] [max jobs] [log dir]"
    exit 1
fi

DATASETs=$1
DESTINATION=$2
REAL=$3
SKIM=$4
MAXFILES=$5
LOGDIR=$6

PROXYFILE=$(ls /tmp/ -lt | grep $USER | grep -m 1 x509 | awk '{print $NF}')

NAME="ntuple"

SRM_PREFIX="/mnt/hadoop/"
SRM_PATH=${DESTINATION#${SRM_PREFIX}}

[[ ! -d $DESTINATION ]] && {
    gfal-mkdir -p gsiftp://se01.cmsaf.mit.edu:2811/${SRM_PATH} ;
}

mkdir -p $LOGDIR

counter=0
for DATASET in `echo $DATASETs`
do

    [[ -f filelist.txt ]] && rm filelist.txt
    ls $DATASET | grep -v "/" | grep -v -e '^[[:space:]]*$' | awk '{print "" $0}' >> filelist.txt

    for i in `cat filelist.txt`
    do
        [[ $counter -ge $MAXFILES ]] && break
        if [ ! -f ${DESTINATION}/${NAME}_$i ] && [ -f ${DATASET}/$i ]
        then
            if [ -s ${DATASET}/$i ] || [ $ifCHECKEMPTY -eq 0 ]
            then
                echo -e "\033[38;5;242mSubmitting a job for output\033[0m ${DESTINATION}/${NAME}_$i"
                infn=`echo $i | awk -F "." '{print $1}'`
                INFILE="${DATASET}/$i"
	        cat > ${NAME}.condor <<EOF

Universe     = vanilla
Initialdir   = $PWD/
Notification = Error
Executable   = $PWD/loop-Bntuple.sh
Arguments    = $INFILE $DESTINATION ${NAME}_${infn}.root $REAL $SKIM $PROXYFILE
GetEnv       = True
Output       = $LOGDIR/log-${infn}.out
Error        = $LOGDIR/log-${infn}.err
Log          = $LOGDIR/log-${infn}.log
Rank         = Mips
+AccountingGroup = "group_cmshi.$(whoami)"
requirements = GLIDEIN_Site == "MIT_CampusFactory" && BOSCOGroup == "bosco_cmshi" && HAS_CVMFS_cms_cern_ch && BOSCOCluster == "ce03.cmsaf.mit.edu"
job_lease_duration = 240
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
transfer_input_files = loop.exe,/tmp/$PROXYFILE

Queue 
EOF

condor_submit ${NAME}.condor -name submit.mit.edu
mv ${NAME}.condor $LOGDIR/log-${infn}.condor
counter=$(($counter+1))	
            fi
        fi
    done
done
echo -e "Submitted \033[1;36m$counter\033[0m jobs to Condor."
