### HiForest Configuration
# Collisions: pp
# Type: Data
# Input: AOD

import FWCore.ParameterSet.Config as cms
process = cms.Process('HiForest')
process.options = cms.untracked.PSet()

import CondCore.CondDB.CondDB_cfi
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
                                #'file:/afs/cern.ch/work/m/mnguyen/public/integration/forest/CMSSW_9_4_10/src/HeavyIonsAnalysis/JetAnalysis/test/highTriMuonMass_smallDR.root'
                                '/store/data/Run2017G/DoubleMuon/AOD/17Nov2017-v1/80004/A6E2F519-172F-E811-B029-0CC47AA98A0E.root'
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

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v6', '')
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

from HeavyIonsAnalysis.Configuration.CommonFunctions_cff import overrideJEC_pp5020
process = overrideJEC_pp5020(process)

#####################################################################################
# Define tree output
#####################################################################################

process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("HiForestAOD.root")
                                  )

#####################################################################################
# Additional Reconstruction and Analysis: Main Body
#####################################################################################

####################################################################################
# Onia
####################################################################################

from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process, muonSelection="Glb", isMC=False, outputFileName="HiForestAOD.root", muonlessPV=True)
#process.onia2MuMuPatGlbGlb.dimuonSelection = cms.string("pt > 3 && mass > 2 && mass < 4")
process.onia2MuMuPatGlbGlb.dimuonSelection = cms.string("pt > 0 && mass > 8 && mass < 14")
process.hionia.SumETvariables   = cms.bool(False)
#process.hionia.applyCuts = True

#####################################################################################

#############################
# Jets
#############################
    
### PP RECO does not include R=3 or R=5 jets.
### re-RECO is only possible for PF, RECO is missing calotowers
#from RecoJets.JetProducers.ak5PFJets_cfi import ak5PFJets
#ak5PFJets.doAreaFastjet = True
#process.ak5PFJets = ak5PFJets
#process.ak4PFJets = ak5PFJets.clone(rParam = 0.4)

from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
process.ak4PFJets = ak4PFJets
process.ak3PFJets = ak4PFJets.clone(rParam = 0.3)
    
#process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak4CaloJetSequence_pp_data_cff')    
process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak3PFJetSequence_pp_data_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak4PFJetSequence_pp_data_cff')
#process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak5PFJetSequence_pp_data_cff')

process.highPurityTracks = cms.EDFilter("TrackSelector",
                                        src = cms.InputTag("generalTracks"),
                                        cut = cms.string('quality("highPurity")')
                                        )

process.ak4PFXpatJets = cms.EDFilter("PatJetXSelector",
                                     src = cms.InputTag("ak4PFpatJetsWithBtagging"),
                                     cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.")
                                     )

process.ak4PFJetSequence.remove(process.ak4PFJetAnalyzer)
process.ak4PFJetSequence*=process.ak4PFXpatJets
process.ak4PFJetSequence*=process.ak4PFJetAnalyzer
process.ak4PFJetAnalyzer.jetTag = "ak4PFXpatJets"


process.ak3PFXpatJets = cms.EDFilter("PatJetXSelector",
                                     src = cms.InputTag("ak3PFpatJetsWithBtagging"),
                                     cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.")
                                     )

process.ak3PFJetSequence.remove(process.ak3PFJetAnalyzer)
process.ak3PFJetSequence*=process.ak3PFXpatJets
process.ak3PFJetSequence*=process.ak3PFJetAnalyzer
process.ak3PFJetAnalyzer.jetTag = "ak3PFXpatJets"

process.ak4PFcorr.payload = 'AK4PF'
process.ak3PFcorr.payload = 'AK3PF'

process.jetSequences = cms.Sequence(
    process.ak3PFJets +
    process.ak4PFJets +
    #    process.ak5PFJets +
    process.highPurityTracks +
    #    process.ak4CaloJetSequence +
    process.ak3PFJetSequence +
    process.ak4PFJetSequence #+
    #    process.ak5PFJetSequence
    )

process.particleFlowNoHF = cms.EDFilter("PdgIdPFCandidateSelector",
                                          src = cms.InputTag("particleFlow"),
                                          pdgId = cms.vint32(211,-211,11,13,-11,-13,22,130)
)

process.load("RecoHI.HiJetAlgos.PFCandCompositeProducer_cfi")
process.pfCandComposites.pfCandTag    = cms.InputTag('particleFlowNoHF')
#process.pfCandComposites.replaceJMM = True
process.pfCandComposites.replaceYMM = True
process.pfCandComposites.compositeTag = cms.InputTag("onia2MuMuPatGlbGlb")

process.ak4PFJets.src = 'pfCandComposites'
process.ak3PFJets.src = 'pfCandComposites'

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
process.load('HeavyIonsAnalysis.JetAnalysis.ExtraTrackReco_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.TrkAnalyzers_cff')

####################################################################################

#####################
# Photons
#####################
process.load('HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizer.gsfElectronLabel   = cms.InputTag("gedGsfElectrons")
process.ggHiNtuplizer.recoPhotonHiIsolationMap = cms.InputTag('photonIsolationHIProducerpp')
process.ggHiNtuplizer.useValMapIso = cms.bool(True) # set to False if it gives error due to "not found" photonIsolationHIProducer
process.ggHiNtuplizer.VtxLabel  = cms.InputTag("offlinePrimaryVertices")
process.ggHiNtuplizer.particleFlowCollection = cms.InputTag("particleFlow")
process.ggHiNtuplizer.doVsIso   = cms.bool(False)
process.ggHiNtuplizer.doGenParticles = False
process.ggHiNtuplizer.doElectronVID = cms.bool(True)
process.ggHiNtuplizerGED = process.ggHiNtuplizer.clone(recoPhotonSrc = cms.InputTag('gedPhotons'),
                                                       recoPhotonHiIsolationMap = cms.InputTag('photonIsolationHIProducerppGED'))

####################################################################################

#####################
# Electron ID
#####################

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format to be processed
# DataFormat.AOD or DataFormat.MiniAOD
dataFormat = DataFormat.AOD
switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce. Check here https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2#Recipe_for_regular_users_for_7_4
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff']

#add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
    
    
    
#########################
# Main analysis list
#########################
    
    
process.ana_step = cms.Path(#process.hltanalysisReco*
                            #process.hltobject *
                            process.hiEvtAnalyzer *
                            process.oniaTreeAna*
                            process.particleFlowNoHF *
                            process.pfCandComposites *
                            process.jetSequences *
                            process.HiForest
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

process.ak3PFJetAnalyzer.trackSelection = process.ak4PFSecondaryVertexTagInfos.trackSelection
process.ak3PFJetAnalyzer.trackPairV0Filter = process.ak4PFSecondaryVertexTagInfos.vertexCuts.v0Filter

process.ak4PFJetAnalyzer.trackSelection = process.ak4PFSecondaryVertexTagInfos.trackSelection
process.ak4PFJetAnalyzer.trackPairV0Filter = process.ak4PFSecondaryVertexTagInfos.vertexCuts.v0Filter
#
#process.ak5PFJetAnalyzer.trackSelection = process.ak4PFSecondaryVertexTagInfos.trackSelection
#process.ak5PFJetAnalyzer.trackPairV0Filter = process.ak4PFSecondaryVertexTagInfos.vertexCuts.v0Filter
#
#process.ak4CaloJetAnalyzer.trackSelection = process.ak4PFSecondaryVertexTagInfos.trackSelection
#process.ak4CaloJetAnalyzer.trackPairV0Filter = process.ak4PFSecondaryVertexTagInfos.vertexCuts.v0Filter        

#process.options   = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.ak3PFJetAnalyzer.jetPtMin = 0.
process.ak3PFJets.jetPtMin = 0.
process.ak4PFJetAnalyzer.jetPtMin = 0.
process.ak4PFJets.jetPtMin = 0.


##################################### trigger selection
process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
process.hltPFJet60 = process.hltHighLevel.clone()
process.hltPFJet60.HLTPaths = ["HLT_HIL1DoubleMu0_v1"]
process.superFilterSequence = cms.Sequence(process.hltPFJet60)
process.superFilterPath = cms.Path(process.superFilterSequence)

process.skimanalysis.superFilters = cms.vstring("superFilterPath")
for path in process.paths:
    getattr(process,path)._seq = process.superFilterSequence*getattr(process,path)._seq
