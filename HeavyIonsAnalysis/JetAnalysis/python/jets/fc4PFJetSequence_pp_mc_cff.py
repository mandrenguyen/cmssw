

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
#fc4PFJetTracksAssociatorAtVertex = fc4PFbTagger.JetTracksAssociatorAtVertex
'''
fc4PFJetTracksAssociatorAtVertex = cms.EDProducer("JetTracksAssociatorExplicit",
                                                  jets = cms.InputTag("fc4PFJets")
                                                  )
'''
#fc4PFJetTracksAssociatorAtVertex.tracks = cms.InputTag("highPurityTracks")
fc4PFSimpleSecondaryVertexHighEffBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
fc4PFSimpleSecondaryVertexHighPurBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
fc4PFCombinedSecondaryVertexBJetTags = fc4PFbTagger.CombinedSecondaryVertexBJetTags
fc4PFCombinedSecondaryVertexV2BJetTags = fc4PFbTagger.CombinedSecondaryVertexV2BJetTags
fc4PFJetBProbabilityBJetTags = fc4PFbTagger.JetBProbabilityBJetTags
fc4PFPatJetPartonAssociationLegacy = fc4PFbTagger.PatJetPartonAssociationLegacy

#fc4PFImpactParameterTagInfos = fc4PFbTagger.ImpactParameterTagInfos
#fc4PFImpactParameterTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices")

fc4PFImpactParameterTagInfos = cms.EDProducer("CandIPProducer",
    candidates = cms.InputTag("particleFlow"),
    computeGhostTrack = cms.bool(True),
    computeProbabilities = cms.bool(True),
    ghostTrackPriorDeltaR = cms.double(0.03),
    jetDirectionUsingGhostTrack = cms.bool(False),
    jetDirectionUsingTracks = cms.bool(False),
    jets = cms.InputTag("fc4PFJets"),
    maxDeltaR = cms.double(0.4),
    maximumChiSquared = cms.double(5.0),
    maximumLongitudinalImpactParameter = cms.double(17.0),
    maximumTransverseImpactParameter = cms.double(0.2),
    minimumNumberOfHits = cms.int32(0),
    minimumNumberOfPixelHits = cms.int32(1),
    minimumTransverseMomentum = cms.double(1.0),
    primaryVertex = cms.InputTag("offlinePrimaryVertices"),
    useTrackQuality = cms.bool(False)
)

fc4PFJetProbabilityBJetTags = fc4PFbTagger.JetProbabilityBJetTags

candidateJetProbabilityComputer = cms.ESProducer("CandidateJetProbabilityESProducer",
    a_dR = cms.double(-0.001053),
    a_pT = cms.double(0.005263),
    b_dR = cms.double(0.6263),
    b_pT = cms.double(0.3684),
    deltaR = cms.double(0.3),
    impactParameterType = cms.int32(0),
    max_pT = cms.double(500),
    max_pT_dRcut = cms.double(0.1),
    max_pT_trackPTcut = cms.double(3),
    maximumDecayLength = cms.double(5.0),
    maximumDistanceToJetAxis = cms.double(0.07),
    min_pT = cms.double(120),
    min_pT_dRcut = cms.double(0.5),
    minimumProbability = cms.double(0.005),
    trackIpSign = cms.int32(1),
    trackQualityClass = cms.string('any'),
    useVariableJTA = cms.bool(False)
)

fc4PFJetProbabilityBJetTags.jetTagComputer = cms.string('candidateJetProbabilityComputer')

candidateJetBProbabilityComputer = cms.ESProducer("CandidateJetBProbabilityESProducer",
    a_dR = cms.double(-0.001053),
    a_pT = cms.double(0.005263),
    b_dR = cms.double(0.6263),
    b_pT = cms.double(0.3684),
    deltaR = cms.double(-1.0),
    impactParameterType = cms.int32(0),
    max_pT = cms.double(500),
    max_pT_dRcut = cms.double(0.1),
    max_pT_trackPTcut = cms.double(3),
    maximumDecayLength = cms.double(5.0),
    maximumDistanceToJetAxis = cms.double(0.07),
    min_pT = cms.double(120),
    min_pT_dRcut = cms.double(0.5),
    minimumProbability = cms.double(0.005),
    numberOfBTracks = cms.uint32(4),
    trackIpSign = cms.int32(1),
    trackQualityClass = cms.string('any'),
    useVariableJTA = cms.bool(False)
)

fc4PFJetBProbabilityBJetTags.jetTagComputer = cms.string('candidateJetBProbabilityComputer')

# use ghost clustering of SVs
#fc4PFSecondaryVertexTagInfos = fc4PFbTagger.SecondaryVertexTagInfos
'''
fc4PFSecondaryVertexTagInfos.useSVClustering = cms.bool(True)
fc4PFSecondaryVertexTagInfos.jetAlgorithm = cms.string('AntiKt')
fc4PFSecondaryVertexTagInfos.rParam = cms.double(0.4)
'''


fc4PFSecondaryVertexTagInfos = cms.EDProducer("CandSecondaryVertexProducer",
    beamSpotTag = cms.InputTag("offlineBeamSpot"),
    constraint = cms.string('BeamSpot'),
    #extSVCollection = cms.InputTag("inclusiveCandidateSecondaryVertices"),
    extSVCollection = cms.InputTag("inclusiveCandidateSecondaryVerticesFiltered"),  #currently no additional filtering, but could implement that
    #extSVCollection = cms.InputTag("candidateBToCharmDecayVertexMerged"),
    extSVDeltaRToJet = cms.double(0.3),
    minimumTrackWeight = cms.double(0.5),
    trackIPTagInfos = cms.InputTag("fc4PFImpactParameterTagInfos"),
    trackSelection = cms.PSet(
        a_dR = cms.double(-0.001053),
        a_pT = cms.double(0.005263),
        b_dR = cms.double(0.6263),
        b_pT = cms.double(0.3684),
        jetDeltaRMax = cms.double(0.3),
        maxDecayLen = cms.double(99999.9),
        maxDistToAxis = cms.double(0.2),
        max_pT = cms.double(500),
        max_pT_dRcut = cms.double(0.1),
        max_pT_trackPTcut = cms.double(3),
        min_pT = cms.double(120),
        min_pT_dRcut = cms.double(0.5),
        normChi2Max = cms.double(99999.9),
        pixelHitsMin = cms.uint32(1),
        ptMin = cms.double(1.0),
        qualityClass = cms.string('any'),
        sip2dSigMax = cms.double(99999.9),
        sip2dSigMin = cms.double(-99999.9),
        sip2dValMax = cms.double(99999.9),
        sip2dValMin = cms.double(-99999.9),
        sip3dSigMax = cms.double(99999.9),
        sip3dSigMin = cms.double(-99999.9),
        sip3dValMax = cms.double(99999.9),
        sip3dValMin = cms.double(-99999.9),
        totalHitsMin = cms.uint32(0),
        useVariableJTA = cms.bool(False)
    ),
    trackSort = cms.string('sip3dSig'),
    useExternalSV = cms.bool(True),
    usePVError = cms.bool(True),
    vertexCuts = cms.PSet(
        distSig2dMax = cms.double(99999.9),
        distSig2dMin = cms.double(2.0),
        distSig3dMax = cms.double(99999.9),
        distSig3dMin = cms.double(-99999.9),
        distVal2dMax = cms.double(2.5),
        distVal2dMin = cms.double(0.01),
        distVal3dMax = cms.double(99999.9),
        distVal3dMin = cms.double(-99999.9),
        fracPV = cms.double(0.79),
        massMax = cms.double(6.5),
        maxDeltaRToJetAxis = cms.double(0.4),
        minimumTrackWeight = cms.double(0.5),
        multiplicityMin = cms.uint32(2),
        useTrackWeights = cms.bool(True),
        v0Filter = cms.PSet(
            k0sMassWindow = cms.double(0.05)
        )
    ),
    vertexReco = cms.PSet(
        finder = cms.string('avr'),
        minweight = cms.double(0.5),
        primcut = cms.double(1.8),
        seccut = cms.double(6.0),
        smoothing = cms.bool(False),
        weightthreshold = cms.double(0.001)
    ),
    vertexSelection = cms.PSet(
        sortCriterium = cms.string('dist3dError')
    )
)


fc4PFSimpleSecondaryVertexHighEffBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
fc4PFSimpleSecondaryVertexHighPurBJetTags = fc4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
fc4PFCombinedSecondaryVertexBJetTags = fc4PFbTagger.CombinedSecondaryVertexBJetTags
fc4PFCombinedSecondaryVertexV2BJetTags = fc4PFbTagger.CombinedSecondaryVertexV2BJetTags

candidateSimpleSecondaryVertex2TrkComputer = cms.ESProducer("CandidateSimpleSecondaryVertexESProducer",
    minTracks = cms.uint32(2),
    unBoost = cms.bool(False),
    use3d = cms.bool(True),
    useSignificance = cms.bool(True)
)


candidateSimpleSecondaryVertex3TrkComputer = cms.ESProducer("CandidateSimpleSecondaryVertexESProducer",
    minTracks = cms.uint32(3),
    unBoost = cms.bool(False),
    use3d = cms.bool(True),
    useSignificance = cms.bool(True)
)

fc4PFSimpleSecondaryVertexHighEffBJetTags.jetTagComputer = cms.string('candidateSimpleSecondaryVertex2TrkComputer')
fc4PFSimpleSecondaryVertexHighPurBJetTags.jetTagComputer = cms.string('candidateSimpleSecondaryVertex3TrkComputer')




candidateCombinedSecondaryVertexV2Computer = cms.ESProducer("CandidateCombinedSecondaryVertexESProducer",
    SoftLeptonFlip = cms.bool(False),
    calibrationRecords = cms.vstring(
        'CombinedSVIVFV2RecoVertex', 
        'CombinedSVIVFV2PseudoVertex', 
        'CombinedSVIVFV2NoVertex'
    ),
    categoryVariableName = cms.string('vertexCategory'),
    charmCut = cms.double(1.5),
    correctVertexMass = cms.bool(True),
    minimumTrackWeight = cms.double(0.5),
    pseudoMultiplicityMin = cms.uint32(2),
    pseudoVertexV0Filter = cms.PSet(
        k0sMassWindow = cms.double(0.05)
    ),
    recordLabel = cms.string(''),
    trackFlip = cms.bool(False),
    trackMultiplicityMin = cms.uint32(2),
    trackPairV0Filter = cms.PSet(
        k0sMassWindow = cms.double(0.03)
    ),
    trackPseudoSelection = cms.PSet(
        a_dR = cms.double(-0.001053),
        a_pT = cms.double(0.005263),
        b_dR = cms.double(0.6263),
        b_pT = cms.double(0.3684),
        jetDeltaRMax = cms.double(0.3),
        maxDecayLen = cms.double(5),
        maxDistToAxis = cms.double(0.07),
        max_pT = cms.double(500),
        max_pT_dRcut = cms.double(0.1),
        max_pT_trackPTcut = cms.double(3),
        min_pT = cms.double(120),
        min_pT_dRcut = cms.double(0.5),
        normChi2Max = cms.double(99999.9),
        pixelHitsMin = cms.uint32(0),
        ptMin = cms.double(0.0),
        qualityClass = cms.string('any'),
        sip2dSigMax = cms.double(99999.9),
        sip2dSigMin = cms.double(2.0),
        sip2dValMax = cms.double(99999.9),
        sip2dValMin = cms.double(-99999.9),
        sip3dSigMax = cms.double(99999.9),
        sip3dSigMin = cms.double(-99999.9),
        sip3dValMax = cms.double(99999.9),
        sip3dValMin = cms.double(-99999.9),
        totalHitsMin = cms.uint32(0),
        useVariableJTA = cms.bool(False)
    ),
    trackSelection = cms.PSet(
        a_dR = cms.double(-0.001053),
        a_pT = cms.double(0.005263),
        b_dR = cms.double(0.6263),
        b_pT = cms.double(0.3684),
        jetDeltaRMax = cms.double(0.3),
        maxDecayLen = cms.double(5),
        maxDistToAxis = cms.double(0.07),
        max_pT = cms.double(500),
        max_pT_dRcut = cms.double(0.1),
        max_pT_trackPTcut = cms.double(3),
        min_pT = cms.double(120),
        min_pT_dRcut = cms.double(0.5),
        normChi2Max = cms.double(99999.9),
        pixelHitsMin = cms.uint32(0),
        ptMin = cms.double(0.0),
        qualityClass = cms.string('any'),
        sip2dSigMax = cms.double(99999.9),
        sip2dSigMin = cms.double(-99999.9),
        sip2dValMax = cms.double(99999.9),
        sip2dValMin = cms.double(-99999.9),
        sip3dSigMax = cms.double(99999.9),
        sip3dSigMin = cms.double(-99999.9),
        sip3dValMax = cms.double(99999.9),
        sip3dValMin = cms.double(-99999.9),
        totalHitsMin = cms.uint32(0),
        useVariableJTA = cms.bool(False)
    ),
    trackSort = cms.string('sip2dSig'),
    useCategories = cms.bool(True),
    useTrackWeights = cms.bool(True),
    vertexFlip = cms.bool(False)
)

fc4PFCombinedSecondaryVertexBJetTags.jetTagComputer = cms.string('candidateCombinedSecondaryVertexV2Computer')
fc4PFCombinedSecondaryVertexV2BJetTags.jetTagComputer = cms.string('candidateCombinedSecondaryVertexV2Computer')


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
            (
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




fc4PFJetBtagging = cms.Sequence(fc4PFJetBtaggingIP
            *fc4PFJetBtaggingSV
            )

fc4PFpatJetsWithBtagging = patJets.clone(jetSource = cms.InputTag("fc4PFJets"),
        genJetMatch          = cms.InputTag("fc4PFmatch"),
        genPartonMatch       = cms.InputTag("fc4PFparton"),
        jetCorrFactorsSource = cms.VInputTag(cms.InputTag("fc4PFcorr")),
        #JetPartonMapSource   = cms.InputTag("fc4PFPatJetFlavourAssociationLegacy"),
        JetPartonMapSource   = cms.InputTag("fc4PFPatJetFlavourAssociation"),
	JetFlavourInfoSource   = cms.InputTag("fc4PFPatJetFlavourAssociation"),
        #trackAssociationSource = cms.InputTag("fc4PFJetTracksAssociatorAtVertex"),
        trackAssociationSource = cms.InputTag(""),
	useLegacyJetMCFlavour = False,
        discriminatorSources = cms.VInputTag(cms.InputTag("fc4PFSimpleSecondaryVertexHighEffBJetTags"),
            cms.InputTag("fc4PFSimpleSecondaryVertexHighPurBJetTags"),
            cms.InputTag("fc4PFCombinedSecondaryVertexBJetTags"),
            cms.InputTag("fc4PFCombinedSecondaryVertexV2BJetTags"),
            cms.InputTag("fc4PFJetBProbabilityBJetTags"),
            cms.InputTag("fc4PFJetProbabilityBJetTags"),
            ),
        tagInfoSources = cms.VInputTag(
           cms.InputTag("fc4PFImpactParameterTagInfos"), cms.InputTag("fc4PFSecondaryVertexTagInfos")
        ),
        jetIDMap = cms.InputTag("fc4PFJetID"),
        addBTagInfo = True,
        addTagInfos = True,
        addDiscriminators = True,
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
                                              doSubEvent = False,
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
                                              isPythia6 = cms.untracked.bool(True),
                                              doGenTaus = False,
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
                                                  #fc4PFJetTracksAssociatorAtVertex
                                                  #*
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
                                     #fc4PFJetTracksAssociatorAtVertex
                                      #              *
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
