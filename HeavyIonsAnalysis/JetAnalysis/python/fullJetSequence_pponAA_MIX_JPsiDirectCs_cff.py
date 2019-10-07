import FWCore.ParameterSet.Config as cms

from HeavyIonsAnalysis.JetAnalysis.rerecoGen_cff import *
from HeavyIonsAnalysis.JetAnalysis.rerecoRho_cff import *
from HeavyIonsAnalysis.JetAnalysis.rerecoJets_cff import *
from HeavyIonsAnalysis.JetAnalysis.rerecoTracks_cff import *

from HeavyIonsAnalysis.JetAnalysis.jets.akPu3CaloJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.ak3PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akPu3PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akCs3PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akFlowPuCs3PFJetSequence_pponPbPb_mc_cff import *

from HeavyIonsAnalysis.JetAnalysis.jets.akPu4CaloJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.ak4PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akPu4PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akCs4PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akFlowPuCs4PFJetSequence_pponPbPb_mc_cff import *


########## Hack the Cs jet inputs ##############


# run the Cs subtraction on the J/Psi-jet, ignoring the Jpsi 
akCs4PFJets.src = cms.InputTag("pfCandComposites")
akCs4PFJets.jetCollInstanceName = cms.string('pfParticlesCs4')
akCs3PFJets.src = cms.InputTag("pfCandComposites")
akCs3PFJets.jetCollInstanceName = cms.string('pfParticlesCs3')
akFlowPuCs4PFJets.src = cms.InputTag("pfCandComposites")
akFlowPuCs4PFJets.jetCollInstanceName = cms.string('pfParticlesFlowPuCs4')
akFlowPuCs3PFJets.src = cms.InputTag("pfCandComposites")
akFlowPuCs3PFJets.jetCollInstanceName = cms.string('pfParticlesFlowPuCs3')



#filter out the jet containing the jpsi
akCs4PFXJets = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("akCs4PFJets"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)

akCs3PFXJets = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("akCs3PFJets"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)

akFlowPuCs4PFXJets = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("akFlowPuCs4PFJets"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)

akFlowPuCs3PFXJets = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("akFlowPuCs3PFJets"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)




akCs4PFmatch.src = "akCs4PFXJets"
akCs4PFparton.src = "akCs4PFXJets"
akCs4PFcorr.src = "akCs4PFXJets"
akCs4PFNjettiness.src = "akCs4PFXJets"
akCs4PFpatJetsWithBtagging.jetSource = "akCs4PFXJets"
akCs4PFPatJetPartonAssociationLegacy.jets = "akCs4PFXJets"
akCs4PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandComposites')

akCs3PFmatch.src = "akCs3PFXJets"
akCs3PFparton.src = "akCs3PFXJets"
akCs3PFcorr.src = "akCs3PFXJets"
akCs3PFNjettiness.src = "akCs3PFXJets"
akCs3PFpatJetsWithBtagging.jetSource = "akCs3PFXJets"
akCs3PFPatJetPartonAssociationLegacy.jets = "akCs3PFXJets"
akCs3PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandComposites')

akFlowPuCs4PFmatch.src = "akFlowPuCs4PFXJets"
akFlowPuCs4PFparton.src = "akFlowPuCs4PFXJets"
akFlowPuCs4PFcorr.src = "akFlowPuCs4PFXJets"
akFlowPuCs4PFNjettiness.src = "akFlowPuCs4PFXJets"
akFlowPuCs4PFpatJetsWithBtagging.jetSource = "akFlowPuCs4PFXJets"
akFlowPuCs4PFPatJetPartonAssociationLegacy.jets = "akFlowPuCs4PFXJets"
akFlowPuCs4PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandComposites')

akFlowPuCs3PFmatch.src = "akFlowPuCs3PFXJets"
akFlowPuCs3PFparton.src = "akFlowPuCs3PFXJets"
akFlowPuCs3PFcorr.src = "akFlowPuCs3PFXJets"
akFlowPuCs3PFNjettiness.src = "akFlowPuCs3PFXJets"
akFlowPuCs3PFpatJetsWithBtagging.jetSource = "akFlowPuCs3PFXJets"
akFlowPuCs3PFPatJetPartonAssociationLegacy.jets = "akFlowPuCs3PFXJets"
akFlowPuCs3PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandComposites')



genSignalSequence = cms.Sequence(
    genParticlesForJets +

    hiSignalGenParticles +
    genParticlesForJetsSignal +

    ak3HiGenJets +
    #ak4HiGenJets +

    signalPartons +

    ak3HiSignalGenJets +
    #ak4HiSignalGenJets +

    ak3HiGenNjettiness #+
    #ak4HiGenNjettiness
)

genCleanedSequence = cms.Sequence(
    genParticlesForJets +

    ak3HiGenJets +
    #ak4HiGenJets +

    myPartons +
    cleanedPartons +

    ak3HiCleanedGenJets #+
    #ak4HiCleanedGenJets
)

jetSequence = cms.Sequence(
    rhoSequence +
    
    highPurityTracks +


    #akPu3CaloJets +
    #ak3PFJets +
    #akPu3PFJets +
    akCs3PFJets +
    akCs3PFXJets +


    #ak4PFJetsWithJPsi +
    #ak4PFXJetsWithJPsi +
    #cs4CandsNoJPsi +
    #akPu4CaloJets +
    #ak4PFJets +
    #akPu4PFJets +
    #akCs4PFJets +
    #pfCandsCs4PlusJPsi +
    #akCs4PFJetsWithJPsi +

    #Add our hybrid jets to collection
    akFlowPuCs3PFJets +
    akFlowPuCs3PFXJets +

    #akFlowPuCs4PFJets +
    #pfCandsFlowPuCs4PlusJPsi + 
    #akFlowPuCs4PFJetsWithJPsi +

    #akPu3CaloJetSequence +
    #ak3PFJetSequence + 
    #akPu3PFJetSequence +
    akCs3PFJetSequence +

    #akPu4CaloJetSequence +
    #ak4PFJetSequence + 
    #akPu4PFJetSequence +
    #akCs4PFJetSequence + 

    #Add our processing sequences
    akFlowPuCs3PFJetSequence #+
    #akFlowPuCs4PFJetSequence
)
