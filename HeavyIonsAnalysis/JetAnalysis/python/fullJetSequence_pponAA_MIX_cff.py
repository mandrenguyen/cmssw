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

#######  Cluster the jpsi into the jet #################

# cluster AK including J/psi, but with no UE subtraction
ak4PFJetsWithJPsi = ak4PFJets.clone(
    src = 'pfCandComposites',
    jetCollInstanceName = cms.string('pfParticlesWithJPsi'),
    writeJetsWithConst = cms.bool(True)
)

ak3PFJetsWithJPsi = ak3PFJets.clone(
    src = 'pfCandComposites',
    jetCollInstanceName = cms.string('pfParticlesWithJPsi'),
    writeJetsWithConst = cms.bool(True)
)

#filter out the jet containing the jpsi
ak4PFXJetsWithJPsi = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("ak4PFJetsWithJPsi"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)

ak3PFXJetsWithJPsi = cms.EDFilter("PFJetXSelector",
                            src = cms.InputTag("ak3PFJetsWithJPsi"),
                            cut = cms.string("pt > 0.0 && abs(rapidity()) < 3.0")
)

# Now remove the J/Psi
cs4CandsNoJPsi = cms.EDFilter("PFCandidateFwdPtrCollectionPdgIdFilter",
                             src = cms.InputTag("ak4PFXJetsWithJPsi","constituents"),
                             pdgId = cms.vint32(211,-211,11,13,-11,-13,22,130)
                         )
cs3CandsNoJPsi = cms.EDFilter("PFCandidateFwdPtrCollectionPdgIdFilter",
                             src = cms.InputTag("ak3PFXJetsWithJPsi","constituents"),
                             pdgId = cms.vint32(211,-211,11,13,-11,-13,22,130)
                         )

#here's the jpsi to insert back later
pfCandJPsi = cms.EDFilter("PdgIdPFCandidateSelector",
                          src = cms.InputTag("pfCandComposites"),
                          pdgId = cms.vint32(1)
)


#####  Hack the Cs jet inputs below ###############


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

########## Hack the Cs jet inputs ##############


# run the Cs subtraction on the J/Psi-jet, ignoring the Jpsi 
akCs4PFJets.src = cms.InputTag("cs4CandsNoJPsi")
akCs4PFJets.jetCollInstanceName = cms.string('pfParticlesCs4NoJPsi')
akCs3PFJets.src = cms.InputTag("cs3CandsNoJPsi")
akCs3PFJets.jetCollInstanceName = cms.string('pfParticlesCs3NoJPsi')
akFlowPuCs4PFJets.src = cms.InputTag("cs4CandsNoJPsi")
akFlowPuCs4PFJets.jetCollInstanceName = cms.string('pfParticlesFlowPuCs4NoJPsi')
akFlowPuCs3PFJets.src = cms.InputTag("cs3CandsNoJPsi")
akFlowPuCs3PFJets.jetCollInstanceName = cms.string('pfParticlesFlowPuCs3NoJPsi')


pfCandsCs4PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akCs4PFJets","pfParticlesCs4NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)
pfCandsCs3PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akCs3PFJets","pfParticlesCs3NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)
pfCandsFlowPuCs4PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akFlowPuCs4PFJets","pfParticlesFlowPuCs4NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)
pfCandsFlowPuCs3PlusJPsi = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(cms.InputTag("akFlowPuCs3PFJets","pfParticlesFlowPuCs3NoJPsi"),
                        cms.InputTag("pfCandJPsi"))    
)



akCs4PFJetsWithJPsi = ak4PFJets.clone(
    src = cms.InputTag("pfCandsCs4PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesCs4WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)
akCs3PFJetsWithJPsi = ak3PFJets.clone(
    src = cms.InputTag("pfCandsCs3PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesCs3WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)


akFlowPuCs4PFJetsWithJPsi = ak4PFJets.clone(
    src = cms.InputTag("pfCandsFlowPuCs4PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesFlowPuCs4WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)
akFlowPuCs3PFJetsWithJPsi = ak3PFJets.clone(
    src = cms.InputTag("pfCandsFlowPUCs3PlusJPsi"),
    jetCollInstanceName = cms.string('pfParticlesFlowPuCs3WithJPsi'),
    writeJetsWithConst = cms.bool(True)
)

akCs4PFmatch.src = "akCs4PFJetsWithJPsi"
akCs4PFparton.src = "akCs4PFJetsWithJPsi"
akCs4PFcorr.src = "akCs4PFJetsWithJPsi"
akCs4PFNjettiness.src = "akCs4PFJetsWithJPsi"
akCs4PFpatJetsWithBtagging.jetSource = "akCs4PFJetsWithJPsi"
akCs4PFpatJets.jetSource = "akCs4PFJetsWithJPsi"
akCs4PFPatJetPartonAssociationLegacy.jets = "akCs4PFJetsWithJPsi"
akCs4PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsCs4PlusJPsi')

akCs3PFmatch.src = "akCs3PFJetsWithJPsi"
akCs3PFparton.src = "akCs3PFJetsWithJPsi"
akCs3PFcorr.src = "akCs3PFJetsWithJPsi"
akCs3PFNjettiness.src = "akCs3PFJetsWithJPsi"
akCs3PFpatJetsWithBtagging.jetSource = "akCs3PFJetsWithJPsi"
akCs3PFpatJets.jetSource = "akCs3PFJetsWithJPsi"
akCs3PFPatJetPartonAssociationLegacy.jets = "akCs3PFJetsWithJPsi"
akCs3PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsCs3PlusJPsi')

akFlowPuCs4PFmatch.src = "akFlowPuCs4PFJetsWithJPsi"
akFlowPuCs4PFparton.src = "akFlowPuCs4PFJetsWithJPsi"
akFlowPuCs4PFcorr.src = "akFlowPuCs4PFJetsWithJPsi"
akFlowPuCs4PFNjettiness.src = "akFlowPuCs4PFJetsWithJPsi"
akFlowPuCs4PFpatJetsWithBtagging.jetSource = "akFlowPuCs4PFJetsWithJPsi"
akFlowPuCs4PFPatJetPartonAssociationLegacy.jets = "akFlowPuCs4PFJetsWithJPsi"
akFlowPuCs4PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsFlowPuCs4PlusJPsi')

akFlowPuCs3PFmatch.src = "akFlowPuCs3PFJetsWithJPsi"
akFlowPuCs3PFparton.src = "akFlowPuCs3PFJetsWithJPsi"
akFlowPuCs3PFcorr.src = "akFlowPuCs3PFJetsWithJPsi"
akFlowPuCs3PFNjettiness.src = "akFlowPuCs3PFJetsWithJPsi"
akFlowPuCs3PFpatJetsWithBtagging.jetSource = "akFlowPuCs3PFJetsWithJPsi"
akFlowPuCs3PFPatJetPartonAssociationLegacy.jets = "akFlowPuCs3PFJetsWithJPsi"
akFlowPuCs3PFJetAnalyzer.pfCandidateLabel = cms.untracked.InputTag('pfCandsFlowPuCs3PlusJPsi')



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

    pfCandJPsi +
    
    ak3PFJetsWithJPsi +
    ak3PFXJetsWithJPsi +
    cs3CandsNoJPsi +
    #akPu3CaloJets +
    #ak3PFJets +
    #akPu3PFJets +
    akCs3PFJets +
    pfCandsCs3PlusJPsi +
    akCs3PFJetsWithJPsi +

    ak4PFJetsWithJPsi +
    ak4PFXJetsWithJPsi +
    cs4CandsNoJPsi +
    #akPu4CaloJets +
    #ak4PFJets +
    #akPu4PFJets +
    akCs4PFJets +
    pfCandsCs4PlusJPsi +
    akCs4PFJetsWithJPsi +

    #Add our hybrid jets to collection
    akFlowPuCs3PFJets +
    pfCandsFlowPuCs3PlusJPsi + 
    akFlowPuCs4PFJets +
    pfCandsFlowPuCs4PlusJPsi + 

    #akPu3CaloJetSequence +
    #ak3PFJetSequence + 
    #akPu3PFJetSequence +
    akCs3PFJetSequence +

    #akPu4CaloJetSequence +
    #ak4PFJetSequence + 
    #akPu4PFJetSequence +
    akCs4PFJetSequence + 

    #Add our processing sequences
    akFlowPuCs3PFJetSequence +
    akFlowPuCs4PFJetSequence
)
