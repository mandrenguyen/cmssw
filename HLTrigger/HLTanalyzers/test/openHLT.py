# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: openHLT --python_file openHLT.py --data --era Run2_2016_pA --geometry=Extended2016,Extended2016Reco --process reHLT --conditions=auto:run2_hlt --step L1REPACK:Full,HLT:PIon --no_exec -n 5
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reHLT',eras.Run2_2018)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.SimL1EmulatorRepack_FullMC_cff')
process.load('HLTrigger.Configuration.HLT_Fake_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
	fileNames = cms.untracked.vstring('file:/home/llr/cms/stahl/TriggerStudy2018/CMSSW_9_0_0/src/Test_RECO.root')
)

process.options = cms.untracked.PSet(

)
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.suppressWarning.append("hltOnlineBeamSpot")
process.MessageLogger.suppressWarning.append("hltOnlineBeamSpotUNPACK")

process.TFileService = cms.Service("TFileService",
                                   fileName =cms.string("openHLT.root"))

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('openHLT nevts:5'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)
# Additional output definition

# Other statements
from HLTrigger.Configuration.CustomConfigs import ProcessName
process = ProcessName(process)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '90X_upgrade2018_realistic_v17', '')

process.GlobalTag.toGet.extend([
        cms.PSet(record = cms.string("L1TUtmTriggerMenuRcd"),
                tag = cms.string("L1Menu_HeavyIons2016_v3_m2_xml"),
                connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
                ),
        cms.PSet(record = cms.string("L1TGlobalPrescalesVetosRcd"),
                tag = cms.string("L1TGlobalPrescalesVetos_Stage2v0_hlt"),
                connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
                )
])


process.load("HLTrigger.HLTanalyzers.HLTAnalyser_cfi")
# Path and EndPath definitions
process.L1RePack_step = cms.Path(process.SimL1Emulator)
process.hltAna_step = cms.EndPath(
    process.hltGetConditions + 
    process.hltGetRaw + 
    process.HLTBeamSpot + 
    process.hltanalysis
    )
'''

from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag, cloneProcessingSnippet
cloneProcessingSnippet(process, process.hltAna_step, "UNPACK")
massSearchReplaceAnyInputTag(process.hltAna_step,"rawDataCollector","rawDataCollector::reHLT")
massSearchReplaceAnyInputTag(process.hltAna_step,"hltGtStage2Digis","simGtStage2Digis")
massSearchReplaceAnyInputTag(process.hltAna_step,"hltGmtStage2Digis:Muon:reHLT","simGmtStage2Digis::reHLT")
massSearchReplaceAnyInputTag(process.hltAna_stepUNPACK,"rawDataCollector","rawDataCollector::HLT")
'''
process.hltanalysis.RunParameters.HistogramFile = cms.untracked.string("openHLT.root")
process.hltanalysis.hltresults = cms.InputTag( 'TriggerResults::reHLT')
process.hltanalysis.HLTProcessName = cms.string('reHLT')
process.hltanalysis.muonFilters = cms.VInputTag();
process.hltanalysis.muon = cms.InputTag("muons")
process.hltanalysis.l1tAlgBlkInputTag = cms.InputTag("simGtStage2Digis","")  # Needed, fix bug of GlobalAlgBlk uninitialized token
process.hltanalysis.l1tExtBlkInputTag = cms.InputTag("simGtStage2Digis","")
process.hltanalysis.gObjectMapRecord = cms.InputTag("hltGtStage2ObjectMap::reHLT")
process.hltanalysis.gmtStage2Digis = cms.string("simGmtStage2Digis")
process.hltanalysis.caloStage2Digis = cms.string("simCaloStage2Digis")
process.hltanalysis.MuCandTag2 = cms.InputTag("hltL2MuonCandidates::reHLT")
process.hltanalysis.MuCandTag3 = cms.InputTag("hltL3MuonCandidates::reHLT")
process.hltanalysis.L3TkTracksFromL2OIStateTag = cms.InputTag("hltL3TkTracksFromL2OIState::reHLT")
process.hltanalysis.L3TkTracksFromL2OIHitTag = cms.InputTag("hltL3TkTracksFromL2OIHit::reHLT")
process.hltanalysis.UseTFileService = cms.untracked.bool(True)
process.hltanalysis.getSimL1 = cms.untracked.bool(True)
process.hltanalysis.RunParameters.isData = cms.untracked.bool(False)
'''
process.hltanalysisUNPACK.RunParameters.HistogramFile = cms.untracked.string("openHLT.root")
process.hltanalysisUNPACK.hltresults = cms.InputTag( 'TriggerResults::HLT')
process.hltanalysisUNPACK.HLTProcessName = cms.string('HLT')
process.hltanalysisUNPACK.muonFilters = cms.VInputTag(
    cms.InputTag("hltL1sDoubleMu0BptxAND",""),                            # 0
    cms.InputTag("hltL1sDoubleMu0MassGT1BptxAND",""),                     # 1
    cms.InputTag("hltL1sDoubleMu10BptxAND",""),                           # 2
    cms.InputTag("hltL1sDoubleMuOpenOSBptxAND",""),                       # 3
    cms.InputTag("hltL1sDoubleMuOpenSSBptxAND",""),                       # 4
    cms.InputTag("hltL1sDoubleMuOpenBptxAND",""),                         # 5
    cms.InputTag("hltL1sSingleMu3BptxAND",""),                            # 6
    cms.InputTag("hltL1sSingleMu5BptxAND",""),                            # 7
    cms.InputTag("hltL1sSingleMu7BptxAND",""),                            # 8
    cms.InputTag("hltL1sSingleMu12BptxAND",""),                           # 9
    cms.InputTag("hltL1fL1sDoubleMuOpenBptxANDL1Filtered0",""),           # 10
    cms.InputTag("hltL2fL1sDoubleMuOpenBptxANDL1f0L2Filtered0",""),       # 11
    cms.InputTag("hltL3fL1sDoubleMuOpenBptxANDL1f0L2f0L3Filtered0",""),   # 12
    );
process.hltanalysisUNPACK.muon = cms.InputTag("muons")
process.hltanalysisUNPACK.l1tAlgBlkInputTag = cms.InputTag("gtStage2Digis","")  # Needed, fix bug of GlobalAlgBlk uninitialized token
process.hltanalysisUNPACK.l1tExtBlkInputTag = cms.InputTag("gtStage2Digis","")
process.hltanalysisUNPACK.gObjectMapRecord = cms.InputTag("hltGtStage2ObjectMap::HLT")
process.hltanalysisUNPACK.gmtStage2Digis = cms.string("gmtStage2Digis")
process.hltanalysisUNPACK.caloStage2Digis = cms.string("caloStage2Digis")
process.hltanalysisUNPACK.MuCandTag2 = cms.InputTag("hltL2MuonCandidatesUNPACK::reHLT")
process.hltanalysisUNPACK.MuCandTag3 = cms.InputTag("hltL3MuonCandidatesUNPACK::reHLT")
process.hltanalysisUNPACK.L3TkTracksFromL2OIStateTag = cms.InputTag("hltL3TkTracksFromL2OIStateUNPACK::reHLT")
process.hltanalysisUNPACK.L3TkTracksFromL2OIHitTag = cms.InputTag("hltL3TkTracksFromL2OIHitUNPACK::reHLT")
process.hltanalysisUNPACK.UseTFileService = cms.untracked.bool(True)

process.HLTL1UnpackerSequence.replace(process.hltGtStage2Digis, process.hltGtStage2DigisUNPACK*process.hltGtStage2Digis)
process.hltGtStage2ObjectMap.ExtInputTag = cms.InputTag("hltGtStage2DigisUNPACK")

process.hltAna2 = cms.EndPath(process.hltAna_stepUNPACK)
process.endjob_step = cms.EndPath(process.endOfProcess)
'''
# Schedule definition
process.schedule = cms.Schedule(process.L1RePack_step)
process.schedule.extend(process.HLTSchedule)
process.schedule.extend([process.hltAna_step])

'''

process.load("HLTrigger.HLTanalyzers.HLTBitAnalyser_cfi")
process.hltbitanalysis.RunParameters.isData = cms.untracked.bool(True)
process.hltbitanalysis.HLTProcessName = cms.string('reHLT')
process.hltbitanalysis.hltresults = cms.InputTag( 'TriggerResults','','reHLT' )
process.hltbitanalysis.l1tAlgBlkInputTag = cms.InputTag("gtStage2Digis")
process.hltbitanalysis.l1tExtBlkInputTag = cms.InputTag("gtStage2Digis")
process.hltbitanalysis.gObjectMapRecord  = cms.InputTag("gtStage2ObjectMap")
process.hltbitanalysis.gmtStage2Digis    = cms.string("gmtStage2Digis")
process.hltbitanalysis.caloStage2Digis   = cms.string("caloStage2Digis")
process.hltbitanalysis.UseTFileService = cms.untracked.bool(True)
process.hltBitAnalysis = cms.EndPath(process.hltbitanalysis)
process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("openHLT.root"))


'''
