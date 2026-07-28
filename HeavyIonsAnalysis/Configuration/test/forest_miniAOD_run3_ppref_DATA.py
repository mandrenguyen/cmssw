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
       # '/store/data/Run2024J/PPRefSingleMuon3/MINIAOD/PromptReco-v1/000/387/506/00000/f5965c7b-0a25-4d33-b44f-85a4336bb9cd.root'
        '/store/data/Run2024J/PPRefHardProbes0/MINIAOD/PromptReco-v1/000/387/721/00000/5ec03202-6428-48c9-86ed-05f26d1476a5.root'
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

#GlobalTag used in Prompt RECO
#https://cms-conddb.cern.ch/cmsDbBrowser/list/Prod/gts/132X_dataRun3_Prompt_v3
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '141X_dataRun3_Prompt_v3', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag

process.GlobalTag.toGet.extend([
    cms.PSet(
        record = cms.string("BTagTrackProbability3DRcd"),
        tag = cms.string("probBTagPDF3D_tag_data_2024ppRef_HardProbes0_v1"),
        connect = cms.string("sqlite_file:JPcalib_Data141X_2024ppRef_HardProbes0_v1.db")
    ),
    #overload 2D version with the "wrong" calibration
    cms.PSet(
        record = cms.string("BTagTrackProbability2DRcd"),
        tag = cms.string("probBTagPDF3D_tag_mc_2024ppRef_Pythia8_v1"),
        connect = cms.string("sqlite_file:JPcalib_MC141X_2024ppRef_Pythia8_v1.db")
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
    process.HiForestInfo +
    process.hltanalysis *
    process.hiEvtAnalyzer *
#    process.hltobject +
#    process.l1object +
#    process.ggHiNtuplizer +
#    process.zdcSequencePP +
#    process.trackSequencePP +
#    process.particleFlowAnalyser +
    process.unpackedTracksAndVertices + 
    process.unpackedMuons +
    process.muonAnalyzer
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
# Select the types of jets filled
matchJets = True             # Enables q/g and heavy flavor jet identification in MC 
jetPtMin = 15
jetAbsEtaMax = 2.5

# Choose which additional information is added to jet trees
doHIJetID = True             # Fill jet ID and composition information branches
doWTARecluster = False        # Add jet phi and eta for WTA axis
doBtagging  =  True         # Note that setting to True increases computing time a lot

# 0 means use original mini-AOD jets, otherwise use R value, e.g., 3,4,8
# Add all the values you want to process to the list
jetLabels = ["4"]

# add candidate tagging for all selected jet radii
from HeavyIonsAnalysis.JetAnalysis.setupJets_ppRef_cff import candidateBtaggingMiniAOD

for jetLabel in jetLabels:
    candidateBtaggingMiniAOD(process, isMC = False, jetPtMin = jetPtMin, jetCorrLevels = ['L2Relative', 'L3Absolute'], doBtagging = doBtagging, labelR = jetLabel)

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

process.load("RecoHI.HiJetAlgos.dynGroomedPATJets_cfi")

process.dynGroomedPFJets = process.dynGroomedPATJets.clone(
    chargedOnly = cms.bool(doChargedOnly),
    aggregateHF = cms.bool(doAggregation),
    # aggregateHF = cms.bool(False),
    isMC = cms.bool(False),
    jetSrc = cms.InputTag("selectedUpdatedPatJetsAK4PFCHSBtag"),
    constitSrc = cms.InputTag("packedPFCandidates"),
    doGenJets = cms.bool(False),
    candToGenParticleMap = cms.InputTag(""),
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
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").rParam = 0.4 if jetLabel=='0' else float(jetLabel)*0.1
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

#save some space
process.ak4PFJetAnalyzer.jetPtMin = 70.


from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
process.hltfilter = hltHighLevel.clone(
    HLTPaths = [
        "HLT_AK4PFJet40_v*", 
        "HLT_AK4PFJet60_v*", 
        "HLT_AK4PFJet80_v*", 
        "HLT_AK4PFJet100_v*", 
        "HLT_AK4PFJet120_v*", 
    ]
)


process.filterSequence = cms.Sequence(
    process.hltfilter
)

process.superFilterPath = cms.Path(process.filterSequence)
process.skimanalysis.superFilters = cms.vstring("superFilterPath")

# filter all path with the production filter sequence                                                                                                                                                                                                                                                                                       

for path in process.paths:
    getattr(process, path)._seq = process.filterSequence * getattr(process,path)._seq
