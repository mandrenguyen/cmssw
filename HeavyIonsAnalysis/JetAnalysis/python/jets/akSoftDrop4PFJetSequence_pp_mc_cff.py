

import FWCore.ParameterSet.Config as cms
from HeavyIonsAnalysis.JetAnalysis.patHeavyIonSequences_cff import patJetGenJetMatch, patJetPartonMatch, patJetCorrFactors, patJets
from HeavyIonsAnalysis.JetAnalysis.inclusiveJetAnalyzer_cff import *
from HeavyIonsAnalysis.JetAnalysis.bTaggers_cff import *
from RecoJets.JetProducers.JetIDParams_cfi import *
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness

akSoftDrop4PFmatch = patJetGenJetMatch.clone(
    src = cms.InputTag("akSoftDrop4PFJets"),
    matched = cms.InputTag("ak4GenJets"),
    resolveByMatchQuality = cms.bool(False),
    maxDeltaR = 0.4
    )

akSoftDrop4PFmatchGroomed = patJetGenJetMatch.clone(
    src = cms.InputTag("akSoftDrop4GenJets"),
    matched = cms.InputTag("ak4GenJets"),
    resolveByMatchQuality = cms.bool(False),
    maxDeltaR = 0.4
    )

akSoftDrop4PFparton = patJetPartonMatch.clone(src = cms.InputTag("akSoftDrop4PFJets")
                                                        )

akSoftDrop4PFcorr = patJetCorrFactors.clone(
    useNPV = cms.bool(False),
    useRho = cms.bool(False),
#    primaryVertices = cms.InputTag("hiSelectedVertex"),
    levels   = cms.vstring('L2Relative','L3Absolute'),
    src = cms.InputTag("akSoftDrop4PFJets"),
    payload = "AK4PF_offline"
    )

akSoftDrop4PFJetID= cms.EDProducer('JetIDProducer', JetIDParams, src = cms.InputTag('akSoftDrop4CaloJets'))

#akSoftDrop4PFclean   = heavyIonCleanedGenJets.clone(src = cms.InputTag('ak4GenJets'))

akSoftDrop4PFbTagger = bTaggers("akSoftDrop4PF",0.4,True,True)

#create objects locally since they dont load properly otherwise
#akSoftDrop4PFmatch = akSoftDrop4PFbTagger.match
akSoftDrop4PFparton = patJetPartonMatch.clone(src = cms.InputTag("akSoftDrop4PFJets"), matched = cms.InputTag("genParticles"))
akSoftDrop4PFPatJetFlavourAssociationLegacy = akSoftDrop4PFbTagger.PatJetFlavourAssociationLegacy
#akSoftDrop4PFPatJetPartons = akSoftDrop4PFbTagger.PatJetPartons
akSoftDrop4PFJetTracksAssociatorAtVertex = akSoftDrop4PFbTagger.JetTracksAssociatorAtVertex
akSoftDrop4PFJetTracksAssociatorAtVertex.tracks = cms.InputTag("highPurityTracks")
akSoftDrop4PFSimpleSecondaryVertexHighEffBJetTags = akSoftDrop4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
akSoftDrop4PFSimpleSecondaryVertexHighPurBJetTags = akSoftDrop4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
akSoftDrop4PFCombinedSecondaryVertexBJetTags = akSoftDrop4PFbTagger.CombinedSecondaryVertexBJetTags
akSoftDrop4PFCombinedSecondaryVertexV2BJetTags = akSoftDrop4PFbTagger.CombinedSecondaryVertexV2BJetTags
akSoftDrop4PFJetBProbabilityBJetTags = akSoftDrop4PFbTagger.JetBProbabilityBJetTags
akSoftDrop4PFPatJetPartonAssociationLegacy = akSoftDrop4PFbTagger.PatJetPartonAssociationLegacy

akSoftDrop4PFImpactParameterTagInfos = akSoftDrop4PFbTagger.ImpactParameterTagInfos
akSoftDrop4PFImpactParameterTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices")
akSoftDrop4PFJetProbabilityBJetTags = akSoftDrop4PFbTagger.JetProbabilityBJetTags

akSoftDrop4PFSecondaryVertexTagInfos = akSoftDrop4PFbTagger.SecondaryVertexTagInfos
akSoftDrop4PFSimpleSecondaryVertexHighEffBJetTags = akSoftDrop4PFbTagger.SimpleSecondaryVertexHighEffBJetTags
akSoftDrop4PFSimpleSecondaryVertexHighPurBJetTags = akSoftDrop4PFbTagger.SimpleSecondaryVertexHighPurBJetTags
akSoftDrop4PFCombinedSecondaryVertexBJetTags = akSoftDrop4PFbTagger.CombinedSecondaryVertexBJetTags
akSoftDrop4PFCombinedSecondaryVertexV2BJetTags = akSoftDrop4PFbTagger.CombinedSecondaryVertexV2BJetTags

akSoftDrop4PFPatJetFlavourIdLegacy = cms.Sequence(akSoftDrop4PFPatJetPartonAssociationLegacy*akSoftDrop4PFPatJetFlavourAssociationLegacy)
#Not working with our PU sub
akSoftDrop4PFPatJetFlavourAssociation = akSoftDrop4PFbTagger.PatJetFlavourAssociation
#akSoftDrop4PFPatJetFlavourId = cms.Sequence(akSoftDrop4PFPatJetPartons*akSoftDrop4PFPatJetFlavourAssociation)
akSoftDrop4PFPatJetFlavourId = cms.Sequence(akSoftDrop4PFPatJetFlavourAssociation)

#adding the subjet taggers
akSoftDrop4PFSubjetImpactParameterTagInfos = akSoftDrop4PFbTagger.SubjetImpactParameterTagInfos
akSoftDrop4PFSubjetJetProbabilityBJetTags = akSoftDrop4PFbTagger.SubjetJetProbabilityBJetTags
akSoftDrop4PFSubjetSecondaryVertexTagInfos = akSoftDrop4PFbTagger.SubjetSecondaryVertexTagInfos
akSoftDrop4PFSubjetJetTracksAssociatorAtVertex = akSoftDrop4PFbTagger.SubjetJetTracksAssociatorAtVertex
akSoftDrop4PFCombinedSubjetSecondaryVertexBJetTags = akSoftDrop4PFbTagger.CombinedSubjetSecondaryVertexBJetTags
akSoftDrop4PFCombinedSubjetSecondaryVertexV2BJetTags = akSoftDrop4PFbTagger.CombinedSubjetSecondaryVertexV2BJetTags

akSoftDrop4PFJetBtaggingIP       = cms.Sequence(akSoftDrop4PFImpactParameterTagInfos *
            (
             akSoftDrop4PFJetProbabilityBJetTags +
             akSoftDrop4PFJetBProbabilityBJetTags 
            )
            )

akSoftDrop4PFJetBtaggingSV = cms.Sequence(akSoftDrop4PFImpactParameterTagInfos
            *
            akSoftDrop4PFSecondaryVertexTagInfos
            * (akSoftDrop4PFSimpleSecondaryVertexHighEffBJetTags+
                akSoftDrop4PFSimpleSecondaryVertexHighPurBJetTags+
                akSoftDrop4PFCombinedSecondaryVertexBJetTags+
                akSoftDrop4PFCombinedSecondaryVertexV2BJetTags
              )
            )



akSoftDrop4PFJetBtagging = cms.Sequence(akSoftDrop4PFJetBtaggingIP
            *akSoftDrop4PFJetBtaggingSV
            )

akSoftDrop4PFpatJetsWithBtagging = patJets.clone(
    jetSource = cms.InputTag("akSoftDrop4PFJets"),
    genJetMatch          = cms.InputTag("akSoftDrop4PFmatch"),
    genPartonMatch       = cms.InputTag("akSoftDrop4PFparton"),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("akSoftDrop4PFcorr")),
    #JetPartonMapSource   = cms.InputTag("akSoftDrop4PFPatJetFlavourAssociationLegacy"),
    JetPartonMapSource   = cms.InputTag("akSoftDrop4PFPatJetFlavourAssociation"),
    JetFlavourInfoSource   = cms.InputTag("akSoftDrop4PFPatJetFlavourAssociation"),
    trackAssociationSource = cms.InputTag("akSoftDrop4PFJetTracksAssociatorAtVertex"),
    useLegacyJetMCFlavour = False,
    discriminatorSources = cms.VInputTag(cms.InputTag("akSoftDrop4PFSimpleSecondaryVertexHighEffBJetTags"),
                                         cms.InputTag("akSoftDrop4PFSimpleSecondaryVertexHighPurBJetTags"),
                                         cms.InputTag("akSoftDrop4PFCombinedSecondaryVertexBJetTags"),
                                         cms.InputTag("akSoftDrop4PFCombinedSecondaryVertexV2BJetTags"),
                                         cms.InputTag("akSoftDrop4PFJetBProbabilityBJetTags"),
                                         cms.InputTag("akSoftDrop4PFJetProbabilityBJetTags"),
                                         ),
    tagInfoSources = cms.VInputTag(
        cms.InputTag("akSoftDrop4PFImpactParameterTagInfos"), cms.InputTag("akSoftDrop4PFSecondaryVertexTagInfos")
        ),
    jetIDMap = cms.InputTag("akSoftDrop4PFJetID"),
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
    )

akSoftDrop4PFNjettiness = Njettiness.clone(
		    src = cms.InputTag("akSoftDrop4PFJets"),
           	    R0  = cms.double( 0.4)
)
akSoftDrop4PFpatJetsWithBtagging.userData.userFloats.src += ['akSoftDrop4PFNjettiness:tau1','akSoftDrop4PFNjettiness:tau2','akSoftDrop4PFNjettiness:tau3']


'''
akSoftDrop4PFPatSubjetFlavourAssociation = cms.EDProducer("JetFlavourClustering",
    bHadrons = cms.InputTag("patJetPartons","bHadrons"),
    cHadrons = cms.InputTag("patJetPartons","cHadrons"),
    ghostRescaling = cms.double(1e-18),
    groomedJets = cms.InputTag("akSoftDrop4PFJets"),
    hadronFlavourHasPriority = cms.bool(False),
    jetAlgorithm = cms.string('AntiKt'),
    jets = cms.InputTag("ak4PFJets"),
    leptons = cms.InputTag("patJetPartons","leptons"),
    partons = cms.InputTag("patJetPartons","physicsPartons"),
    rParam = cms.double(0.4),
    subjets = cms.InputTag("akSoftDrop4PFJets","SubJets")
)
'''
akSoftDrop4PFPatSubjetPartonAssociationLegacy= cms.EDProducer("JetPartonMatcher",
    coneSizeToAssociate = cms.double(0.3),
    jets = cms.InputTag("akSoftDrop4PFJets","SubJets"),
    partons = cms.InputTag("patJetPartonsLegacy")
)

akSoftDrop4PFPartonSubjetMatch =  cms.EDProducer("MCMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag("genParticles"),
    maxDPtRel = cms.double(3.0),
    maxDeltaR = cms.double(0.2),  #Correct??
    mcPdgId = cms.vint32(
        1, 2, 3, 4, 5, 
        21
    ),
    mcStatus = cms.vint32(3, 23),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(False),
    src = cms.InputTag("akSoftDrop4PFJets","SubJets")
)





akSoftDrop4PFPatSubjetFlavourAssociationLegacy = cms.EDProducer("JetFlavourIdentifier",
    physicsDefinition = cms.bool(False),
    srcByReference = cms.InputTag("akSoftDrop4PFPatSubjetPartonAssociationLegacy")
)




akSoftDrop4PFGenSubjetMatch = cms.EDProducer("GenJetMatcher",
               checkCharge = cms.bool(False),
               matched = cms.InputTag("akSoftDrop4GenJets","SubJets"),
               maxDeltaR = cms.double(0.4),
               mcPdgId = cms.vint32(),
               mcStatus = cms.vint32(),
               resolveAmbiguities = cms.bool(True),
               resolveByMatchQuality = cms.bool(False),
               src = cms.InputTag("akSoftDrop4PFJets","SubJets")
               )


akSoftDrop4PFSubjetNjettiness = cms.EDProducer("NjettinessAdder",
    Njets = cms.vuint32(1, 2, 3, 4),
    R0 = cms.double(0.2),  #??
    Rcutoff = cms.double(999.0),
    akAxesR0 = cms.double(999.0),
    axesDefinition = cms.uint32(6),
    beta = cms.double(1.0),
    cone = cms.double(0.2),  #??
    measureDefinition = cms.uint32(0),
    nPass = cms.int32(999),
    src = cms.InputTag("akSoftDrop4PFJets","SubJets")
)



akSoftDrop4PFpatSubjetsWithBtagging = cms.EDProducer("PATJetProducer",
                                                     JetFlavourInfoSource = cms.InputTag("akSoftDrop4PFPatJetFlavourAssociation","SubJets"),
                                                     JetPartonMapSource = cms.InputTag("akSoftDrop4PFPatSubjetFlavourAssociationLegacy"),
                                                     addAssociatedTracks = cms.bool(False),
                                                     addBTagInfo = cms.bool(True), 
                                                     addDiscriminators = cms.bool(True),  
                                                     addEfficiencies = cms.bool(False),
                                                     addGenJetMatch = cms.bool(True), 
                                                     addGenPartonMatch = cms.bool(True), 
                                                     addJetCharge = cms.bool(False),
                                                     addJetCorrFactors = cms.bool(False), #Not used for heavy ions ?!
                                                     addJetFlavourInfo = cms.bool(True), 
                                                     addJetID = cms.bool(False),
                                                     addPartonJetMatch = cms.bool(False),
                                                     addResolutions = cms.bool(False),
                                                     addTagInfos = cms.bool(True),
                                                     discriminatorSources = cms.VInputTag( cms.InputTag("akSoftDrop4PFCombinedSubjetSecondaryVertexV2BJetTags"), cms.InputTag("akSoftDrop4PFSubjetJetProbabilityBJetTags") ),
                                                     efficiencies = cms.PSet(
        
        ),
                                                     embedGenJetMatch = cms.bool(True), 
                                                     embedGenPartonMatch = cms.bool(True),
                                                     embedPFCandidates = cms.bool(False),
                                                     genJetMatch = cms.InputTag("akSoftDrop4PFGenSubjetMatch"), 
                                                     genPartonMatch = cms.InputTag("akSoftDrop4PFPartonSubjetMatch"), 
                                                     getJetMCFlavour = cms.bool(True), 
                                                     jetChargeSource = cms.InputTag("patJetCharge"), 
                                                     jetCorrFactorsSource = cms.VInputTag(cms.InputTag("akSoftDrop4PFcorr")),  #FIXME
                                                     jetIDMap = cms.InputTag("ak4JetID"),
                                                     jetSource = cms.InputTag("akSoftDrop4PFJets","SubJets"),
                                                     partonJetSource = cms.InputTag("NOT_IMPLEMENTED"),
                                                     resolutions = cms.PSet(),                                       
                                                     tagInfoSources = cms.VInputTag(
        cms.InputTag("akSoftDrop4PFSubjetImpactParameterTagInfos"), cms.InputTag("akSoftDrop4PFSubjetSecondaryVertexTagInfos")
        ),
                                                     trackAssociationSource = cms.InputTag("akSoftDrop4PFSubjetJetTracksAssociatorAtVertex"),  # this one is not pp for some reason??
                                                     useLegacyJetMCFlavour = cms.bool(False),
                                                     userData = cms.PSet(
        userCands = cms.PSet(
            src = cms.VInputTag("")
            ),
        userClasses = cms.PSet(
            src = cms.VInputTag("")
            ),
        userFloats = cms.PSet(
            src = cms.VInputTag("", "akSoftDrop4PFSubjetNjettiness:tau1", "akSoftDrop4PFSubjetNjettiness:tau2", "akSoftDrop4PFSubjetNjettiness:tau3")
            ),
        userFunctionLabels = cms.vstring(),
        userFunctions = cms.vstring(),
        userInts = cms.PSet(
            src = cms.VInputTag("")
            )
        )
                                                     )


akSoftDrop4PFpatJetMerger = cms.EDProducer("BoostedJetMerger",
    jetSrc = cms.InputTag("akSoftDrop4PFpatJetsWithBtagging"),
    subjetSrc = cms.InputTag("akSoftDrop4PFpatSubjetsWithBtagging")
)


akSoftDrop4PFPackedPatJets =  cms.EDProducer("JetSubstructurePacker",
    algoLabels = cms.vstring('SoftDrop'),  # call it what you will
    algoTags = cms.VInputTag(cms.InputTag("akSoftDrop4PFpatJetMerger")),
    distMax = cms.double(0.4),
    fixDaughters = cms.bool(False),
    #jetSrc = cms.InputTag("ak4PFpatJetsWithBtagging"),
    jetSrc = cms.InputTag("akSoftDrop4PFpatJetsWithBtagging"),
    packedPFCandidates = cms.InputTag("packedPFCandidates")  # not used
)




akSoftDrop4PFSubjetSequence = cms.Sequence (akSoftDrop4PFPatSubjetPartonAssociationLegacy + akSoftDrop4PFPatSubjetFlavourAssociationLegacy + akSoftDrop4PFGenSubjetMatch + akSoftDrop4PFPartonSubjetMatch + akSoftDrop4PFSubjetNjettiness +akSoftDrop4PFpatSubjetsWithBtagging +akSoftDrop4PFpatJetMerger +akSoftDrop4PFPackedPatJets)



akSoftDrop4PFJetAnalyzer = inclusiveJetAnalyzer.clone(
    #jetTag = cms.InputTag("akSoftDrop4PFpatJetsWithBtagging"),
    jetTag = cms.InputTag("akSoftDrop4PFPackedPatJets"),  # one pat for jets and subjets
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
    bTagJetName = cms.untracked.string("akSoftDrop4PF"),
    jetName = cms.untracked.string("akSoftDrop4PF"),
    genPtMin = cms.untracked.double(5),
    hltTrgResults = cms.untracked.string('TriggerResults::'+'HISIGNAL'),
    doTower = cms.untracked.bool(False),
    doSubJets = cms.untracked.bool(True),
    doGenSubJets = cms.untracked.bool(True),     
    subjetGenTag = cms.untracked.InputTag("akSoftDrop4GenJets"),
    doExtendedFlavorTagging = cms.untracked.bool(True),
    jetFlavourInfos = cms.InputTag("akSoftDrop4PFPatJetFlavourAssociation"),
    subjetFlavourInfos = cms.InputTag("akSoftDrop4PFPatJetFlavourAssociation","SubJets"),
    groomedJets = cms.InputTag("akSoftDrop4PFJets"),
    isPythia6 = cms.untracked.bool(True),
    doGenTaus = False
    )

akSoftDrop4PFJetSequence_mc = cms.Sequence(
                                                  #akSoftDrop4PFclean
                                                  #*
                                                  akSoftDrop4PFmatch
                                                  #*
                                                  #akSoftDrop4PFmatchGroomed
                                                  *
                                                  akSoftDrop4PFparton
                                                  *
                                                  akSoftDrop4PFcorr
                                                  *
                                                  #akSoftDrop4PFJetID
                                                  #*
                                                  #akSoftDrop4PFPatJetFlavourIdLegacy  # works for PbPb
                                                  #*
			                          akSoftDrop4PFPatJetFlavourId  # doesn't work for PbPb yet
                                                  *
                                                  akSoftDrop4PFJetTracksAssociatorAtVertex
                                                  *
                                                  akSoftDrop4PFJetBtagging
                                                  *
                                                  akSoftDrop4PFNjettiness #No constituents for calo jets in pp. Must be removed for pp calo jets but I'm not sure how to do this transparently (Marta)
                                                  *
                                                  akSoftDrop4PFpatJetsWithBtagging
                                                  *
                                                  akSoftDrop4PFSubjetSequence
                                                  *
                                                  akSoftDrop4PFJetAnalyzer
                                                  )

akSoftDrop4PFJetSequence_data = cms.Sequence(akSoftDrop4PFcorr
                                                    *
                                                    #akSoftDrop4PFJetID
                                                    #*
                                                    akSoftDrop4PFJetTracksAssociatorAtVertex
                                                    *
                                                    akSoftDrop4PFJetBtagging
                                                    *
                                                    akSoftDrop4PFNjettiness 
                                                    *
                                                    akSoftDrop4PFpatJetsWithBtagging
                                                    *
                                                    akSoftDrop4PFJetAnalyzer
                                                    )

akSoftDrop4PFJetSequence_jec = cms.Sequence(akSoftDrop4PFJetSequence_mc)
akSoftDrop4PFJetSequence_mb = cms.Sequence(akSoftDrop4PFJetSequence_mc)

akSoftDrop4PFJetSequence = cms.Sequence(akSoftDrop4PFJetSequence_mc)
