# for the list of used tags please see:
# https://twiki.cern.ch/twiki/bin/view/CMS/Onia2MuMuSamples
from os import environ
RELEASE = [int(i) for i in environ['CMSSW_VERSION'].split("_")[1:4]]

import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

# set up process
process = cms.Process("Onia2MuMuPAT")

# Conditions
HLTProName = "HLT"
isPbPb = True;          
isMC = False;
useGeneralTracks = False;
useEventPlane = True;
muonSelection = "GlbTrk" # Single muon selection: Glb, GlbTrk, Trk are availale

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Names
options.outputFile = "OniaTree.root"
options.secondaryOutputFile = "Jpsi_DataSet.root"
options.inputFiles ='/store/group/phys_heavyions/velicanu/store/t0streamer/Data/Express/000/261/396/RECO/bubba_RAW2DIGI_L1Reco_RECO.root'
options.maxEvents = -1 # -1 means all events

# get and parse the command line arguments
options.parseArguments()
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContentHeavyIons_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
process.load("Configuration.StandardSequences.MagneticField_38T_cff")

# Global Tag:
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
if isMC:
  if isPbPb:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc_HIon', '')
  else:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
else:  
  process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")

#Centrality Tags for CMSSW 7_5_X:               
# Only use if the centrality info is not present or need to apply new calibration
# process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
# process.newCentralityBin = process.centralityBin.clone()

# Event Plane (works on CMSSW_7_5_4):
keepEventPlane = True if (RELEASE[0]==7 and RELEASE[1]==5 and RELEASE[2]>3) else False
if isPbPb:
  process.load("RecoHI.HiEvtPlaneAlgos.HiEvtPlane_cfi")
  if isMC:
    process.GlobalTag.toGet.extend([
        cms.PSet(record = cms.string("HeavyIonRPRcd"),
                 tag = cms.string("HeavyIonRPRcd_Hydjet_74x_v03_mc"),
                 connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
                 )
        ])


'''
# BSC or HF coincidence (masked unprescaled L1 bits)
process.load('L1Trigger.Skimmer.l1Filter_cfi')
process.bscOrHfCoinc = process.l1Filter.clone(
    algorithms = cms.vstring('*','L1_HcalHfCoincPmORBscMinBiasThresh1_BptxAND_instance1', 'L1_NotBsc2_BscMinBiasOR', 'L1_HcalHfCoincidencePm')
    )
'''
# HLT Dimuon Triggers
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltOniaHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# HLT PbPbP MENU:  /online/collisions/2015/HeavyIons/v1.0/HLT/V6
process.hltOniaHI.HLTPaths =  [
    "HLT_HIL1DoubleMu0_v1",
    "HLT_HIL1DoubleMu0_2HF_v1",
    "HLT_HIL1DoubleMu0_2HF0_v1",
    "HLT_HIL1DoubleMu10_v1",
    "HLT_HIL2DoubleMu0_NHitQ_v2",
    "HLT_HIL2DoubleMu0_NHitQ_2HF_v1",
    "HLT_HIL2DoubleMu0_NHitQ_2HF0_v1",
    "HLT_HIL1DoubleMu0_2HF_Cent30100_v1",
    "HLT_HIL1DoubleMu0_2HF0_Cent30100_v1",
    "HLT_HIL2DoubleMu0_2HF_Cent30100_NHitQ_v1",
    "HLT_HIL1DoubleMu0_Cent30_v1",
    "HLT_HIL2DoubleMu0_2HF0_Cent30100_NHitQ_v1",
    "HLT_HIL2DoubleMu0_Cent30_NHitQ_v1",
    "HLT_HIL2DoubleMu0_Cent30_OS_NHitQ_v1",
    "HLT_HIL3DoubleMu0_Cent30_v1",
    "HLT_HIL3DoubleMu0_Cent30_OS_m2p5to4p5_v1",
    "HLT_HIL3DoubleMu0_Cent30_OS_m7to14_v1",
    "HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
    "HLT_HIL3DoubleMu0_OS_m7to14_v1",
    "HLT_HIL2Mu3_NHitQ10_2HF_v1",
    "HLT_HIL2Mu3_NHitQ10_2HF0_v1",
    "HLT_HIL3Mu3_NHitQ15_2HF_v1",
    "HLT_HIL3Mu3_NHitQ15_2HF0_v1",
    "HLT_HIL2Mu5_NHitQ10_2HF_v1",
    "HLT_HIL2Mu5_NHitQ10_2HF0_v1",
    "HLT_HIL3Mu5_NHitQ15_2HF_v1",
    "HLT_HIL3Mu5_NHitQ15_2HF0_v1",
    "HLT_HIL2Mu7_NHitQ10_2HF_v1",
    "HLT_HIL2Mu7_NHitQ10_2HF0_v1",
    "HLT_HIL3Mu7_NHitQ15_2HF_v1",
    "HLT_HIL3Mu7_NHitQ15_2HF0_v1",
    "HLT_HIL2Mu15_v2",
    "HLT_HIL2Mu15_2HF_v1",
    "HLT_HIL2Mu15_2HF0_v1",
    "HLT_HIL3Mu15_v1",
    "HLT_HIL3Mu15_2HF_v1",
    "HLT_HIL3Mu15_2HF0_v1",
    "HLT_HIL2Mu20_v1",
    "HLT_HIL2Mu20_2HF_v1",	
    "HLT_HIL2Mu20_2HF0_v1",
    "HLT_HIL3Mu20_v1",
    "HLT_HIL3Mu20_2HF_v1",
    "HLT_HIL3Mu20_2HF0_v1"
    ]
process.hltOniaHI.throw = False
process.hltOniaHI.andOr = True
process.hltOniaHI.TriggerResultsTag = cms.InputTag("TriggerResults","",HLTProName)

from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import *
onia2MuMuPAT(process, GlobalTag=process.GlobalTag.globaltag, MC=isMC, HLT=HLTProName, Filter=True)

### Temporal fix for the PAT Trigger prescale warnings.
process.patTriggerFull = cms.EDProducer( "PATTriggerProducer",
                                         l1GtReadoutRecordInputTag = cms.InputTag("gtDigis","","RECO"),
                                         onlyStandAlone = cms.bool( True ),
                                         processName    = cms.string( HLTProName )                    
                                         )
###

##### Onia2MuMuPAT input collections/options
process.onia2MuMuPatGlbGlb.dimuonSelection          = cms.string("mass > 0")
process.onia2MuMuPatGlbGlb.resolvePileUpAmbiguity   = False
if isPbPb:
  process.onia2MuMuPatGlbGlb.primaryVertexTag         = cms.InputTag("hiSelectedVertex")
  process.patMuonsWithoutTrigger.pvSrc                = cms.InputTag("hiSelectedVertex")
  process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = False
  if isMC:
    process.genMuons.src = "genParticles"
    process.onia2MuMuPatGlbGlb.genParticles = "genParticles"
else: # ispp
  process.onia2MuMuPatGlbGlb.primaryVertexTag         = cms.InputTag("offlinePrimaryVertices")
  process.patMuonsWithoutTrigger.pvSrc                = cms.InputTag("offlinePrimaryVertices")
  # Adding muonLessPV gives you lifetime values wrt. muonLessPV only
  process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = True
  if isMC:
    process.genMuons.src = "genParticles"
    process.onia2MuMuPatGlbGlb.genParticles = "genParticles"

##### Remove few paths for MC
if isMC:
  process.patMuonSequence.remove(process.hltOniaHI)

##### Dimuon pair selection
commonP1 = "|| (innerTrack.isNonnull && genParticleRef(0).isNonnull)"
commonP2 = " && abs(innerTrack.dxy)<4 && abs(innerTrack.dz)<35"
if muonSelection == "Glb":
  highP = "isGlobalMuon"; # At least one muon must pass this selection
  process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("("+highP+commonP1+")"+commonP2)
  lowP = "isGlobalMuon"; # BOTH muons must pass this selection
  process.onia2MuMuPatGlbGlb.lowerPuritySelection = cms.string("("+lowP+commonP1+")"+commonP2)
elif muonSelection == "GlbTrk":
  highP = "isGlobalMuon";
  process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("("+highP+commonP1+")"+commonP2)
  lowP = "(isGlobalMuon && isTrackerMuon)";
  process.onia2MuMuPatGlbGlb.lowerPuritySelection = cms.string("("+lowP+commonP1+")"+commonP2)
elif muonSelection == "Trk":
  highP = "isTrackerMuon";
  process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("("+highP+commonP1+")"+commonP2)
  lowP = "isTrackerMuon";
  process.onia2MuMuPatGlbGlb.lowerPuritySelection = cms.string("("+lowP+commonP1+")"+commonP2)
else:
  print "ERROR: Incorrect muon selection " + muonSelection + " . Valid options are: Glb, Trk, GlbTrk"

##### If single track collection has to be kept
if useGeneralTracks:
  process.outOnia2MuMu.outputCommands.append("keep *_standAloneMuons_*_*")
  if isPbPb: process.outOnia2MuMu.outputCommands.append("keep *_hiGeneralTracks_*_*")
  else: process.outOnia2MuMu.outputCommands.append("keep *_generalTracks_*_*")

##### If event plane collection has to be kept
if useEventPlane:
  process.outOnia2MuMu.outputCommands.append("keep *_hiEvtPlane_*_*")

process.source.fileNames      = cms.untracked.vstring(options.inputFiles)
process.maxEvents             = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )



process.hionia = cms.EDAnalyzer('HiOniaAnalyzer',
                                #-- Collections
                                # l1GtReadoutRecordInputTag = cms.InputTag("gtDigis","","RECO"), # Only use if prescale warnings are shown
                                srcMuon             = cms.InputTag("patMuonsWithTrigger"),     # Name of PAT Muon Collection
                                srcMuonNoTrig       = cms.InputTag("patMuonsWithoutTrigger"),  # Name of PAT Muon Without Trigger Collection
                                src                 = cms.InputTag("onia2MuMuPatGlbGlb"),      # Name of Onia Skim Collection
                                EvtPlane            = cms.InputTag("hiEvtPlane",""),           # Name of Event Plane Collection. For RECO use: hiEventPlane,recoLevel

                                triggerResultsLabel = cms.InputTag("TriggerResults","","HLT"), # Label of Trigger Results

                                #-- Reco Details
                                useBeamSpot = cms.bool(False),  
                                useRapidity = cms.bool(True),
                                
                                #--
                                maxAbsZ = cms.double(24.0),
                                
                                pTBinRanges      = cms.vdouble(0.0, 6.0, 8.0, 9.0, 10.0, 12.0, 15.0, 40.0),
                                etaBinRanges     = cms.vdouble(0.0, 2.5),
                                centralityRanges = cms.vdouble(20,40,100),

                                onlyTheBest        = cms.bool(False),		
                                applyCuts          = cms.bool(True),
                                selTightGlobalMuon = cms.bool(False),
                                storeEfficiency    = cms.bool(False),
                      
                                removeSignalEvents = cms.untracked.bool(False),  # Remove/Keep signal events
                                removeTrueMuons    = cms.untracked.bool(False),  # Remove/Keep gen Muons
                                storeSameSign      = cms.untracked.bool(True),   # Store/Drop same sign dimuons
                                
                                #-- Gen Details
                                oniaPDG = cms.int32(443),
                                muonSel = cms.string(muonSelection),
                                isHI = cms.untracked.bool(isPbPb),
                                isPA = cms.untracked.bool(False),
                                isMC = cms.untracked.bool(isMC),
                                isPromptMC = cms.untracked.bool(False),
                                useEvtPlane = cms.untracked.bool(useEventPlane),
                                useGeTracks = cms.untracked.bool(useGeneralTracks),
                                runVersionChange = cms.untracked.uint32(182133),

                                #-- Histogram configuration
                                combineCategories = cms.bool(False),
                                fillRooDataSet    = cms.bool(False),
                                fillTree          = cms.bool(True),
                                fillHistos        = cms.bool(False),
                                minimumFlag       = cms.bool(True),
                                fillSingleMuons   = cms.bool(True),
                                fillRecoTracks    = cms.bool(False),
                                histFileName      = cms.string(options.outputFile),		
                                dataSetName       = cms.string(options.secondaryOutputFile),
                                
                                # HLT PbPbP MENU:  /online/collisions/2015/HeavyIons/v1.0/HLT/V6

                                dblTriggerPathNames = cms.vstring("HLT_HIL1DoubleMu0_v1",
                                                                  "HLT_HIL1DoubleMu0_2HF_v1",
                                                                  "HLT_HIL1DoubleMu0_2HF0_v1",
                                                                  "HLT_HIL1DoubleMu10_v1",
                                                                  "HLT_HIL2DoubleMu0_NHitQ_v2",
                                                                  "HLT_HIL2DoubleMu0_NHitQ_2HF_v1",
                                                                  "HLT_HIL2DoubleMu0_NHitQ_2HF0_v1",
                                                                  "HLT_HIL1DoubleMu0_2HF_Cent30100_v1",
                                                                  "HLT_HIL1DoubleMu0_2HF0_Cent30100_v1",
                                                                  "HLT_HIL2DoubleMu0_2HF_Cent30100_NHitQ_v1",
                                                                  "HLT_HIL1DoubleMu0_Cent30_v1",
                                                                  "HLT_HIL2DoubleMu0_2HF0_Cent30100_NHitQ_v1",
                                                                  "HLT_HIL2DoubleMu0_Cent30_NHitQ_v1",
                                                                  "HLT_HIL2DoubleMu0_Cent30_OS_NHitQ_v1",
                                                                  "HLT_HIL3DoubleMu0_Cent30_v1",
                                                                  "HLT_HIL3DoubleMu0_Cent30_OS_m2p5to4p5_v1",
                                                                  "HLT_HIL3DoubleMu0_Cent30_OS_m7to14_v1",
                                                                  "HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
                                                                  "HLT_HIL3DoubleMu0_OS_m7to14_v1"),
                                
                                dblTriggerFilterNames = cms.vstring("hltHIDoubleMu0L1Filtered",
                                                                    "hltHIDoubleMu0MinBiasL1Filtered",
                                                                    "hltHIDoubleMu0HFTower0Filtered",
                                                                    "hltHIDoubleMu10L1Filtered",
                                                                    "hltHIL2DoubleMu0NHitQFiltered",
                                                                    "hltHIL2DoubleMu0NHitQ2HFFiltered",
                                                                    "hltHIL2DoubleMu0NHitQ2HF0Filtered",
                                                                    "hltHIDoubleMu0MinBiasCent30to100L1Filtered",
                                                                    "hltHIDoubleMu0HFTower0Cent30to100L1Filtered",
                                                                    "hltHIL2DoubleMu02HFcent30100NHitQFiltered",
                                                                    "hltHIDoubleMu0MinBiasCent30L1Filtered",
                                                                    "hltHIL2DoubleMu02HF0cent30100NHitQFiltered",
                                                                    "hltHIL2DoubleMu0cent30NHitQFiltered",
                                                                    "hltHIL2DoubleMu0cent30OSNHitQFiltered",
                                                                    "hltHIDimuonOpenCentrality30L3Filter",
                                                                    "hltHIDimuonOpenCentrality30OSm2p5to4p5L3Filter",
                                                                    "hltHIDimuonOpenCentrality30OSm7to14L3Filter",
                                                                    "hltHIDimuonOpenOSm2p5to4p5L3Filter",
                                                                    "hltHIDimuonOpenOSm7to14L3Filter"),
                                
                                sglTriggerPathNames = cms.vstring("HLT_HIL2Mu3_NHitQ10_2HF_v1",
                                                                  "HLT_HIL2Mu3_NHitQ10_2HF0_v1",
                                                                  "HLT_HIL3Mu3_NHitQ15_2HF_v1",
                                                                  "HLT_HIL3Mu3_NHitQ15_2HF0_v1",
                                                                  "HLT_HIL2Mu5_NHitQ10_2HF_v1",
                                                                  "HLT_HIL2Mu5_NHitQ10_2HF0_v1",
                                                                  "HLT_HIL3Mu5_NHitQ15_2HF_v1",
                                                                  "HLT_HIL3Mu5_NHitQ15_2HF0_v1",
                                                                  "HLT_HIL2Mu7_NHitQ10_2HF_v1",
                                                                  "HLT_HIL2Mu7_NHitQ10_2HF0_v1",
                                                                  "HLT_HIL3Mu7_NHitQ15_2HF_v1",
                                                                  "HLT_HIL3Mu7_NHitQ15_2HF0_v1",
                                                                  "HLT_HIL2Mu15_v2",
                                                                  "HLT_HIL2Mu15_2HF_v1",
                                                                  "HLT_HIL2Mu15_2HF0_v1",
                                                                  "HLT_HIL3Mu15_v1",
                                                                  "HLT_HIL3Mu15_2HF_v1",
                                                                  "HLT_HIL3Mu15_2HF0_v1",
                                                                  "HLT_HIL2Mu20_v1",
                                                                  "HLT_HIL2Mu20_2HF_v1",	
                                                                  "HLT_HIL2Mu20_2HF0_v1",
                                                                  "HLT_HIL3Mu20_v1",
                                                                  "HLT_HIL3Mu20_2HF_v1",
                                                                  "HLT_HIL3Mu20_2HF0_v1"),
                                
                                sglTriggerFilterNames = cms.vstring("hltHIL2Mu3N10HitQ2HFL2Filtered",
                                                                    "hltHIL2Mu3N10HitQ2HF0L2Filtered",
                                                                    "hltHISingleMu3NHit152HFL3Filtered",
                                                                    "hltHISingleMu3NHit152HF0L3Filtered",
                                                                    "hltHIL2Mu5N10HitQ2HFL2Filtered",
                                                                    "hltHIL2Mu5N10HitQ2HF0L2Filtered",
                                                                    "hltHISingleMu5NHit152HFL3Filtered",
                                                                    "hltHISingleMu5NHit152HF0L3Filtered",
                                                                    "hltHIL2Mu7N10HitQ2HFL2Filtered",
                                                                    "hltHIL2Mu7N10HitQ2HF0L2Filtered",
                                                                    "hltHISingleMu7NHit152HFL3Filtered",
                                                                    "hltHISingleMu7NHit152HF0L3Filtered",
                                                                    "hltHIL2Mu15L2Filtered",
                                                                    "hltHIL2Mu152HFFiltered",
                                                                    "hltHIL2Mu15N10HitQ2HF0L2Filtered",
                                                                    "hltHISingleMu15L3Filtered",
                                                                    "hltHISingleMu152HFL3Filtered",
                                                                    "hltHISingleMu152HF0L3Filtered",
                                                                    "hltHIL2Mu20L2Filtered",
                                                                    "hltHIL2Mu202HFL2Filtered",
                                                                    "hltHIL2Mu202HF0L2Filtered",
                                                                    "hltHIL3SingleMu20L3Filtered",
                                                                    "hltHISingleMu202HFL3Filtered",
                                                                    "hltHISingleMu202HF0L3Filtered")

                                )

if isPbPb:
  process.hionia.primaryVertexTag = cms.InputTag("hiSelectedVertex")
  process.hionia.genParticles     = cms.InputTag("genParticles")
  process.hionia.muonLessPV       = cms.bool(False)
  process.hionia.CentralitySrc    = cms.InputTag("hiCentrality")
  process.hionia.srcTracks        = cms.InputTag("hiGeneralTracks") 
  if isMC:
    process.hionia.CentralityBinSrc = cms.InputTag("centralityBin","HFtowersHydjetDrum5")  
  else:
    process.hionia.CentralityBinSrc = cms.InputTag("centralityBin","HFtowers")
  
  #process.p = cms.EndPath(process.hiEvtPlane*process.hionia)
  process.Onia2MuMuPAT.append(process.hionia)
else:    
  process.hionia.primaryVertexTag = cms.InputTag("offlinePrimaryVertices")
  process.hionia.genParticles     = cms.InputTag("genParticles")
  process.hionia.muonLessPV       = cms.bool(True)
  process.hionia.CentralitySrc    = cms.InputTag("")
  process.hionia.CentralityBinSrc = cms.InputTag("")
  process.hionia.srcTracks        = cms.InputTag("generalTracks")  

  process.Onia2MuMuPAT.append(process.hionia)


##### Construct the sequence
process.schedule = cms.Schedule(process.Onia2MuMuPAT)
  
from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
MassReplaceInputTag(process)


'''

FOR PbPb: /online/collisions/2015/HeavyIons/v1.0/HLT/V6

dblTriggerPathNames = cms.vstring("HLT_HIL1DoubleMu0_v1",
	"HLT_HIL1DoubleMu0_2HF_v1",
	"HLT_HIL1DoubleMu0_2HF0_v1",
	"HLT_HIL1DoubleMu10_v1",
	"HLT_HIL2DoubleMu0_NHitQ_v2",
	"HLT_HIL2DoubleMu0_NHitQ_2HF_v1",
	"HLT_HIL2DoubleMu0_NHitQ_2HF0_v1",
	"HLT_HIL1DoubleMu0_2HF_Cent30100_v1",
	"HLT_HIL1DoubleMu0_2HF0_Cent30100_v1",
	"HLT_HIL2DoubleMu0_2HF_Cent30100_NHitQ_v1",
	"HLT_HIL1DoubleMu0_Cent30_v1",
	"HLT_HIL2DoubleMu0_2HF0_Cent30100_NHitQ_v1",
	"HLT_HIL2DoubleMu0_Cent30_NHitQ_v1",
	"HLT_HIL2DoubleMu0_Cent30_OS_NHitQ_v1",
	"HLT_HIL3DoubleMu0_Cent30_v1",
	"HLT_HIL3DoubleMu0_Cent30_OS_m2p5to4p5_v1",
	"HLT_HIL3DoubleMu0_Cent30_OS_m7to14_v1",
	"HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
	"HLT_HIL3DoubleMu0_OS_m7to14_v1"),


dblTriggerFilterNames = cms.vstring("hltHIDoubleMu0L1Filtered",
	"hltHIDoubleMu0MinBiasL1Filtered",
	"hltHIDoubleMu0HFTower0Filtered",
	"hltHIDoubleMu10L1Filtered",
	"hltHIL2DoubleMu0NHitQFiltered",
	"hltHIL2DoubleMu0NHitQ2HFFiltered",
	"hltHIL2DoubleMu0NHitQ2HF0Filtered",
	"hltHIDoubleMu0MinBiasCent30to100L1Filtered",
	"hltHIDoubleMu0HFTower0Cent30to100L1Filtered",
	"hltHIL2DoubleMu02HFcent30100NHitQFiltered",
	"hltHIDoubleMu0MinBiasCent30L1Filtered",
	"hltHIL2DoubleMu02HF0cent30100NHitQFiltered",
	"hltHIL2DoubleMu0cent30NHitQFiltered",
	"hltHIL2DoubleMu0cent30OSNHitQFiltered",
	"hltHIDimuonOpenCentrality30L3Filter",
	"hltHIDimuonOpenCentrality30OSm2p5to4p5L3Filter",
	"hltHIDimuonOpenCentrality30OSm7to14L3Filter",
	"hltHIDimuonOpenOSm2p5to4p5L3Filter",
	"hltHIDimuonOpenOSm7to14L3Filter"),

sglTriggerPathNames = cms.vstring("HLT_HIL2Mu3_NHitQ10_2HF_v1",
	"HLT_HIL2Mu3_NHitQ10_2HF0_v1",
	"HLT_HIL3Mu3_NHitQ15_2HF_v1",
	"HLT_HIL3Mu3_NHitQ15_2HF0_v1",
	"HLT_HIL2Mu5_NHitQ10_2HF_v1",
	"HLT_HIL2Mu5_NHitQ10_2HF0_v1",
	"HLT_HIL3Mu5_NHitQ15_2HF_v1",
	"HLT_HIL3Mu5_NHitQ15_2HF0_v1",
	"HLT_HIL2Mu7_NHitQ10_2HF_v1",
	"HLT_HIL2Mu7_NHitQ10_2HF0_v1",
	"HLT_HIL3Mu7_NHitQ15_2HF_v1",
	"HLT_HIL3Mu7_NHitQ15_2HF0_v1",
	"HLT_HIL2Mu15_v2",
	"HLT_HIL2Mu15_2HF_v1",
	"HLT_HIL2Mu15_2HF0_v1",
	"HLT_HIL3Mu15_v1",
	"HLT_HIL3Mu15_2HF_v1",
	"HLT_HIL3Mu15_2HF0_v1",
	"HLT_HIL2Mu20_v1",
	"HLT_HIL2Mu20_2HF_v1",	
	"HLT_HIL2Mu20_2HF0_v1",
	"HLT_HIL3Mu20_v1",
	"HLT_HIL3Mu20_2HF_v1",
	"HLT_HIL3Mu20_2HF0_v1"),

sglTriggerFilterNames = cms.vstring("hltHIL2Mu3N10HitQ2HFL2Filtered",
	"hltHIL2Mu3N10HitQ2HF0L2Filtered",
	"hltHISingleMu3NHit152HFL3Filtered",
	"hltHISingleMu3NHit152HF0L3Filtered",
	"hltHIL2Mu5N10HitQ2HFL2Filtered",
	"hltHIL2Mu5N10HitQ2HF0L2Filtered",
	"hltHISingleMu5NHit152HFL3Filtered",
	"hltHISingleMu5NHit152HF0L3Filtered",
	"hltHIL2Mu7N10HitQ2HFL2Filtered",
	"hltHIL2Mu7N10HitQ2HF0L2Filtered",
	"hltHISingleMu7NHit152HFL3Filtered",
	"hltHISingleMu7NHit152HF0L3Filtered",
	"hltHIL2Mu15L2Filtered",
	"hltHIL2Mu152HFFiltered",
	"hltHIL2Mu15N10HitQ2HF0L2Filtered",
	"hltHISingleMu15L3Filtered",
	"hltHISingleMu152HFL3Filtered",
	"hltHISingleMu152HF0L3Filtered",
	"hltHIL2Mu20L2Filtered",
	"hltHIL2Mu202HFL2Filtered",
	"hltHIL2Mu202HF0L2Filtered",
	"hltHIL3SingleMu20L3Filtered",
	"hltHISingleMu202HFL3Filtered",
	"hltHISingleMu202HF0L3Filtered")


FOR pp: /users/HiMuonTrigDev/pp5TeV/NovDev/V4

dblTriggerPathNames = cms.vstring("HLT_HIL1DoubleMu0_v1",
	"HLT_HIL1DoubleMu10_v1",
	"HLT_HIL2DoubleMu0_NHitQ_v1",
	"HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
	"HLT_HIL3DoubleMu0_OS_m7to14_v1"),

dblTriggerFilterNames = cms.vstring("hltHIDoubleMu0L1Filtered",
	"hltHIDoubleMu10MinBiasL1Filtered",
	"hltHIL2DoubleMu0NHitQFiltered",
	"hltHIDimuonOpenOSm2p5to4p5L3Filter",
	"hltHIDimuonOpenOSm7to14L3Filter"),

sglTriggerPathNames = cms.vstring("HLT_HIL2Mu3_NHitQ10_v1",
	"HLT_HIL3Mu3_NHitQ15_v1",
	"HLT_HIL2Mu5_NHitQ10_v1",
	"HLT_HIL3Mu5_NHitQ15_v1",
	"HLT_HIL2Mu7_NHitQ10_v1",
	"HLT_HIL3Mu7_NHitQ15_v1",
	"HLT_HIL2Mu15_v1",
	"HLT_HIL3Mu15_v1",
	"HLT_HIL2Mu20_v1",
	"HLT_HIL3Mu20_v1"),

sglTriggerFilterNames = cms.vstring("hltHIL2Mu3N10HitQL2Filtered",
	"hltHISingleMu3NHit15L3Filtered",
	"hltHIL2Mu5N10HitQL2Filtered",
	"hltHISingleMu5NHit15L3Filtered",
	"hltHISingleMu5NHit15L3Filtered",
	"hltHISingleMu7NHit15L3Filtered",
	"hltHIL2Mu15L2Filtered",
	"hltHISingleMu15L3Filtered",
	"hltHIL2Mu20L2Filtered",
	"hltHIL3SingleMu20L3Filtered")

'''
