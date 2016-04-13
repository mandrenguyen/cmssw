### HiForest Configuration
# Collisions: PbPb
# Type: MonteCarlo
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
                            duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
                            fileNames = cms.untracked.vstring(
        #"/store/user/mnguyen//embeddedFCR/Pythia6_TuneZ2_5020GeV/Pythia6_TuneZ2_bfcr100_Hydjet_5020GeV_RECO/160310_080743/0000/step3_RAW2DIGI_L1Reco_RECO_99.root"
#'/store/user/mnguyen/embeddedQCD/Pythia6_TuneZ2_Hydjet_5020GeV/Pythia6_qcd30_TuneZ2_Hydjet_5020GeV_RECO/160207_080507/0000/step3_RAW2DIGI_L1Reco_RECO_26.root',
        'file:/tmp/mnguyen/step3_RAW2DIGI_L1Reco_RECO_26.root',
                                ),
                            #eventsToProcess = cms.untracked.VEventRange("1:13900-1:13900"),
                            #lumisToProcess = cms.untracked.VLuminosityBlockRange('1:139-1:139')
                            )

# Number of events we want to process, -1 = all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


#####################################################################################
# Load Global Tag, Geometry, etc.
#####################################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '75X_mcRun2_HeavyIon_v13', '') #for now track GT manually, since centrality tables updated ex post facto
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

from HeavyIonsAnalysis.Configuration.CommonFunctions_cff import overrideJEC_PbPb5020
process = overrideJEC_PbPb5020(process)

process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
# process.centralityBin.Centrality = cms.InputTag("hiCentrality")
# process.centralityBin.centralityVariable = cms.string("HFtowers")
#process.centralityBin.nonDefaultGlauberModel = cms.string("HydjetDrum5")

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
# full gen jets followed by filters to select signal-only genjets
process.load('HeavyIonsAnalysis.JetAnalysis.GenJetSequence')
process.load('HeavyIonsAnalysis.JetAnalysis.hiSignalGenFilters')


# nominal jet reco sequence
process.load('HeavyIonsAnalysis.JetAnalysis.FullJetSequence_ntuplizerPbPb')
process.genParticlesForJets.ignoreParticleIDs+=[12,14,16]
process.akPu3PFJetAnalyzer.matchJets = True
process.akPu4PFJetAnalyzer.matchJets = True
process.akVs3PFJetAnalyzer.matchJets = True
process.akVs4PFJetAnalyzer.matchJets = True
process.akCs3PFJetAnalyzer.matchJets = True
process.akCs4PFJetAnalyzer.matchJets = True
process.akPu3PFJetAnalyzer.matchTag = "akPu3CalopatJetsWithBtagging"
process.akPu4PFJetAnalyzer.matchTag = "akPu4CalopatJetsWithBtagging"
process.akVs3PFJetAnalyzer.matchTag = "akVs3CalopatJetsWithBtagging"
process.akVs4PFJetAnalyzer.matchTag = "akVs4CalopatJetsWithBtagging"
process.akCs3PFJetAnalyzer.matchTag = "akPu3PFpatJetsWithBtagging"
process.akCs4PFJetAnalyzer.matchTag = "akPu4PFpatJetsWithBtagging"

# replace above with this one for JEC:
#process.load('HeavyIonsAnalysis.JetAnalysis.FullJetSequence_JECPbPb')

#rho analyzer
process.load('HeavyIonsAnalysis.JetAnalysis.hiFJRhoAnalyzer_cff')

####################################################################################

#############################
# Gen Analyzer
#############################
process.load('HeavyIonsAnalysis.EventAnalysis.HiMixAnalyzerRECO_cff')
process.load('GeneratorInterface.HiGenCommon.HeavyIon_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.HiGenAnalyzer_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.runanalyzer_cff')
process.HiGenParticleAna.genParticleSrc = cms.untracked.InputTag("hiSignalGenParticles")
# Temporary disactivation - until we have DIGI & RECO in CMSSW_7_5_7_patch4
process.HiGenParticleAna.doHI = False

process.bParticleAna = process.HiGenParticleAna.clone(
    ptMin = 0.,
    stableOnly = False,
    bOnly = True,
    etaMax =999.,
    saveMothers = True,
    saveDaughters = True
)



#####################################################################################

############################
# Event Analysis
############################
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_mc_cfi')
process.hiEvtAnalyzer.doMC = cms.bool(True) #general MC info
process.hiEvtAnalyzer.doHiMC = cms.bool(True) #HI specific MC info
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cff')
process.load("HeavyIonsAnalysis.JetAnalysis.pfcandAnalyzer_cfi")
process.pfcandAnalyzer.skipCharged = False
process.pfcandAnalyzer.pfPtMin = 5.
process.load("HeavyIonsAnalysis.JetAnalysis.pfcandAnalyzerCS_cfi")
process.pfcandAnalyzerCS.skipCharged = False
process.pfcandAnalyzerCS.pfPtMin = 5.

#####################################################################################

#########################
# Track Analyzer
#########################
process.load('HeavyIonsAnalysis.JetAnalysis.ExtraTrackReco_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.TrkAnalyzers_cff')

# Use this instead for track corrections
## process.load('HeavyIonsAnalysis.JetAnalysis.TrkAnalyzers_Corr_cff')

#####################################################################################

#####################
# Photons
#####################

#process.load('HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi')
#process.ggHiNtuplizerGED = process.ggHiNtuplizer.clone(recoPhotonSrc = cms.InputTag('gedPhotonsTmp'),
#                                                       recoPhotonHiIsolationMap = cms.InputTag('photonIsolationHIProducerGED')
#)

#####################################################################################


#####################
# tupel and necessary PAT sequences
#####################

#process.load("HeavyIonsAnalysis.VectorBosonAnalysis.tupelSequence_PbPb_mc_cff")

#####################################################################################

#########################
# Main analysis list
#########################


process.ana_step = cms.Path(
# Temporary disactivation - until we have DIGI & RECO in CMSSW_7_5_7_patch4
# process.mixAnalyzer *
                            process.runAnalyzer *
                            process.hltanalysis *
                            process.centralityBin *
                            process.hiEvtAnalyzer*
                            process.akHiGenJets +
                            process.hiSignalGenFilters + 
                            process.jetSequences +
                            process.hiFJRhoAnalyzer +
                            #process.ggHiNtuplizer +
                            #process.ggHiNtuplizerGED +
                            process.pfcandAnalyzer +
                            #process.pfcandAnalyzerCS +
                            process.HiGenParticleAna*
                            process.bParticleAna*
                            process.HiForest #+
                            #process.trackSequencesPbPb #+
                            #process.tupelPatSequence
                            )

#####################################################################################

#########################
# Event Selection
#########################

process.load('HeavyIonsAnalysis.JetAnalysis.EventSelection_cff')
process.pcollisionEventSelection = cms.Path(process.collisionEventSelectionAOD)
process.pHBHENoiseFilterResultProducer = cms.Path( process.HBHENoiseFilterResultProducer )
process.HBHENoiseFilterResult = cms.Path(process.fHBHENoiseFilterResult)
process.HBHENoiseFilterResultRun1 = cms.Path(process.fHBHENoiseFilterResultRun1)
process.HBHENoiseFilterResultRun2Loose = cms.Path(process.fHBHENoiseFilterResultRun2Loose)
process.HBHENoiseFilterResultRun2Tight = cms.Path(process.fHBHENoiseFilterResultRun2Tight)
process.HBHEIsoNoiseFilterResult = cms.Path(process.fHBHEIsoNoiseFilterResult)
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter )

process.load('HeavyIonsAnalysis.Configuration.hfCoincFilter_cff')
process.phfCoincFilter1 = cms.Path(process.hfCoincFilter)
process.phfCoincFilter2 = cms.Path(process.hfCoincFilter2)
process.phfCoincFilter3 = cms.Path(process.hfCoincFilter3)
process.phfCoincFilter4 = cms.Path(process.hfCoincFilter4)
process.phfCoincFilter5 = cms.Path(process.hfCoincFilter5)

process.pclusterCompatibilityFilter = cms.Path(process.clusterCompatibilityFilter)

process.pAna = cms.EndPath(process.skimanalysis)

# Customization
##########################################UE##########################################
from CondCore.DBCommon.CondDBSetup_cfi import *
process.uetable = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(0)
        ),
      timetype = cms.string('runnumber'),
      toGet = cms.VPSet(
          cms.PSet(record = cms.string("JetCorrectionsRecord"),
                   tag = cms.string("UETableCompatibilityFormat_PF_HYDJET_5020GeV_754_38T_v02_mc"),
                   label = cms.untracked.string("UETable_PF")
          ),
          cms.PSet(record = cms.string("JetCorrectionsRecord"),
                   tag = cms.string("UETableCompatibilityFormat_Calo_HYDJET_5020GeV_754_38T_v02_mc"),
                   label = cms.untracked.string("UETable_Calo")
          )
      ),
      connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
)
process.es_prefer_uetable = cms.ESPrefer('PoolDBESSource','uetable')
##########################################UE##########################################

process.akCs3PFmatch.resolveByMatchQuality = True
process.akCs4PFmatch.resolveByMatchQuality = True
process.akVs3PFmatch.resolveByMatchQuality = True
process.akVs4PFmatch.resolveByMatchQuality = True
