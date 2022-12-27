import FWCore.ParameterSet.Config as cms

ak4PFJetsWithMuon = cms.EDFilter("PFJetXSelector",
                                 src = cms.InputTag("ak4PFJets"),
                                 offPV = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                 cut = cms.string("pt > 5.0 && abs(rapidity()) < 3.0"),
                                 dummy = cms.bool(False)
)

ak3PFJetsWithMuon = cms.EDFilter("PFJetXSelector",
                                 src = cms.InputTag("ak3PFJets"),
                                 offPV = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                 cut = cms.string("pt > 5.0 && abs(rapidity()) < 3.0"),
                                 dummy = cms.bool(False)
)
