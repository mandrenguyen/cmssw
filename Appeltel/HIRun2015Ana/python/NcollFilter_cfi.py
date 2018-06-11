import FWCore.ParameterSet.Config as cms

ncollFilter  = cms.EDFilter('NcollFilter',
    generators = cms.VInputTag(cms.InputTag("generatorSmeared")),
    ncollmax = cms.int32(3000)
)
