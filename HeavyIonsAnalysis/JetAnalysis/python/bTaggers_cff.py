import FWCore.ParameterSet.Config as cms

#from PhysicsTools.PatAlgos.patHeavyIonSequences_cff import *
from HeavyIonsAnalysis.JetAnalysis.patHeavyIonSequences_cff import *
from PhysicsTools.PatAlgos.mcMatchLayer0.jetFlavourId_cff import *


#load all the b-tagging algorithms
from RecoBTag.SecondaryVertex.secondaryVertexNegativeTagInfos_cfi import *
from RecoBTag.SecondaryVertex.negativeSimpleSecondaryVertexHighEffBJetTags_cfi import *
from RecoBTag.SecondaryVertex.negativeSimpleSecondaryVertexHighPurBJetTags_cfi import *
#from RecoBTag.SecondaryVertex.negativeCombinedSecondaryVertexComputer_cfi import *
#from RecoBTag.SecondaryVertex.negativeCombinedSecondaryVertexBJetTags_cfi import *
#from RecoBTag.SecondaryVertex.positiveCombinedSecondaryVertexComputer_cfi import *
#from RecoBTag.SecondaryVertex.positiveCombinedSecondaryVertexBJetTags_cfi import *
from RecoBTag.SecondaryVertex.negativeCombinedSecondaryVertexV2Computer_cfi import *
from RecoBTag.SecondaryVertex.negativeCombinedSecondaryVertexV2BJetTags_cfi import *
from RecoBTag.SecondaryVertex.positiveCombinedSecondaryVertexV2Computer_cfi import *
from RecoBTag.SecondaryVertex.positiveCombinedSecondaryVertexV2BJetTags_cfi import *

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
        self.ImpactParameterTagInfos = impactParameterTagInfos.clone()
        self.ImpactParameterTagInfos.jetTracks = cms.InputTag(jetname+"JetTracksAssociatorAtVertex")
        self.ImpactParameterTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices")
        self.TrackCountingHighEffBJetTags          = trackCountingHighEffBJetTags.clone()
        self.TrackCountingHighEffBJetTags.tagInfos = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"))
        self.TrackCountingHighPurBJetTags          = trackCountingHighPurBJetTags.clone()
        self.TrackCountingHighPurBJetTags.tagInfos = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"))
        self.JetProbabilityBJetTags                = jetProbabilityBJetTags.clone()
        self.JetProbabilityBJetTags.tagInfos       = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"))
        self.JetBProbabilityBJetTags               = jetBProbabilityBJetTags.clone()
        self.JetBProbabilityBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"))

        # secondary vertex b-tag
        #self.SecondaryVertexTagInfos                     = secondaryVertexTagInfos.clone()
        self.SecondaryVertexTagInfos                     = inclusiveSecondaryVertexFinderTagInfos.clone()
        self.SecondaryVertexTagInfos.trackIPTagInfos     = cms.InputTag(jetname+"ImpactParameterTagInfos")
        self.SimpleSecondaryVertexHighEffBJetTags               = simpleSecondaryVertexHighEffBJetTags.clone()
        self.SimpleSecondaryVertexHighEffBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"SecondaryVertexTagInfos"))
        self.SimpleSecondaryVertexHighPurBJetTags               = simpleSecondaryVertexHighPurBJetTags.clone()
        self.SimpleSecondaryVertexHighPurBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"SecondaryVertexTagInfos"))
        self.CombinedSecondaryVertexBJetTags             = combinedSecondaryVertexV2BJetTags.clone()
        self.CombinedSecondaryVertexBJetTags.tagInfos    = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                cms.InputTag(jetname+"SecondaryVertexTagInfos"))
        self.CombinedSecondaryVertexV2BJetTags          = combinedSecondaryVertexV2BJetTags.clone()
        self.CombinedSecondaryVertexV2BJetTags.tagInfos = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                cms.InputTag(jetname+"SecondaryVertexTagInfos"))

        self.SecondaryVertexNegativeTagInfos                     = secondaryVertexNegativeTagInfos.clone()
        self.SecondaryVertexNegativeTagInfos.trackIPTagInfos     = cms.InputTag(jetname+"ImpactParameterTagInfos")

        self.NegativeSimpleSecondaryVertexHighEffBJetTags               = negativeSimpleSecondaryVertexHighEffBJetTags.clone()
        self.NegativeSimpleSecondaryVertexHighEffBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"SecondaryVertexNegativeTagInfos"))
        self.NegativeSimpleSecondaryVertexHighPurBJetTags               = negativeSimpleSecondaryVertexHighPurBJetTags.clone()
        self.NegativeSimpleSecondaryVertexHighPurBJetTags.tagInfos      = cms.VInputTag(cms.InputTag(jetname+"SecondaryVertexNegativeTagInfos"))

        self.NegativeCombinedSecondaryVertexBJetTags                    = negativeCombinedSecondaryVertexV2BJetTags.clone()
        self.NegativeCombinedSecondaryVertexBJetTags.tagInfos    = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                                                                                 cms.InputTag(jetname+"SecondaryVertexNegativeTagInfos"))
        self.PositiveCombinedSecondaryVertexBJetTags                    = positiveCombinedSecondaryVertexV2BJetTags.clone()
        self.PositiveCombinedSecondaryVertexBJetTags.tagInfos    = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                                                                                 cms.InputTag(jetname+"SecondaryVertexTagInfos"))

        self.NegativeCombinedSecondaryVertexV2BJetTags                    = negativeCombinedSecondaryVertexV2BJetTags.clone()
        self.NegativeCombinedSecondaryVertexV2BJetTags.tagInfos    = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                                                                                 cms.InputTag(jetname+"SecondaryVertexNegativeTagInfos"))
        self.PositiveCombinedSecondaryVertexV2BJetTags                    = positiveCombinedSecondaryVertexV2BJetTags.clone()
        self.PositiveCombinedSecondaryVertexV2BJetTags.tagInfos    = cms.VInputTag(cms.InputTag(jetname+"ImpactParameterTagInfos"),
                                                                                 cms.InputTag(jetname+"SecondaryVertexTagInfos"))

        self.SoftPFMuonsTagInfos                = softPFMuonsTagInfos.clone()
        self.SoftPFMuonsTagInfos.jets           = cms.InputTag(jetname+"Jets")
        #self.SoftPFMuonsTagInfos.primaryVertex  = cms.InputTag("offlinePrimaryVertices")
        self.SoftPFMuonBJetTags                = softPFMuonBJetTags.clone()
        self.SoftPFMuonBJetTags.tagInfos       = cms.VInputTag(cms.InputTag(jetname+"SoftPFMuonsTagInfos"))
        self.SoftPFMuonByIP3dBJetTags          = softPFMuonByIP3dBJetTags.clone()
        self.SoftPFMuonByIP3dBJetTags.tagInfos = cms.VInputTag(cms.InputTag(jetname+"SoftPFMuonsTagInfos"))
        self.SoftPFMuonByPtBJetTags            = softPFMuonByPtBJetTags.clone()
        self.SoftPFMuonByPtBJetTags.tagInfos   = cms.VInputTag(cms.InputTag(jetname+"SoftPFMuonsTagInfos"))

        self.PositiveSoftPFMuonByPtBJetTags                = positiveSoftPFMuonByPtBJetTags.clone()
        self.PositiveSoftPFMuonByPtBJetTags.tagInfos       = cms.VInputTag(cms.InputTag(jetname+"SoftPFMuonsTagInfos"))

        # soft muon negative taggers
        self.NegativeSoftPFMuonByPtBJetTags                = negativeSoftPFMuonByPtBJetTags.clone()
        self.NegativeSoftPFMuonByPtBJetTags.tagInfos       = cms.VInputTag(cms.InputTag(jetname+"SoftPFMuonsTagInfos"))

	if doSubjets:
		self.SubjetImpactParameterTagInfos = impactParameterTagInfos.clone()
		self.SubjetImpactParameterTagInfos.jetTracks = cms.InputTag(jetname+"SubjetJetTracksAssociatorAtVertex")
		self.SubjetJetProbabilityBJetTags = jetProbabilityBJetTags.clone()
		self.SubjetJetProbabilityBJetTags.tagInfos = cms.VInputTag(cms.InputTag(jetname+"SubjetImpactParameterTagInfos"))
		self.SubjetSecondaryVertexTagInfos = secondaryVertexTagInfos.clone() 
		self.SubjetSecondaryVertexTagInfos.trackIPTagInfos = cms.InputTag(jetname+'SubjetImpactParameterTagInfos')
		self.SubjetSecondaryVertexTagInfos.fatJets = cms.InputTag('ak4PFJets')
		self.SubjetSecondaryVertexTagInfos.groomedFatJets = cms.InputTag(jetname+'Jets')
		self.SubjetSecondaryVertexTagInfos.useSVClustering = cms.bool(True)
		self.SubjetSecondaryVertexTagInfos.useExternalSV = cms.bool(True)
		self.SubjetSecondaryVertexTagInfos.jetAlgorithm = cms.string('AntiKt')
		self.SubjetSecondaryVertexTagInfos.useSVMomentum = cms.bool(True)
		self.SubjetSecondaryVertexTagInfos.rParam = cms.double(0.4)
		self.SubjetSecondaryVertexTagInfos.extSVCollection = cms.InputTag('inclusiveSecondaryVertices')
		self.SubjetSecondaryVertexTagInfos.vertexCuts.maxDeltaRToJetAxis = cms.double(0.1)

		self.SubjetSecondaryVertexNegativeTagInfos = self.SubjetSecondaryVertexTagInfos.clone()
		self.SubjetSecondaryVertexNegativeTagInfos.vertexCuts.distVal2dMin = -2.5
		self.SubjetSecondaryVertexNegativeTagInfos.vertexCuts.distVal2dMax = -0.01
		self.SubjetSecondaryVertexNegativeTagInfos.vertexCuts.distSig2dMin = -99999.9
		self.SubjetSecondaryVertexNegativeTagInfos.vertexCuts.distSig2dMax = -3.0
		self.SubjetSecondaryVertexNegativeTagInfos.vertexCuts.maxDeltaRToJetAxis = -0.5

		self.SubjetJetTracksAssociatorAtVertex = cms.EDProducer("JetTracksAssociatorExplicit",
			jets = cms.InputTag(jetname+'Jets','SubJets')
		)
		if ispp:
			self.SubjetJetTracksAssociatorAtVertex.tracks = cms.InputTag('highPurityTracks')
		else:
			self.SubjetJetTracksAssociatorAtVertex.tracks = cms.InputTag('highPurityTracks')

		self.CombinedSubjetSecondaryVertexV2BJetTags = combinedSecondaryVertexV2BJetTags.clone(
			tagInfos = cms.VInputTag(cms.InputTag(jetname+"SubjetImpactParameterTagInfos"),
			cms.InputTag(jetname+"SubjetSecondaryVertexTagInfos"))
		)
		self.CombinedSubjetSecondaryVertexBJetTags = combinedSecondaryVertexV2BJetTags.clone(
			tagInfos = cms.VInputTag(cms.InputTag(jetname+"SubjetImpactParameterTagInfos"),
			cms.InputTag(jetname+"SubjetSecondaryVertexTagInfos"))
		)
		self.CombinedSubjetNegativeSecondaryVertexV2BJetTags = negativeCombinedSecondaryVertexV2BJetTags.clone(
                        tagInfos = cms.VInputTag(cms.InputTag(jetname+"SubjetImpactParameterTagInfos"),
                        cms.InputTag(jetname+"SubjetSecondaryVertexNegativeTagInfos"))
                ) 

        self.JetTracksAssociator = cms.Sequence(self.JetTracksAssociatorAtVertex)
        self.JetBtaggingIP       = cms.Sequence(self.ImpactParameterTagInfos * (
                self.TrackCountingHighEffBJetTags +
                self.TrackCountingHighPurBJetTags +
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

        self.JetBtaggingNegSV = cms.Sequence(self.ImpactParameterTagInfos *
                self.SecondaryVertexNegativeTagInfos * (self.NegativeSimpleSecondaryVertexHighEffBJetTags +
                    self.NegativeSimpleSecondaryVertexHighPurBJetTags +
                    self.NegativeCombinedSecondaryVertexBJetTags +
                    self.PositiveCombinedSecondaryVertexBJetTags +
                    self.NegativeCombinedSecondaryVertexV2BJetTags +
                    self.PositiveCombinedSecondaryVertexV2BJetTags
                    )
                )


        self.JetBtaggingMu = cms.Sequence(self.SoftPFMuonsTagInfos 
                                          * (self.SoftPFMuonBJetTags +
                                             self.SoftPFMuonByIP3dBJetTags +
                                             self.SoftPFMuonByPtBJetTags +
                                             self.NegativeSoftPFMuonByPtBJetTags +
                                             self.PositiveSoftPFMuonByPtBJetTags
            )
                                          )

        self.JetBtagging = cms.Sequence(self.JetBtaggingIP
                *self.JetBtaggingSV
                *self.JetBtaggingNegSV
                *self.JetBtaggingMu
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

                #self.PatJetFlavourId               = cms.Sequence(self.PatJetPartons*self.PatJetFlavourAssociation)
                self.PatJetFlavourId               = cms.Sequence(self.PatJetFlavourAssociation)
        #self.match   = patJetGenJetMatch.clone(
        #    src      = cms.InputTag(jetname+"Jets"),
        #    matched  = cms.InputTag(jetname+"clean"),
        #    maxDeltaR = rParam 
        #    )
        self.parton  = patJetPartonMatch.clone(src      = cms.InputTag(jetname+"Jets"),
                                                matched = cms.InputTag("genParticles")
                                                )

