import FWCore.ParameterSet.Config as cms


ecalShiftParticleFlow = cms.EDProducer('EcalEscaleShiftPFProducer',
                                           src    = cms.InputTag('particleFlow'),
                                           removePreshower = cms.bool(False), 
                                           scaleEB = cms.double(1.01),  # SETME
                                           scaleEE = cms.double(1.15),  # SETME

)

