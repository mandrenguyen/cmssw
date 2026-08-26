import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras

#----------------------------------------------------------------------------

isMC = True # if input is MONTECARLO: True or if it's DATA: False
pdgId = 443 # J/Psi : 443, Y(1S) : 553
useMomFormat = "array" # default "array" for TClonesArray of TLorentzVector. Use "vector" for std::vector<float> of pt, eta, phi, M

#----------------------------------------------------------------------------

# Print Onia Tree settings:
print( " " )
print( "[INFO] Settings used for ONIA TREE: " )
print( "[INFO] isMC = " + ("True" if isMC else "False") )
print( "[INFO] only processing generation information!" )

# set up process
process = cms.Process("HIOnia", eras.Run3_2024_ppRef)

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Name
options.outputFile = "Oniatree_GenOnly_PromptJpsi_141X.root"

options.inputFiles =[
  '/store/user/fdamas/GenOnly_Run3/PromptJpsi_NoDecayCuts_5p36TeV/PromptJpsi/250401_212724/0000/promptJpsi_noDecayCuts_5p36TeV_GEN_101.root'
]
options.maxEvents = -1 # -1 means all events

# Get and parse the command line arguments
options.parseArguments()


## Global tag (does not matter here but required by oniaTreeAnalyzer)
if isMC:
  globalTag = '141X_mcRun3_2024_realistic_ppRef5TeV_v5'

#----------------------------------------------------------------------------

# load the Geometry
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')

# Global Tag:
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, globalTag, '')


#----------------------------------------------------------------------------

# For OniaTree Analyzer
from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process, isMC=isMC, pdgID=pdgId, outputFileName=options.outputFile, muonTriggerList={'DoubleMuonTrigger':cms.vstring(), 'SingleMuonTrigger':cms.vstring()})

process.oniaTreeAna = cms.Path(process.oniaTreeAna)

process.hionia.mom4format       = cms.string(useMomFormat)
process.hionia.isHI = cms.untracked.bool(False)

process.oniaTreeAna = cms.Path(process.genMuons * process.hionia)
process.hionia.genealogyInfo    = cms.bool(False)
process.hionia.genOnly = cms.bool(True) 


#----------------------------------------------------------------------------
#Options:
process.source = cms.Source("PoolSource",
#process.source = cms.Source("NewEventStreamFileReader", # for streamer data
		fileNames = cms.untracked.vstring( options.inputFiles ),
		)
process.TFileService = cms.Service("TFileService",
		fileName = cms.string( options.outputFile )
		)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.options   = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.schedule  = cms.Schedule( process.oniaTreeAna )