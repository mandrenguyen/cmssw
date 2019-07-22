import FWCore.ParameterSet.Config as cms

hiPuRhoProducer = cms.EDProducer('HiPuRhoProducer',
                                 src = cms.InputTag('PFTowers'),
                                 rParam = cms.double(.3),
                                 nSigmaPU = cms.double(1.0),
                                 puPtMin = cms.double(15.0),
                                 radiusPU = cms.double(.5),
                                 minimumTowersFraction = cms.double(0.5)
                                 )
