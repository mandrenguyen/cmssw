import FWCore.ParameterSet.Config as cms

process = cms.Process('SKIM')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.EventContent.EventContentHeavyIons_cff')

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
)
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/data/Run2017G/DoubleMuon/AOD/17Nov2017-v1/60003/362355D9-3030-E811-8BE2-FA163E66E373.root',
                                      #'/store/data/Run2017G/DoubleMuon/AOD/17Nov2017-v1/90000/B6F95CE3-5B2E-E811-9F6F-A0369FD20D28.root'
    )
)

# Other statements
process.GlobalTag.globaltag = '94X_dataRun2_ReReco_EOY17_v6'

#Three muons
process.TrimuonFilter = cms.EDFilter("CandViewCountFilter",
                                     src = cms.InputTag('muons'),
                                     minNumber = cms.uint32(3),
                                     filter = cms.bool(True),   # otherwise it won't filter the events
                                     )

#Bc candidate
process.BcCandidates = cms.EDProducer("CandViewShallowCloneCombiner",
                                      decay = cms.string("muons muons muons"),
                                      cut = cms.string("2.8 < mass < 8.5 && (daughter(0).isGlobalMuon || daughter(0).isTrackerMuon) && abs(daughter(0).eta) < 2.4 && daughter(0).pt > 0.9 && daughter(0).p > 3.1 && (abs(daughter(0).eta) > 1.0 || daughter(0).pt > 3.3) && (daughter(1).isGlobalMuon || daughter(1).isTrackerMuon) && abs(daughter(1).eta) < 2.4 && daughter(1).pt > 0.9 && daughter(1).p > 3.1 && (abs(daughter(1).eta) > 1.0 || daughter(1).pt > 3.3) && (daughter(2).isGlobalMuon || daughter(2).isTrackerMuon) && abs(daughter(2).eta) < 2.4 && daughter(2).pt > 0.9 && daughter(2).p > 3.1 && (abs(daughter(2).eta) > 1.0 || daughter(2).pt > 3.3) && ( abs( sqrt((daughter(0).energy + daughter(1).energy)^2 - (daughter(0).pz + daughter(1).pz)^2 - (daughter(0).px + daughter(1).px)^2 - (daughter(0).py + daughter(1).py)^2) - 3.096 ) < 1. || abs( sqrt((daughter(0).energy + daughter(2).energy)^2 - (daughter(0).pz + daughter(2).pz)^2 - (daughter(0).px + daughter(2).px)^2 - (daughter(0).py + daughter(2).py)^2) - 3.096 ) < 1. || abs( sqrt((daughter(2).energy + daughter(1).energy)^2 - (daughter(2).pz + daughter(1).pz)^2 - (daughter(2).px + daughter(1).px)^2 - (daughter(2).py + daughter(1).py)^2) - 3.096 ) < 1. )"),#tracker muon acceptance cuts + one dimuon in loose Jpsi mass range
                                      )
process.BcCandidatesFilter = cms.EDFilter("CandViewCountFilter",
                                          src = cms.InputTag('BcCandidates'),
                                          minNumber = cms.uint32(1),
                                          )

# One good muon
process.OneGoodMu = cms.EDFilter("CandViewSelector",
                                 src = cms.InputTag("muons"),
                                 cut = cms.string("(isGlobalMuon || isTrackerMuon) && abs(eta) < 2.4 && ((abs(eta) < 1.2 && pt > 3.5) || (1.2<abs(eta) && abs(eta)<2.1 && pt > 5.47-1.89*abs(eta)) || (2.1<abs(eta) && pt > 1.5)) && abs(innerTrack.dxy)<2 && abs(innerTrack.dz)<30"),
                                 filter = cms.bool(True),   # otherwise it won't filter the events
                             )

# Valid vertex filter
process.primaryVertexFilter = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlinePrimaryVertices"),
    cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2 && tracksSize >= 2"), 
    filter = cms.bool(True),   # otherwise it won't filter the events
    )

# Output definition
process.out = cms.OutputModule("PoolOutputModule", 
                               fileName = cms.untracked.string("pp5TeV_TrimuonSkim.root"),
                               outputCommands =  cms.untracked.vstring(
        'keep *',
        'drop *_*_*_SKIM',
        'drop EcalRecHitsSorted_*__RECO',
        'drop recoGsfElectrons_*_*_RECO',
        'drop recoPhotons_*_*_RECO',
        'drop recoPFTaus_*_*_RECO',
        'drop *_*_Tau_RECO',
        'drop *_*_EGamma_RECO',
        'drop *_dt4DCosmicSegments_*_RECO',
        'drop *_electronGsfTracks_*_RECO',
        'drop *_*_CleanedCosmicsMuons_RECO',
        'drop *_particleFlowEGamma_*_RECO',
        'drop recoPFTauTransverseImpactParameters_*_*_RECO',
        'drop *_cosmicMuons1Leg_*_RECO',
        'drop recoPFTauDiscriminator_*_*_RECO',
        'drop recoPhotonCores_*_*_RECO',
        'drop recoHIPhotonIsolationedmValueMap_*_*_RECO',
        'drop l1tTauBXVector_*_*_RECO',
        'drop l1tEGammaBXVector_*_*_RECO',
        ),
                               SelectEvents = cms.untracked.PSet( 
        SelectEvents = cms.vstring("TrimuonFilterPath")
        )
)
process.TrimuonFilterPath = cms.Path(process.BcCandidates*process.BcCandidatesFilter
                                     *process.OneGoodMu#*process.primaryVertexFilter
)
process.e = cms.EndPath(process.out)
