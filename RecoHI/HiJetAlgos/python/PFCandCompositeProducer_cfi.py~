import FWCore.ParameterSet.Config as cms


pfCandComposites = cms.EDProducer('PFCandCompositeProducer',
                                  pfCandTag    = cms.InputTag('particleFlow'),
                                  compositeTag = cms.InputTag('onia2MuMuPatGlbGlb'),
                                  jpsiTrigFilter = cms.string("hltL1fL1sDoubleMu0L1Filtered0"),
                                  isHI = cms.bool(False),
                                  replaceJMM = cms.bool(False),
                                  replaceYMM = cms.bool(True),
                                  replaceDKPi = cms.bool(False)
                                  )
