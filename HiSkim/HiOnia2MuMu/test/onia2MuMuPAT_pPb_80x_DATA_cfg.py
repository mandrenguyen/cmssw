import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from   Configuration.StandardSequences.Eras import eras


#----------------------------------------------------------------------------

# Setup Settings for ONIA SKIM:

isMC           = False     # if input is MONTECARLO: True or if it's DATA: False
applyEventSel  = True      # if we want to apply 2016 pPb Event Selection
muonSelection  = "Trk"     # Single muon selection: Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker) are availale

#----------------------------------------------------------------------------


# Print Onia Skim settings:
print( " " )
print( "[INFO] Settings used for ONIA SKIM: " )
print( "[INFO] isMC          = " + ("True" if isMC else "False") )
print( "[INFO] applyEventSel = " + ("True" if applyEventSel else "False") )
print( "[INFO] muonSelection = " + muonSelection )
print( " " ) 

# set up process
process = cms.Process("Onia2MuMuPAT",eras.Run2_2016_pA)

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# setup any defaults you want
options.inputFiles = 'file:/home/llr/cms/stahl/ElectroWeakAnalysis/CMSSW_8_0_26_patch2/src/HeavyIonsAnalysis/ElectroWeakAnalysis/test/DATA/0E6009E4-65B0-E611-9C6B-02163E014503.root'
options.outputFile = 'onia2MuMuPAT_DATA_80X.root'

options.maxEvents = -1 # -1 means all events

# get and parse the command line arguments
options.parseArguments()
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.MessageLogger.categories.extend(["HiOnia2MuMuPAT_muonLessSizeORpvTrkSize"])
process.MessageLogger.cerr.HiOnia2MuMuPAT_muonLessSizeORpvTrkSize = cms.untracked.PSet( limit = cms.untracked.int32(5) )

# load the Geometry and Magnetic Field for the TransientTrackBuilder
process.load('Configuration.StandardSequences.Services_cff')
process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.Reconstruction_cff')

# Global Tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

# HLT Dimuon Triggers
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltOniaHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# HLT pPb MENU 2016
process.hltOniaHI.HLTPaths =  [
  "HLT_PAL1DoubleMuOpen_v1",
  "HLT_PAL1DoubleMuOpen_OS_v1",
  "HLT_PAL1DoubleMuOpen_SS_v1",
  "HLT_PAL1DoubleMu0_v1",
  "HLT_PAL1DoubleMu0_MGT1_v1",
  "HLT_PAL1DoubleMu0_HighQ_v1",
  "HLT_PAL2DoubleMu0_v1",
  "HLT_PAL3DoubleMu0_v1",
  "HLT_PAL3DoubleMu0_HIon_v1",
  "HLT_PAL1DoubleMu10_v1",
  "HLT_PAL2DoubleMu10_v1",
  "HLT_PAL3DoubleMu10_v1",
  "HLT_PAL2Mu12_v1",
  "HLT_PAL2Mu15_v1",
  "HLT_PAL3Mu3_v1",
  "HLT_PAL3Mu5_v3",
  "HLT_PAL3Mu7_v1",
  "HLT_PAL3Mu12_v1",
  "HLT_PAL3Mu15_v1"
]

process.hltOniaHI.throw = False
process.hltOniaHI.andOr = True
process.hltOniaHI.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT")

from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import *
onia2MuMuPAT(process, GlobalTag=process.GlobalTag.globaltag, MC=isMC, HLT="HLT", Filter=True, useL1Stage2=True)

##### Onia2MuMuPAT input collections/options
process.onia2MuMuPatGlbGlb.dimuonSelection          = cms.string("mass > 0")
process.onia2MuMuPatGlbGlb.resolvePileUpAmbiguity   = True
process.onia2MuMuPatGlbGlb.srcTracks                = cms.InputTag("generalTracks")
process.onia2MuMuPatGlbGlb.primaryVertexTag         = cms.InputTag("offlinePrimaryVertices")
process.patMuonsWithoutTrigger.pvSrc                = cms.InputTag("offlinePrimaryVertices")
# Adding muonLessPV gives you lifetime values wrt. muonLessPV only
process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = True
if isMC:
  process.genMuons.src = "genParticles"
  process.onia2MuMuPatGlbGlb.genParticles = "genParticles"

##### Dimuon pair selection
commonP1 = "|| (innerTrack.isNonnull && genParticleRef(0).isNonnull)"
commonP2 = " && abs(innerTrack.dxy)<4 && abs(innerTrack.dz)<35"
if muonSelection == "Glb":
  highP = "isGlobalMuon"; # At least one muon must pass this selection
  process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("("+highP+commonP1+")"+commonP2)
  lowP = "isGlobalMuon"; # BOTH muons must pass this selection
  process.onia2MuMuPatGlbGlb.lowerPuritySelection = cms.string("("+lowP+commonP1+")"+commonP2)
elif muonSelection == "GlbTrk":
  highP = "(isGlobalMuon && isTrackerMuon)";
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

##### Event Selection
if applyEventSel:
  process.load("HeavyIonsAnalysis.Configuration.collisionEventSelection_cff")
  process.patMuonSequence.replace(process.hltOniaHI , process.hltOniaHI * process.collisionEventSelectionPA )

##### Remove few paths for MC
if isMC:
  process.patMuonSequence.remove(process.hltOniaHI)



process.source.fileNames      = cms.untracked.vstring(options.inputFiles)        
process.maxEvents             = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.outOnia2MuMu.fileName = cms.untracked.string( options.outputFile )
process.e                     = cms.EndPath(process.outOnia2MuMu)
process.schedule              = cms.Schedule(process.Onia2MuMuPAT,process.e)

from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
MassReplaceInputTag(process)
