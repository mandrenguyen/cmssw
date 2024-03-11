import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask

process = cms.Process("PATtest")

## MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")

## Options and Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

## Source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring()
)
## Maximal Number of Events
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic_hi')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC103X_2018PbPb_v4"),                                                                                                                                      connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
             
         )
])

process.load("Configuration.StandardSequences.MagneticField_cff")

## Output Module Configuration (expects a path 'p')
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('patTuple.root'),
                               ## save only events passing the full path
                               #SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
                               ## save PAT output; you need a '*' to unpack the list of commands
                               ## 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContentNoCleaning )
                               )

patAlgosToolsTask = getPatAlgosToolsTask(process)
#process.load("DeepNTuples.DeepNtuplizer.candidateBtaggingMiniAOD_cff")
#process.p = cms.Path(process.candidateBtagging)
process.outpath = cms.EndPath(process.out, patAlgosToolsTask)

## and add them to the event content
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

bTagInfos = [
        'pfImpactParameterTagInfos',
        'pfInclusiveSecondaryVertexFinderTagInfos',
        'pfSecondaryVertexTagInfos',
        'pfDeepCSVTagInfos',
]

updateJetCollection(
   process,
        labelName = "DeepFlavour",
        jetSource = cms.InputTag('slimmedJets'),
        pvSource = cms.InputTag('offlineSlimmedPrimaryVerticesRecovery'),
        svSource = cms.InputTag('slimmedSecondaryVertices','','PAT'),
        jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
        #btagInfos = bTagInfos,
        btagDiscriminators = [
                'pfJetProbabilityBJetTags',
                'pfCombinedInclusiveSecondaryVertexV2BJetTags',
                'pfCombinedSecondaryVertexV2BJetTags',
                'pfDeepCSVJetTags:probudsg', 
                'pfDeepCSVJetTags:probb', 
                'pfDeepCSVJetTags:probc', 
                'pfDeepCSVJetTags:probbb', 
                #'pfDeepCSVJetTags:probcc'      
        ],
)

if hasattr(process,'updatedPatJetsTransientCorrectedDeepFlavour'):
        process.updatedPatJetsTransientCorrectedDeepFlavour.addTagInfos = cms.bool(True)
        process.updatedPatJetsTransientCorrectedDeepFlavour.addBTagInfo = cms.bool(True)

from PhysicsTools.PatAlgos.patInputFiles_cff import filesRelValTTbarPileUpMINIAODSIM

#process.source.fileNames = filesRelValTTbarPileUpMINIAODSIM
#process.source.fileNames = cms.untracked.vstring('/store/mc/RunIISummer19UL18MiniAOD/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/260000/00C28834-56C0-2343-B436-AA8521756E9E.root')
#process.source.fileNames = cms.untracked.vstring('file:/eos/cms/store/group/phys_top/gkrintir/TT_TuneCP5_HydjetDrumMB_5p02TeV-powheg-pythia8/TT_TuneCP5_HydjetDrumMB_5p02TeV-powheg-pythia8-v2/210114_011633/0000/step3_PAT_4.root')
#process.source.fileNames = cms.untracked.vstring('/store/himc/RunIIpp5Spring18MiniAOD/TT_TuneCP5_5p02TeV-powheg-pythia8/MINIAODSIM/94X_mc2017_realistic_forppRef5TeV-v1/60000/04907A40-C3B7-E811-BA32-7CD30ACDE0CC.root')
process.source.fileNames = cms.untracked.vstring('/store/himc/HINPbPbSpring21MiniAOD/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/MINIAODSIM/FixL1CaloGT_New_Release_112X_upgrade2018_realistic_HI_v9-v1/260000/6700a2b8-9c0d-4e1a-a774-2463e1e57785.root') 

process.maxEvents.input = 100

from Configuration.EventContent.EventContent_cff import MINIAODSIMEventContent
process.out.outputCommands = MINIAODSIMEventContent.outputCommands
process.out.outputCommands.append('drop *')
process.out.outputCommands = cms.untracked.vstring()
process.out.outputCommands.append('keep *_selectedUpdatedPatJets*_*_*')
#process.out.outputCommands.append('keep *_offlineSlimmedPrimaryVerticesRecovery*_*_*')
#process.out.outputCommands.append('keep *_centrality*_*_*')
#process.out.outputCommands.append('keep *_slimmedSecondaryVertices*_*_*')
#process.out.outputCommands.append('keep *_packedPFCandidates*_*_*')
process.out.outputCommands.append('keep *_pfDeepCSVTagInfos*_*_*')
process.out.outputCommands.append('keep *_pfDeepFlavourTagInfos*_*_*')
process.out.outputCommands.append('keep *_pfDeepFlavourJetTags*_*_*')
process.out.outputCommands.append('keep *_updatedPatJets*_*_*')

#process.out.fileName = 'test_deep_flavour_MINIAODSIM.root'
process.out.fileName = 'test_deep_flavour_reduced.root'

#                                         ##
#   process.options.wantSummary = False   ##  (to suppress the long output at the end of the job)
# process.add_(cms.Service("InitRootHandlers", DebugLevel =cms.untracked.int32(3)))

