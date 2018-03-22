

import FWCore.ParameterSet.Config as cms
from HeavyIonsAnalysis.JetAnalysis.patHeavyIonSequences_cff import patJetGenJetMatch, patJetPartonMatch, patJetCorrFactors, patJets
from HeavyIonsAnalysis.JetAnalysis.inclusiveJetAnalyzer_cff import *
from HeavyIonsAnalysis.JetAnalysis.bTaggers_cff import *
from RecoJets.JetProducers.JetIDParams_cfi import *
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness

ak4PFmatch = patJetGenJetMatch.clone(
    src = cms.InputTag("ak4PFJets"),
    matched = cms.InputTag("ak4GenJets"),
    resolveByMatchQuality = cms.bool(False),
    maxDeltaR = 0.4
    )

ak4PFmatchGroomed = patJetGenJetMatch.clone(
    src = cms.InputTag("ak4GenJets"),
    matched = cms.InputTag("ak4GenJets"),
    resolveByMatchQuality = cms.bool(False),
    maxDeltaR = 0.4
    )

ak4PFparton = patJetPartonMatch.clone(src = cms.InputTag("ak4PFJets")
                                                        )

ak4PFcorr = patJetCorrFactors.clone(
    useNPV = cms.bool(False),
    useRho = cms.bool(False),
#    primaryVertices = cms.InputTag("hiSelectedVertex"),
    levels   = cms.vstring('L2Relative','L3Absolute'),
    src = cms.InputTag("ak4PFJets"),
    payload = "AK4PF_offline"
    )

ak4PFJetID= cms.EDProducer('JetIDProducer', JetIDParams, src = cms.InputTag('ak4CaloJets'))

#ak4PFclean   = heavyIonCleanedGenJets.clone(src = cms.InputTag('ak4GenJets'))

ak4PFbTagger = bTaggers("ak4PF",0.4,True,False)

#create objects locally since they dont load properly otherwise
#ak4PFmatch = ak4PFbTagger.match
ak4PFparton = patJetPartonMatch.clone(src = cms.InputTag("ak4PFJets"), matched = cms.InputTag("genParticles"))
ak4PFPatJetFlavourAssociationLegacy = ak4PFbTagger.PatJetFlavourAssociationLegacy
#ak4PFPatJetPartons = ak4PFbTagger.PatJetPartons
ak4PFJetTracksAssociatorAtVertex = ak4PFbTagger.JetTracksAssociatorAtVertex
ak4PFJetTracksAssociatorAtVertex.tracks = cms.InputTag("highPurityTracks")
ak4PFSimpleSecondaryVertexHighEffBJetTags = ak4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
ak4PFSimpleSecondaryVertexHighPurBJetTags = ak4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
ak4PFCombinedSecondaryVertexBJetTags = ak4PFbTagger.CombinedSecondaryVertexBJetTags
ak4PFCombinedSecondaryVertexV2BJetTags = ak4PFbTagger.CombinedSecondaryVertexV2BJetTags
ak4PFJetBProbabilityBJetTags = ak4PFbTagger.JetBProbabilityBJetTags
ak4PFPatJetPartonAssociationLegacy = ak4PFbTagger.PatJetPartonAssociationLegacy

ak4PFImpactParameterTagInfos = ak4PFbTagger.ImpactParameterTagInfos
ak4PFImpactParameterTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices")
ak4PFJetProbabilityBJetTags = ak4PFbTagger.JetProbabilityBJetTags

ak4PFSecondaryVertexTagInfos = ak4PFbTagger.SecondaryVertexTagInfos
ak4PFSimpleSecondaryVertexHighEffBJetTags = ak4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
ak4PFSimpleSecondaryVertexHighPurBJetTags = ak4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
ak4PFCombinedSecondaryVertexBJetTags = ak4PFbTagger.CombinedSecondaryVertexBJetTags
ak4PFCombinedSecondaryVertexV2BJetTags = ak4PFbTagger.CombinedSecondaryVertexV2BJetTags


ak4PFPatJetFlavourIdLegacy = cms.Sequence(ak4PFPatJetPartonAssociationLegacy*ak4PFPatJetFlavourAssociationLegacy)
#Not working with our PU sub
ak4PFPatJetFlavourAssociation = ak4PFbTagger.PatJetFlavourAssociation
#ak4PFPatJetFlavourId = cms.Sequence(ak4PFPatJetPartons*ak4PFPatJetFlavourAssociation)
ak4PFPatJetFlavourId = cms.Sequence(ak4PFPatJetFlavourAssociation)

#adding the subjet taggers
#SUBJETDUMMY_ak4PFSubjetImpactParameterTagInfos = ak4PFbTagger.SubjetImpactParameterTagInfos
#SUBJETDUMMY_ak4PFSubjetJetProbabilityBJetTags = ak4PFbTagger.SubjetJetProbabilityBJetTags
#SUBJETDUMMY_ak4PFSubjetSecondaryVertexTagInfos = ak4PFbTagger.SubjetSecondaryVertexTagInfos
#SUBJETDUMMY_ak4PFSubjetSecondaryVertexNegativeTagInfos = ak4PFbTagger.SubjetSecondaryVertexNegativeTagInfos
#SUBJETDUMMY_ak4PFSubjetJetTracksAssociatorAtVertex = ak4PFbTagger.SubjetJetTracksAssociatorAtVertex
#SUBJETDUMMY_ak4PFCombinedSubjetSecondaryVertexBJetTags = ak4PFbTagger.CombinedSubjetSecondaryVertexBJetTags
#SUBJETDUMMY_ak4PFCombinedSubjetSecondaryVertexV2BJetTags = ak4PFbTagger.CombinedSubjetSecondaryVertexV2BJetTags
#SUBJETDUMMY_ak4PFCombinedSubjetNegativeSecondaryVertexV2BJetTags = ak4PFbTagger.CombinedSubjetNegativeSecondaryVertexV2BJetTags

ak4PFJetBtaggingIP       = cms.Sequence(ak4PFImpactParameterTagInfos *
            (
             ak4PFJetProbabilityBJetTags +
             ak4PFJetBProbabilityBJetTags 
            )
            )

ak4PFJetBtaggingSV = cms.Sequence(ak4PFImpactParameterTagInfos
            *
            ak4PFSecondaryVertexTagInfos
            * (ak4PFSimpleSecondaryVertexHighEffBJetTags+
                ak4PFSimpleSecondaryVertexHighPurBJetTags+
                ak4PFCombinedSecondaryVertexBJetTags+
                ak4PFCombinedSecondaryVertexV2BJetTags
              )
            )


ak4PFJetBtagging = cms.Sequence(ak4PFJetBtaggingIP
            *ak4PFJetBtaggingSV
            )

ak4PFpatJetsWithBtagging = patJets.clone(jetSource = cms.InputTag("ak4PFJets"),
        genJetMatch          = cms.InputTag("ak4PFmatch"),
        genPartonMatch       = cms.InputTag("ak4PFparton"),
        jetCorrFactorsSource = cms.VInputTag(cms.InputTag("ak4PFcorr")),
        #JetPartonMapSource   = cms.InputTag("ak4PFPatJetFlavourAssociationLegacy"),
        JetPartonMapSource   = cms.InputTag("ak4PFPatJetFlavourAssociation"),
	JetFlavourInfoSource   = cms.InputTag("ak4PFPatJetFlavourAssociation"),
        trackAssociationSource = cms.InputTag("ak4PFJetTracksAssociatorAtVertex"),
	useLegacyJetMCFlavour = False,
        discriminatorSources = cms.VInputTag(cms.InputTag("ak4PFSimpleSecondaryVertexHighEffBJetTags"),
            cms.InputTag("ak4PFSimpleSecondaryVertexHighPurBJetTags"),
            cms.InputTag("ak4PFCombinedSecondaryVertexBJetTags"),
            cms.InputTag("ak4PFCombinedSecondaryVertexV2BJetTags"),
            cms.InputTag("ak4PFJetBProbabilityBJetTags"),
            cms.InputTag("ak4PFJetProbabilityBJetTags"),
            ),
        tagInfoSources = cms.VInputTag(
           cms.InputTag("ak4PFImpactParameterTagInfos"), cms.InputTag("ak4PFSecondaryVertexTagInfos")
        ),
        jetIDMap = cms.InputTag("ak4PFJetID"),
        addBTagInfo = True,
        addTagInfos = True,
        addDiscriminators = True,
        #M# addAssociatedTracks = True,
        addAssociatedTracks = False,
        addJetCharge = False,
        addJetID = False,
        getJetMCFlavour = True,
        addGenPartonMatch = True,
        addGenJetMatch = True,
        embedGenJetMatch = True,
        embedGenPartonMatch = True,
        # embedCaloTowers = False,
        # embedPFCandidates = True
        )

ak4PFNjettiness = Njettiness.clone(
		    src = cms.InputTag("ak4PFJets"),
           	    R0  = cms.double( 0.4)
)
ak4PFpatJetsWithBtagging.userData.userFloats.src += ['ak4PFNjettiness:tau1','ak4PFNjettiness:tau2','ak4PFNjettiness:tau3']

ak4PFJetAnalyzer = inclusiveJetAnalyzer.clone(jetTag = cms.InputTag("ak4PFpatJetsWithBtagging"),
                                                             genjetTag = 'ak4GenJets',
                                                             rParam = 0.4,
                                                             matchJets = cms.untracked.bool(False),
                                                             matchTag = 'patJetsWithBtagging',
                                                             pfCandidateLabel = cms.untracked.InputTag('particleFlow'),
                                                             trackTag = cms.InputTag("generalTracks"),
                                                             fillGenJets = True,
                                                             isMC = True,
							     doSubEvent = False,
                                                             useHepMC = cms.untracked.bool(False),
							     genParticles = cms.untracked.InputTag("genParticles"),
							     eventInfoTag = cms.InputTag("generator"),
                                                             doLifeTimeTagging = cms.untracked.bool(True),
                                                             doLifeTimeTaggingExtras = cms.untracked.bool(False),
                                                             bTagJetName = cms.untracked.string("ak4PF"),
                                                             jetName = cms.untracked.string("ak4PF"),
                                                             genPtMin = cms.untracked.double(5),
                                                             hltTrgResults = cms.untracked.string('TriggerResults::'+'HISIGNAL'),
							     doTower = cms.untracked.bool(False),
							     doSubJets = cms.untracked.bool(False),
                                                             doGenSubJets = cms.untracked.bool(False),     
                                                             subjetGenTag = cms.untracked.InputTag("ak4GenJets"),
							     doExtendedFlavorTagging = cms.untracked.bool(True),
							     jetFlavourInfos = cms.InputTag("ak4PFPatJetFlavourAssociation"),
							     subjetFlavourInfos = cms.InputTag("ak4PFPatJetFlavourAssociation","SubJets"),
							     groomedJets = cms.InputTag("ak4PFJets"),
							     isPythia6 = cms.untracked.bool(True),
                                                             doGenTaus = False
                                                            )

ak4PFJetSequence_mc = cms.Sequence(
                                                  #ak4PFclean
                                                  #*
                                                  ak4PFmatch
                                                  #*
                                                  #ak4PFmatchGroomed
                                                  *
                                                  ak4PFparton
                                                  *
                                                  ak4PFcorr
                                                  *
                                                  #ak4PFJetID
                                                  #*
                                                  #ak4PFPatJetFlavourIdLegacy  # works for PbPb
                                                  #*
			                          ak4PFPatJetFlavourId  # doesn't work for PbPb yet
                                                  *
                                                  ak4PFJetTracksAssociatorAtVertex
                                                  *
                                                  ak4PFJetBtagging
                                                  *
                                                  ak4PFNjettiness #No constituents for calo jets in pp. Must be removed for pp calo jets but I'm not sure how to do this transparently (Marta)
                                                  *
                                                  ak4PFpatJetsWithBtagging
                                                  *
                                                  ak4PFJetAnalyzer
                                                  )

ak4PFJetSequence_data = cms.Sequence(ak4PFcorr
                                                    *
                                                    #ak4PFJetID
                                                    #*
                                                    ak4PFJetTracksAssociatorAtVertex
                                                    *
                                                    ak4PFJetBtagging
                                                    *
                                                    ak4PFNjettiness 
                                                    *
                                                    ak4PFpatJetsWithBtagging
                                                    *
                                                    ak4PFJetAnalyzer
                                                    )

ak4PFJetSequence_jec = cms.Sequence(ak4PFJetSequence_mc)
ak4PFJetSequence_mb = cms.Sequence(ak4PFJetSequence_mc)

ak4PFJetSequence = cms.Sequence(ak4PFJetSequence_mc)
