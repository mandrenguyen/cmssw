#!/bin/bash

[[ $0 != *.sh ]] && {
    echo -e "\e[31;1merror:\e[0m use \e[32;1m./script.sh\e[0m instead of \e[32;1msource script.sh\e[0m" ; return 1; }

# ===> Please check the info below
REAL=0
SKIM=0
MAXFILENO=10000000
OUTPUTSUBDIR=
# <=== 

## data
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326381_326577/190525_192744/000*/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326381_326577_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326580_326855/190525_194019/000*/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326580_326855_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326856_327078/190525_195812/000*/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326856_327078_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327327/190514_054351/000*/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327327_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327400_327564_v2/190515_195903/000*/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327400_327564_v2_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_Missinglumi/190806_210915/0000/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_Missinglumi_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_Missinglumi/190806_211936/0000/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_Missinglumi_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_Missinglumi/190807_015346/0000/' ; OUTPUTSUBDIR='ntmix_20190806_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_Missinglumi_skimBpt10'

## same sign
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_samesign_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327327/190530_175552/000*/' ; OUTPUTSUBDIR='ntmix_20190808_Bfinder_samesign_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327327_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_samesign_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327400_327564/190531_190120/000*/' ; OUTPUTSUBDIR='ntmix_20190808_Bfinder_samesign_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327400_327564_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_samesign_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_Missinglumi/190808_174319/0000/' ; OUTPUTSUBDIR='ntmix_20190808_Bfinder_samesign_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_Missinglumi_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_samesign_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_326381_326855/190808_152952/000*/' ; OUTPUTSUBDIR='ntmix_20190808_Bfinder_samesign_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_326381_326855_skimBpt10'
# INPUTDIR='/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_samesign_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_326856_327078/190808_174005/000*/' ; OUTPUTSUBDIR='ntmix_20190808_Bfinder_samesign_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_HF_and_MuonJSON_326856_327078_skimBpt10'

INPUTS=(
    '/mnt/hadoop/cms/store/user/wangj/NonPromptPsi2S_pThat-10_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-10_1033p1_official_pt6tkpt0p9dls0/190712_201714/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-10_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptPsi2S_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-15_1033p1_official_pt6tkpt0p9dls0/190712_201749/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-15_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptPsi2S_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-30_1033p1_official_pt6tkpt0p9dls0/190712_201821/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-30_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptPsi2S_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-50_1033p1_official_pt6tkpt0p9dls0/190712_201855/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-50_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptPsi2S_pThat-5_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-5_1033p1_official_pt6tkpt0p9dls0/190712_201641/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptPsi2S_Pthat-5_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptXRho_pThat-10_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-10_1033p1_official_pt6tkpt0p9dls0/190712_193407/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-10_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptXRho_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-15_1033p1_official_pt6tkpt0p9dls0/190712_193929/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-15_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptXRho_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-30_1033p1_official_pt6tkpt0p9dls0/190712_194235/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-30_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptXRho_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-50_1033p1_official_pt6tkpt0p9dls0/190712_194757/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-50_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/NonPromptXRho_pThat-5_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-5_1033p1_official_pt6tkpt0p9dls0/190712_193051/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_NonPromptXRho_Pthat-5_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptPsi2S_pThat-10_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-10_1033p1_official_pt6tkpt0p9dls0/190712_201317/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-10_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptPsi2S_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-15_1033p1_official_pt6tkpt0p9dls0/190712_201352/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-15_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptPsi2S_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-30_1033p1_official_pt6tkpt0p9dls0/190712_201424/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-30_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptPsi2S_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-50_1033p1_official_pt6tkpt0p9dls0/190712_201458/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-50_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptPsi2S_pThat-5_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-5_1033p1_official_pt6tkpt0p9dls0/190712_201243/0000/ ntmix_20190808_Bfinder_20190712_Hydjet_Pythia8_PromptPsi2S_Pthat-5_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXPiPi_pThat-10_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-10_1033p1_official_pt6tkpt0p9dls0/190808_172922/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-10_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXPiPi_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-15_1033p1_official_pt6tkpt0p9dls0/190808_173014/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-15_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXPiPi_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-30_1033p1_official_pt6tkpt0p9dls0/190808_173058/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-30_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXPiPi_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-50_1033p1_official_pt6tkpt0p9dls0/190808_173156/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-50_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXPiPi_pThat-5_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-5_1033p1_official_pt6tkpt0p9dls0/190808_172845/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXPiPi_Pthat-5_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXRho_pThat-10_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-10_1033p1_official_pt6tkpt0p9dls0/190731_050249/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-10_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXRho_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-15_1033p1_official_pt6tkpt0p9dls0/190731_050314/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-15_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXRho_pThat-30_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-30_1033p1_official_pt6tkpt0p9dls0/190731_050340/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-30_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXRho_pThat-50_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-50_1033p1_official_pt6tkpt0p9dls0/190731_050021/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-50_1033p1_official_pt6tkpt0p9dls0'
    '/mnt/hadoop/cms/store/user/wangj/PromptXRho_pThat-5_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/crab_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-5_1033p1_official_pt6tkpt0p9dls0/190731_045902/0000/ ntmix_20190808_Bfinder_20190730_Hydjet_Pythia8_PromptXRho_Pthat-5_1033p1_official_pt6tkpt0p9dls0'
)

##############################################################
# Do not modify lines below if you do not know what it means #
##############################################################

##
movetosubmit=${1:-0}
runjobs=${2:-0}

[[ $movetosubmit -eq 0 && $runjobs -eq 0 ]] && {
    echo "Usage: ./submit_condor_checkfile.sh [move script] [submit jobs]" ;
    echo "Test compiling macros..." ; 
    cd .. ;
    g++ loop.C $(root-config --libs --cflags) -Werror -Wall -O2 -g -o loop.exe && rm loop.exe ;
    # g++ loop.C $(root-config --libs --cflags) -g -o loop.exe && rm loop.exe ;
    cd condor ;
}

#
OUTPUTPRIDIR="/mnt/hadoop/cms/store/user/jwang/BntupleRun2018condor/"
WORKDIR="/work/$USER/Bntuple/"
mkdir -p $WORKDIR

#
if [[ $movetosubmit -eq 1 ]]
then
    if [[ $(hostname) == "submit-hi2.mit.edu" || $(hostname) == "submit.mit.edu" || $(hostname) == "submit-hi1.mit.edu" ]]
    then
        cd ..
        g++ loop.C $(root-config --libs --cflags) -Werror -Wall -O2 -g -o loop.exe
        cd condor

        mv ../loop.exe $WORKDIR/
        cp $0 $WORKDIR/
        cp loop-Bntuple.sh $WORKDIR/
        cp loop-condor-checkfile.sh $WORKDIR/
    else
        echo -e "\e[31;1merror:\e[0m compile macros on \e[32;1msubmit-hiX.mit.edu\e[0m or \e[32;1msubmit.mit.edu\e[0m."
    fi
fi

#
if [[ $runjobs -eq 1 ]]
then
    if [[ $(hostname) == "submit.mit.edu" ]]
    then
##
        if [[ x$OUTPUTSUBDIR == x ]]
        then
            for ii in "${INPUTS[@]}"
            do
                thisinput=($ii)
                INPUTDIR=${thisinput[0]}
                OUTPUTSUBDIR=${thisinput[1]}
                OUTPUTDIR="${OUTPUTPRIDIR}/${OUTPUTSUBDIR}"
                LOGDIR="logs/log_${OUTPUTSUBDIR}"
                echo "./loop-condor-checkfile.sh \"$INPUTDIR\" $OUTPUTDIR $REAL $SKIM $MAXFILENO $LOGDIR"
                ./loop-condor-checkfile.sh "$INPUTDIR" $OUTPUTDIR $REAL $SKIM $MAXFILENO $LOGDIR
            done
        else
            OUTPUTDIR="${OUTPUTPRIDIR}/${OUTPUTSUBDIR}"
            LOGDIR="logs/log_${OUTPUTSUBDIR}"
            echo "./loop-condor-checkfile.sh \"$INPUTDIR\" $OUTPUTDIR $REAL $SKIM $MAXFILENO $LOGDIR"
            ./loop-condor-checkfile.sh "$INPUTDIR" $OUTPUTDIR $REAL $SKIM $MAXFILENO $LOGDIR
        fi
    else
        echo -e "\e[31;1merror:\e[0m submit jobs on \e[32;1msubmit.mit.edu\e[0m."
    fi
fi


#################################################################################################################################################################################################################
##                                                                                      X3872ToJpsiRho_prompt                                                                                                  ##
## ls -d /mnt/hadoop/cms/store/user/wangj/Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat*_TuneCP5_5020GeV_Drum5F/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat*_1033p1_pt6tkpt0p7dls0_v3/*/0000 ##
#################################################################################################################################################################################################################
# INPUTDIR="/mnt/hadoop/cms/store/user/wangj/Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat5_TuneCP5_5020GeV_Drum5F/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat5_1033p1_pt6tkpt0p7dls0_v3/190529_035808/0000/" ; OUTPUTSUBDIR="ntuple_20190609_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat5_1033p1_pt6tkpt0p7dls0_v3" ;
# INPUTDIR="/mnt/hadoop/cms/store/user/wangj/Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat10_TuneCP5_5020GeV_Drum5F/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat10_1033p1_pt6tkpt0p7dls0_v3/190529_035851/0000/" ; OUTPUTSUBDIR="ntuple_20190609_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat10_1033p1_pt6tkpt0p7dls0_v3" ;
# INPUTDIR="/mnt/hadoop/cms/store/user/wangj/Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat15_TuneCP5_5020GeV_Drum5F/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat15_1033p1_pt6tkpt0p7dls0_v3/190529_040529/0000/" ; OUTPUTSUBDIR="ntuple_20190609_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat15_1033p1_pt6tkpt0p7dls0_v3" ;
# INPUTDIR="/mnt/hadoop/cms/store/user/wangj/Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat30_TuneCP5_5020GeV_Drum5F/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat30_1033p1_pt6tkpt0p7dls0_v3/190529_040949/0000/" ; OUTPUTSUBDIR="ntuple_20190609_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat30_1033p1_pt6tkpt0p7dls0_v3" ;
# INPUTDIR="/mnt/hadoop/cms/store/user/wangj/Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat50_TuneCP5_5020GeV_Drum5F/crab_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat50_1033p1_pt6tkpt0p7dls0_v3/190529_041105/0000/" ; OUTPUTSUBDIR="ntuple_20190609_Bfinder_20190520_Hydjet_Pythia8_X3872ToJpsiRho_prompt_Pthat50_1033p1_pt6tkpt0p7dls0_v3" ;
#################################################################################################################################################################################################################

#################################################################################################################################################################################################################
##                                                                                                   Data                                                                                                      ##
#################################################################################################################################################################################################################
# for cc in 0..6 ; do INPUTDIR="/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327327/190514_054351/000${cc}/" ; OUTPUTSUBDIR="ntmix_20190711_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327123_327327_skimBpt10" ; # 0-6
# for cc in 0..7 ; do INPUTDIR="/mnt/hadoop/cms/store/user/wangj/HIDoubleMuonPsiPeri/crab_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327400_327564_v2/190515_195903/000${cc}/" ; OUTPUTSUBDIR="ntmix_20190711_Bfinder_20190513_HIDoubleMuonPsiPeri_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_327400_327564_v2_skimBpt10" ; # 0-7
# for cc in 0..1 ; do INPUTDIR="/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326381_326577/190525_192744/000${cc}/" ; OUTPUTSUBDIR="ntmix_20190711_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326381_326577_skimBpt10" ; # 0-1
# for cc in 0..6 ; do INPUTDIR="/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326580_326855/190525_194019/000${cc}/" ; OUTPUTSUBDIR="ntmix_20190711_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326580_326855_skimBpt10" ; # 0-6
# for cc in 0..4 ; do INPUTDIR="/mnt/hadoop/cms/store/user/wangj/HIDoubleMuon/crab_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326856_327078/190525_195812/000${cc}/" ; OUTPUTSUBDIR="ntmix_20190711_Bfinder_20190513_HIDoubleMuon_HIRun2018A_04Apr2019_v1_1033p1_GoldenJSON_326856_327078_skimBpt10" ; # 0-4
#################################################################################################################################################################################################################
