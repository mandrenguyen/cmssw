import FWCore.ParameterSet.Config as cms

#these producers are running in reco with 93X but not in 92X
from HeavyIonsAnalysis.JetAnalysis.jets.HiRecoJets_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.HiRecoPFJets_cff import *

#not in official reco yet
from RecoHI.HiJetAlgos.hiFJGridEmptyAreaCalculator_cff import hiFJGridEmptyAreaCalculator

from HeavyIonsAnalysis.JetAnalysis.jets.akPu3CaloJetSequence_PbPb_mb_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akPu3PFJetSequence_PbPb_mb_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akCs3PFJetSequence_PbPb_mb_cff import *

from HeavyIonsAnalysis.JetAnalysis.jets.akPu4CaloJetSequence_PbPb_mb_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akPu4PFJetSequence_PbPb_mb_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akCs4PFJetSequence_PbPb_mb_cff import *

from HeavyIonsAnalysis.JetAnalysis.jets.akPu5CaloJetSequence_PbPb_mb_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akPu5PFJetSequence_PbPb_mb_cff import *

#to be added later
#from HeavyIonsAnalysis.JetAnalysis.jets.akCsSoftDrop4PFJetSequence_PbPb_mb_cff import *
#from HeavyIonsAnalysis.JetAnalysis.jets.akCsSoftDrop5PFJetSequence_PbPb_mb_cff import *

highPurityTracks = cms.EDFilter("TrackSelector",
                                src = cms.InputTag("hiGeneralTracks"),
                                cut = cms.string('quality("highPurity")'))

from RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi import *
offlinePrimaryVertices.TrackLabel = 'highPurityTracks'

jetSequences = cms.Sequence(

    highPurityTracks +
    offlinePrimaryVertices +

    #these sequences are running in reco with 93X but not in 92X
    PFTowers
    *akPu3PFJets*akPu4PFJets*akPu5PFJets
    *kt4PFJetsForRho
    *hiFJRhoProducer
    *hiFJGridEmptyAreaCalculator #not yet in 93X reco
    *akCs3PFJets*akCs4PFJets

    + akPu3CaloJets*akPu4CaloJets*akPu5CaloJets +

    #to be added later
    #akCsSoftDrop4PFJets +
    #akCsSoftDrop5PFJets +
    
#    akPu3CaloJetSequence +
#    akPu3PFJetSequence +
#    akCs3PFJetSequence +

#    akPu4CaloJetSequence +
    akPu4PFJetSequence #+
#    akCs4PFJetSequence +

#    akPu5CaloJetSequence +
#    akPu5PFJetSequence# +

    #to be added later
    #akCsSoftDrop4PFJetSequence +
    #akCsSoftDrop5PFJetSequence
)
