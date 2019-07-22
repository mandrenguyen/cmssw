### HiForest Configuration
# Collisions: PbPb
# Type: Embedded Monte Carlo
# Input: AOD

import FWCore.ParameterSet.Config as cms
process = cms.Process('HiForest')

###############################################################################
# HiForest labelling info
###############################################################################

process.load("HeavyIonsAnalysis.JetAnalysis.HiForest_cff")
#print "Comment this back !!!! "
process.HiForest.inputLines = cms.vstring("HiForest 103X")
import subprocess, os
version = subprocess.check_output(['git',
    '-C', os.path.expandvars('$CMSSW_BASE/src'), 'describe', '--tags'])
if version == '':
    version = 'no git info'
process.HiForest.HiForestVersion = cms.string(version)

###############################################################################
# Input source
###############################################################################

process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(
        "/store/user/anstahll/Dilepton/MC/Embedded/JPsiMM_5p02TeV_TuneCP5_Embd_RECO_20190326/JPsiMM_5p02TeV_TuneCP5_Embd/JPsiMM_5p02TeV_TuneCP5_Embd_RECO_20190326/190327_060527/0004/HIN-HINPbPbAutumn18DRHIMix-00008_step2_4159.root",
        ),
    )

# Number of events we want to process, -1 = all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
    )

###############################################################################
# Load Global Tag, Geometry, etc.
###############################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2018_realistic_hi', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '103X_upgrade2018_realistic_HI_v11', '') 
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

print('\n\033[31m~*~ USING CENTRALITY TABLE FOR Hydjet Drum5F ~*~\033[0m\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_HydjetDrum5F_v1032x01_mc"),
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        label = cms.untracked.string("HFtowers")
        ),
    ])

from HeavyIonsAnalysis.Configuration.CommonFunctions_cff import overrideJEC_PbPb5020
process = overrideJEC_PbPb5020(process)

process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")

###############################################################################
# Define tree output
###############################################################################

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestAOD.root"))

###############################################################################
# Additional Reconstruction and Analysis: Main Body
###############################################################################

#############################
# Jets
#############################
# jet reco sequence
process.load('HeavyIonsAnalysis.JetAnalysis.fullJetSequence_pponAA_MIX_cff')
# replace above with this one for JEC:
# process.load('HeavyIonsAnalysis.JetAnalysis.fullJetSequence_JEC_cff')

# temporary
process.akPu4Calocorr.payload = "AK4Calo"
process.akPu4PFcorr.payload = "AK4PF"
process.akCs4PFcorr.payload = "AK4PF"
process.akFlowPuCs4PFcorr.payload = "AK4PF"
process.akPu4PFJets.jetPtMin = 1

process.akPu3Calocorr.payload = "AK3Calo"
process.akPu3PFcorr.payload = "AK3PF"
process.akCs3PFcorr.payload = "AK3PF"
process.akFlowPuCs3PFcorr.payload = "AK3PF"
process.akPu3PFJets.jetPtMin = 1

process.load('HeavyIonsAnalysis.JetAnalysis.hiFJRhoAnalyzer_cff')
process.load("HeavyIonsAnalysis.JetAnalysis.pfcandAnalyzer_cfi")
process.pfcandAnalyzer.doTrackMatching  = cms.bool(True)

###############################################################################

process.hiPuRhoR3Analyzer = process.hiFJRhoAnalyzer.clone(etaMap = cms.InputTag('hiPuRhoR3Producer','mapEtaEdges','HiForest'),
                                                          rho = cms.InputTag('hiPuRhoR3Producer','mapToRho'),
                                                          rhoExtra = cms.InputTag('hiPuRhoR3Producer','mapToRhoExtra'),
                                                          rhom = cms.InputTag('hiPuRhoR3Producer','mapToRhoM'),
                                                          rhoCorr = cms.InputTag('hiPuRhoR3Producer','mapToRhoMedian'),
                                                          rhomCorr = cms.InputTag('hiPuRhoR3Producer','mapToRhoM'),
                                                          rhoCorr1Bin = cms.InputTag('hiPuRhoR3Producer','mapToRho'),
                                                          rhomCorr1Bin = cms.InputTag('hiPuRhoR3Producer','mapToRhoM'),
                                                          nTow = cms.InputTag('hiPuRhoR3Producer','mapToNTow'),
                                                          towExcludePt = cms.InputTag('hiPuRhoR3Producer','mapToTowExcludePt'),
                                                          towExcludePhi = cms.InputTag('hiPuRhoR3Producer','mapToTowExcludePhi'),
                                                          towExcludeEta = cms.InputTag('hiPuRhoR3Producer','mapToTowExcludeEta'),
                                                          rhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculator','mapRhoVsEtaGrid'),
                                                          meanRhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculator','mapMeanRhoVsEtaGrid'),
                                                          etaMaxRhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculator','mapEtaMaxGrid'),
                                                          etaMinRhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculator','mapEtaMinGrid'),
                                                          rhoFlowFitParams = cms.InputTag('hiFJRhoFlowModulationProducer','rhoFlowFitParams'),
                                                          ptJets = cms.InputTag('hiPuRhoR3Producer', 'ptJets'),
                                                          etaJets = cms.InputTag('hiPuRhoR3Producer', 'etaJets'),
                                                          areaJets = cms.InputTag('hiPuRhoR3Producer', 'areaJets'),
                                                          )



#############################
# Gen Analyzer
#############################
process.load('HeavyIonsAnalysis.EventAnalysis.runanalyzer_cfi')
process.load('HeavyIonsAnalysis.TrackAnalysis.HiGenAnalyzer_cfi')
# making cuts looser so that we can actually check dNdEta
process.HiGenParticleAna.ptMin = cms.untracked.double(0.4) # default is 5
process.HiGenParticleAna.etaMax = cms.untracked.double(5.) # default is 2

###############################################################################

############################
# Event Analysis
############################
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_mc_cfi')
process.hiEvtAnalyzer.doMC = cms.bool(True) # general MC info
process.hiEvtAnalyzer.doHiMC = cms.bool(True) # HI specific MC info
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')

###############################################################################

#########################
# Track Analyzer
#########################
process.load('HeavyIonsAnalysis.TrackAnalysis.ExtraTrackReco_cff')
process.load('HeavyIonsAnalysis.TrackAnalysis.TrkAnalyzers_cff')

# Use this instead for track corrections
# process.load('HeavyIonsAnalysis.TrackAnalysis.TrkAnalyzers_Corr_cff')

###############################################################################

#####################
# Photons
#####################
process.load('HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi')

###############################################################################

#######################
# B-tagging
######################
# replace pp CSVv2 with PbPb CSVv2 (positive and negative taggers unchanged!)
process.load('RecoBTag.CSVscikit.csvscikitTagJetTags_cfi')
process.load('RecoBTag.CSVscikit.csvscikitTaggerProducer_cfi')
process.akPu4PFCombinedSecondaryVertexV2BJetTags = process.pfCSVscikitJetTags.clone()
process.akPu4PFCombinedSecondaryVertexV2BJetTags.tagInfos = cms.VInputTag(
    cms.InputTag("akPu4PFImpactParameterTagInfos"),
    cms.InputTag("akPu4PFSecondaryVertexTagInfos"))
process.akCs4PFCombinedSecondaryVertexV2BJetTags = process.pfCSVscikitJetTags.clone()
process.akCs4PFCombinedSecondaryVertexV2BJetTags.tagInfos = cms.VInputTag(
    cms.InputTag("akCs4PFImpactParameterTagInfos"),
    cms.InputTag("akCs4PFSecondaryVertexTagInfos"))
process.akPu4CaloCombinedSecondaryVertexV2BJetTags = process.pfCSVscikitJetTags.clone()
process.akPu4CaloCombinedSecondaryVertexV2BJetTags.tagInfos = cms.VInputTag(
    cms.InputTag("akPu4CaloImpactParameterTagInfos"),
    cms.InputTag("akPu4CaloSecondaryVertexTagInfos"))

# trained on CS jets
process.CSVscikitTags.weightFile = cms.FileInPath(
    'HeavyIonsAnalysis/JetAnalysis/data/TMVA_Btag_CsJets_PbPb_BDTG.weights.xml')

###############################################################################

#########################
# RecHits & pfTowers (HF, Castor & ZDC)
#########################
process.load('HeavyIonsAnalysis.JetAnalysis.rechitanalyzer_cfi')

###############################################################################
#Recover peripheral primary vertices
#https://twiki.cern.ch/twiki/bin/view/CMS/HITracking2018PbPb#Peripheral%20Vertex%20Recovery
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVerticesRecovery_cfi")


## onia + jet stuff

process.particleFlowNoHF = cms.EDFilter("PdgIdPFCandidateSelector",
                                        src = cms.InputTag("particleFlow"),
                                        pdgId = cms.vint32(211,-211,11,13,-11,-13,22,130)
                                    )

process.load("RecoHI.HiJetAlgos.PFCandCompositeProducer_cfi")
process.pfCandComposites.pfCandTag    = cms.InputTag('particleFlowNoHF')
process.pfCandComposites.replaceJMM = True
process.pfCandComposites.compositeTag = cms.InputTag("onia2MuMuPatGlbGlb")




from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
#oniaTreeAnalyzer(process, isPbPb=False, isMC=True, applyEventSel=False, muonSelection="Glb", outputFileName="HiForestAOD.root")
#oniaTreeAnalyzer(process, muonTriggerList=[[],[],[],[]], HLTProName='HLT', muonSelection="Trk", useL1Stage2=True, isMC=True, pdgID=443, outputFileName="OniaTree.root", doTrimu=False)
oniaTreeAnalyzer(process, muonSelection="Glb", isMC=True, outputFileName="HiForestAOD.root")

process.pfcandAnalyzer.pfCandidateLabel = 'pfCandJPsi'


#########################
# Main analysis list
#########################

process.ana_step = cms.Path(
    process.offlinePrimaryVerticesRecovery +
    process.HiForest +
    process.runAnalyzer +
    process.hltanalysis +
    process.centralityBin +
    process.hiEvtAnalyzer +
#    process.HiGenParticleAna +
    process.genSignalSequence +
    process.oniaTreeAna +
    process.particleFlowNoHF +
    process.pfCandComposites +
    process.jetSequence +
#    process.ggHiNtuplizer +
#    process.ggHiNtuplizerGED +
#    process.hiFJRhoAnalyzer +
    process.pfcandAnalyzer +
    process.pfcandAnalyzerCS +
    process.trackSequencesPP #+
#    process.rechitanalyzerpp
    )

## edm output for debugging purposes
#process.output = cms.OutputModule(
#    "PoolOutputModule",
#    fileName = cms.untracked.string('HiForestEDM.root'),
#    outputCommands = cms.untracked.vstring(
#         'keep *',
#        # drop aliased products
#        'drop *_akULPu3PFJets_*_*',
#    'drop *_akULPu4PFJets_*_*',
#    )
#)
#
#process.output_path = cms.EndPath(process.output)

###############################################################################

#########################
# Event Selection
#########################

process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
process.pclusterCompatibilityFilter = cms.Path(process.clusterCompatibilityFilter)
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)
process.pBeamScrapingFilter = cms.Path(process.beamScrapingFilter)
process.collisionEventSelectionAOD = cms.Path(process.collisionEventSelectionAOD)
process.collisionEventSelectionAODv2 = cms.Path(process.collisionEventSelectionAODv2)

process.load('HeavyIonsAnalysis.Configuration.hfCoincFilter_cff')
process.phfCoincFilter1Th3 = cms.Path(process.hfCoincFilterTh3)
process.phfCoincFilter2Th3 = cms.Path(process.hfCoincFilter2Th3)
process.phfCoincFilter3Th3 = cms.Path(process.hfCoincFilter3Th3)
process.phfCoincFilter4Th3 = cms.Path(process.hfCoincFilter4Th3)
process.phfCoincFilter5Th3 = cms.Path(process.hfCoincFilter5Th3)
process.phfCoincFilter1Th4 = cms.Path(process.hfCoincFilterTh4)
process.phfCoincFilter2Th4 = cms.Path(process.hfCoincFilter2Th4)
process.phfCoincFilter3Th4 = cms.Path(process.hfCoincFilter3Th4)
process.phfCoincFilter4Th4 = cms.Path(process.hfCoincFilter4Th4)
process.phfCoincFilter5Th4 = cms.Path(process.hfCoincFilter5Th4)
process.phfCoincFilter1Th5 = cms.Path(process.hfCoincFilterTh5)
process.phfCoincFilter4Th2 = cms.Path(process.hfCoincFilter4Th2)

process.load("HeavyIonsAnalysis.VertexAnalysis.PAPileUpVertexFilter_cff")
process.pVertexFilterCutG = cms.Path(process.pileupVertexFilterCutG)
process.pVertexFilterCutGloose = cms.Path(process.pileupVertexFilterCutGloose)
process.pVertexFilterCutGtight = cms.Path(process.pileupVertexFilterCutGtight)
process.pVertexFilterCutGplus = cms.Path(process.pileupVertexFilterCutGplus)
process.pVertexFilterCutE = cms.Path(process.pileupVertexFilterCutE)
process.pVertexFilterCutEandG = cms.Path(process.pileupVertexFilterCutEandG)

process.load('HeavyIonsAnalysis.JetAnalysis.EventSelection_cff')
process.pHBHENoiseFilterResultProducer = cms.Path(process.HBHENoiseFilterResultProducer)
process.HBHENoiseFilterResult = cms.Path(process.fHBHENoiseFilterResult)
process.HBHENoiseFilterResultRun1 = cms.Path(process.fHBHENoiseFilterResultRun1)
process.HBHENoiseFilterResultRun2Loose = cms.Path(process.fHBHENoiseFilterResultRun2Loose)
process.HBHENoiseFilterResultRun2Tight = cms.Path(process.fHBHENoiseFilterResultRun2Tight)
process.HBHEIsoNoiseFilterResult = cms.Path(process.fHBHEIsoNoiseFilterResult)

process.pAna = cms.EndPath(process.skimanalysis)

from HLTrigger.Configuration.CustomConfigs import MassReplaceInputTag
process = MassReplaceInputTag(process,"offlinePrimaryVertices","offlinePrimaryVerticesRecovery")
process.offlinePrimaryVerticesRecovery.oldVertexLabel = "offlinePrimaryVertices"
# Customization
###############################################################################
#process.akCs4PFJets.src = 'pfCandComposites'
process.genParticlesForJets.storeJMM = cms.untracked.bool(True)
