### HiForest Configuration
# Collisions: pp
# Type: Data
# Input: AOD

import FWCore.ParameterSet.Config as cms
process = cms.Process('HiForest')
process.options = cms.untracked.PSet()

#####################################################################################
# HiForest labelling info
#####################################################################################

process.load("HeavyIonsAnalysis.JetAnalysis.HiForest_cff")
process.HiForest.inputLines = cms.vstring("HiForest V3",)
import subprocess
version = subprocess.Popen(["(cd $CMSSW_BASE/src && git describe --tags)"], stdout=subprocess.PIPE, shell=True).stdout.read()
if version == '':
    version = 'no git info'
process.HiForest.HiForestVersion = cms.string(version)

#####################################################################################
# Input source
#####################################################################################

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
        '/store/data/Run2015E/HighPtJet80/AOD/PromptReco-v1/000/262/167/00000/CC523A5C-F293-E511-849A-02163E013526.root'
        #'/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/445/00000/48045897-4E8E-E511-B52E-02163E01469D.root'
        #'/store/data/Run2015E/DoubleMu/AOD/PromptReco-v1/000/262/163/00000/100EECED-D491-E511-8BBA-02163E013451.root'
        #'/store/data/Run2015E/HighPtJet80/AOD/PromptReco-v1/000/262/173/00000/3E8293B5-9894-E511-90E8-02163E011FA1.root'                        
        #'/store/data/Run2015E/HighPtJet80/AOD/PromptReco-v1/000/262/272/00000/803A4255-7696-E511-B178-02163E0142DD.root'
                            )
)

# Number of events we want to process, -1 = all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1))


#####################################################################################
# Load Global Tag, Geometry, etc.
#####################################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

from HeavyIonsAnalysis.Configuration.CommonFunctions_cff import overrideJEC_pp5020
process = overrideJEC_pp5020(process)

#####################################################################################
# Define tree output
#####################################################################################

process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("HiForestAOD.root"))

#####################################################################################
# Additional Reconstruction and Analysis: Main Body
#####################################################################################

####################################################################################

#############################
# Jets
#############################

### PP RECO does not include R=3 or R=5 jets.
### re-RECO is only possible for PF, RECO is missing calotowers
from RecoJets.JetProducers.ak5PFJets_cfi import ak5PFJets
ak5PFJets.doAreaFastjet = True
process.ak5PFJets = ak5PFJets
process.ak3PFJets = ak5PFJets.clone(rParam = 0.3)
process.ak4PFJets = ak5PFJets.clone(rParam = 0.4)

process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak4CaloJetSequence_pp_data_cff')

process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak3PFJetSequence_pp_data_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak4PFJetSequence_pp_data_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak5PFJetSequence_pp_data_cff')

process.highPurityTracks = cms.EDFilter("TrackSelector",
                                        src = cms.InputTag("generalTracks"),
                                        cut = cms.string('quality("highPurity")')
                                        )



process.ak4PFXpatJets = cms.EDFilter("PatJetXSelector",
                                     src = cms.InputTag("ak4PFpatJetsWithBtagging"),
                                     cut = cms.string("pt > 10.0 && abs(rapidity()) < 2.4")
                                     )



process.ak4PFJetSequence.remove(process.ak4PFJetAnalyzer)
process.ak4PFJetSequence*=process.ak4PFXpatJets
process.ak4PFJetSequence*=process.ak4PFJetAnalyzer
process.ak4PFJetAnalyzer.jetTag = "ak4PFXpatJets"


process.jetSequences = cms.Sequence(
    #process.ak3PFJets +
    process.ak4PFJets +
    #process.ak5PFJets +
    process.highPurityTracks +
    #process.ak4CaloJetSequence +
    #process.ak3PFJetSequence +
    process.ak4PFJetSequence #+
    #process.ak5PFJetSequence
    )



# How to turn on the jet constituents 
#process.ak4PFJetAnalyzer.doJetConstituents = cms.untracked.bool(True)
#process.ak4PFJetAnalyzer.doNewJetVars = cms.untracked.bool(True)


#####################################################################################

############################
# Event Analysis
############################
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.hiEvtAnalyzer.Vertex = cms.InputTag("offlinePrimaryVertices")
process.hiEvtAnalyzer.doCentrality = cms.bool(False)
process.hiEvtAnalyzer.doEvtPlane = cms.bool(False)

process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cff')
from HeavyIonsAnalysis.EventAnalysis.dummybranches_cff import addHLTdummybranchesForPP
addHLTdummybranchesForPP(process)

process.load("HeavyIonsAnalysis.JetAnalysis.pfcandAnalyzer_cfi")
process.pfcandAnalyzer.skipCharged = False
process.pfcandAnalyzer.pfPtMin = 0
process.pfcandAnalyzer.pfCandidateLabel = cms.InputTag("particleFlow")
process.pfcandAnalyzer.doVS = cms.untracked.bool(False)
process.pfcandAnalyzer.doUEraw_ = cms.untracked.bool(False)
process.pfcandAnalyzer.genLabel = cms.InputTag("genParticles")
process.load("HeavyIonsAnalysis.JetAnalysis.hcalNoise_cff")

#####################################################################################

#########################
# Track Analyzer
#########################
#process.load('HeavyIonsAnalysis.JetAnalysis.ExtraTrackReco_cff')
#process.load('HeavyIonsAnalysis.JetAnalysis.TrkAnalyzers_cff')

####################################################################################

#####################
# Photons
#####################
#process.load('HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi')
#process.ggHiNtuplizer.gsfElectronLabel   = cms.InputTag("gedGsfElectrons")
#process.ggHiNtuplizer.recoPhotonHiIsolationMap = cms.InputTag('photonIsolationHIProducerpp')
#process.ggHiNtuplizer.VtxLabel  = cms.InputTag("offlinePrimaryVertices")
#process.ggHiNtuplizer.particleFlowCollection = cms.InputTag("particleFlow")
#process.ggHiNtuplizer.doVsIso   = cms.bool(False)
#process.ggHiNtuplizer.doGenParticles = False
#process.ggHiNtuplizer.doElectronVID = cms.bool(True)
#process.ggHiNtuplizerGED = process.ggHiNtuplizer.clone(recoPhotonSrc = cms.InputTag('gedPhotons'),
#                                                       recoPhotonHiIsolationMap = cms.InputTag('photonIsolationHIProducerppGED'))
#
#####################################################################################

#####################
# Electron ID
#####################

#from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format to be processed
# DataFormat.AOD or DataFormat.MiniAOD
#dataFormat = DataFormat.AOD
#switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce. Check here https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2#Recipe_for_regular_users_for_7_4
#my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff']

#add them to the VID producer
#for idmod in my_id_modules:
#    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

#####################################################################################

#####################
# tupel and necessary PAT sequences
#####################

#process.load("HeavyIonsAnalysis.VectorBosonAnalysis.tupelSequence_pp_cff")

#####################################################################################


AddCaloMuon = False
runOnMC = False
HIFormat = False
UseGenPlusSim = False
VtxLabel = "offlinePrimaryVerticesWithBS"
TrkLabel = "generalTracks"
from Bfinder.finderMaker.finderMaker_75X_cff import finderMaker_75X
finderMaker_75X(process, AddCaloMuon, runOnMC, HIFormat, UseGenPlusSim, VtxLabel, TrkLabel)

process.Dfinder.printInfo = False
process.Dfinder.tkEtaCut = cms.double(2.5)
process.Dfinder.tkPtCut = cms.double(1.0)
process.Dfinder.alphaCut = cms.vdouble(0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0)
process.Dfinder.svpvDistanceCut_lowptD = cms.vdouble(3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
process.Dfinder.svpvDistanceCut_highptD = cms.vdouble(3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)


#########################
# Main analysis list
#########################


process.ana_step = cms.Path(process.hltanalysis *
			    process.hltobject *
                            process.hiEvtAnalyzer *
                            process.DfinderSequence *
                            process.jetSequences +
                            #process.egmGsfElectronIDSequence + #Should be added in the path for VID module
                            #process.ggHiNtuplizer +
                            #process.ggHiNtuplizerGED +
                            #process.pfcandAnalyzer +
                            process.HiForest #+
                            #process.trackSequencesPP +
                            #process.tupelPatSequence
                            )

#####################################################################################

#########################
# Event Selection
#########################

process.load('HeavyIonsAnalysis.JetAnalysis.EventSelection_cff')
process.pHBHENoiseFilterResultProducer = cms.Path( process.HBHENoiseFilterResultProducer )
process.HBHENoiseFilterResult = cms.Path(process.fHBHENoiseFilterResult)
process.HBHENoiseFilterResultRun1 = cms.Path(process.fHBHENoiseFilterResultRun1)
process.HBHENoiseFilterResultRun2Loose = cms.Path(process.fHBHENoiseFilterResultRun2Loose)
process.HBHENoiseFilterResultRun2Tight = cms.Path(process.fHBHENoiseFilterResultRun2Tight)
process.HBHEIsoNoiseFilterResult = cms.Path(process.fHBHEIsoNoiseFilterResult)

process.PAprimaryVertexFilter = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlinePrimaryVertices"),
    cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2 && tracksSize >= 2"),
    filter = cms.bool(True), # otherwise it won't filter the events
)

process.NoScraping = cms.EDFilter("FilterOutScraping",
 applyfilter = cms.untracked.bool(True),
 debugOn = cms.untracked.bool(False),
 numtrack = cms.untracked.uint32(10),
 thresh = cms.untracked.double(0.25)
)

process.pPAprimaryVertexFilter = cms.Path(process.PAprimaryVertexFilter)
process.pBeamScrapingFilter=cms.Path(process.NoScraping)

process.load("HeavyIonsAnalysis.VertexAnalysis.PAPileUpVertexFilter_cff")

process.pVertexFilterCutG = cms.Path(process.pileupVertexFilterCutG)
process.pVertexFilterCutGloose = cms.Path(process.pileupVertexFilterCutGloose)
process.pVertexFilterCutGtight = cms.Path(process.pileupVertexFilterCutGtight)
process.pVertexFilterCutGplus = cms.Path(process.pileupVertexFilterCutGplus)
process.pVertexFilterCutE = cms.Path(process.pileupVertexFilterCutE)
process.pVertexFilterCutEandG = cms.Path(process.pileupVertexFilterCutEandG)

process.pAna = cms.EndPath(process.skimanalysis)

# Customization
process.hltobject.triggerNames = cms.vstring(
    #'HLT_AK4CaloJet40_Eta5p1_v', 
    #'HLT_AK4CaloJet60_Eta5p1_v', 
    #'HLT_AK4CaloJet80_Eta5p1_v', 
    #'HLT_AK4PFJet40_Eta5p1_v', 
    #'HLT_AK4PFJet60_Eta5p1_v', 
    'HLT_AK4PFJet80_Eta5p1_v', 
    #'HLT_DmesonPPTrackingGlobal_Dpt8_v',
    #'HLT_DmesonPPTrackingGlobal_Dpt15_v',
    #'HLT_DmesonPPTrackingGlobal_Dpt20_v',
    #'HLT_DmesonPPTrackingGlobal_Dpt30_v',
    #'HLT_DmesonPPTrackingGlobal_Dpt40_v',
    #'HLT_DmesonPPTrackingGlobal_Dpt50_v',
    #'HLT_DmesonPPTrackingGlobal_Dpt60_v',
)




process.load("RecoHI.HiJetAlgos.PFCandCompositeProducer_cfi")
process.pfCandComposites.compositeTag = cms.InputTag('Dfinder')
process.pfCandComposites.removeDKPi = True
process.ak4PFJets.src = 'pfCandComposites'
process.jetSequences.insert(0,process.pfCandComposites)


process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
process.hltJet60 = process.hltHighLevel.clone()
process.hltJet60.HLTPaths = ["HLT_AK4PFJet80_Eta5p1_v*"]

process.superFilterSequence = cms.Sequence(process.hltJet60)
process.superFilterPath = cms.Path(process.superFilterSequence)

process.skimanalysis.superFilters = cms.vstring("superFilterPath")
for path in process.paths:
    getattr(process,path)._seq = process.superFilterSequence*getattr(process,path)._seq
