import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.GenJetParameters_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *

fc4GenJets = cms.EDProducer(
    "FastjetJetProducer",
    GenJetParameters,
    AnomalousCellParameters,
    jetAlgorithm = cms.string("FlavorCone"),
    rParam       = cms.double(0.4),
    useFlavorCone = cms.bool(True),
    flavorConeSeeds = cms.InputTag("patJetPartons","bHadrons")
    )

fc4GenJets.jetPtMin = 0.0
