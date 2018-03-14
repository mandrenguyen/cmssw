import FWCore.ParameterSet.Config as cms

patJetPartonsLegacy = cms.EDProducer("PartonSelector",
    withLeptons = cms.bool(False),
    src = cms.InputTag("genParticles")
)

patJetPartons = cms.EDProducer("HadronAndPartonSelector",
    fullChainPhysPartons = cms.bool(True),
    particles = cms.InputTag("genParticles"),
    partonMode = cms.string('Auto'),
    src = cms.InputTag("generator")
)


makePartons = cms.Sequence(patJetPartonsLegacy + patJetPartons)
