import FWCore.ParameterSet.Config as cms


pfCandComposites = cms.EDProducer('PFCandCompositeProducer',
                                  pfCandTag    = cms.InputTag('packedPFCandidates'),
                                  compositeTag = cms.InputTag('onia2MuMuPatGlbGlb'),
                                  jpsiTrigFilter = cms.string("hltL1fL1sDoubleMu0L1Filtered0"),
                                  isHI = cms.bool(False),
                                  replaceJMM = cms.bool(True),
                                  replaceYMM = cms.bool(False),
                                  replaceDKPi = cms.bool(False)
                                  )
