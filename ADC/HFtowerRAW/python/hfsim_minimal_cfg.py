import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_pp_on_PbPb_cff import Run3_pp_on_PbPb
process = cms.Process('HFt', Run3_pp_on_PbPb)

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.Services_cff")

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
	'/store/user/sochandr/D0tokpi_pthat30_PbPb_2023_Digi_MC_update/D0tokpi_pthat30_PbPb_2023_set2/D0tokpi_pthat30_PbPb_2023_Digi_MC_generation_update/230824_170429/0000/JME-RunIIAutumn18DR-00003_step1_8.root'
    ),
)

process.TFileService = cms.Service('TFileService',
    fileName = cms.string(
        'hfoutput.root'
    )
)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(
    process.GlobalTag,
    'auto:phase1_2023_realistic_hi',
    ''
)

process.hft = cms.EDAnalyzer('MiniHFtowers',
                             tower_tag = cms.InputTag("towerMaker"),
)

process.rawtodigi = cms.Path(
    # process.RawToDigi
    process.hcalDigis +
    process.ecalDigis #+
    # process.siPixelDigis 
)

process.recotowers = cms.Path(
	process.bunchSpacingProducer *
	process.calolocalreco *
	process.hcalGlobalRecoSequence *
	process.caloTowersRec
)

process.output = cms.EndPath(
    process.hft
)

process.schedule = cms.Schedule(
    process.rawtodigi,
    process.recotowers,
    process.output
)

# import FWCore.ParameterSet.VarParsing as VarParsing
# ivars = VarParsing.VarParsing('analysis')

# ivars.maxEvents = 10
# ivars.outputFile='hfoutput.root'
# ivars.inputFiles='/store/user/subehera/MB_Hydjet_Run3_GENSIM/MB_Hydjet_Run3_DIGIRAW_approxSiStripClusters/220910_091752/0000/step3_apprximateCluster_22.root'
# ivars.parseArguments()

# process.source.fileNames = cms.untracked.vstring(ivars.inputFiles)
# process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(ivars.maxEvents))
# process.TFileService.fileName = cms.string(ivars.outputFile)
