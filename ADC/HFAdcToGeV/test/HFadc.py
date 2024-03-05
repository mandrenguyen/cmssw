# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: l1Ntuple -s RAW2DIGI --no_exec --python_filename=L1Ntuple_2022Data.py -n 1000 --no_output --era=Run3 --data --conditions=auto:run3_data --customise=L1Trigger/Configuration/customiseReEmul.L1TReEmulFromRAWsimHcalTP --customise=L1Trigger/L1TNtuples/customiseL1Ntuple.L1NtupleAODRAWEMU --customise=L1Trigger/Configuration/customiseSettings.L1TSettingsToCaloParams_2022_v0_4 --filein=/store/data/Run2022E/ZeroBias/RAW/v1/000/359/343/00000/005628be-177a-49a9-8616-8f1c37f92bfb.root
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_pp_on_PbPb_2023_cff import Run3_pp_on_PbPb_2023

process = cms.Process('HFADC',Run3_pp_on_PbPb_2023)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load("Configuration.StandardSequences.Reconstruction_Data_cff")
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("NewEventStreamFileReader",
    fileNames = cms.untracked.vstring(""),
    # secondaryFileNames = cms.untracked.vstring()
)


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# Output definition

# Additional output definition
process.TFileService = cms.Service("TFileService",
   fileName = cms.string("/eos/cms/store/group/phys_heavyions/wangj/L1PbPb2023/adc/HFadc_r373871_HLT_HIZeroBias_v8.root"))

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '132X_dataRun3_HLT_v2', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.hcalDigis.saveQIE10DataNSamples = cms.untracked.vint32( 6)
process.hcalDigis.saveQIE10DataTags = cms.untracked.vstring( "MYDATA" )

process.HFAdcana = cms.EDAnalyzer("HFAdcToGeV",
    digiLabel = cms.untracked.InputTag("hcalDigis"),
    minimized = cms.untracked.bool(True),
    fillhf = cms.bool(False) # only turn this on when you have or know how to produce "towerMaker"
)
process.hfadc = cms.Path(process.HFAdcana)

# from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
# process.hltfilter = hltHighLevel.clone(HLTPaths = ["HLT_HIMinimumBias_v2"])
# process.filterSequence = cms.Sequence(process.hltfilter)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.hfadc,process.endjob_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)


#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
# from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
# process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
# from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
# process = customiseEarlyDelete(process)
# End adding early deletion

# process.HcalTPGCoderULUT.FG_HF_thresholds = cms.vuint32(14, 19)

process.Trigger = cms.EDFilter( "TriggerResultsFilter",
      triggerConditions = cms.vstring(
        "HLT_HIZeroBias_v8"
         ),
      hltResults = cms.InputTag( "TriggerResults", "", "HLT" ),
      l1tResults = cms.InputTag( "gtStage2Digis" ),
      l1tIgnoreMask = cms.bool( False ),
      l1techIgnorePrescales = cms.bool( True ),
      daqPartitions = cms.uint32( 1 ),
      throw = cms.bool( True )
)
for path in process.paths:
    getattr(process,path)._seq = process.Trigger * getattr(process,path)._seq
    
from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
# MassReplaceInputTag(process, old="rawDataCollector", new="rawDataMapperByLabel")
MassReplaceInputTag(process, old="rawDataCollector", new="rawPrimeDataRepacker")
# MassReplaceInputTag(process, old="rawDataCollector", new="rawDataRepacker")
# delattr(process, "rawPrimeDataRepacker")

import FWCore.ParameterSet.VarParsing as VarParsing
ivars = VarParsing.VarParsing('analysis')
ivars.register('streamer',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "txt")
ivars.streamer = "/afs/cern.ch/work/w/wangj/RECO2023/CMSSW_13_2_4/src/reco2023/list/PhysicsHIPhysicsRawPrime0_374322.txt"
ivars.outputFile = "/eos/cms/store/group/phys_heavyions/wangj/L1PbPb2023/adc/HFadc_r374322_HLT_HIZeroBias_v8.root"
ivars.parseArguments() # get and parse the command line arguments
process.source.fileNames = open(ivars.streamer, "r").read().splitlines()
process.TFileService.fileName = ivars.outputFile
