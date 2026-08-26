import FWCore.ParameterSet.Config as cms

oniaGenJetParticles = cms.EDProducer(
    "OniaGenJetParticleSelector",
    src=cms.InputTag("mergedGenParticles"),
    resonancePdgIds=cms.vint32(443),
    ignoreParticleIDs=cms.vint32(12, 14, 16),
)
