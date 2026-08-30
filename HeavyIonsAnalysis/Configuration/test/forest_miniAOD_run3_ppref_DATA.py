### HiForest Configuration
# Collisions: pp
# Type: Data
# Input: miniAOD

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_2024_ppRef_cff import Run3_2024_ppRef
process = cms.Process('HiForest', Run3_2024_ppRef)
process.options = cms.untracked.PSet()

#####################################################################################
# HiForest labelling info
#####################################################################################

process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 141X, data")

#####################################################################################
# Input source
#####################################################################################

process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(
        '/store/data/Run2025G/Muon0/MINIAOD/PromptReco-v1/000/397/954/00000/0afd6830-f9ab-468d-bec4-7e135dfca49d.root'
    )
)

# Number of events we want to process, -1 = all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(2000)
)

#####################################################################################
# Load Global Tag, Geometry, etc.
#####################################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

#GlobalTag used in Prompt RECO
#https://cms-conddb.cern.ch/cmsDbBrowser/list/Prod/gts/132X_dataRun3_Prompt_v3
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '141X_dataRun3_Prompt_v3', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag

# FIXME: Old calibration here, might need to update
# Commenting out until understood
#process.GlobalTag.toGet.extend([
#    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
#             tag = cms.string("JPcalib_MC94X_2017pp_v2"),
#             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
#
#         )
#      ])

#####################################################################################
# Define tree output
#####################################################################################

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestMiniAOD_OniaJet.root"))

#####################################################################################
# Additional Reconstruction and Analysis: Main Body
#####################################################################################

#############################
# Jets
#############################
process.load("HeavyIonsAnalysis.JetAnalysis.ak4PFJetSequence_ppref_data_cff")
#####################################################################################

############################
# Event Analysis
############################
# use data version to avoid PbPb MC
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.hiEvtAnalyzer.Vertex = cms.InputTag("offlineSlimmedPrimaryVertices")
process.hiEvtAnalyzer.doCentrality = cms.bool(False)
process.hiEvtAnalyzer.doEvtPlane = cms.bool(False)
process.hiEvtAnalyzer.doEvtPlaneFlat = cms.bool(False)
process.hiEvtAnalyzer.doMET = cms.bool(False)   ## Missing Et
#Turn off MC info
process.hiEvtAnalyzer.doMC = cms.bool(False) # general MC info
process.hiEvtAnalyzer.doHiMC = cms.bool(False) # HI specific MC info
process.hiEvtAnalyzer.doHFfilters = cms.bool(False) # Disable HF filters for ppRef

process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')

process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')

# FIXME: Do we have an updated trigger list?
#from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_data_2023_skimmed
#process.hltobject.triggerNames = trigger_list_data_2023_skimmed

#####################################################################################

#########################
# Track Analyzer
#########################
process.load('HeavyIonsAnalysis.TrackAnalysis.TrackAnalyzers_cff')

#####################################################################################

#####################
# photons
######################
process.load('HeavyIonsAnalysis.EGMAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizer.doGenParticles = cms.bool(False)
process.ggHiNtuplizer.doMuons = cms.bool(False) # unpackedMuons collection not found from file
process.ggHiNtuplizer.useValMapIso = cms.bool(False) # True here causes seg fault
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

####################################################################################

# muons
process.load("HeavyIonsAnalysis.MuonAnalysis.unpackedMuons_cfi")
process.unpackedMuons.muonSelectors = cms.vstring()
process.load("HeavyIonsAnalysis.MuonAnalysis.muonAnalyzer_cfi")

#####################
# Onia reconstruction and ntuple
#####################
# Select the reconstructed onium species used throughout this workflow.
# Supported values are "JPsi" and "Y" (Upsilon(1S)).
oniaMode = "Y"
# Candidate quality used for the jet-input replacement.  "Soft" reproduces
# the historical hybrid-soft choice; "Tight" uses CutBasedIdTight.
oniaMuonID = "Tight"
applyOniaMuonAcceptance = True
if oniaMode == "JPsi":
    oniaPdgId = 443
    oniaDimuonSelection = "2.6 < mass && mass < 4.0 && charge == 0"
    oniaMass = 3.096900
    assignClosestOniaMass = True
elif oniaMode == "Y":
    oniaPdgId = 553
    oniaDimuonSelection = "8.0 < mass && mass < 14.0 && charge == 0"
    oniaMass = 9.460300
    assignClosestOniaMass = False
else:
    raise ValueError('oniaMode must be either "JPsi" or "Y"')

oniaTriggerList = {
    "DoubleMuonTrigger": cms.vstring(
        "HLT_PPRefL1DoubleMu0_Open_v",
        "HLT_PPRefL1DoubleMu0_v",
        "HLT_PPRefL1DoubleMu0_SQ_v",
        "HLT_PPRefL1DoubleMu2_v",
        "HLT_PPRefL1DoubleMu2_SQ_v",
        "HLT_PPRefL2DoubleMu0_Open_v",
        "HLT_PPRefL2DoubleMu0_v",
        "HLT_PPRefL3DoubleMu0_Open_v",
        "HLT_PPRefL3DoubleMu0_v",
    ),
    "SingleMuonTrigger": cms.vstring(
        "HLT_PPRefL1SingleMu7_v",
        "HLT_PPRefL1SingleMu12_v",
        "HLT_PPRefL2SingleMu7_v",
        "HLT_PPRefL2SingleMu12_v",
        "HLT_PPRefL2SingleMu15_v",
        "HLT_PPRefL3SingleMu3_v",
        "HLT_PPRefL3SingleMu5_v",
        "HLT_PPRefL3SingleMu7_v",
        "HLT_PPRefL3SingleMu12_v",
        "HLT_PPRefL3SingleMu15_v",
        "HLT_PPRefL3SingleMu20_v",
    ),
}

from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(
    process,
    muonTriggerList=oniaTriggerList,
    HLTProName="HLT",
    muonSelection="Glb",
    L1Stage=2,
    isMC=False,
    pdgID=oniaPdgId,
    outputFileName="HiForestMiniAOD_OniaJet.root",
)

process.onia2MuMuPatGlbGlb.dimuonSelection = cms.string(
    oniaDimuonSelection
)
# If True, write only events where an injected reconstructed onium is present
# in the particle collection passed to jet clustering.
doOniaEventFilter = True
process.hionia.SumETvariables = cms.bool(False)
process.hionia.checkTrigNames = cms.bool(False)
process.hionia.isHI = cms.untracked.bool(False)
process.hionia.mom4format = cms.string("array")

from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import changeToMiniAOD
changeToMiniAOD(process)
process.unpackedMuons.addPropToMuonSt = cms.bool(True)
process.hionia.primaryVertexTag = cms.InputTag("unpackedTracksAndVertices")

# Build the actual jet-input collection first, then require that it contains
# an inserted onium pseudo-particle.  pdgId 1 is the marker used by the Onia
# PF-candidate producer, while the mass identifies the selected onium mode.
process.load("HeavyIonsAnalysis.JetAnalysis.oniaPFCandidateProducer_cfi")
process.oniaPFCandidates.massOnia = oniaMass
process.oniaPFCandidates.assignClosestOniaMass = assignClosestOniaMass
process.oniaPFCandidates.checkOnia = True
process.oniaPFCandidates.oniaMuonID = oniaMuonID
process.oniaPFCandidates.applyMuonAcceptance = applyOniaMuonAcceptance
process.particleFlowAnalyser.pfCandidateSrc = cms.InputTag("oniaPFCandidates")
process.particleFlowAnalyser.chargedOnly = True
process.particleFlowAnalyser.ptMin = 0.5

process.oniaJetInputCandidates = cms.EDFilter(
    "CandViewSelector",
    src=cms.InputTag("oniaPFCandidates"),
    cut=cms.string("pdgId == 1 && abs(mass - %.6f) < 0.001" % oniaMass),
    filter=cms.bool(False),
)
process.oniaJetInputFilter = cms.EDFilter(
    "CandViewCountFilter",
    src=cms.InputTag("oniaJetInputCandidates"),
    minNumber=cms.uint32(1),
)
process.oniaJetInputPreparation = cms.Sequence(
    process.patMuonSequence
    * process.onia2MuMuPatGlbGlb
    * process.oniaPFCandidates
)
if doOniaEventFilter:
    process.oniaEventSelection = cms.Sequence(
        process.oniaJetInputPreparation
        * process.oniaJetInputCandidates
        * process.oniaJetInputFilter
    )
else:
    process.oniaEventSelection = cms.Sequence(process.oniaJetInputPreparation)

#########################
# ZDC RecHit Producer && Analyzer
#########################
# to prevent crash related to HcalSeverityLevelComputerRcd record
process.load("RecoLocalCalo.HcalRecAlgos.hcalRecAlgoESProd_cfi")
process.load('HeavyIonsAnalysis.ZDCAnalysis.ZDCAnalyzersPP_cff')

###############################################################################

#########################
# Main analysis list
#########################

process.forest = cms.Path(
    process.oniaEventSelection +
    process.HiForestInfo +
    process.hltanalysis *
    process.hiEvtAnalyzer *
#    process.hltobject +
    #process.l1object +
    #process.ggHiNtuplizer +
    process.unpackedTracksAndVertices +
    #process.trackSequencePP +
    process.particleFlowAnalyser +
    process.hionia #+
    #process.muonAnalyzer
)


# Schedule definition
process.pAna = cms.EndPath(process.skimanalysis)

process.primaryVertexFilter = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlineSlimmedPrimaryVertices"),
    cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2"), #in miniADO trackSize()==0, however there is no influence.
    filter = cms.bool(True), # otherwise it won't filter the event
)
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)

#####################################################################################
# Replace each selected dimuon's two packed-PF muons with one onium candidate,
# then recluster anti-kT R=0.4 jets from the modified collection.
from HeavyIonsAnalysis.JetAnalysis.clusterJetsFromMiniAOD_cff import setupPprefJets
process.oniaJetSequence = cms.Sequence()
setupPprefJets(
    "ak4OniaPF",
    process.oniaJetSequence,
    process,
    isMC=False,
    radius=0.4,
    JECTag="AK4PF",
)
process.ak4OniaPFJets.src = cms.InputTag("oniaPFCandidates")
process.ak4OniaPFJets.jetPtMin = 0.0
process.ak4OniaPFpatJetCorrFactors.levels = ["L2Relative", "L3Absolute"]
process.ak4OniaPFpatJetCorrFactors.primaryVertices = "offlineSlimmedPrimaryVertices"

# setupPprefJets currently schedules its b-tagging chain unconditionally.
# These products are not requested here and JetProbability relies on an old
# EventSetup calibration, so keep only jet clustering, corrections, and PAT.
for module in (
    process.ak4OniaPFpfImpactParameterTagInfos,
    process.ak4OniaPFpfSecondaryVertexTagInfos,
    process.ak4OniaPFpfDeepCSVTagInfos,
    process.ak4OniaPFpfDeepCSVJetTags,
    process.ak4OniaPFpfJetProbabilityBJetTags,
):
    process.oniaJetSequence.remove(module)

process.ak4OniaPFJetAnalyzer = process.ak4PFJetAnalyzer.clone(
    jetTag="ak4OniaPFpatJetsWithOnia",
    jetName="ak4OniaPF",
    matchJets=False,
    doBtagging=False,
    doHiJetID=True,
    doWTARecluster=False,
    doSubEvent=False,
    jetPtMin=0,
    jetAbsEtaMax=cms.untracked.double(2.5),
    rParam=0.4,
)

process.load("HeavyIonsAnalysis.JetAnalysis.dynGroomedPatJets_cfi")
process.dynGroomedPatJets.isMC = False
process.dynGroomedPatJets.chargedOnly = False
process.dynGroomedPatJets.jetSrc = "ak4OniaPFpatJetsWithOnia"
process.dynGroomedPatJets.constitSrc = "oniaPFCandidates"
process.ak4OniaPFJetAnalyzer.doSubJets = True
process.ak4OniaPFJetAnalyzer.subjetTag = cms.untracked.InputTag("dynGroomedPatJets")
process.ak4OniaPFpatJetsWithOnia = cms.EDFilter(
    "OniaPatJetSelector",
    src=cms.InputTag("ak4OniaPFpatJets"),
    cut=cms.string("pt > 0.0 && abs(rapidity()) < 3.0"),
    oniaMass=cms.double(oniaMass),
    oniaMassTolerance=cms.double(0.001),
)
process.forest += (
    process.oniaJetSequence
    + process.ak4OniaPFpatJetsWithOnia
    + process.dynGroomedPatJets
    + process.ak4OniaPFJetAnalyzer
)

# Exclude the standalone filtered path created by the Onia helper.  The Onia
# producer and analyzer are part of the combined forest path above.
process.schedule = cms.Schedule(
    process.pprimaryVertexFilter,
    process.forest,
)
