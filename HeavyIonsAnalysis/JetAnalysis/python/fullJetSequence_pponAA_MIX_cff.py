import FWCore.ParameterSet.Config as cms

from HeavyIonsAnalysis.JetAnalysis.rerecoGen_cff import *
from HeavyIonsAnalysis.JetAnalysis.rerecoRho_cff import *
from HeavyIonsAnalysis.JetAnalysis.rerecoJets_cff import *
from HeavyIonsAnalysis.JetAnalysis.rerecoTracks_cff import *

from HeavyIonsAnalysis.JetAnalysis.jets.akPu3CaloJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.ak3PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akPu3PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akCs3PFJetSequence_pponPbPb_mc_cff import *

from HeavyIonsAnalysis.JetAnalysis.jets.akPu4CaloJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.ak4PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akPu4PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akCs4PFJetSequence_pponPbPb_mc_cff import *

#Hybrid with flow analyzers
from HeavyIonsAnalysis.JetAnalysis.jets.akFlowPuCs3PFJetSequence_pponPbPb_mc_cff import *
from HeavyIonsAnalysis.JetAnalysis.jets.akFlowPuCs4PFJetSequence_pponPbPb_mc_cff import *

#Import the rho producer and flow modulator
from RecoHI.HiJetAlgos.hiPuRhoProducer import hiPuRhoProducer
from RecoHI.HiJetAlgos.hiFJRhoFlowModulationProducer_cff import hiFJRhoFlowModulationProducer

ak4PFJetsForFlow = cms.EDProducer("FastjetJetProducer",
    Active_Area_Repeats = cms.int32(1),
    GhostArea = cms.double(0.01),
    Ghost_EtaMax = cms.double(5.0),
    Rho_EtaMax = cms.double(4.4),
    doAreaDiskApprox = cms.bool(False),
    doAreaFastjet = cms.bool(True),
#    doOutputJets = cms.bool(True),
    doPUOffsetCorr = cms.bool(False),
    doPVCorrection = cms.bool(False),
    doRhoFastjet = cms.bool(False),
    inputEMin = cms.double(0.0),
    inputEtMin = cms.double(0.0),
    jetAlgorithm = cms.string('AntiKt'),
    jetPtMin = cms.double(15.0),
    jetType = cms.string('PFJet'),
    maxBadEcalCells = cms.uint32(9999999),
    maxBadHcalCells = cms.uint32(9999999),
    maxProblematicEcalCells = cms.uint32(9999999),
    maxProblematicHcalCells = cms.uint32(9999999),
    maxRecoveredEcalCells = cms.uint32(9999999),
    maxRecoveredHcalCells = cms.uint32(9999999),
    minSeed = cms.uint32(14327),
    nSigmaPU = cms.double(1.0),
    rParam = cms.double(0.4),
    radiusPU = cms.double(0.5),
    src = cms.InputTag("pfcandCleanerPt4Eta2", "particleFlowCleaned"),
    srcPVs = cms.InputTag(""),
    useDeterministicSeed = cms.bool(True),
    voronoiRfact = cms.double(-0.9)
)

#We will also need some cleaned candidates for our jets, declare directly
pfcandCleanerPt4Eta2 = cms.EDProducer("HiPFCandCleaner",
                                              pfPtMin = cms.double(4.),
                                              pfAbsEtaMax = cms.double(2.),
                                              pfCandidateLabel = cms.InputTag("particleFlow")
                                              )

hiFJRhoFlowModulationProducer.jetTag = cms.InputTag("ak4PFJetsForFlow")
hiFJRhoFlowModulationProducer.doFlatTest = cms.bool(True)

#Define our PU rho producer
hiPuRhoR3Producer = hiPuRhoProducer.clone(rParam = cms.double(.3),
                                         puPtMin = cms.double(15.0),
                                         radiusPU = cms.double(.5),
                                         minimumTowersFraction = cms.double(0.7),
                                         medianWindowWidth = cms.int32(2),
                                         towSigmaCut = cms.double(5.)
                                         )


#Define the hybrid jets. We will run with the flow correction on
akFlowPuCs3PFJets = akCs3PFJets.clone(rParam = cms.double(0.3),
                                      etaMap = cms.InputTag('hiPuRhoR3Producer','mapEtaEdges'),
                                      rho = cms.InputTag('hiPuRhoR3Producer','mapToRho'),
                                      rhom = cms.InputTag('hiPuRhoR3Producer','mapToRhoM'),
                                      rhoFlowFitParams = cms.InputTag('hiFJRhoFlowModulationProducer','rhoFlowFitParams'),
                                      jetCollInstanceName = cms.string("pfParticlesCs"),
                                      useModulatedRho = cms.bool(True)
                                      )

akFlowPuCs4PFJets = akFlowPuCs3PFJets.clone(rParam = cms.double(0.4))

genSignalSequence = cms.Sequence(
    genParticlesForJets +

    hiSignalGenParticles +
    genParticlesForJetsSignal +

    ak3HiGenJets +
    ak4HiGenJets +

    signalPartons +

    ak3HiSignalGenJets +
    ak4HiSignalGenJets +

    ak3HiGenNjettiness +
    ak4HiGenNjettiness
)

genCleanedSequence = cms.Sequence(
    genParticlesForJets +

    ak3HiGenJets +
    ak4HiGenJets +

    myPartons +
    cleanedPartons +

    ak3HiCleanedGenJets +
    ak4HiCleanedGenJets
)

jetSequence = cms.Sequence(
    rhoSequence +
    
    #Run the Pu rho producer
    hiPuRhoR3Producer + 
    #Run our flow modulator
    pfcandCleanerPt4Eta2 +
    ak4PFJetsForFlow +
    hiFJRhoFlowModulationProducer +

    highPurityTracks +

    akPu3CaloJets +
    ak3PFJets +
    akPu3PFJets +
    akCs3PFJets +

    akPu4CaloJets +
    ak4PFJets +
    akPu4PFJets +
    akCs4PFJets +

    #Add our hybrid jets to collection
    akFlowPuCs3PFJets +
    akFlowPuCs4PFJets +

    akPu3CaloJetSequence +
    ak3PFJetSequence + 
    akPu3PFJetSequence +
    akCs3PFJetSequence +

    akPu4CaloJetSequence +
    ak4PFJetSequence + 
    akPu4PFJetSequence +
    akCs4PFJetSequence + 

    #Add our processing sequences
    akFlowPuCs3PFJetSequence +
    akFlowPuCs4PFJetSequence
)
