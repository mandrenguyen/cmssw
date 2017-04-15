import FWCore.ParameterSet.Config as cms


pfCandComposites = cms.EDProducer('PFCandCompositeProducer',
                                  pfCandTag    = cms.InputTag('particleFlow'),
                                  compositeTag = cms.InputTag('onia2MuMuPatGlbGlb'),
                                  jpsiTrigFilter = cms.string("hltHIDoubleMu0L1Filtered"),
                                  isHI = cms.bool(False),
                                  )

