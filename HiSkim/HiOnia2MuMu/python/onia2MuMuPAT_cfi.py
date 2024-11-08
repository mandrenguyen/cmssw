import FWCore.ParameterSet.Config as cms

onia2MuMuPAT = cms.EDProducer('Onia2MuMuPAT',
  muons = cms.InputTag("patMuons"),
  beamSpotTag = cms.InputTag("offlineBeamSpot"),
  primaryVertexTag = cms.InputTag("offlinePrimaryVertices"),
  srcTracks = cms.InputTag("generalTracks"),
  genParticles = cms.InputTag("genParticles"),
  higherPuritySelection = cms.string(""), ## At least one muon must pass this selection. No need to repeat lowerPuritySelection in there, already included
  lowerPuritySelection  = cms.string("isGlobalMuon"), ## BOTH muons must pass this selection
  dimuonSelection  = cms.string("2 < mass && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25"), ## The dimuon must pass this selection before vertexing
  DimuTrkSelection  = cms.string(""), ## The Jpsi+track must pass this selection before vertexing
  trimuonSelection  = cms.string(""), ## The trimuon must pass this selection before vertexing
  LateDimuonSel  = cms.string(""), ## The dimuon must pass this selection before being written out
  LateDimuTrkSel  = cms.string(""), ## The Jpsi+track must pass this selection before being written out
  LateTrimuonSel  = cms.string(""), ## The trimuon must pass this selection before being written out
  addCommonVertex = cms.bool(True), ## Embed the full reco::Vertex out of the common vertex fit
  addMuonlessPrimaryVertex = cms.bool(True), ## Embed the primary vertex re-made from all the tracks except the two muons
  onlySoftMuons = cms.bool(True), ## Keep only the isSoftMuons (without highPurity) for the single muons + the di(tri)muon combinations
  onlySingleMuons = cms.bool(False), ## Do not make dimuons
  doTrimuons = cms.bool(False), ## Make collections of trimuon candidates in addition to dimuons, and keep only events with >0 trimuons
  DimuonTrk  = cms.bool(False), ## Make collections of Jpsi+track candidates in addition to dimuons, and keep only events with >0 Jpsi+trk 
  flipJpsiDirection = cms.int32(0), ## flip the Jpsi direction, before combining it with a third muon
  resolvePileUpAmbiguity = cms.bool(False),   ## Order PVs by their vicinity to the J/psi vertex, not by sumPt                            
  trackMass = cms.int32(0.13957018) ## mass assigned to the track to be combined with the dimuons
)
