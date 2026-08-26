import FWCore.ParameterSet.Config as cms

onia2MuMuPAT = cms.EDProducer('HiOnia2MuMuPAT',
        muons                    = cms.InputTag("patMuonsWithTrigger"),
        beamSpotTag              = cms.InputTag("offlineBeamSpot"),
        primaryVertexTag         = cms.InputTag("offlinePrimaryVertices"),
        srcTracks                = cms.InputTag("generalTracks"),
        genParticles             = cms.InputTag("genParticles"),
        # At least one muon must pass this selection
        higherPuritySelection    = cms.string(""), ## No need to repeat lowerPuritySelection in there, already included
        # BOTH muons must pass this selection
        lowerPuritySelection     = cms.string("((isGlobalMuon && isTrackerMuon) || (innerTrack.isNonnull && genParticleRef(0).isNonnull)) && abs(innerTrack.dxy)<4 && abs(innerTrack.dz)<35"),
        dimuonSelection          = cms.string(""), ## The dimuon must pass this selection before vertexing
        DimuTrkSelection         = cms.string(""), ## The dimuon+track must pass this selection before vertexing
        trimuonSelection         = cms.string(""), ## The trimuon must pass this selection before vertexing
        LateDimuonSel            = cms.string(""), ## The dimuon must pass this selection before being written out
        LateDimuTrkSel           = cms.string(""), ## The dimuon+track must pass this selection before being written out
        LateTrimuonSel           = cms.string(""), ## The trimuon must pass this selection before being written out
        addCommonVertex          = cms.bool(True), ## Embed the full reco::Vertex out of the common vertex fit
        addMuonlessPrimaryVertex = cms.bool(False), ## Embed the primary vertex re-made from all the tracks except the two muons
        resolvePileUpAmbiguity   = cms.bool(True), ## Order PVs by their vicinity to the J/psi vertex, not by sumPt
        onlySoftMuons            = cms.bool(False), ## Keep only the isSoftMuons (without highPurity) for the single muons + the di(tri)muon combinations
        onlySingleMuons          = cms.bool(False), ## Do not make dimuons
        doTrimuons               = cms.bool(False), ## Make collections of trimuon candidates in addition to dimuons, and keep only events with >0 trimuons
        DimuonTrk                = cms.bool(False), ## Make collections of Jpsi+track candidates in addition to dimuons, and keep only events with >0 Jpsi+trk
        flipJpsiDirection        = cms.int32(False), ## flip the Jpsi direction, before combining it with a third muon
        particleType             = cms.int32(211), ## pdgInt assigned to the track to be combined with the dimuons
        trackMassHypothesis      = cms.double(0.13957018), ## mass assigned to the track to be combined with the dimuons
        dimuonMassHypothesis     = cms.double(3.09609) ## dimuon mass hypothesis for KinematicConstrainedVertexFitter and lifetime estimates
)
