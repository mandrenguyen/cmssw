import FWCore.ParameterSet.Config as cms


pfCandComposites = cms.EDProducer('PFCandCompositeProducer',
                                  pfCandTag    = cms.InputTag('particleFlow'),
                                  compositeTag = cms.InputTag('onia2MuMuPatGlbGlb'),
                                  jpsiTrigFilter = cms.string("hltL3f0L3Mu0L2Mu0DR3p5FilteredNHitQ10M1to5"),
                                  isHI = cms.bool(True),
                                  replaceJMM = cms.bool(True),
                                  replaceDKPi = cms.bool(False)
                                  )
