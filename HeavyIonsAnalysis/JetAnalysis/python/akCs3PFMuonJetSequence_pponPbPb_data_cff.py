import FWCore.ParameterSet.Config as cms

# recluster unsubtracted jets
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
ak3PFJets = ak4PFJets.clone(rParam = 0.3)
ak3PFJets.src = "packedPFCandidates"

#filter out jets with muons
ak3PFJetsWithMuon = cms.EDFilter("PFJetXSelector",
                                 src = cms.InputTag("ak3PFJets"),
                                 offPV = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                 cut = cms.string("abs(rapidity()) < 3.0"),
                                 dummy = cms.bool(False)
)


# separate out the muons and non-muons
ak3PFMuons = cms.EDProducer("gnarlyMuonSelector", constTag = cms.InputTag("ak3PFJetsWithMuon:constituents") )
                                                                                

# run the Cs subtraction on the muon-jet, ignoring the Muon                                                                                                           
from RecoHI.HiJetAlgos.HiRecoPFJets_cff import akCs3PFJets
                                                                                 
akCs3PFJetsNoMuon = akCs3PFJets.clone(
    src = cms.InputTag("ak3PFMuons:notMuons"),
    jetCollInstanceName = cms.string('pfParticlesCs3NoMuon')
)

#assemble the mixed PF candidates
pfCandsCs3PlusMuon = cms.EDProducer(
    "PFCandidateListMerger",
    src = cms.VInputTag(
        cms.InputTag("akCs3PFJetsNoMuon","pfParticlesCs3NoMuon"),
        cms.InputTag("ak3PFMuons:muons")
    )
)

akCs3PFJets = ak3PFJets.clone(src = cms.InputTag("pfCandsCs3PlusMuon") )

akCs3PFpatJetCorrFactors = cms.EDProducer("JetCorrFactorsProducer",
                                          emf = cms.bool(False),
                                          extraJPTOffset = cms.string('L1FastJet'),
                                          flavorType = cms.string('J'),
                                          levels = cms.vstring(
                                              'L2Relative', 
                                              'L2L3Residual'
                                          ),
                                          payload = cms.string('AK3PF'),
                                          primaryVertices = cms.InputTag("offlinePrimaryVertices"),
                                          rho = cms.InputTag("fixedGridRhoFastjetAll"),
                                          src = cms.InputTag("akCs3PFJets"),
                                          useNPV = cms.bool(False),
                                          useRho = cms.bool(False)
                                      )

from HeavyIonsAnalysis.JetAnalysis.candidateBtaggingMiniAOD_cff import *
akCs3PFIPTagInfos = pfImpactParameterTagInfos.clone(jets = 'akCs3PFJets')
akCs3PFSVTagInfos = pfSecondaryVertexTagInfos.clone(trackIPTagInfos = 'akCs3PFIPTagInfos')
akCs3PFDeepCSVTagInfos = pfDeepCSVTagInfos.clone(svTagInfos = 'akCs3PFSVTagInfos')
akCs3PFDeepCSVJetTags = pfDeepCSVJetTags.clone( src = 'akCs3PFDeepCSVTagInfos' )
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
from RecoBTau.JetTagComputer.jetTagRecord_cfi import *
from RecoBTag.ImpactParameter.candidateJetProbabilityComputer_cfi import  *
from RecoBTag.ImpactParameter.pfJetProbabilityBJetTags_cfi import *

akCs3PFJetProbabilityBJetTags = pfJetProbabilityBJetTags.clone(tagInfos = ["akCs3PFIPTagInfos"])

from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cfi import patJets
akCs3PFpatJets = patJets.clone(
    jetSource = "akCs3PFJets",
    addAssociatedTracks = False,
    addBTagInfo = True,
    addDiscriminators = True,
    addGenJetMatch = False,
    addGenPartonMatch = False,
    addJetCharge = False,
    addJetCorrFactors = True,
    addJetFlavourInfo = False,
    jetCorrFactorsSource = ["akCs3PFpatJetCorrFactors"],
    discriminatorSources = cms.VInputTag(
        cms.InputTag("akCs3PFDeepCSVJetTags","probb"), cms.InputTag("akCs3PFDeepCSVJetTags","probc"), 
        cms.InputTag("akCs3PFDeepCSVJetTags","probudsg"), cms.InputTag("akCs3PFDeepCSVJetTags","probbb"),
        cms.InputTag("akCs3PFJetProbabilityBJetTags")
    ),
)

from HeavyIonsAnalysis.JetAnalysis.inclusiveJetAnalyzer_cff import *
akCs3PFJetAnalyzer = inclusiveJetAnalyzer.clone(
    jetTag = cms.InputTag("akCs3PFpatJets"),
    rParam = 0.3,
    fillGenJets = False,
    isMC = False,
    bTagJetName = cms.untracked.string("akCs3PF"),
    jetName = cms.untracked.string("akCs3PF"),
    hltTrgResults = cms.untracked.string('TriggerResults::'+'HISIGNAL'),
    addDeepCSV = True,
    jetPtMin=20.
    )



'''
ak3PFpatJets = akCs3PFpatJets.clone(jetSource = "ak3PFJets")
ak3PFpatJets.jetCorrFactorsSource = ["ak3PFpatJetCorrFactors"]

ak3PFpatJetsWithMuon= ak3PFpatJets.clone(jetSource = "ak3PFJetsWithMuon")
akCs3PFpatJetsNoMuon = ak3PFpatJets.clone(jetSource = "akCs3PFJetsNoMuon")

ak3PFpatJetCorrFactors = akCs3PFpatJetCorrFactors.clone(src = "ak3PFJets")
ak3PFpatJetWithMuonCorrFactors = akCs3PFpatJetCorrFactors.clone(src = "ak3PFJetsWithMuon")
akCs3PFpatJetNoMuonCorrFactors = akCs3PFpatJetCorrFactors.clone(src = "akCs3PFJetsNoMuon")

ak3PFpatJets.jetCorrFactorsSource = ["ak3PFpatJetCorrFactors"]
akCs3PFpatJetsNoMuon.jetCorrFactorsSource = ["akCs3PFpatJetNoMuonCorrFactors"]
ak3PFpatJetsWithMuon.jetCorrFactorsSource = ["ak3PFpatJetWithMuonCorrFactors"]

ak3PFJetAnalyzer = akCs3PFJetAnalyzer.clone(jetTag = "ak3PFpatJets")
ak3PFJetWithMuonAnalyzer = akCs3PFJetAnalyzer.clone(jetTag = "ak3PFpatJetsWithMuon")
akCs3PFJetNoMuonAnalyzer = akCs3PFJetAnalyzer.clone(jetTag = "akCs3PFpatJetsNoMuon")
'''

akCs3PFJetSequence = cms.Sequence(ak3PFJets + ak3PFJetsWithMuon + ak3PFMuons + akCs3PFJetsNoMuon  + pfCandsCs3PlusMuon + akCs3PFJets + akCs3PFpatJetCorrFactors + akCs3PFIPTagInfos + akCs3PFSVTagInfos + akCs3PFDeepCSVTagInfos + akCs3PFDeepCSVJetTags + akCs3PFJetProbabilityBJetTags + akCs3PFpatJets + akCs3PFJetAnalyzer) 

# extra stuff for debugging
#+ ak3PFpatJetCorrFactors + ak3PFpatJetWithMuonCorrFactors +  akCs3PFpatJetNoMuonCorrFactors + ak3PFpatJets + ak3PFpatJetsWithMuon + akCs3PFpatJetsNoMuon + ak3PFJetWithMuonAnalyzer + akCs3PFJetNoMuonAnalyzer + ak3PFJetAnalyzer)


