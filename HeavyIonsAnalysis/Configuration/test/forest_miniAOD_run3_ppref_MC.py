### HiForest Configuration
# Collisions: pp
# Type: MC
# Input: miniAOD

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_2024_ppRef_cff import Run3_2024_ppRef
process = cms.Process('HiForest', Run3_2024_ppRef)
process.options = cms.untracked.PSet()

#####################################################################################
# HiForest labelling info
#####################################################################################

process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 141X, mc")

#####################################################################################
# Input source
#####################################################################################

process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(
#        '/store/user/bharikri/Run3MC_pp/MINIAOD/2024_Mar_21_Pythia8_ppRef_QCDPhoton30_PU10_TuneCP5_14_0_0_LLR/Pythia8_ppRef_QCDPhoton30_TuneCP5/2024_Mar_21_step3_RAW2DIGI_MINIAODSIM_Pythia8_ppRef_QCDPhoton30_PU10_TuneCP5_14_0_0/240326_082338/0000/step3_pp_673.root'
        '/store/mc/RunIIIpp5p36Winter24MiniAOD/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/MINIAODSIM/141X_mcRun3_2024_realistic_ppRef5TeV_v7-v2/140000/e391a0bf-69c8-4630-a51e-eb3bcf042365.root',
    )
)

# Number of events we want to process, -1 = all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

#####################################################################################
# Load Global Tag, Geometry, etc.
#####################################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

# TODO: Global tag complete guess from the list. Probably wrong. But does not crash
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '141X_mcRun3_2024_realistic_ppRef5TeV_v7', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag

# TODO: Old calibration here, might need to update
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC94X_2017pp_v2"),
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")

         )
      ])

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
process.load("HeavyIonsAnalysis.JetAnalysis.ak4PFJetSequence_ppref_mc_cff")
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
process.hiEvtAnalyzer.doMC = cms.bool(True) # general MC info
process.hiEvtAnalyzer.doHiMC = cms.bool(False) # HI specific MC info
process.hiEvtAnalyzer.doHFfilters = cms.bool(False) # Disable HF filters for ppRef

process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')

# TODO: Many of these triggers are not available in the test file
from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_mc
process.hltobject.triggerNames = trigger_list_mc

# Gen particles
process.load('HeavyIonsAnalysis.EventAnalysis.HiGenAnalyzer_cfi')

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
process.ggHiNtuplizer.doGenParticles = cms.bool(True)
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
# Select the reconstructed and generated onium species used throughout this
# workflow.  Supported values are "JPsi" and "Y" (Upsilon(1S)).
oniaMode = "JPsi"
# Candidate quality used for the jet-input replacement.  "Soft" reproduces
# the historical hybrid-soft choice; "Tight" uses CutBasedIdTight.
oniaMuonID = "Soft"
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
# If True, write only events containing at least one reconstructed onium
# candidate.  Set False to retain every processed event.
doOniaEventFilter = True
oniaTreeAnalyzer(
    process,
    muonTriggerList=oniaTriggerList,
    HLTProName="HLT",
    muonSelection="All",
    L1Stage=2,
    isMC=True,
    pdgID=oniaPdgId,
    outputFileName="HiForestMiniAOD_OniaJet.root",
)

process.onia2MuMuPatGlbGlb.dimuonSelection = cms.string(
    oniaDimuonSelection
)

process.hionia.SumETvariables = cms.bool(False)
process.hionia.checkTrigNames = cms.bool(False)
process.hionia.isHI = cms.untracked.bool(False)
process.hionia.genealogyInfo = cms.bool(True)
process.hionia.mom4format = cms.string("array")

from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import changeToMiniAOD
changeToMiniAOD(process)
process.unpackedMuons.addPropToMuonSt = cms.bool(True)

# Construct the same muon-replaced PF jet input used for data.  The nominal
# mass test identifies the injected onium, not the reconstructed dimuon mass,
# whose accepted window remains broad above.
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
# Main analysis list
#########################

process.forest = cms.Path(
    process.oniaEventSelection +
    process.HiForestInfo +
    process.hltanalysis *
    process.hiEvtAnalyzer *
    process.hltobject +
    #process.l1object +
    process.HiGenParticleAna +
    #process.ggHiNtuplizer +
    #process.trackSequencePP +
    process.particleFlowAnalyser +
    process.hionia #+
    #process.muonAnalyzer
)

# Recluster generator jets after replacing the stable decay products of each
# generated onium -> mu+mu- with the corresponding undecayed resonance.
from GeneratorInterface.RivetInterface.mergedGenParticles_cfi import mergedGenParticles
process.mergedGenParticles = mergedGenParticles.clone()
from HeavyIonsAnalysis.JetAnalysis.oniaGenJetParticleSelector_cfi import oniaGenJetParticles
process.packedGenParticlesForJetsNoNu = oniaGenJetParticles.clone(
    src="mergedGenParticles",
    resonancePdgIds=[oniaPdgId],
)

# Recluster anti-kT R=0.4 reco jets from the muon-replaced PF collection and
# match them to the corresponding Onia-aware generator jets.
from HeavyIonsAnalysis.JetAnalysis.clusterJetsFromMiniAOD_cff import setupPprefJets
process.oniaJetSequence = cms.Sequence()
setupPprefJets(
    "ak4OniaPF",
    process.oniaJetSequence,
    process,
    isMC=True,
    radius=0.4,
    JECTag="AK4PF",
)
process.ak4OniaPFJets.src = "oniaPFCandidates"
process.ak4OniaPFJets.jetPtMin = 0.0
process.ak4GenJetsNoNu.src = "packedGenParticlesForJetsNoNu"
process.ak4OniaPFpatJetCorrFactors.levels = ["L2Relative", "L3Absolute"]
process.ak4OniaPFpatJetCorrFactors.primaryVertices = "offlineSlimmedPrimaryVertices"

# No b tagging or parton-flavour products are needed for this Onia-jet tree.
for module in (
    process.ak4OniaPFpatJetPartonMatch,
    process.ak4OniaPFpatJetPartonAssociationLegacy,
    process.ak4OniaPFpatJetFlavourAssociationLegacy,
    process.ak4OniaPFpatJetPartons,
    process.ak4OniaPFpfImpactParameterTagInfos,
    process.ak4OniaPFpfSecondaryVertexTagInfos,
    process.ak4OniaPFpfDeepCSVTagInfos,
    process.ak4OniaPFpfDeepCSVJetTags,
    process.ak4OniaPFpfJetProbabilityBJetTags,
):
    process.oniaJetSequence.remove(module)
process.ak4OniaPFpatJets.addGenPartonMatch = False
process.ak4OniaPFpatJets.embedGenPartonMatch = False
process.ak4OniaPFpatJets.getJetMCFlavour = False
process.ak4OniaPFpatJets.addJetFlavourInfo = False
process.ak4OniaPFpatJets.addBTagInfo = False

process.ak4OniaPFpatJetsWithOnia = cms.EDFilter(
    "OniaPatJetSelector",
    src=cms.InputTag("ak4OniaPFpatJets"),
    cut=cms.string("pt > 0.0 && abs(rapidity()) < 3.0"),
    oniaMass=cms.double(oniaMass),
    oniaMassTolerance=cms.double(0.001),
)

# Dynamic grooming of both reco and generator Onia jets.
process.load("HeavyIonsAnalysis.JetAnalysis.dynGroomedPatJets_cfi")
process.dynGroomedPatJets.isMC = True
process.dynGroomedPatJets.chargedOnly = False
process.dynGroomedPatJets.jetSrc = "ak4OniaPFpatJetsWithOnia"
process.dynGroomedPatJets.constitSrc = "oniaPFCandidates"
process.load("HeavyIonsAnalysis.JetAnalysis.dynGroomedGenJets_cfi")
process.dynGroomedGenJets.isMC = True
process.dynGroomedGenJets.chargedOnly = False
process.dynGroomedGenJets.jetSrc = "ak4GenJetsNoNu"
process.dynGroomedGenJets.constitSrc = "packedGenParticlesForJetsNoNu"

process.ak4OniaPFJetAnalyzer = process.ak4PFJetAnalyzer.clone(
    jetTag="ak4OniaPFpatJetsWithOnia",
    jetName="ak4OniaPF",
    matchJets=True,
    matchTag="ak4OniaPFpatJets",
    genjetTag="ak4GenJetsNoNu",
    doBtagging=False,
    doHiJetID=True,
    doWTARecluster=False,
    doSubEvent=False,
    jetPtMin=0,
    jetAbsEtaMax=cms.untracked.double(2.5),
    rParam=0.4,
    doSubJets=cms.untracked.bool(True),
    subjetTag=cms.untracked.InputTag("dynGroomedPatJets"),
    doGenSubJets=cms.untracked.bool(True),
    subjetGenTag=cms.untracked.InputTag("dynGroomedGenJets"),
)

process.forest += (
    process.mergedGenParticles
    + process.packedGenParticlesForJetsNoNu
    + process.oniaJetSequence
    + process.ak4OniaPFpatJetsWithOnia
    + process.dynGroomedPatJets
    + process.dynGroomedGenJets
    + process.ak4OniaPFJetAnalyzer
)

# The Onia setup helper also defines a standalone filtered path.  Everything
# needed for the combined ntuple is already included in process.forest.
process.schedule = cms.Schedule(process.forest)
