import FWCore.ParameterSet.Config as cms

process = cms.Process('NCOLLFILTER')
process.load('Configuration.StandardSequences.Services_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring( 
        'dummy.root'
  )
)

process.load("Appeltel/HIRun2015Ana/fileNames_cfi");

# Output

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('/data_CMS/cms/mnguyen/ncoll_filtered.root'),
    SelectEvents = cms.untracked.PSet(
                SelectEvents = cms.vstring("filter_step")
                )
)


# ncoll filter
process.load('Appeltel.HIRun2015Ana.NcollFilter_cfi')
process.ncFilter = process.ncollFilter.clone(
    ncollmax = 3000
)

process.RandomNumberGeneratorService.ncFilter = cms.PSet(
    initialSeed = cms.untracked.uint32(2015),
    engineName = cms.untracked.string('TRandom3')
)

from IOMC.RandomEngine.RandomServiceHelper import RandomNumberServiceHelper
randSvc = RandomNumberServiceHelper(process.RandomNumberGeneratorService)
randSvc.populate()



#paths and schedule
process.filter_step = cms.Path(process.ncFilter)
process.output_step = cms.EndPath(process.output)

process.schedule = cms.Schedule(process.filter_step, process.output_step)
