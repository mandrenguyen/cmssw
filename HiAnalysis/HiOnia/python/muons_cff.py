import FWCore.ParameterSet.Config as cms

muons = cms.EDProducer("MuonUnpacker",
                       addPropToMuonSt = cms.bool(False),
                       beamSpot = cms.InputTag("offlineBeamSpot"),
                       fallbackToME1 = cms.bool(True),
                       #mightGet = cms.optional.untracked.vstring,
                       muonSelectors = cms.vstring(
                           'AllTrackerMuons', 
                           'TMOneStationTight'
                       ),
                       muons = cms.InputTag("slimmedMuons"),
                       primaryVertices = cms.InputTag("unpackedTracksAndVertices"),
                       tracks = cms.InputTag("unpackedTracksAndVertices"),
                       useMB2InOverlap = cms.bool(True),
                       useSimpleGeometry = cms.bool(True),
                       useState = cms.string('atVertex'),
                       useStation2 = cms.bool(True),
                       useTrack = cms.string('tracker')
                   )
