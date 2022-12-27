import FWCore.ParameterSet.Config as cms

# recluster unsubtracted jets
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
ak4PFJets.src = "packedPFCandidates"

#filter out jets with muons
ak4PFJetsWithMuon = cms.EDFilter("PFJetXSelector",
                                 src = cms.InputTag("ak4PFJets"),
                                 offPV = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                 cut = cms.string("abs(rapidity()) < 3.0"),
                                 dummy = cms.bool(False)
)


# separate out the muons and non-muons
ak4PFMuons = cms.EDProducer("gnarlyMuonSelector", constTag = cms.InputTag("ak4PFJetsWithMuon:constituents") )
                                                                                

# run the Cs subtraction on the muon-jet, ignoring the Muon                                                                                                           
from RecoHI.HiJetAlgos.HiRecoPFJets_cff import akCs4PFJets
                                                                                 
akCs4PFJetsNoMuon = akCs4PFJets.clone(
    src = cms.InputTag("ak4PFMuons:notMuons"),
    jetCollInstanceName = cms.string('pfParticlesCs4NoMuon')
)

#assemble the mixed PF candidates
pfCandsCs4PlusMuon = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(
        cms.InputTag("akCs4PFJetsNoMuon","pfParticlesCs4NoMuon"),
        cms.InputTag("ak4PFMuons:muons")
    )
)

akCs4PFJets = ak4PFJets.clone(src = cms.InputTag("pfCandsCs4PlusMuon") )

akCs4PFpatJetCorrFactors = cms.EDProducer("JetCorrFactorsProducer",
                                          emf = cms.bool(False),
                                          extraJPTOffset = cms.string('L1FastJet'),
                                          flavorType = cms.string('J'),
                                          levels = cms.vstring(
                                              'L2Relative', 
                                              'L2L3Residual'
                                          ),
                                          payload = cms.string('AK4PF'),
                                          primaryVertices = cms.InputTag("offlinePrimaryVertices"),
                                          rho = cms.InputTag("fixedGridRhoFastjetAll"),
                                          src = cms.InputTag("akCs4PFJets"),
                                          useNPV = cms.bool(False),
                                          useRho = cms.bool(False)
                                      )

from HeavyIonsAnalysis.JetAnalysis.candidateBtaggingMiniAOD_cff import *
akCs4PFIPTagInfos = pfImpactParameterTagInfos.clone(jets = 'akCs4PFJets')
akCs4PFSVTagInfos = pfSecondaryVertexTagInfos.clone(trackIPTagInfos = 'akCs4PFIPTagInfos')
akCs4PFDeepCSVTagInfos = pfDeepCSVTagInfos.clone(svTagInfos = 'akCs4PFSVTagInfos')
akCs4PFDeepCSVJetTags = pfDeepCSVJetTags.clone( src = 'akCs4PFDeepCSVTagInfos' )
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
from RecoBTau.JetTagComputer.jetTagRecord_cfi import *
from RecoBTag.ImpactParameter.candidateJetProbabilityComputer_cfi import  *
from RecoBTag.ImpactParameter.pfJetProbabilityBJetTags_cfi import *

akCs4PFJetProbabilityBJetTags = pfJetProbabilityBJetTags.clone(tagInfos = ["akCs4PFIPTagInfos"])

from RecoJets.Configuration.RecoGenJets_cff import ak4GenJetsNoNu
ak4GenJetsNoNu.src = 'packedGenParticlesSignal'
ak4GenJetsNoNu.rParam = 0.4

akCs4PFmatch = cms.EDProducer("GenJetMatcher",
                              checkCharge = cms.bool(False),
                              matched = cms.InputTag("ak4GenJetsNoNu"),
                              maxDeltaR = cms.double(0.4),
                              mcPdgId = cms.vint32(),
                              mcStatus = cms.vint32(),
                              resolveAmbiguities = cms.bool(True),
                              resolveByMatchQuality = cms.bool(True),
                              src = cms.InputTag("akCs4PFJets")
                          )


akCs4PFPatJetPartonAssociationLegacy = cms.EDProducer("JetPartonMatcher",
                                                      coneSizeToAssociate = cms.double(0.4),
                                                      jets = cms.InputTag("akCs4PFJets"),
                                                      partons = cms.InputTag("allPartons")
                                                  )


akCs4PFPatJetFlavourAssociationLegacy = cms.EDProducer("JetFlavourIdentifier",
                                                       physicsDefinition = cms.bool(False),
                                                       srcByReference = cms.InputTag("akCs4PFPatJetPartonAssociationLegacy")
)

akCs4PFparton = cms.EDProducer("MCMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag("hiSignalGenParticles"),
    maxDPtRel = cms.double(3.0),
    maxDeltaR = cms.double(0.4),
    mcPdgId = cms.vint32(1, 2, 3, 4, 5, 21 ),
    mcStatus = cms.vint32(3, 23),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(False),
    src = cms.InputTag("akCs4PFJets")
)


from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cfi import patJets
akCs4PFpatJets = patJets.clone(
    jetSource = "akCs4PFJets",
    addAssociatedTracks = False,
    addBTagInfo = True,
    addDiscriminators = True,
    addGenJetMatch = True,
    addGenPartonMatch = True,
    addJetCharge = False,
    addJetCorrFactors = True,
    addJetFlavourInfo = True,
    jetCorrFactorsSource = ["akCs4PFpatJetCorrFactors"],    
    discriminatorSources = cms.VInputTag(
        cms.InputTag("akCs4PFDeepCSVJetTags","probb"), cms.InputTag("akCs4PFDeepCSVJetTags","probc"), 
        cms.InputTag("akCs4PFDeepCSVJetTags","probudsg"), cms.InputTag("akCs4PFDeepCSVJetTags","probbb"),
        cms.InputTag("akCs4PFJetProbabilityBJetTags")
    ),
    genJetMatch = cms.InputTag("akCs4PFmatch"),
    embedGenPartonMatch = False,
    useLegacyJetMCFlavour = True,
    getJetMCFlavour = True,
    genPartonMatch = cms.InputTag("akCs4PFparton"),
    JetPartonMapSource = cms.InputTag("akCs4PFPatJetFlavourAssociationLegacy"),
)

from HeavyIonsAnalysis.JetAnalysis.inclusiveJetAnalyzer_cff import *
akCs4PFJetAnalyzer = inclusiveJetAnalyzer.clone(
    jetTag = cms.InputTag("akCs4PFpatJets"),
    rParam = 0.4,
    fillGenJets = True,
    isMC = True,
    bTagJetName = cms.untracked.string("akCs4PF"),
    jetName = cms.untracked.string("akCs4PF"),
    hltTrgResults = cms.untracked.string('TriggerResults::'+'HISIGNAL'),
    addDeepCSV = True,
    jetPtMin=20.,
    matchJets = True,
    matchTag = 'ak4PFpatJets',
    genjetTag = 'ak4GenJetsNoNu'
    )

ak4PFPatJetPartons = cms.EDProducer("HadronAndPartonSelector",
    fullChainPhysPartons = cms.bool(True),
    particles = cms.InputTag("hiSignalGenParticles"),
    partonMode = cms.string('Auto'),
    src = cms.InputTag("generator")
)


ak4PFPatJetFlavourAssociation = cms.EDProducer("JetFlavourClustering",
                                               bHadrons = cms.InputTag("ak4PFPatJetPartons","bHadrons"),
                                               cHadrons = cms.InputTag("ak4PFPatJetPartons","cHadrons"),
                                               ghostRescaling = cms.double(1e-18),
                                               hadronFlavourHasPriority = cms.bool(False),
                                               jetAlgorithm = cms.string('AntiKt'),
                                               #jets = cms.InputTag("ak4PFJetsWithMuon"),
                                               jets = cms.InputTag("ak4PFJets"),
                                               leptons = cms.InputTag("ak4PFPatJetPartons","leptons"),
                                               partons = cms.InputTag("ak4PFPatJetPartons","physicsPartons"),
                                               rParam = cms.double(0.4)
)




ak4PFpatJets= akCs4PFpatJets.clone(
    #jetSource = "ak4PFJetsWithMuon", 
    jetSource = "ak4PFJets", 
    addBTagInfo = False,
    addDiscriminators = False,
    addGenJetMatch = False,
    addJetCorrFactors = False,
    jetCorrFactorsSource = [],
    discriminatorSources = [],
    useLegacyJetMCFlavour = False,
    getJetMCFlavour = True,
    addJetFlavourInfo = False,
    JetFlavourInfoSource = cms.InputTag("ak4PFPatJetFlavourAssociation"),
    addGenPartonMatch = False,
    genPartonMatch = "",
)


'''
ak4PFpatJets = akCs4PFpatJets.clone(jetSource = "ak4PFJets")
ak4PFpatJets.jetCorrFactorsSource = ["ak4PFpatJetCorrFactors"]

akCs4PFpatJetsNoMuon = ak4PFpatJets.clone(jetSource = "akCs4PFJetsNoMuon")

ak4PFpatJetCorrFactors = akCs4PFpatJetCorrFactors.clone(src = "ak4PFJets")
ak4PFpatJetWithMuonCorrFactors = akCs4PFpatJetCorrFactors.clone(src = "ak4PFJetsWithMuon")
akCs4PFpatJetNoMuonCorrFactors = akCs4PFpatJetCorrFactors.clone(src = "akCs4PFJetsNoMuon")

ak4PFpatJets.jetCorrFactorsSource = ["ak4PFpatJetCorrFactors"]
akCs4PFpatJetsNoMuon.jetCorrFactorsSource = ["akCs4PFpatJetNoMuonCorrFactors"]
ak4PFpatJetsWithMuon.jetCorrFactorsSource = ["ak4PFpatJetWithMuonCorrFactors"]

ak4PFJetAnalyzer = akCs4PFJetAnalyzer.clone(jetTag = "ak4PFpatJets")
ak4PFJetWithMuonAnalyzer = akCs4PFJetAnalyzer.clone(jetTag = "ak4PFpatJetsWithMuon")
akCs4PFJetNoMuonAnalyzer = akCs4PFJetAnalyzer.clone(jetTag = "akCs4PFpatJetsNoMuon")
'''

akCs4PFJetSequence = cms.Sequence(ak4PFJets + ak4PFJetsWithMuon + ak4PFMuons + akCs4PFJetsNoMuon  + ak4PFPatJetPartons + ak4PFPatJetFlavourAssociation  + ak4PFpatJets + pfCandsCs4PlusMuon + akCs4PFJets + akCs4PFpatJetCorrFactors + akCs4PFIPTagInfos + akCs4PFSVTagInfos + akCs4PFDeepCSVTagInfos + akCs4PFDeepCSVJetTags + akCs4PFJetProbabilityBJetTags + ak4GenJetsNoNu + akCs4PFmatch + akCs4PFPatJetPartonAssociationLegacy + akCs4PFPatJetFlavourAssociationLegacy + akCs4PFparton + akCs4PFpatJets + akCs4PFJetAnalyzer) 

# extra stuff for debugging
#+ ak4PFpatJetCorrFactors + ak4PFpatJetWithMuonCorrFactors +  akCs4PFpatJetNoMuonCorrFactors + ak4PFpatJets + akCs4PFpatJetsNoMuon + ak4PFJetWithMuonAnalyzer + akCs4PFJetNoMuonAnalyzer + ak4PFJetAnalyzer)


