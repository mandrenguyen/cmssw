### HiForest Configuration
# Input: miniAOD
# Type: mc

import FWCore.ParameterSet.Config as cms
process = cms.Process('HiForest')
process.options = cms.untracked.PSet()

###############################################################################

# HiForest info
process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 106X, data")

# import subprocess, os
# version = subprocess.check_output(
#     ['git', '-C', os.path.expandvars('$CMSSW_BASE/src'), 'describe', '--tags'])
# if version == '':
#     version = 'no git info'
# process.HiForestInfo.HiForestVersion = cms.string(version)

###############################################################################

# input files
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(
        # '/store/data/Run2017G/LowEGJet/MINIAOD/UL2017_MiniAODv2-v2/2810000/01869167-7867-434D-A952-5BEC77B73ABA.root'
        # '/store/data/Run2017G/HighEGJet/MINIAOD/UL2017_MiniAODv2-v2/2530000/0128C28F-AFE6-8F42-B9E2-F3D58AA96AAC.root'
        # '/store/data/Run2017G/SingleMuonTnP/MINIAOD/09Aug2019_UL2017-v1/10000/031B3B58-47D5-4F43-907D-E95BC492311C.root'
         '/store/data/Run2017G/SingleMuon/MINIAOD/UL2017_MiniAODv2-v1/30000/8A5CEA04-5852-CE47-AAF7-8C0B33A70FD3.root'
        # '/store/data/Run2017G/HighEGJet/MINIAOD/UL2017_MiniAODv2-v2/50000/2ECDBCF3-B35E-D24F-864B-62AAF9367181.root'
        #'file:/data_CMS/cms/kalipoliti/HighEGJet/hardToOpenFile/2ECDBCF3-B35E-D24F-864B-62AAF9367181.root'
        ),
    )


# Select specific event
#lostLumis = cms.untracked.VLuminosityBlockRange('306705:1062', '306705:1091', 
#                                                '306705:1101', '306705:1102',
#                                                '306777:1957', '306777:1961',
#                                                '306777:1965', '306777:1966',
#                                                '306777:1970', '306777:1971')
#process.source.lumisToProcess = lostLumis
# process.source.eventsToProcess = cms.untracked.VEventRange('1:110394257')

# number of events to process, set to -1 to process all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100000)
    )

# Multi-thread 
# process.options.numberOfThreads = cms.untracked.int32(8)

# To skip events with 'Found zero products matching all criteria' error
# process.options = cms.untracked.PSet(SkipEvent = cms.untracked.vstring('ProductNotFound'))

###############################################################################

# load Global Tag, geometry, etc.
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')


from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_dataRun2_v35', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")

process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_Data94X_2017pp_v2"),
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")

         )
      ])

###############################################################################

# root output
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestMiniAOD.root"))

# # edm output for debugging purposes
# process.output = cms.OutputModule(
#     "PoolOutputModule",
#     fileName = cms.untracked.string('HiForestEDM.root'),
#     outputCommands = cms.untracked.vstring(
#         'keep *',
#         )
#     )

# process.output_path = cms.EndPath(process.output)

###############################################################################

# event analysis
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.hiEvtAnalyzer.Vertex = cms.InputTag("offlineSlimmedPrimaryVertices")
process.hiEvtAnalyzer.doCentrality = cms.bool(False)
process.hiEvtAnalyzer.doEvtPlane = cms.bool(False)
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

# from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_mc
# process.hltobject.triggerNames = trigger_list_mc
# process.hltobject.triggerNames = cms.vstring('HLT_HIAK4CaloJet30_v', 'HLT_HIAK4CaloJet40_v', 'HLT_HIAK4CaloJet60_v')

################################
# electrons, photons, muons
SS2018PbPbMC = "HeavyIonsAnalysis/EGMAnalysis/data/SS2018PbPbMC.dat"
process.load('HeavyIonsAnalysis.EGMAnalysis.correctedElectronProducer_cfi')
process.correctedElectrons.correctionFile = SS2018PbPbMC

process.load('HeavyIonsAnalysis.EGMAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizer.doGenParticles = cms.bool(True)
process.ggHiNtuplizer.doMuons = cms.bool(False)
process.ggHiNtuplizer.electronSrc = "correctedElectrons"
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
################################
# jet reco sequence
process.load("HeavyIonsAnalysis.JetAnalysis.ak4PFJetSequence_ppref_data_cff")
################################
# tracks
process.load("HeavyIonsAnalysis.TrackAnalysis.TrackAnalyzers_cff")
#muons
process.load("HeavyIonsAnalysis.TrackAnalysis.unpackedTracksAndVertices_cfi")
process.load("HeavyIonsAnalysis.MuonAnalysis.muonAnalyzer_cfi")
process.muonAnalyzer.doGen = cms.bool(True)

#################################
# rho
process.load("RecoJets.JetProducers.fixedGridRhoProducerFastjet_cfi")
process.fixedGridRhoFastjetAll.pfCandidatesTag = cms.InputTag("packedPFCandidates")
process.rhoSequence = cms.Sequence(
    process.fixedGridRhoFastjetAll
)

from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process, muonSelection="GlbTrk", isMC=False, outputFileName="HiForestAOD.root", muonlessPV=True)
from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import changeToMiniAOD
changeToMiniAOD(process)

process.onia2MuMuPatGlbGlb.dimuonSelection       = cms.string("2.6 < mass && mass < 3.5 && charge == 0 && pt > 3")
process.hionia.SumETvariables   = cms.bool(False)
process.hionia.applyCuts   = True
process.hionia.checkTrigNames = False
process.hionia.primaryVertexTag = "unpackedTracksAndVertices"
process.patTriggerAll.onlyStandAlone = True

process.particleFlowNoHF = cms.EDFilter("CandPtrSelector",
                                 src=cms.InputTag("packedPFCandidates"),
                                 cut=cms.string("abs(pdgId)!=1 && abs(pdgId)!=2 && abs(eta)<3.0")
)


process.load("RecoHI.HiJetAlgos.PFCandCompositeProducer_cfi")
process.pfCandComposites.pfCandTag    = cms.InputTag('packedPFCandidates')
process.pfCandComposites.replaceJMM = True
process.pfCandComposites.compositeTag = cms.InputTag("onia2MuMuPatGlbGlb")

#temporarily remove vertex refit                                                                                                                                                                                                                                                                   
process.pfCandComposites.isHI = True
process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = False
process.hionia.muonLessPV = False

#muons:  Had to get loaded after onia tree, otherwise gets reconfigured                                                                                                                                                                                                                            
process.load("HeavyIonsAnalysis.MuonAnalysis.unpackedMuons_cfi")
process.unpackedMuons.muonSelectors = []

from HeavyIonsAnalysis.JetAnalysis.clusterJetsFromMiniAOD_cff import setupPprefJets

# Recluster using an alias "0" in order not to get mixed up with the default AK4 collections
process.jetsR4 = cms.Sequence()
setupPprefJets('ak04PF', process.jetsR4, process, isMC = 0, radius = 0.40, JECTag = 'AK4PF')
process.ak04PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
process.ak04PFpatJetCorrFactors.primaryVertices = "offlineSlimmedPrimaryVertices"
process.ak04PFJets.src = 'pfCandComposites'
process.jetsR4.remove(process.ak04PFpfImpactParameterTagInfos)
process.jetsR4.remove(process.ak04PFpfSecondaryVertexTagInfos)
process.jetsR4.remove(process.ak04PFpfDeepCSVTagInfos)
process.jetsR4.remove(process.ak04PFpfDeepCSVJetTags)
process.jetsR4.remove(process.ak04PFpfJetProbabilityBJetTags)
process.ak04PFpatJets.addBTagInfo = False


process.load("RecoHI.HiJetAlgos.dynGroomedPatJets_cfi")
process.dynGroomedPatJets.chargedOnly = cms.bool(False)
process.dynGroomedPatJets.jetSrc = cms.InputTag("ak4PFXpatJets")
process.dynGroomedPatJets.constitSrc = cms.InputTag("pfCandComposites")

process.ak4PFXpatJets = cms.EDFilter("PatJetXSelector",
                                     src = cms.InputTag("ak04PFpatJets"),
                                     cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.")
                                     )

process.jetsR4.remove(process.ak4PFJetAnalyzer)
process.ak4PFJetAnalyzer.jetTag = "ak4PFXpatJets"
process.ak4PFJetAnalyzer.doCandidateBtagging = False
process.ak4PFJetAnalyzer.doSubJets = False
process.ak4PFJetAnalyzer.doSubJetsNew = True
process.ak4PFJetAnalyzer.jetName = 'ak04PF'
process.ak4PFJetAnalyzer.doSubEvent = False 
process.ak4PFJetAnalyzer.groomedJets = cms.untracked.InputTag("dynGroomedPatJets")


process.jpsiJets = cms.Sequence(
    process.particleFlowNoHF *
    process.pfCandComposites *
    process.jetsR4 *
    process.ak4PFXpatJets *
    process.dynGroomedPatJets *
    process.ak4PFJetAnalyzer
)


doChargedOnly = True
doLatekt_ = False
###############################################################################

###############################################################################
# main forest sequence
process.forest = cms.Path(
    process.HiForestInfo +
    #process.hltanalysis +
    # process.hltobject +
    #process.l1object +
    # process.trackSequencePbPb +
    # process.particleFlowAnalyser +
    process.hiEvtAnalyzer +
    process.unpackedTracksAndVertices +
    process.unpackedMuons +
    # process.updatePATJetSequence + 
    #process.tagInfoSequence +
    #process.recoJetSequence + 
    #process.unpackedMuons +
    #process.correctedElectrons #+
    #process.ggHiNtuplizer +
    #process.muonAnalyzer +
    process.oniaTreeAna * 
    process.rhoSequence +
    process.jpsiJets
    )

## Customization
process.Onia2MuMuPAT.remove(process.patTriggerAll)
process.forest.remove(process.patTriggerAll)    



#########################
# Event Selection -> add the needed filters here
#########################

process.ak4PFJetAnalyzer.jetPtMin = 0.
process.ak04PFJets.jetPtMin = 0.

##################################### trigger selection                                                                                                                                                                                                                                            
process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
process.hltPFJet60 = process.hltHighLevel.clone()
process.hltPFJet60.HLTPaths = ["HLT_HIL1DoubleMu0_v1"]
process.superFilterSequence = cms.Sequence(process.hltPFJet60)
process.superFilterPath = cms.Path(process.superFilterSequence)

process.skimanalysis.superFilters = cms.vstring("superFilterPath")
for path in process.paths:
    getattr(process,path)._seq = process.superFilterSequence*getattr(process,path)._seq

process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)
process.primaryVertexFilter.src = 'offlineSlimmedPrimaryVertices'
process.pAna = cms.EndPath(process.skimanalysis)
