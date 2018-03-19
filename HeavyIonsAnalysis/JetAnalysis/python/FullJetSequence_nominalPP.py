import FWCore.ParameterSet.Config as cms

### PP RECO does not include R=3 or R=5 jets.
### re-RECO is only possible for PF, RECO is missing calotowers
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
ak4PFJets.doAreaFastjet = True
ak3PFJets = ak4PFJets.clone(rParam = 0.3)
ak5PFJets = ak4PFJets.clone(rParam = 0.5)
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
ak3GenJets = ak4GenJets.clone(rParam = 0.3)
ak5GenJets = ak4GenJets.clone(rParam = 0.5)
from RecoJets.JetProducers.fc4GenJets_cfi import fc4GenJets

#SoftDrop PF jets
from RecoJets.JetProducers.PFJetParameters_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *
akSoftDrop4PFJets = cms.EDProducer(
    "FastjetJetProducer",
    PFJetParameters,
    AnomalousCellParameters,
    jetAlgorithm = cms.string("AntiKt"),
    rParam       = cms.double(0.4),
    useSoftDrop = cms.bool(True),
    zcut = cms.double(0.1),
    beta = cms.double(0.0),
    R0   = cms.double(0.4),
    useExplicitGhosts = cms.bool(True),
    writeCompound = cms.bool(True),
    jetCollInstanceName=cms.string("SubJets")
)
akSoftDrop5PFJets = akSoftDrop4PFJets.clone(rParam = cms.double(0.5), R0 = cms.double(0.5))

from HeavyIonsAnalysis.JetAnalysis.akSoftDrop4GenJets_cfi import akSoftDrop4GenJets
akSoftDrop5GenJets = akSoftDrop4GenJets.clone(rParam = 0.5)

#Filter PF jets
akFilter4PFJets = cms.EDProducer(
    "FastjetJetProducer",
    PFJetParameters,
    AnomalousCellParameters,
    jetAlgorithm = cms.string("AntiKt"),
    rParam       = cms.double(0.4),
    useFiltering = cms.bool(True),
    nFilt = cms.int32(4),
    rFilt = cms.double(0.15),
    useExplicitGhosts = cms.bool(True),
    writeCompound = cms.bool(True),
    jetCollInstanceName=cms.string("SubJets")
)
akFilter5PFJets = akFilter4PFJets.clone(rParam = cms.double(0.5))

from RecoJets.Configuration.GenJetParticles_cff import *
from RecoHI.HiJetAlgos.HiGenJets_cff import *
from HeavyIonsAnalysis.JetAnalysis.makePartons_cff import patJetPartonsLegacy, patJetPartons

#from HeavyIonsAnalysis.JetAnalysis.jets.ak3PFJetSequence_pp_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.ak4PFJetSequence_pp_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.fc4PFJetSequence_pp_mc_cff import *
#from HeavyIonsAnalysis.JetAnalysis.jets.ak5PFJetSequence_pp_mc_cff import *
#from HeavyIonsAnalysis.JetAnalysis.jets.ak4CaloJetSequence_pp_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akSoftDrop4PFJetSequence_pp_mc_cff import *
#from HeavyIonsAnalysis.JetAnalysis.jets.akSoftDrop5PFJetSequence_pp_mc_cff import *

highPurityTracks = cms.EDFilter("TrackSelector",
                                src = cms.InputTag("generalTracks"),
                                cut = cms.string('quality("highPurity")')
)


inclusiveCandidateSecondaryVerticesFiltered = cms.EDFilter("CandidateBVertexFilter",
    minVertices = cms.int32(0),
    primaryVertices = cms.InputTag("offlinePrimaryVertices"),
    secondaryVertices = cms.InputTag("inclusiveCandidateSecondaryVertices"),
    useVertexKinematicAsJetAxis = cms.bool(True),
    vertexFilter = cms.PSet(
        distSig2dMax = cms.double(99999.9),
        #distSig2dMin = cms.double(3.0),
        distSig2dMin = cms.double(2.0),  # changed to IVF settings
        distSig3dMax = cms.double(99999.9),
        distSig3dMin = cms.double(-99999.9),
        distVal2dMax = cms.double(2.5),
        distVal2dMin = cms.double(0.01),
        distVal3dMax = cms.double(99999.9),
        distVal3dMin = cms.double(-99999.9),
        #fracPV = cms.double(0.65),  #changed to IVF settings
        fracPV = cms.double(0.79), 
        massMax = cms.double(6.5),
        #maxDeltaRToJetAxis = cms.double(0.1),  # this is not working
        maxDeltaRToJetAxis = cms.double(999),
        minimumTrackWeight = cms.double(0.5),   
        multiplicityMin = cms.uint32(2),
        useTrackWeights = cms.bool(True),
        v0Filter = cms.PSet(
            k0sMassWindow = cms.double(0.05)
        )
    )
)



candidateBToCharmDecayVertexMerged = cms.EDProducer("CandidateBtoCharmDecayVertexMerger",
    maxDRUnique = cms.double(0.4),
    maxPtreltomerge = cms.double(7777.0),
    maxvecSumIMifsmallDRUnique = cms.double(5.5),
    minCosPAtomerge = cms.double(0.99),  #default
    #minCosPAtomerge = cms.double(0.95),  
#    minCosPAtomerge = cms.double(0.85),  
    primaryVertices = cms.InputTag("offlinePrimaryVertices"),
    secondaryVertices = cms.InputTag("inclusiveCandidateSecondaryVerticesFiltered")
    #secondaryVertices = cms.InputTag("inclusiveCandidateSecondaryVertices")
)





from RecoJets.JetProducers.fc4PFJets_cfi import fc4PFJets


# Other radii jets and calo jets need to be reconstructed
jetSequences = cms.Sequence(
    patJetPartonsLegacy +
    patJetPartons +
    genParticlesForJets +
    #ak3GenJets +
    ak4GenJets +
    #ak5GenJets +
    fc4GenJets +
    #ak3PFJets +
    #ak5PFJets +
    inclusiveCandidateSecondaryVerticesFiltered +
    candidateBToCharmDecayVertexMerged +
    fc4PFJets +
    akSoftDrop4PFJets +
    #akSoftDrop5PFJets +
    #akFilter4PFJets +
    #akFilter5PFJets +
    akSoftDrop4GenJets +
    #akSoftDrop5GenJets +
    highPurityTracks +
    #ak3PFJetSequence +
    ak4PFJetSequence +
    fc4PFJetSequence +
    #ak5PFJetSequence +
    #ak4CaloJetSequence +
    akSoftDrop4PFJetSequence 
    #akSoftDrop5PFJetSequence
)
