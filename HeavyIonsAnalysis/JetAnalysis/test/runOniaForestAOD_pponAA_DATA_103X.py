### HiForest Configuration
# Collisions: PbPb
# Type: Data
# Input: AOD

import FWCore.ParameterSet.Config as cms
process = cms.Process('HiForest')

###############################################################################
# HiForest labelling info
###############################################################################

process.load("HeavyIonsAnalysis.JetAnalysis.HiForest_cff")
#print "Comment this back !!!!!!"
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
        '/store/hidata/HIRun2018A/HIDoubleMuonPsiPeri/AOD/04Apr2019-v1/270007/FFAD54FB-50FC-5C4D-91D4-3DF29F38B230.root'
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
process.GlobalTag = GlobalTag(process.GlobalTag, '103X_dataRun2_Prompt_v2', '')
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

print('\n\033[31m~*~ USING CENTRALITY TABLE FOR PbPb 2018 DATA ~*~\033[0m\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run2v1031x02_offline"),
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
# Onia
#############################
from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process, muonSelection="Glb", isMC=False, outputFileName="HiForestAOD.root")
process.onia2MuMuPatGlbGlb.dimuonSelection = cms.string("pt > 6.5")
process.hionia.SumETvariables   = cms.bool(False)
###############################################################################

#############################
# Jets
#############################
# jet reco sequence
process.load('HeavyIonsAnalysis.JetAnalysis.fullJetSequence_pponAA_data_cff')
#process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak4PFJetSequence_pponPbPb_data_cff')
#process.load('HeavyIonsAnalysis.JetAnalysis.jets.ak3PFJetSequence_pponPbPb_data_cff')

# replace above with this one for JEC:
# process.load('HeavyIonsAnalysis.JetAnalysis.fullJetSequence_JEC_cff')

# temporary
process.akPu4Calocorr.payload = "AK4Calo"
process.akPu4PFcorr.payload = "AK4PF"
process.akCs4PFcorr.payload = "AK4PF"
process.akFlowPuCs4PFcorr.payload = "AK4PF"
process.akPu4PFJets.jetPtMin = 5
process.akPu3Calocorr.payload = "AK3Calo"
process.akPu3PFcorr.payload = "AK3PF"
process.akCs3PFcorr.payload = "AK3PF"
process.akFlowPuCs3PFcorr.payload = "AK3PF"
process.akPu3PFJets.jetPtMin = 5

#from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
#process.ak4PFJets = ak4PFJets
#from RecoJets.JetProducers.ak3PFJets_cfi import ak3PFJets
#process.ak3PFJets = process.ak4PFJets.clone(rParam = 0.3)

process.load('HeavyIonsAnalysis.JetAnalysis.hiFJRhoAnalyzer_cff')
process.load("HeavyIonsAnalysis.JetAnalysis.pfcandAnalyzer_cfi")
process.pfcandAnalyzer.doTrackMatching  = cms.bool(True)

process.highPurityTracks = cms.EDFilter("TrackSelector",
                                        src = cms.InputTag("generalTracks"),
                                        cut = cms.string('quality("highPurity")')
                                        )


## This jet sequence has been horribly hacked for the onia-jet analysis


process.particleFlowNoHF = cms.EDFilter("PdgIdPFCandidateSelector",
                                          src = cms.InputTag("particleFlow"),
                                          pdgId = cms.vint32(211,-211,11,13,-11,-13,22,130)
)


process.load("RecoHI.HiJetAlgos.PFCandCompositeProducer_cfi")
process.pfCandComposites.pfCandTag    = cms.InputTag('particleFlowNoHF')
process.pfCandComposites.replaceJMM = True
process.pfCandComposites.compositeTag = cms.InputTag("onia2MuMuPatGlbGlb")



# cluster AK including J/psi, but with no UE subtraction
process.ak4PFJetsWithJPsi = process.ak4PFJets.clone(
    src = 'pfCandComposites',
    jetCollInstanceName = cms.string('pfParticlesWithJPsi'),
    writeJetsWithConst = cms.bool(True)
)

process.ak3PFJetsWithJPsi = process.ak3PFJets.clone(
    src = 'pfCandComposites',
    jetCollInstanceName = cms.string('pfParticlesWithJPsi'),
    writeJetsWithConst = cms.bool(True)
)

#filter out the jet containing the jpsi
process.ak4PFXJetsWithJPsi = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("ak4PFJetsWithJPsi"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)

process.ak3PFXJetsWithJPsi = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("ak3PFJetsWithJPsi"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)

# Now remove the J/Psi
process.cs4CandsNoJPsi = cms.EDFilter("PFCandidateFwdPtrCollectionPdgIdFilter",
                             src = cms.InputTag("ak4PFXJetsWithJPsi","constituents"),
                             pdgId = cms.vint32(211,-211,11,13,-11,-13,22,130)
                         )

process.cs3CandsNoJPsi = cms.EDFilter("PFCandidateFwdPtrCollectionPdgIdFilter",
                             src = cms.InputTag("ak3PFXJetsWithJPsi","constituents"),
                             pdgId = cms.vint32(211,-211,11,13,-11,-13,22,130)
                         )

# run the Cs subtraction on the J/Psi-jet, ignoring the Jpsi 
process.akCs4PFJets.src = cms.InputTag("cs4CandsNoJPsi")
process.akCs4PFJets.jetCollInstanceName = cms.string('pfParticlesCs4NoJPsi')

process.akCs3PFJets.src = cms.InputTag("cs3CandsNoJPsi")
process.akCs3PFJets.jetCollInstanceName = cms.string('pfParticlesCs3NoJPsi')

process.akFlowPuCs4PFJets.src = cms.InputTag("cs4CandsNoJPsi")
process.akFlowPuCs4PFJets.jetCollInstanceName = cms.string('pfParticlesFlowPuCs4NoJPsi')

process.akFlowPuCs3PFJets.src = cms.InputTag("cs3CandsNoJPsi")
process.akFlowPuCs3PFJets.jetCollInstanceName = cms.string('pfParticlesFlowPuCs3NoJPsi')

### put the J/Psi back with the candidates

#here's the jpsi
process.pfCandJPsi = cms.EDFilter("PdgIdPFCandidateSelector",
                          src = cms.InputTag("pfCandComposites"),
                          pdgId = cms.vint32(1)
)



process.pfCandsCs4PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akCs4PFJets","pfParticlesCs4NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)


process.pfCandsCs3PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akCs3PFJets","pfParticlesCs3NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)

process.pfCandsFlowPuCs4PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akFlowPuCs4PFJets","pfParticlesFlowPuCs4NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)
process.pfCandsFlowPuCs3PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akFlowPuCs3PFJets","pfParticlesFlowPuCs3NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)


process.akCs4PFJetsWithJPsi = process.ak4PFJets.clone(
    src = cms.InputTag("pfCandsCs4PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesCs4WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)

process.akCs3PFJetsWithJPsi = process.ak3PFJets.clone(
    src = cms.InputTag("pfCandsCs3PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesCs3WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)



process.akFlowPuCs4PFJetsWithJPsi = process.ak4PFJets.clone(
    src = cms.InputTag("pfCandsFlowPuCs4PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesFlowPuCs4WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)
process.akFlowPuCs3PFJetsWithJPsi = process.ak3PFJets.clone(
    src = cms.InputTag("pfCandsFlowPuCs3PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesFlowPuCs3WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)


process.akCs4PFcorr.src = "akCs4PFJetsWithJPsi"
process.akCs4PFNjettiness.src = "akCs4PFJetsWithJPsi"
process.akCs4PFpatJetsWithBtagging.jetSource = "akCs4PFJetsWithJPsi"
process.akCs4PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsCs4PlusJPsi')

process.akCs3PFcorr.src = "akCs3PFJetsWithJPsi"
process.akCs3PFNjettiness.src = "akCs3PFJetsWithJPsi"
process.akCs3PFpatJetsWithBtagging.jetSource = "akCs3PFJetsWithJPsi"
process.akCs3PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsCs3PlusJPsi')

process.akFlowPuCs4PFcorr.src = "akFlowPuCs4PFJetsWithJPsi"
process.akFlowPuCs4PFNjettiness.src = "akFlowPuCs4PFJetsWithJPsi"
process.akFlowPuCs4PFpatJetsWithBtagging.jetSource = "akFlowPuCs4PFJetsWithJPsi"
process.akFlowPuCs4PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsFlowPuCs4PlusJPsi')

process.akFlowPuCs3PFcorr.src = "akFlowPuCs3PFJetsWithJPsi"
process.akFlowPuCs3PFNjettiness.src = "akFlowPuCs3PFJetsWithJPsi"
process.akFlowPuCs3PFpatJetsWithBtagging.jetSource = "akFlowPuCs3PFJetsWithJPsi"
process.akFlowPuCs3PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsFlowPuCs3PlusJPsi')



process.pfcandAnalyzer.pfCandidateLabel = 'pfCandJPsi'



process.jetSequence = cms.Sequence(
    process.rhoSequence +
    process.highPurityTracks +
    process.pfCandJPsi +
    
#    process.ak4PFJetsWithJPsi +
#    process.ak4PFXJetsWithJPsi +
#    process.cs4CandsNoJPsi +
#    process.akCs4PFJets +
#    process.pfCandsCs4PlusJPsi +
#    process.akCs4PFJetsWithJPsi +
#    process.akCs4PFJetSequence +
#
    process.ak3PFJetsWithJPsi +
    process.ak3PFXJetsWithJPsi +
    process.cs3CandsNoJPsi +
#    process.akCs3PFJets +
#    process.pfCandsCs3PlusJPsi +
#    process.akCs3PFJetsWithJPsi +
#    process.akCs3PFJetSequence +
#
    #Add our hybrid jets to collection
    process.akFlowPuCs3PFJets +
    process.pfCandsFlowPuCs3PlusJPsi + 
    process.akFlowPuCs3PFJetsWithJPsi +
    #process.akFlowPuCs4PFJets +
    #process.pfCandsFlowPuCs4PlusJPsi + 
    #process.akFlowPuCs4PFJetsWithJPsi +
    process.akFlowPuCs3PFJetSequence #+
    #process.akFlowPuCs4PFJetSequence


    )


###############################################################################

############################
# Event Analysis
############################
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

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
process.ggHiNtuplizer.doGenParticles = False
process.ggHiNtuplizerGED.doGenParticles = False

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
# ZDC RecHit Producer
process.load('RecoHI.ZDCRecHit.QWZDC2018Producer_cfi')
process.load('RecoHI.ZDCRecHit.QWZDC2018RecHit_cfi')

process.load('HeavyIonsAnalysis.JetAnalysis.rechitanalyzer_cfi')
process.rechitanalyzerpp.doZDCRecHit = True
process.rechitanalyzerpp.zdcRecHitSrc = cms.InputTag("QWzdcreco")
process.pfTowerspp.doHF = False

###############################################################################
#Recover peripheral primary vertices
#https://twiki.cern.ch/twiki/bin/view/CMS/HITracking2018PbPb#Peripheral%20Vertex%20Recovery
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVerticesRecovery_cfi")


#########################
# Main analysis list
#########################

process.ana_step = cms.Path(
    process.offlinePrimaryVerticesRecovery +
    process.HiForest +
    process.hltanalysis +
    #process.hltobject +
    #process.l1object +
    process.centralityBin +
    process.hiEvtAnalyzer +
    process.oniaTreeAna +
    process.particleFlowNoHF +
    process.pfCandComposites +
    process.jetSequence #+
    #process.ggHiNtuplizer +
    #process.ggHiNtuplizerGED +
    #process.hiFJRhoAnalyzer +
    #process.pfcandAnalyzer +
    #process.pfcandAnalyzerCS +
    #process.trackSequencesPP +
    #process.zdcdigi +
    #process.QWzdcreco +
    #process.rechitanalyzerpp
    )

# # edm output for debugging purposes
# process.output = cms.OutputModule(
#     "PoolOutputModule",
#     fileName = cms.untracked.string('HiForestEDM.root'),
#     outputCommands = cms.untracked.vstring(
#         'keep *',
#         # drop aliased products
#         'drop *_akULPu3PFJets_*_*',
#         'drop *_akULPu4PFJets_*_*',
#         )
#     )

# process.output_path = cms.EndPath(process.output)

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

###############################################################################

# Customization
process.pfcandAnalyzer.pfCandidateLabel = 'pfCandJPsi'

##################################### trigger selection
process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
process.hltPFJet60 = process.hltHighLevel.clone()
process.hltPFJet60.HLTPaths = ["HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1"]
process.superFilterSequence = cms.Sequence(process.hltPFJet60)
process.superFilterPath = cms.Path(process.superFilterSequence)

process.skimanalysis.superFilters = cms.vstring("superFilterPath")
for path in process.paths:
    getattr(process,path)._seq = process.superFilterSequence*getattr(process,path)._seq

from HLTrigger.Configuration.CustomConfigs import MassReplaceInputTag
process = MassReplaceInputTag(process,"particleFlow","pfCandComposites")
process.particleFlowNoHF.src = "particleFlow"
process.patMuonsWithoutTrigger.pfMuonSource = "particleFlow"
process.pfcandAnalyzer.pfCandidateLabel = 'particleFlow'
