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

process.GlobalTag.toGet.extend([
    cms.PSet(
        record = cms.string("BTagTrackProbability3DRcd"),
        tag = cms.string("probBTagPDF3D_tag_mc_2024ppRef_Pythia8_v1"),
        connect = cms.string("sqlite_file:JPcalib_MC141X_2024ppRef_Pythia8_v1.db")
    ),
    #overload 2D version with the "wrong" calibration
    cms.PSet(
        record = cms.string("BTagTrackProbability2DRcd"),
        tag = cms.string("probBTagPDF3D_tag_data_2024ppRef_HardProbes0_v1"),
        connect = cms.string("sqlite_file:JPcalib_Data141X_2024ppRef_HardProbes0_v1.db")
    )

])

#####################################################################################
# Define tree output
#####################################################################################

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestMiniAOD.root"))

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

#########################
# Main analysis list
#########################

process.forest = cms.Path(
    process.HiForestInfo +
    process.hltanalysis *
    process.hiEvtAnalyzer *
    #process.hltobject +
    #process.l1object +
    #process.HiGenParticleAna +
    #process.ggHiNtuplizer +
    #process.trackSequencePP +
    process.unpackedTracksAndVertices +
    process.unpackedMuons +
    process.muonAnalyzer
)

#####################################################################################
# Select the types of jets filled 
matchJets = True             # Enables q/g and heavy flavor jet identification in MC
jetPtMin = 15
jetAbsEtaMax = 2.5

# Choose which additional information is added to jet trees
doHIJetID = True             # Fill jet ID and composition information branches
doWTARecluster = False        # Add jet phi and eta for WTA axis
doBtagging  =  True         # Note that setting to True increases computing time a lot

# 0 means use original mini-AOD jets, otherwise use R value, e.g., 3,4,8
# Generator level jets in original miniAOD jets contain neutrinos
# You will need to do reclustering with R-value to get generator level jets without neutrinos
# Add all the values you want to process to the list
jetLabels = ["4"]

# add candidate tagging for all selected jet radii
from HeavyIonsAnalysis.JetAnalysis.setupJets_ppRef_cff import candidateBtaggingMiniAOD

    
                                                                                         



for jetLabel in jetLabels:
    candidateBtaggingMiniAOD(process, isMC = True, jetPtMin = jetPtMin, jetCorrLevels = ['L2Relative', 'L3Absolute'], doBtagging = doBtagging, labelR = jetLabel)


    
#extra stuff for aggregation
doDeclustering = True
doAggregation = True
doChargedOnly = True
doLatekt_ = False

tmva_variables = ["trkIp3dSig", "trkIp2dSig", "trkDistToAxis",
                  "svtxdls", "svtxdls2d", "svtxm", "svtxmcorr",
                  "svtxnormchi2", "svtxNtrk", "svtxTrkPtOverSv",
                  "jtpt"]

taggedGenParticlesName_ = "HFdecayProductTagger"    

process.load("RecoHI.HiJetAlgos.TrackToGenParticleMapProducer_cfi")
process.TrackToGenParticleMapProducer.jetSrc = cms.InputTag("selectedUpdatedPatJetsAK4PFCHSBtag")
process.TrackToGenParticleMapProducer.genParticleSrc = cms.InputTag(taggedGenParticlesName_, "patPackedGenParticles")
process.TrackToGenParticleMapProducer.chargedOnly = doChargedOnly
process.forest += process.TrackToGenParticleMapProducer
## Creates the genConstitToGenParticleMap and trackToGenParticleMap

process.load("RecoHI.HiJetAlgos.dynGroomedPATJets_cfi")
process.dynGroomedGenJets = process.dynGroomedPATJets.clone(
    chargedOnly = cms.bool(doChargedOnly),
    aggregateHF = cms.bool(doAggregation),
    # aggregateHF = cms.bool(True),
    jetSrc = cms.InputTag("selectedUpdatedPatJetsAK4PFCHSBtag"),
    constitSrc = cms.InputTag("packedGenParticles"),
    doGenJets = cms.bool(True),
    candToGenParticleMap = cms.InputTag("TrackToGenParticleMapProducer", "genConstitToGenParticleMap"),
    doLateKt = cms.bool(doLatekt_),
)
process.forest += process.dynGroomedGenJets

process.dynGroomedPFJets = process.dynGroomedPATJets.clone(
    chargedOnly = cms.bool(doChargedOnly),
    aggregateHF = cms.bool(doAggregation),
    # aggregateHF = cms.bool(False),
    jetSrc = cms.InputTag("selectedUpdatedPatJetsAK4PFCHSBtag"),
    constitSrc = cms.InputTag("packedPFCandidates"),
    doGenJets = cms.bool(False),
    candToGenParticleMap = cms.InputTag("TrackToGenParticleMapProducer", "trackToGenParticleMap"),
    aggregateWithTruthInfo = cms.bool(False),
    aggregateWithTMVA = cms.bool(True),
    aggregateWithCuts = cms.bool(False),
    #xgb_path = cms.FileInPath("RecoHI/HiJetAlgos/data/sig_vs_bkg.model"),
    tmva_path = cms.FileInPath("RecoHI/HiJetAlgos/data/TMVAClassification_BDTG.weights.xml"),
    tmva_variables = cms.vstring(tmva_variables),
    doLateKt = cms.bool(doLatekt_),
    trkInefRate = cms.double(0)
)
process.forest += process.dynGroomedPFJets

ipTagInfoLabel_ = "pfImpactParameter"
svTagInfoLabel_ = "pfInclusiveSecondaryVertexFinder"
svNegTagInfoLabel_ = "pfInclusiveSecondaryVertexFinderNegative"

doTracks = True
if doTracks:    
    process.ak4PFJetAnalyzer.doTracks = cms.untracked.bool(True)    
    process.ak4PFJetAnalyzer.ipTagInfoLabel = cms.untracked.string(ipTagInfoLabel_)
    
doSvtx = True
if doSvtx:    
    process.ak4PFJetAnalyzer.doSvtx = cms.untracked.bool(True)    
    process.ak4PFJetAnalyzer.svTagInfoLabel = cms.untracked.string(svTagInfoLabel_)
    process.ak4PFJetAnalyzer.svNegTagInfoLabel = cms.untracked.string(svNegTagInfoLabel_)  


for jetLabel in jetLabels:
    # setup jet analyzer
    setattr(process,"ak"+jetLabel+"PFJetAnalyzer",process.ak4PFJetAnalyzer.clone())
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetTag = "selectedUpdatedPatJetsAK"+jetLabel+"PFCHSBtag"
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetName = 'ak'+jetLabel+'PF'
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").matchJets = matchJets
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").matchTag = 'patJetsAK'+jetLabel+'PFUnsubJets'
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").doBtagging = doBtagging
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").doHiJetID = doHIJetID
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").doWTARecluster = doWTARecluster
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetPtMin = jetPtMin
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetAbsEtaMax = cms.untracked.double(jetAbsEtaMax)
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").rParam = 0.4 if jetLabel=="0" else float(jetLabel)*0.1
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetFlavourInfos = "ak"+jetLabel+"PFFlavourInfos"
    if jetLabel != "0": getattr(process,"ak"+jetLabel+"PFJetAnalyzer").genjetTag = "ak"+jetLabel+"GenJetsReclusterNoNu"
    if doBtagging:
        getattr(process,"ak"+jetLabel+"PFJetAnalyzer").pfJetProbabilityBJetTags = cms.untracked.string("pfJetProbabilityBJetTagsAK"+jetLabel+"PFCHSBtag")
        getattr(process,"ak"+jetLabel+"PFJetAnalyzer").pfWrongJetProbabilityBJetTags = cms.untracked.string("pfWrongJetProbabilityBJetTagsAK"+jetLabel+"PFCHSBtag")
        getattr(process,"ak"+jetLabel+"PFJetAnalyzer").pfUnifiedParticleTransformerAK4JetTags = cms.untracked.string("pfUnifiedParticleTransformerAK4JetTagsAK"+jetLabel+"PFCHSBtag")
        getattr(process,"ak"+jetLabel+"PFJetAnalyzer").pfNegativeUnifiedParticleTransformerAK4JetTags = cms.untracked.string("pfNegativeUnifiedParticleTransformerAK4JetTagsAK"+jetLabel+"PFCHSBtag")
    process.forest += getattr(process,"ak"+jetLabel+"PFJetAnalyzer")



## Impact parameter tag infos
process.load("RecoBTag.ImpactParameter.pfImpactParameterTagInfos_cfi")
process.pfImpactParameterTagInfos.candidates  = "packedPFCandidates"
process.pfImpactParameterTagInfos.primaryVertex = "offlineSlimmedPrimaryVertices"
#process.pfImpactParameterTagInfos.jets = "patJetsAK4PFCHS"
process.pfImpactParameterTagInfos.jets = "updatedPatJetsAK4PFCHSBtag"

## Secondary vertex tag infos
process.load("RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfos_cfi")
process.pfInclusiveSecondaryVertexFinderTagInfos.extSVCollection = "slimmedSecondaryVertices"

process.patAlgosToolsTask.replace(process.pfImpactParameterTagInfosAK4PFCHSBtag, process.pfImpactParameterTagInfos)
process.patAlgosToolsTask.replace(process.pfInclusiveSecondaryVertexFinderTagInfosAK4PFCHSBtag, process.pfInclusiveSecondaryVertexFinderTagInfos)




process.pfJetProbabilityBJetTagsAK4PFCHSBtag.tagInfos = cms.VInputTag("pfImpactParameterTagInfos")
process.pfWrongJetProbabilityBJetTagsAK4PFCHSBtag.tagInfos = cms.VInputTag("pfImpactParameterTagInfos")
process.pfWrongJetProbabilityBJetTagsAK4PFCHSBtag.jetTagComputer = cms.string('wrongCandidateJetProbabilityComputer')
process.updatedPatJetsTransientCorrectedAK4PFCHSBtag.tagInfoSources = cms.VInputTag(
    cms.InputTag("pfImpactParameterTagInfos"), cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfos"), cms.InputTag("pfParticleTransformerAK4TagInfosAK4PFCHSBtag"), cms.InputTag("pfUnifiedParticleTransformerAK4TagInfosAK4PFCHSBtag"), cms.InputTag("pfInclusiveSecondaryVertexFinderNegativeTagInfos")
)

    
#more stuff for aggregation
process.ak4PFJetAnalyzer.genParticles = cms.untracked.InputTag(taggedGenParticlesName_, "patPackedGenParticles") 
process.ak4PFJetAnalyzer.bHadrons = cms.untracked.InputTag(taggedGenParticlesName_, "bHadrons")
process.ak4PFJetAnalyzer.groomedJets = cms.untracked.InputTag("dynGroomedPFJets")
process.ak4PFJetAnalyzer.groomedGenJets = cms.untracked.InputTag("dynGroomedGenJets")

process.bDecayAna = process.HiGenParticleAna.clone(
    genParticleSrc = cms.InputTag(taggedGenParticlesName_, "patPackedGenParticles"),
    useRefVector = cms.untracked.bool(False),
    partonMEOnly = cms.untracked.bool(False),
    chargedOnly = doChargedOnly,
    doHI = False,
    etaMax = cms.untracked.double(10),
    ptMin = cms.untracked.double(0),
    stableOnly = False
)
process.forest += process.bDecayAna
# Creates the gen particle ntuple bDecayAna/hi
    
process.bHadronAna = process.bDecayAna.clone(
    genParticleSrc = cms.InputTag(taggedGenParticlesName_, "bHadrons"),
    chargedOnly = False
)
process.forest += process.bHadronAna

#save some space  # for MC just limit track and SV storage to 70
#process.ak4PFJetAnalyzer.jetPtMin = 70.
