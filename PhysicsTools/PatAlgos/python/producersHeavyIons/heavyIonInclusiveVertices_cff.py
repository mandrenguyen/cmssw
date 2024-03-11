import FWCore.ParameterSet.Config as cms

from RecoVertex.AdaptiveVertexFinder.inclusiveCandidateVertexFinder_cfi import *
from RecoVertex.AdaptiveVertexFinder.candidateVertexMerger_cfi import *
from RecoVertex.AdaptiveVertexFinder.candidateVertexArbitrator_cfi import *
#from PhysicsTools.PatAlgos.slimming.slimmedSecondaryVertices_cfi import *

inclusiveCandidateVertexFinder.primaryVertices = 'offlineSlimmedPrimaryVertices'
candidateVertexArbitrator.primaryVertices = 'offlineSlimmedPrimaryVertices'

inclusiveCandidateVertexFinder.tracks = "packedPFCandidates"
candidateVertexArbitrator.tracks = "packedPFCandidates"

inclusiveCandidateSecondaryVertices = candidateVertexMerger.clone(
    secondaryVertices = "candidateVertexArbitrator",
    maxFraction = 0.2,
    minSignificance = 10.
)
