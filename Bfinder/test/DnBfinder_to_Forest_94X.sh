#!/bin/bash

PATHTOTEST=$CMSSW_BASE/src/HeavyIonsAnalysis/JetAnalysis/test
FORESTS=(runForestAOD_pp_DATA_94X runForestAOD_pp_MC_94X)
RUNONMC=(False True)
# DIFFPATH=("process.hltanalysisReco *" "process.hltanalysis * process.runAnalyzer *")
INFILES=(
    # "file:/afs/cern.ch/work/w/wangj/public/DoubleMuon/Run2017G-17Nov2017-v1/DEBD2F77-0E38-E811-8161-A4BF0108B83A.root"
    "file:/afs/cern.ch/work/w/wangj/public/HIZeroBias2/Run2017G-17Nov2017-v1/141428A3-8F24-E811-B2A0-1CC1DE056008.root"
    "file:/afs/cern.ch/work/w/wangj/public/BJPsiMM_TuneCUETP8M1_5p02TeV_pythia8/RunIIpp5Spring18DR-94X_mc2017_realistic_forppRef5TeV-v2/C00FF4F0-D421-E911-9F69-141877410E71.root"
)

cc=0
for FOREST in ${FORESTS[@]}
do
##
    cp ${PATHTOTEST}/${FOREST}.py ${PATHTOTEST}/${FOREST}_wDfinder.py

    echo '
#################### D/B finder ################# 
AddCaloMuon = False 
runOnMC = '${RUNONMC[cc]}' ## !!
HIFormat = False 
UseGenPlusSim = False 
VtxLabel = "offlinePrimaryVerticesWithBS" 
TrkLabel = "generalTracks" 
useL1Stage2 = True
HLTProName = "HLT"
from Bfinder.finderMaker.finderMaker_75X_cff import finderMaker_75X 
finderMaker_75X(process, AddCaloMuon, runOnMC, HIFormat, UseGenPlusSim, VtxLabel, TrkLabel, useL1Stage2, HLTProName)
process.Dfinder.MVAMapLabel = cms.InputTag(TrkLabel,"MVAValues")
process.Dfinder.makeDntuple = cms.bool(True)
process.Dfinder.tkPtCut = cms.double(0.5) # before fit
process.Dfinder.tkEtaCut = cms.double(2.5) # before fit
process.Dfinder.dPtCut = cms.vdouble(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0) # before fit
process.Dfinder.VtxChiProbCut = cms.vdouble(0.05, 0.05, 0.0, 0.0, 0.0, 0.0, 0.05, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.05, 0.05)
process.Dfinder.dCutSeparating_PtVal = cms.vdouble(5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5.)
process.Dfinder.tktkRes_svpvDistanceCut_lowptD = cms.vdouble(0., 0., 0., 0., 0., 0., 0., 0., 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 0., 0.)
process.Dfinder.tktkRes_svpvDistanceCut_highptD = cms.vdouble(0., 0., 0., 0., 0., 0., 0., 0., 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 0., 0.)
process.Dfinder.svpvDistanceCut_lowptD = cms.vdouble(0., 0., 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 0., 0., 0., 0., 0., 0., 2.5, 2.5)
process.Dfinder.svpvDistanceCut_highptD = cms.vdouble(0., 0., 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 0., 0., 0., 0., 0., 0., 2.5, 2.5)
process.Dfinder.Dchannel = cms.vint32(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

process.dfinder = cms.Path(process.DfinderSequence)

' >> ${PATHTOTEST}/${FOREST}_wDfinder.py

##
    cp ${PATHTOTEST}/${FOREST}.py ${PATHTOTEST}/${FOREST}_wBfinder.py

    echo '
#################### D/B finder #################
AddCaloMuon = False
runOnMC = '${RUNONMC[cc]}' ## !!
HIFormat = False
UseGenPlusSim = False
VtxLabel = "offlinePrimaryVerticesWithBS"
TrkLabel = "generalTracks"
useL1Stage2 = True
HLTProName = "HLT"
from Bfinder.finderMaker.finderMaker_75X_cff import finderMaker_75X
finderMaker_75X(process, AddCaloMuon, runOnMC, HIFormat, UseGenPlusSim, VtxLabel, TrkLabel, useL1Stage2, HLTProName)

process.Bfinder.MVAMapLabel = cms.InputTag(TrkLabel,"MVAValues")
process.Bfinder.makeBntuple = cms.bool(True)
process.Bfinder.tkPtCut = cms.double(0.2) # before fit
process.Bfinder.jpsiPtCut = cms.double(0.0) # before fit
process.Bfinder.bPtCut = cms.vdouble(2.0, 5.0, 5.0, 2.0, 2.0, 2.0, 5.0) # before fit
process.Bfinder.Bchannel = cms.vint32(1, 0, 0, 1, 1, 1, 0)
process.Bfinder.VtxChiProbCut = cms.vdouble(0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.10)
process.Bfinder.svpvDistanceCut = cms.vdouble(2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0.0)
process.Bfinder.doTkPreCut = cms.bool(True)
process.Bfinder.doMuPreCut = cms.bool(True)
process.Bfinder.MuonTriggerMatchingPath = cms.vstring(
    "HLT_HIL1DoubleMu0_v1")
process.Bfinder.MuonTriggerMatchingFilter = cms.vstring(
    "hltL1fL1sDoubleMu0L1Filtered0")
process.p = cms.Path(process.BfinderSequence)

' >> ${PATHTOTEST}/${FOREST}_wBfinder.py

#
    cp ${PATHTOTEST}/${FOREST}_wDfinder.py ${PATHTOTEST}/${FOREST}_onlyDfinder.py
    cp ${PATHTOTEST}/${FOREST}_wBfinder.py ${PATHTOTEST}/${FOREST}_onlyBfinder.py

    for ifile in ${PATHTOTEST}/${FOREST}_onlyBfinder.py ${PATHTOTEST}/${FOREST}_onlyDfinder.py
    do
        sed -i "/D\/B finder/i \\
process.ana_step = cms.Path( \\
    process.hltanalysis * \\
    process.hiEvtAnalyzer * \\
    # process.hltobject + \\
    process.HiForest \\
    ) \\
" $ifile
    done

#
    for ifile in ${PATHTOTEST}/${FOREST}_onlyBfinder.py ${PATHTOTEST}/${FOREST}_onlyDfinder.py ${PATHTOTEST}/${FOREST}_wBfinder.py ${PATHTOTEST}/${FOREST}_wDfinder.py
    do
        echo '
###############################
import FWCore.ParameterSet.VarParsing as VarParsing
ivars = VarParsing.VarParsing('"'"'analysis'"'"')

ivars.maxEvents = -1
ivars.outputFile='"'"'HiForestAOD.root'"'"'
ivars.inputFiles='"'${INFILES[cc]}'"'
ivars.parseArguments() # get and parse the command line arguments

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(ivars.inputFiles)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(ivars.maxEvents)
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string(ivars.outputFile))
' >> $ifile
    done
##
    cc=$((cc+1))
done
