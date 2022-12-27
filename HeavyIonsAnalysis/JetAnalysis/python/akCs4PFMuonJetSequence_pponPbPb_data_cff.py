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

from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cfi import patJets
akCs4PFpatJets = patJets.clone(
    jetSource = "akCs4PFJets",
    addAssociatedTracks = False,
    addBTagInfo = True,
    addDiscriminators = True,
    addGenJetMatch = False,
    addGenPartonMatch = False,
    addJetCharge = False,
    addJetCorrFactors = True,
    addJetFlavourInfo = False,
    jetCorrFactorsSource = ["akCs4PFpatJetCorrFactors"],
    discriminatorSources = cms.VInputTag(
        cms.InputTag("akCs4PFDeepCSVJetTags","probb"), cms.InputTag("akCs4PFDeepCSVJetTags","probc"), 
        cms.InputTag("akCs4PFDeepCSVJetTags","probudsg"), cms.InputTag("akCs4PFDeepCSVJetTags","probbb"),
        cms.InputTag("akCs4PFJetProbabilityBJetTags")
    ),
)

from HeavyIonsAnalysis.JetAnalysis.inclusiveJetAnalyzer_cff import *
akCs4PFJetAnalyzer = inclusiveJetAnalyzer.clone(
    jetTag = cms.InputTag("akCs4PFpatJets"),
    rParam = 0.4,
    fillGenJets = False,
    isMC = False,
    bTagJetName = cms.untracked.string("akCs4PF"),
    jetName = cms.untracked.string("akCs4PF"),
    hltTrgResults = cms.untracked.string('TriggerResults::'+'HISIGNAL'),
    addDeepCSV = True,
    jetPtMin=20.
    )



'''
ak4PFpatJets = akCs4PFpatJets.clone(jetSource = "ak4PFJets")
ak4PFpatJets.jetCorrFactorsSource = ["ak4PFpatJetCorrFactors"]

ak4PFpatJetsWithMuon= ak4PFpatJets.clone(jetSource = "ak4PFJetsWithMuon")
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

akCs4PFJetSequence = cms.Sequence(ak4PFJets + ak4PFJetsWithMuon + ak4PFMuons + akCs4PFJetsNoMuon  + pfCandsCs4PlusMuon + akCs4PFJets + akCs4PFpatJetCorrFactors + akCs4PFIPTagInfos + akCs4PFSVTagInfos + akCs4PFDeepCSVTagInfos + akCs4PFDeepCSVJetTags + akCs4PFJetProbabilityBJetTags + akCs4PFpatJets + akCs4PFJetAnalyzer) 

# extra stuff for debugging
#+ ak4PFpatJetCorrFactors + ak4PFpatJetWithMuonCorrFactors +  akCs4PFpatJetNoMuonCorrFactors + ak4PFpatJets + ak4PFpatJetsWithMuon + akCs4PFpatJetsNoMuon + ak4PFJetWithMuonAnalyzer + akCs4PFJetNoMuonAnalyzer + ak4PFJetAnalyzer)


