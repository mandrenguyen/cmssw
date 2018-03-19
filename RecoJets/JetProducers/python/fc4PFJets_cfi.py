import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.PFJetParameters_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *

fc4PFJets = cms.EDProducer(
    "FastjetJetProducer",
    PFJetParameters,
    AnomalousCellParameters,
    jetAlgorithm = cms.string("FlavorCone"),
    rParam       = cms.double(0.4),
    useFlavorCone = cms.bool(True),
    #flavorConeSeeds = cms.InputTag("inclusiveCandidateSecondaryVertices")
    #flavorConeSeeds = cms.InputTag("inclusiveCandidateSecondaryVerticesFiltered")
    flavorConeSeeds = cms.InputTag("candidateBToCharmDecayVertexMerged")
    )

fc4PFJets.jetPtMin = 0.0

