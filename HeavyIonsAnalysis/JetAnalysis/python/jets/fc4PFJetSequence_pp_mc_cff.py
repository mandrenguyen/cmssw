

import FWCore.ParameterSet.Config as cms
from HeavyIonsAnalysis.JetAnalysis.patHeavyIonSequences_cff import patJetGenJetMatch, patJetPartonMatch, patJetCorrFactors, patJets
from HeavyIonsAnalysis.JetAnalysis.inclusiveJetAnalyzer_cff import *
from HeavyIonsAnalysis.JetAnalysis.bTaggers_cff import *
from RecoJets.JetProducers.JetIDParams_cfi import *
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness

fc4PFmatch = patJetGenJetMatch.clone(
    src = cms.InputTag("fc4PFJets"),
    matched = cms.InputTag("fc4GenJets"),
    resolveByMatchQuality = cms.bool(False),
    maxDeltaR = 0.4
    )

fc4PFmatchGroomed = patJetGenJetMatch.clone(
    src = cms.InputTag("fc4GenJets"),
    matched = cms.InputTag("fc4GenJets"),
    resolveByMatchQuality = cms.bool(False),
    maxDeltaR = 0.4
    )

fc4PFparton = patJetPartonMatch.clone(src = cms.InputTag("fc4PFJets")
                                                        )

fc4PFcorr = patJetCorrFactors.clone(
    useNPV = cms.bool(False),
    useRho = cms.bool(False),
#    primaryVertices = cms.InputTag("hiSelectedVertex"),
    levels   = cms.vstring('L2Relative','L3Absolute'),
    src = cms.InputTag("fc4PFJets"),
    payload = "AK4PF_offline"
    )

fc4PFJetID= cms.EDProducer('JetIDProducer', JetIDParams, src = cms.InputTag('ak4CaloJets'))

#fc4PFclean   = heavyIonCleanedGenJets.clone(src = cms.InputTag('ak4GenJets'))

fc4PFbTagger = bTaggers("fc4PF",0.4,True,False)

#create objects locally since they dont load properly otherwise
#fc4PFmatch = fc4PFbTagger.match
fc4PFparton = patJetPartonMatch.clone(src = cms.InputTag("fc4PFJets"), matched = cms.InputTag("genParticles"))
fc4PFPatJetFlavourAssociationLegacy = fc4PFbTagger.PatJetFlavourAssociationLegacy
#fc4PFPatJetPartons = fc4PFbTagger.PatJetPartons
fc4PFJetTracksAssociatorAtVertex = fc4PFbTagger.JetTracksAssociatorAtVertex

#fc4PFJetTracksAssociatorAtVertex = cms.EDProducer("JetTracksAssociatorExplicit",
#                                                  jets = cms.InputTag("fc4PFJets")
#                                                  )

fc4PFJetTracksAssociatorAtVertex.tracks = cms.InputTag("highPurityTracks")
fc4PFSimpleSecondaryVertexHighEffBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
fc4PFSimpleSecondaryVertexHighPurBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
fc4PFCombinedSecondaryVertexBJetTags = fc4PFbTagger.CombinedSecondaryVertexBJetTags
fc4PFCombinedSecondaryVertexV2BJetTags = fc4PFbTagger.CombinedSecondaryVertexV2BJetTags
fc4PFJetBProbabilityBJetTags = fc4PFbTagger.JetBProbabilityBJetTags
fc4PFSoftPFMuonByPtBJetTags = fc4PFbTagger.SoftPFMuonByPtBJetTags
fc4PFSoftPFMuonByIP3dBJetTags = fc4PFbTagger.SoftPFMuonByIP3dBJetTags
fc4PFTrackCountingHighEffBJetTags = fc4PFbTagger.TrackCountingHighEffBJetTags
fc4PFTrackCountingHighPurBJetTags = fc4PFbTagger.TrackCountingHighPurBJetTags
fc4PFPatJetPartonAssociationLegacy = fc4PFbTagger.PatJetPartonAssociationLegacy

fc4PFImpactParameterTagInfos = fc4PFbTagger.ImpactParameterTagInfos
fc4PFImpactParameterTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices")
fc4PFJetProbabilityBJetTags = fc4PFbTagger.JetProbabilityBJetTags

fc4PFSecondaryVertexTagInfos = fc4PFbTagger.SecondaryVertexTagInfos
fc4PFSimpleSecondaryVertexHighEffBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
fc4PFSimpleSecondaryVertexHighPurBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
fc4PFCombinedSecondaryVertexBJetTags = fc4PFbTagger.CombinedSecondaryVertexBJetTags
fc4PFCombinedSecondaryVertexV2BJetTags = fc4PFbTagger.CombinedSecondaryVertexV2BJetTags

fc4PFSecondaryVertexNegativeTagInfos = fc4PFbTagger.SecondaryVertexNegativeTagInfos
fc4PFNegativeSimpleSecondaryVertexHighEffBJetTags = fc4PFbTagger.NegativeSimpleSecondaryVertexHighEffBJetTags
fc4PFNegativeSimpleSecondaryVertexHighPurBJetTags = fc4PFbTagger.NegativeSimpleSecondaryVertexHighPurBJetTags
fc4PFNegativeCombinedSecondaryVertexBJetTags = fc4PFbTagger.NegativeCombinedSecondaryVertexBJetTags
fc4PFPositiveCombinedSecondaryVertexBJetTags = fc4PFbTagger.PositiveCombinedSecondaryVertexBJetTags
fc4PFNegativeCombinedSecondaryVertexV2BJetTags = fc4PFbTagger.NegativeCombinedSecondaryVertexV2BJetTags
fc4PFPositiveCombinedSecondaryVertexV2BJetTags = fc4PFbTagger.PositiveCombinedSecondaryVertexV2BJetTags

fc4PFSoftPFMuonsTagInfos = fc4PFbTagger.SoftPFMuonsTagInfos
fc4PFSoftPFMuonsTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices")
fc4PFSoftPFMuonBJetTags = fc4PFbTagger.SoftPFMuonBJetTags
fc4PFSoftPFMuonByIP3dBJetTags = fc4PFbTagger.SoftPFMuonByIP3dBJetTags
fc4PFSoftPFMuonByPtBJetTags = fc4PFbTagger.SoftPFMuonByPtBJetTags
fc4PFNegativeSoftPFMuonByPtBJetTags = fc4PFbTagger.NegativeSoftPFMuonByPtBJetTags
fc4PFPositiveSoftPFMuonByPtBJetTags = fc4PFbTagger.PositiveSoftPFMuonByPtBJetTags
fc4PFPatJetFlavourIdLegacy = cms.Sequence(fc4PFPatJetPartonAssociationLegacy*fc4PFPatJetFlavourAssociationLegacy)
#Not working with our PU sub
fc4PFPatJetFlavourAssociation = fc4PFbTagger.PatJetFlavourAssociation
#fc4PFPatJetFlavourId = cms.Sequence(fc4PFPatJetPartons*fc4PFPatJetFlavourAssociation)
fc4PFPatJetFlavourId = cms.Sequence(fc4PFPatJetFlavourAssociation)

#adding the subjet taggers
#SUBJETDUMMY_fc4PFSubjetImpactParameterTagInfos = fc4PFbTagger.SubjetImpactParameterTagInfos
#SUBJETDUMMY_fc4PFSubjetJetProbabilityBJetTags = fc4PFbTagger.SubjetJetProbabilityBJetTags
#SUBJETDUMMY_fc4PFSubjetSecondaryVertexTagInfos = fc4PFbTagger.SubjetSecondaryVertexTagInfos
#SUBJETDUMMY_fc4PFSubjetSecondaryVertexNegativeTagInfos = fc4PFbTagger.SubjetSecondaryVertexNegativeTagInfos
#SUBJETDUMMY_fc4PFSubjetJetTracksAssociatorAtVertex = fc4PFbTagger.SubjetJetTracksAssociatorAtVertex
#SUBJETDUMMY_fc4PFCombinedSubjetSecondaryVertexBJetTags = fc4PFbTagger.CombinedSubjetSecondaryVertexBJetTags
#SUBJETDUMMY_fc4PFCombinedSubjetSecondaryVertexV2BJetTags = fc4PFbTagger.CombinedSubjetSecondaryVertexV2BJetTags
#SUBJETDUMMY_fc4PFCombinedSubjetNegativeSecondaryVertexV2BJetTags = fc4PFbTagger.CombinedSubjetNegativeSecondaryVertexV2BJetTags

fc4PFJetBtaggingIP       = cms.Sequence(fc4PFImpactParameterTagInfos *
            (fc4PFTrackCountingHighEffBJetTags +
             fc4PFTrackCountingHighPurBJetTags +
             fc4PFJetProbabilityBJetTags +
             fc4PFJetBProbabilityBJetTags 
            )
            )

fc4PFJetBtaggingSV = cms.Sequence(fc4PFImpactParameterTagInfos
            *
            fc4PFSecondaryVertexTagInfos
            * (fc4PFSimpleSecondaryVertexHighEffBJetTags+
                fc4PFSimpleSecondaryVertexHighPurBJetTags+
                fc4PFCombinedSecondaryVertexBJetTags+
                fc4PFCombinedSecondaryVertexV2BJetTags
              )
            )

fc4PFJetBtaggingNegSV = cms.Sequence(fc4PFImpactParameterTagInfos
            *
            fc4PFSecondaryVertexNegativeTagInfos
            * (fc4PFNegativeSimpleSecondaryVertexHighEffBJetTags+
                fc4PFNegativeSimpleSecondaryVertexHighPurBJetTags+
                fc4PFNegativeCombinedSecondaryVertexBJetTags+
                fc4PFPositiveCombinedSecondaryVertexBJetTags+
                fc4PFNegativeCombinedSecondaryVertexV2BJetTags+
                fc4PFPositiveCombinedSecondaryVertexV2BJetTags
              )
            )

fc4PFJetBtaggingMu = cms.Sequence(fc4PFSoftPFMuonsTagInfos * (fc4PFSoftPFMuonBJetTags
                +
                fc4PFSoftPFMuonByIP3dBJetTags
                +
                fc4PFSoftPFMuonByPtBJetTags
                +
                fc4PFNegativeSoftPFMuonByPtBJetTags
                +
                fc4PFPositiveSoftPFMuonByPtBJetTags
              )
            )

fc4PFJetBtagging = cms.Sequence(fc4PFJetBtaggingIP
            *fc4PFJetBtaggingSV
            *fc4PFJetBtaggingNegSV
#            *fc4PFJetBtaggingMu
            )

fc4PFpatJetsWithBtagging = patJets.clone(jetSource = cms.InputTag("fc4PFJets"),
        genJetMatch          = cms.InputTag("fc4PFmatch"),
        genPartonMatch       = cms.InputTag("fc4PFparton"),
        jetCorrFactorsSource = cms.VInputTag(cms.InputTag("fc4PFcorr")),
        #JetPartonMapSource   = cms.InputTag("fc4PFPatJetFlavourAssociationLegacy"),
        JetPartonMapSource   = cms.InputTag("fc4PFPatJetFlavourAssociation"),
	JetFlavourInfoSource   = cms.InputTag("fc4PFPatJetFlavourAssociation"),
        trackAssociationSource = cms.InputTag("fc4PFJetTracksAssociatorAtVertex"),
	useLegacyJetMCFlavour = False,
        discriminatorSources = cms.VInputTag(cms.InputTag("fc4PFSimpleSecondaryVertexHighEffBJetTags"),
            cms.InputTag("fc4PFSimpleSecondaryVertexHighPurBJetTags"),
            cms.InputTag("fc4PFCombinedSecondaryVertexBJetTags"),
            cms.InputTag("fc4PFCombinedSecondaryVertexV2BJetTags"),
            cms.InputTag("fc4PFJetBProbabilityBJetTags"),
            cms.InputTag("fc4PFJetProbabilityBJetTags"),
            #cms.InputTag("fc4PFSoftPFMuonByPtBJetTags"),
            #cms.InputTag("fc4PFSoftPFMuonByIP3dBJetTags"),
            cms.InputTag("fc4PFTrackCountingHighEffBJetTags"),
            cms.InputTag("fc4PFTrackCountingHighPurBJetTags"),
            ),
        jetIDMap = cms.InputTag("fc4PFJetID"),
        addBTagInfo = True,
        addTagInfos = True,
        addDiscriminators = True,
        addAssociatedTracks = True,
        addJetCharge = False,
        addJetID = False,
        getJetMCFlavour = True,
        addGenPartonMatch = True,
        addGenJetMatch = True,
        embedGenJetMatch = True,
        embedGenPartonMatch = True,
        # embedCaloTowers = False,
        # embedPFCandidates = True
                                         addJetCorrFactors = cms.bool(False),  # Applying ak4 JEC gives crazy results for some reason
        )

fc4PFNjettiness = Njettiness.clone(
		    src = cms.InputTag("fc4PFJets"),
           	    R0  = cms.double( 0.4)
)
fc4PFpatJetsWithBtagging.userData.userFloats.src += ['fc4PFNjettiness:tau1','fc4PFNjettiness:tau2','fc4PFNjettiness:tau3']

fc4PFJetAnalyzer = inclusiveJetAnalyzer.clone(jetTag = cms.InputTag("fc4PFpatJetsWithBtagging"),
                                              genjetTag = 'fc4GenJets',
                                              rParam = 0.4,
                                              matchJets = cms.untracked.bool(True),
                                              matchTag = 'ak4PFpatJetsWithBtagging',
                                              pfCandidateLabel = cms.untracked.InputTag('particleFlow'),
                                              trackTag = cms.InputTag("generalTracks"),
                                              fillGenJets = True,
                                              isMC = True,
                                              doSubEvent = True,
                                              useHepMC = cms.untracked.bool(False),
                                              genParticles = cms.untracked.InputTag("genParticles"),
                                              eventInfoTag = cms.InputTag("generator"),
                                              doLifeTimeTagging = cms.untracked.bool(True),
                                              doLifeTimeTaggingExtras = cms.untracked.bool(False),
                                              bTagJetName = cms.untracked.string("fc4PF"),
                                              jetName = cms.untracked.string("fc4PF"),
                                              genPtMin = cms.untracked.double(5),
                                              hltTrgResults = cms.untracked.string('TriggerResults::'+'HISIGNAL'),
                                              doTower = cms.untracked.bool(False),
                                              doSubJets = cms.untracked.bool(False),
                                              doGenSubJets = cms.untracked.bool(False),     
                                              subjetGenTag = cms.untracked.InputTag("fc4GenJets"),
                                              doExtendedFlavorTagging = cms.untracked.bool(True),
                                              jetFlavourInfos = cms.InputTag("fc4PFPatJetFlavourAssociation"),
                                              subjetFlavourInfos = cms.InputTag("fc4PFPatJetFlavourAssociation","SubJets"),
                                              groomedJets = cms.InputTag("fc4PFJets"),
                                              isPythia6 = cms.untracked.bool(False),
                                              doGenTaus = True,
                                              useJEC = cms.untracked.bool(False)
                                                            )

fc4PFJetSequence_mc = cms.Sequence(
                                                  #fc4PFclean
                                                  #*
                                                  fc4PFmatch
                                                  #*
                                                  #fc4PFmatchGroomed
                                                  *
                                                  fc4PFparton
                                                  *
                                                  fc4PFcorr
                                                  *
                                                  #fc4PFJetID
                                                  #*
                                                  #fc4PFPatJetFlavourIdLegacy  # works for PbPb
                                                  #*
			                          fc4PFPatJetFlavourId  # doesn't work for PbPb yet
                                                  *
                                                  fc4PFJetTracksAssociatorAtVertex
                                                  *
                                                  fc4PFJetBtagging
                                                  *
                                                  fc4PFNjettiness #No constituents for calo jets in pp. Must be removed for pp calo jets but I'm not sure how to do this transparently (Marta)
                                                  *
                                                  fc4PFpatJetsWithBtagging
                                                  *
                                                  fc4PFJetAnalyzer
                                                  )

fc4PFJetSequence_data = cms.Sequence(fc4PFcorr
                                                    *
                                                    #fc4PFJetID
                                                    #*
                                                    fc4PFJetTracksAssociatorAtVertex
                                                    *
                                                    fc4PFJetBtagging
                                                    *
                                                    fc4PFNjettiness 
                                                    *
                                                    fc4PFpatJetsWithBtagging
                                                    *
                                                    fc4PFJetAnalyzer
                                                    )

fc4PFJetSequence_jec = cms.Sequence(fc4PFJetSequence_mc)
fc4PFJetSequence_mb = cms.Sequence(fc4PFJetSequence_mc)

fc4PFJetSequence = cms.Sequence(fc4PFJetSequence_mc)
