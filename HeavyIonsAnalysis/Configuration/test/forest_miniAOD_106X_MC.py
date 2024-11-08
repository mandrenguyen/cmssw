### HiForest Configuration
# Input: miniAOD
# Type: mc
import sys

# fname = sys.argv[2]
# print("file in:", fname)

import FWCore.ParameterSet.Config as cms
process = cms.Process('HiForest')
process.options = cms.untracked.PSet()

###############################################################################

# HiForest info
process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 106X, mc")

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
        #'/store/user/mnguyen/Herwig_CH3_bjet_5TeV/Herwig_CH3_bjet_5TeV_MINI_v7/240123_124848/0000/mini_PAT_1.root' # herwig bjet
        #'/store/himc/RunIISummer20UL17pp5TeVMiniAODv2/JPsiMM_TuneCUETP8M1_5p02TeV-pythia8/MINIAODSIM/106X_mc2017_realistic_forppRef5TeV_v3-v3/40000/8955DE2D-4001-9D43-A91F-76DEAAD6F2B6.root'
        #'/store/himc/RunIISummer20UL17pp5TeVMiniAODv2/JPsiMM_TuneCUETP8M1_5p02TeV-pythia8/MINIAODSIM/106X_mc2017_realistic_forppRef5TeV_v3-v3/50000/E89E8CDB-4D53-AC44-AFB7-0B6EB6CB9BE4.root'
        #'file:./E89E8CDB-4D53-AC44-AFB7-0B6EB6CB9BE4.root'
        '/store/himc/RunIIpp5Spring18MiniAOD/BJPsiMM_TuneCUETP8M1_5p02TeV_pythia8/MINIAODSIM/94X_mc2017_realistic_forppRef5TeV_ext1-v1/130000/04D9CB39-2BB1-E911-A6A0-141877411367.root'
        ),
      #                      skipEvents = cms.untracked.uint32(72),
    )

# Select specific event
# process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange('1:7897')
# process.source.eventsToProcess = cms.untracked.VEventRange('1:110394257')

# number of events to process, set to -1 to process all events
process.maxEvents = cms.untracked.PSet(
    #input = cms.untracked.int32(-1)
    input = cms.untracked.int32(10000)
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
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_mc2017_realistic_v10', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")

process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC94X_2017pp_v2"),
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
# )
# process.output_path = cms.EndPath(process.output)

###############################################################################

#############################
# Gen Analyzer
#############################
process.load('HeavyIonsAnalysis.EventAnalysis.HiGenAnalyzer_cfi')
# making cuts looser so that we can actually check dNdEta
process.HiGenParticleAna.ptMin = cms.untracked.double(0.4) # default is 5
process.HiGenParticleAna.etaMax = cms.untracked.double(5.) # default is 2.5

# event analysis
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_mc_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_mc
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
process.load("HeavyIonsAnalysis.JetAnalysis.ak4PFJetSequence_ppref_mc_cff")
################################

process.load("HeavyIonsAnalysis.TrackAnalysis.unpackedTracksAndVertices_cfi")
process.load("HiAnalysis.HiOnia.unpackedMuonsWithGenMatch_cfi")

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
#oniaTreeAnalyzer(process, muonSelection="Glb", isMC=True, outputFileName="HiForestAOD.root", muonlessPV=True)
oniaTreeAnalyzer(process, muonSelection="GlbTrk", isMC=True, outputFileName="HiForestAOD.root", muonlessPV=True)
from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import changeToMiniAOD
changeToMiniAOD(process)

process.onia2MuMuPatGlbGlb.dimuonSelection       = cms.string("2.6 < mass && mass < 3.5 && charge == 0 && pt > 3")
process.hionia.SumETvariables   = cms.bool(False)
process.hionia.applyCuts   = True
process.hionia.checkTrigNames = False
process.hionia.primaryVertexTag = "unpackedTracksAndVertices"
#process.patMuonsWithTriggerSequence.insert(1, process.unpackedMuonsWithGenMatch)
#process.Onia2MuMuPAT.insert(1, process.unpackedMuonsWithGenMatch)

from HLTrigger.Configuration.CustomConfigs import MassReplaceInputTag
#process = MassReplaceInputTag(process,"unpackedMuons","slimmedMuons")
#process = MassReplaceInputTag(process,"unpackedTracksAndVertices","offlineSlimmedPrimaryVertices")
process.patTriggerAll.onlyStandAlone = True

process.particleFlowNoHF = cms.EDFilter("CandPtrSelector",
                                 src=cms.InputTag("packedPFCandidates"),
                                 cut=cms.string("abs(pdgId)!=1 && abs(pdgId)!=2 && abs(eta)<3.0")
)


process.load("RecoHI.HiJetAlgos.PFCandCompositeProducer_cfi")
#process.pfCandComposites.pfCandTag    = cms.InputTag('particleFlowNoHF')
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
setupPprefJets('ak04PF', process.jetsR4, process, isMC = 1, radius = 0.40, JECTag = 'AK4PF')
process.ak04PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
process.ak04PFpatJetCorrFactors.primaryVertices = "offlineSlimmedPrimaryVertices"
process.ak04PFJets.src = 'pfCandComposites'
process.jetsR4.remove(process.ak04PFpatJetPartonMatch)
process.ak04PFpatJetPartons.particles = "prunedGenParticles"
process.ak4GenJetsNoNu.src = "genParticlesForJets"
process.jetsR4.remove(process.ak04PFpfImpactParameterTagInfos)
process.jetsR4.remove(process.ak04PFpfSecondaryVertexTagInfos)
process.jetsR4.remove(process.ak04PFpfDeepCSVTagInfos)
process.jetsR4.remove(process.ak04PFpfDeepCSVJetTags)
process.jetsR4.remove(process.ak04PFpfJetProbabilityBJetTags)
process.ak04PFpatJets.useLegacyJetMCFlavour = False
process.ak04PFpatJets.getJetMCFlavour = False
process.ak04PFpatJets.addGenPartonMatch = False
process.ak04PFpatJets.addBTagInfo = False
process.ak04PFpatJets.addJetFlavourInfo  = False


process.load("PhysicsTools.JetMCAlgos.SelectPartons_cff")
process.allPartons = process.myPartons.clone(src = "prunedGenParticles")


process.load("GeneratorInterface.RivetInterface.mergedGenParticles_cfi")

process.load("RecoJets.Configuration.GenJetParticles_cff")
process.genParticlesForJets.src = 'mergedGenParticles'
process.genParticlesForJets.storeJMM = cms.untracked.bool(True)

process.load("RecoHI.HiJetAlgos.dynGroomedGenJets_cfi")
process.dynGroomedGenJets.chargedOnly = cms.bool(False)
process.dynGroomedGenJets.jetSrc = cms.InputTag("ak4GenJetsNoNu")
process.dynGroomedGenJets.constitSrc = cms.InputTag("genParticlesForJets")


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
process.ak4PFJetAnalyzer.groomedGenJets = cms.untracked.InputTag("dynGroomedGenJets")
process.ak4PFJetAnalyzer.genjetTag = "ak4GenJetsNoNu"
process.ak4PFJetAnalyzer.doGenSubJets = True

process.ak04PFJets.jetPtMin = 0.
process.ak4PFJetAnalyzer.jetPtMin = 0.
process.ak4PFJetAnalyzer.genPtMin = 0.

process.jpsiJets = cms.Sequence(
    process.particleFlowNoHF * 
    process.pfCandComposites *
    process.allPartons *
    process.mergedGenParticles *
    process.genParticlesForJets *
    process.jetsR4 *
    process.ak4PFXpatJets *
    process.dynGroomedGenJets *
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
    process.hltanalysis +
    # process.hltobject +
    #process.l1object +
    # process.trackSequencePbPb +
    # process.particleFlowAnalyser +
    process.hiEvtAnalyzer + 
    process.unpackedTracksAndVertices +
    process.unpackedMuons +
    #process.unpackedMuonsWithGenMatch +
    process.oniaTreeAna *
    #process.HiGenParticleAna +
    process.rhoSequence + 
    process.jpsiJets 
    )

#process.forest.remove(process.unpackedMuons)
#process.forest.remove(process.unpackedTracksAndVertices)
#process.Onia2MuMuPAT.remove(process.unpackedMuons)
#process.Onia2MuMuPAT.remove(process.unpackedTracksAndVertices)
process.Onia2MuMuPAT.remove(process.patTriggerAll)
process.forest.remove(process.patTriggerAll)
## Customization





    
#########################
# Jet Selection
#########################
    
# for b tagging SF
# process.mujetSelector = cms.EDFilter("PatJetXSelector",
#                              src = cms.InputTag("slimmedJets"),
#                              offPV = cms.InputTag("offlineSlimmedPrimaryVertices"),
#                              cut = cms.string("pt > 5.0 && abs(rapidity()) < 3."),
#                              dummy = cms.bool(False)
#                          )
# process.recoJetSequence += process.mujetSelector
# process.ak4PFJetAnalyzer.mujetTag = cms.InputTag("mujetSelector")

#########################
# Event Selection -> add the needed filters here
#########################
    


#process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
#process.pclusterCompatibilityFilter = cms.Path(process.clusterCompatibilityFilter)
#process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)
#process.pAna = cms.EndPath(process.skimanalysis)

process.Onia2MuMuPAT.insert(4,process.unpackedMuonsWithGenMatch)
