import FWCore.ParameterSet.Config as cms

filteredParticleFlow = cms.EDFilter(
    "HiBadParticleFilter",
    PFCandidates  = cms.InputTag("particleFlow"),   # Collection to test
    taggingMode   = cms.bool(False),
    minMuonTrackRelErr = cms.double(2.0),          # minimum ptError/pt on muon best track
    minMuonPt     = cms.double(10.0),               # minimum muon pt 
    minChargedHadronPt = cms.double(10.0),
    minMuonTrackRelPtErr = cms.double(2.),
    maxMuonSeededDzSig = cms.double(5.),
    maxMuonSeededDxySig = cms.double(5.),
    minCaloCompatibility = cms.double(0.2),
    minTrackRelPtErrTight = cms.double(0.1),
    minTrackRelPtErrLoose = cms.double(0.3),
    minTrackNHitsTight = cms.uint32(10),
    minTrackNHitsLoose = cms.uint32(7),
)
