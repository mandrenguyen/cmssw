### HiForest Configuration
# Input: miniAOD
# Type: data

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run2_2018_pp_on_AA_cff import Run2_2018_pp_on_AA
from Configuration.ProcessModifiers.run2_miniAOD_pp_on_AA_103X_cff import run2_miniAOD_pp_on_AA_103X
process = cms.Process('HiForest', Run2_2018_pp_on_AA,run2_miniAOD_pp_on_AA_103X)

###############################################################################

# HiForest info
process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 112X, data")

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
        "/store/hidata/HIRun2018A/HISingleMuon/MINIAOD/PbPb18_MiniAODv1-v1/00000/00345f79-641f-4002-baf1-19ae8e83c48b.root"
    ),
)
#input file produced from:
#"file:/afs/cern.ch/work/r/rbi/public/forest/HIHardProbes_HIRun2018A-PromptReco-v2_AOD.root"

# number of events to process, set to -1 to process all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
    )

###############################################################################

# load Global Tag, geometry, etc.
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')


from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data_promptlike_hi', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag

centralityTag = "CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run2v1031x02_offline"
process.HiForestInfo.info.append(centralityTag)

print('\n')
print('\033[31m~*~ CENTRALITY TABLE FOR 2018 PBPB DATA ~*~\033[0m')
print('\033[36m~*~ TAG: ' + centralityTag + ' ~*~\033[0m')
print('\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(
        record = cms.string("HeavyIonRcd"),
        tag = cms.string(centralityTag),
        label = cms.untracked.string("HFtowers"),
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        ),
    ])
print('\n\033[31m~*~ "using swapped HI JP calib! ~*~\033[0m\n')  
process.GlobalTag.toGet.extend([
    cms.PSet(
        record = cms.string("BTagTrackProbability3DRcd"),
        #tag = cms.string("JPcalib_Data103X_2018PbPb_v1"),
        tag = cms.string("JPcalib_MC103X_2018PbPb_v4"), 
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
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
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_data
#process.hltobject.triggerNames = trigger_list_data
process.hltobject.triggerNames = cms.vstring("HLT_HIL3Mu3_NHitQ10_v1","HLT_HIL3Mu5_NHitQ10_v1","HLT_HIL3Mu7_NHitQ10_v1")    
process.hltobject.triggerNames += cms.vstring("HLT_HIL3Mu3Eta2p5_PuAK4CaloJet40Eta2p1_v","HLT_HIL3Mu5Eta2p5_PuAK4CaloJet40Eta2p1_v")
process.hltobject.triggerNames += cms.vstring("HLT_HIL3Mu3Eta2p5_PuAK4CaloJet60Eta2p1_v","HLT_HIL3Mu5Eta2p5_PuAK4CaloJet60Eta2p1_v")
process.hltobject.triggerNames += cms.vstring("HLT_HIL3Mu3Eta2p5_PuAK4CaloJet80Eta2p1_v","HLT_HIL3Mu5Eta2p5_PuAK4CaloJet80Eta2p1_v")
process.hltobject.triggerNames += cms.vstring("HLT_HIL3Mu3Eta2p5_PuAK4CaloJet100Eta2p1_v","HLT_HIL3Mu5Eta2p5_PuAK4CaloJet100Eta2p1_v")

#process.hltobject.triggerNames = cms.vstring("HLT_HIPuAK4CaloJet100Eta5p1_v","HLT_HIPuAK4CaloJet80Eta5p1_v")
#process.hltobject.triggerNames = cms.vstring("HLT_HIPuAK4CaloJet4Eta5p1_v","HLT_HIPuAK4CaloJet60Eta5p1_v")

process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')
################################
# electrons, photons, muons
SSHIRun2018A = "HeavyIonsAnalysis/EGMAnalysis/data/SSHIRun2018A.dat"
process.load('HeavyIonsAnalysis.EGMAnalysis.correctedElectronProducer_cfi')
process.correctedElectrons.correctionFile = SSHIRun2018A

process.load('HeavyIonsAnalysis.MuonAnalysis.unpackedMuons_cfi')
process.load('HeavyIonsAnalysis.EGMAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizer.electronSrc = "correctedElectrons"
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
################################
# jet reco sequence
process.load("HeavyIonsAnalysis.JetAnalysis.extraJets_cff")
process.load('HeavyIonsAnalysis.JetAnalysis.akCs3PFMuonJetSequence_pponPbPb_data_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.akCs4PFMuonJetSequence_pponPbPb_data_cff')
################################
# tracks
process.load("HeavyIonsAnalysis.TrackAnalysis.TrackAnalyzers_cff")
###############################################################################



###############################################################################
# main forest sequence
process.forest = cms.Path(
    process.HiForestInfo +
    process.hltanalysis +
    process.hltobject +
    #process.l1object +
    #process.trackSequencePbPb +
    #process.particleFlowAnalyser +
    process.hiEvtAnalyzer +
    #process.unpackedMuons +
    #process.correctedElectrons +
    #process.ggHiNtuplizer +
    process.extraJetsData +
    process.akCs3PFJetSequence +
    process.akCs4PFJetSequence
    #process.akCs4PFJetAnalyzer
    )

#customisation

#########################
# Event Selection -> add the needed filters here
#########################

process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
process.pclusterCompatibilityFilter = cms.Path(process.clusterCompatibilityFilter)
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)
process.load('HeavyIonsAnalysis.EventAnalysis.hffilter_cfi')
process.pphfCoincFilter2Th4 = cms.Path(process.phfCoincFilter2Th4)
process.pAna = cms.EndPath(process.skimanalysis)


from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
process.hltfilter = hltHighLevel.clone(
    HLTPaths = [
        "HLT_HIL3Mu*_NHitQ10_v*",                                                                                                                           
        "HLT_HIL3Mu*Eta2p5_PuAK4CaloJet*Eta2p1_v*",
        #"HLT_HIPuAK4CaloJet*Eta5p1_v*",                                                                                                                           
        #"HLT_HIPuAK4CaloJet100Eta5p1_v*",                                                                                                                        
        #"HLT_HIPuAK4CaloJet80Eta5p1_v*",                                                                                                                        
        #"HLT_HIPuAK4CaloJet60Eta5p1_v*",                                                                                                                      
        #"HLT_HIPuAK4CaloJet40Eta5p1_v*",                                                                                                                         
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
