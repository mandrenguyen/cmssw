import FWCore.ParameterSet.Config as cms

#from PhysicsTools.PatAlgos.patHeavyIonSequences_cff import *
from HeavyIonsAnalysis.JetAnalysis.patHeavyIonSequences_cff import *
from PhysicsTools.PatAlgos.mcMatchLayer0.jetFlavourId_cff import *


#load all the b-tagging algorithms

from RecoJets.JetAssociationProducers.ak5JTA_cff import *
from RecoBTag.Configuration.RecoBTag_cff import *

class bTaggers:
    def __init__(self,jetname,rParam,ispp,doSubjets):
        self.JetTracksAssociatorAtVertex = ak5JetTracksAssociatorAtVertex.clone()
        self.JetTracksAssociatorAtVertex.jets = cms.InputTag(jetname+"Jets")
	if(ispp):
		self.JetTracksAssociatorAtVertex.tracks = cms.InputTag("generalTracks")
	else:
        	self.JetTracksAssociatorAtVertex.tracks = cms.InputTag("hiGeneralTracks")
        #M# self.ImpactParameterTagInfos = impactParameterTagInfos.clone()
        self.ImpactParameterTagInfos = pfImpactParameterTagInfos.clone()
        self.ImpactParameterTagInfos.jets = cms.InputTag(jetname+"Jets")
        #M# self.ImpactParameterTagInfos.jetTracks = cms.InputTag(jetname+"JetTracksAssociatorAtVertex")
        self.ImpactParameterTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices")
        #M# self.JetProbabilityBJetTags                = jetProbabilityBJetTags.clone()
        self.JetProbabilityBJetTags                = pfJetProbabilityBJetTags.clone()
        self.JetProbabilityBJetTags.tagInfos       = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"))
        #M# self.JetBProbabilityBJetTags               = jetBProbabilityBJetTags.clone()
        self.JetBProbabilityBJetTags               = pfJetBProbabilityBJetTags.clone()
        self.JetBProbabilityBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"))

        # secondary vertex b-tag
        #self.SecondaryVertexTagInfos                     = secondaryVertexTagInfos.clone()
        #M# self.SecondaryVertexTagInfos                     = inclusiveSecondaryVertexFinderTagInfos.clone()
        self.SecondaryVertexTagInfos                     = pfInclusiveSecondaryVertexFinderTagInfos.clone()
        self.SecondaryVertexTagInfos.trackIPTagInfos     = cms.InputTag(jetname+"ImpactParameterTagInfos")
        #M# self.SimpleSecondaryVertexHighEffBJetTags               = simpleSecondaryVertexHighEffBJetTags.clone()
        self.SimpleSecondaryVertexHighEffBJetTags               = pfSimpleSecondaryVertexHighEffBJetTags.clone()
        self.SimpleSecondaryVertexHighEffBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"SecondaryVertexTagInfos"))
        #M# self.SimpleSecondaryVertexHighPurBJetTags               = simpleSecondaryVertexHighPurBJetTags.clone()
        self.SimpleSecondaryVertexHighPurBJetTags               = pfSimpleSecondaryVertexHighPurBJetTags.clone()
        self.SimpleSecondaryVertexHighPurBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"SecondaryVertexTagInfos"))
        #M# self.CombinedSecondaryVertexBJetTags             = combinedSecondaryVertexV2BJetTags.clone()
        self.CombinedSecondaryVertexBJetTags             = pfCombinedSecondaryVertexV2BJetTags.clone()
        self.CombinedSecondaryVertexBJetTags.tagInfos    = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                cms.InputTag(jetname+"SecondaryVertexTagInfos"))
        #M# self.CombinedSecondaryVertexV2BJetTags          = combinedSecondaryVertexV2BJetTags.clone()
        self.CombinedSecondaryVertexV2BJetTags          = pfCombinedSecondaryVertexV2BJetTags.clone()
        self.CombinedSecondaryVertexV2BJetTags.tagInfos = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                cms.InputTag(jetname+"SecondaryVertexTagInfos"))



	if doSubjets:
            #M# self.SubjetImpactParameterTagInfos = impactParameterTagInfos.clone()
            self.SubjetImpactParameterTagInfos = pfImpactParameterTagInfos.clone()
            #self.SubjetImpactParameterTagInfos.jets =  cms.InputTag("akSoftDrop4PFJets")  #M#
            self.SubjetImpactParameterTagInfos.explicitJTA = cms.bool(True)  #M#
            self.SubjetImpactParameterTagInfos.jets = cms.InputTag("akSoftDrop4PFJets","SubJets")  #M#
            self.SubjetImpactParameterTagInfos.maxDeltaR = 0.2  #M#
            #M# self.SubjetImpactParameterTagInfos.jetTracks = cms.InputTag(jetname+"SubjetJetTracksAssociatorAtVertex")
            #M# self.SubjetJetProbabilityBJetTags = jetProbabilityBJetTags.clone()
            self.SubjetJetProbabilityBJetTags = pfJetProbabilityBJetTags.clone()
            self.SubjetJetProbabilityBJetTags.tagInfos = cms.VInputTag(cms.InputTag(jetname+"SubjetImpactParameterTagInfos"))
            #M# self.SubjetSecondaryVertexTagInfos = secondaryVertexTagInfos.clone() 
            self.SubjetSecondaryVertexTagInfos = pfSecondaryVertexTagInfos.clone() 
            self.SubjetSecondaryVertexTagInfos.trackIPTagInfos = cms.InputTag(jetname+'SubjetImpactParameterTagInfos')
            self.SubjetSecondaryVertexTagInfos.fatJets = cms.InputTag('ak4PFJets')
            self.SubjetSecondaryVertexTagInfos.groomedFatJets = cms.InputTag(jetname+'Jets')
            self.SubjetSecondaryVertexTagInfos.useSVClustering = cms.bool(True)
            self.SubjetSecondaryVertexTagInfos.useExternalSV = cms.bool(True)
            self.SubjetSecondaryVertexTagInfos.jetAlgorithm = cms.string('AntiKt')
            self.SubjetSecondaryVertexTagInfos.useSVMomentum = cms.bool(True)
            self.SubjetSecondaryVertexTagInfos.rParam = cms.double(0.4)
            #M# self.SubjetSecondaryVertexTagInfos.extSVCollection = cms.InputTag('inclusiveSecondaryVertices')
            self.SubjetSecondaryVertexTagInfos.extSVCollection = cms.InputTag('inclusiveCandidateSecondaryVertices')
            #M# self.SubjetSecondaryVertexTagInfos.vertexCuts.maxDeltaRToJetAxis = cms.double(0.1)  
            self.SubjetSecondaryVertexTagInfos.vertexCuts.maxDeltaRToJetAxis = cms.double(0.2)  #M#  

            
            self.SubjetJetTracksAssociatorAtVertex = cms.EDProducer("JetTracksAssociatorExplicit",
                                                                    jets = cms.InputTag(jetname+'Jets','SubJets')
                                                                    )
            
            self.SubjetJetTracksAssociatorAtVertex.tracks = cms.InputTag('highPurityTracks')

           #M# self.CombinedSubjetSecondaryVertexV2BJetTags = combinedSecondaryVertexV2BJetTags.clone(
            self.CombinedSubjetSecondaryVertexV2BJetTags = pfCombinedSecondaryVertexV2BJetTags.clone(
                tagInfos = cms.VInputTag(cms.InputTag(jetname+"SubjetImpactParameterTagInfos"),
                                         cms.InputTag(jetname+"SubjetSecondaryVertexTagInfos"))
		)
            #M# self.CombinedSubjetSecondaryVertexBJetTags = combinedSecondaryVertexV2BJetTags.clone(
            self.CombinedSubjetSecondaryVertexBJetTags = pfCombinedSecondaryVertexV2BJetTags.clone(
                tagInfos = cms.VInputTag(cms.InputTag(jetname+"SubjetImpactParameterTagInfos"),
                                         cms.InputTag(jetname+"SubjetSecondaryVertexTagInfos"))
		)
            
        #M# self.JetTracksAssociator = cms.Sequence(self.JetTracksAssociatorAtVertex)
        self.JetBtaggingIP       = cms.Sequence(self.ImpactParameterTagInfos * (
                self.JetProbabilityBJetTags +
                self.JetBProbabilityBJetTags 
                )
                                                )

        self.JetBtaggingSV = cms.Sequence(self.ImpactParameterTagInfos *
                                          self.SecondaryVertexTagInfos * (self.SimpleSecondaryVertexHighEffBJetTags +
                                                                          self.SimpleSecondaryVertexHighPurBJetTags +
                                                                          self.CombinedSecondaryVertexBJetTags +
                                                                          self.CombinedSecondaryVertexV2BJetTags
                                                                          )
                                          )
        
        
        self.JetBtagging = cms.Sequence(self.JetBtaggingIP
                                        *self.JetBtaggingSV
                                        )

        self.PatJetPartonAssociationLegacy       = patJetPartonAssociationLegacy.clone(
            jets = cms.InputTag(jetname+"Jets"),
            partons = cms.InputTag("patJetPartonsLegacy")
            )

        self.PatJetFlavourAssociationLegacy      = patJetFlavourAssociationLegacy.clone(
            srcByReference = cms.InputTag(jetname+"PatJetPartonAssociationLegacy")
            )

        self.patJetFlavourIdLegacy = cms.Sequence( self.PatJetPartonAssociationLegacy * self.PatJetFlavourAssociationLegacy)

        self.PatJetFlavourAssociation = patJetFlavourAssociation.clone(
            jets = cms.InputTag(jetname+"Jets"),
            rParam = rParam,
            bHadrons = cms.InputTag("patJetPartons","bHadrons"),
            cHadrons = cms.InputTag("patJetPartons","cHadrons"),
            partons = cms.InputTag("patJetPartons","algorithmicPartons"),
            leptons = cms.InputTag("patJetPartons","leptons"),
            )

	if doSubjets:
		self.PatJetFlavourAssociation.jets="ak4PFJets"
		self.PatJetFlavourAssociation.groomedJets = cms.InputTag(jetname+'Jets')
		self.PatJetFlavourAssociation.subjets = cms.InputTag(jetname+'Jets', 'SubJets')

                self.PatJetFlavourId               = cms.Sequence(self.PatJetFlavourAssociation)

        self.parton  = patJetPartonMatch.clone(src      = cms.InputTag(jetname+"Jets"),
                                                matched = cms.InputTag("genParticles")
                                                )

