import FWCore.ParameterSet.Config as cms

evtPlaneCalibTree = cms.EDAnalyzer("EvtPlaneCalibTree",
                            vertexTag_=cms.InputTag("hiSelectedVertex"),
                            centralityTag_=cms.InputTag("hiCentrality"),
                            caloTag_ = cms.InputTag("towerMaker"),
                            centralityBinTag_ = cms.InputTag("centralityBin","HFtowers"),
                            centralityVariable = cms.string("HFtowers"),
                            inputPlanesTag_ = cms.InputTag("hiEvtPlane",""),
                            FlatOrder_ = cms.untracked.int32(9),
                            NumFlatBins_ = cms.untracked.int32(40),
                            CentBinCompression_ = cms.untracked.int32(5),
                            minet_ = cms.untracked.double(-1.),
                            maxet_ = cms.untracked.double(-1.),
                            minpt_ = cms.untracked.double(0.3),
                            maxpt_ = cms.untracked.double(3.0),
                            flatnvtxbins_ = cms.int32(10),
                            flatminvtx_ = cms.double(-15.0),
                            flatdelvtx_ = cms.double(3.0),
#                            dzerr_ = cms.untracked.double(10.),
                            dzdzerror_ = cms.untracked.double(3.),
                            d0d0error_ = cms.untracked.double(3.),
                            pterror_ = cms.untracked.double(0.1),
                            useNtrkBins_ = cms.untracked.bool(False),
                            genMC_ = cms.untracked.bool(False),
                            bTag_ = cms.InputTag("mcEvtPlane","b","FlatCalib"),
                            bypassCentrality_ = cms.untracked.bool(False),
                            trackTag = cms.InputTag("hiGeneralTracks"),
                            minvz_ = cms.untracked.double(-15.),
                            maxvz_ = cms.untracked.double(15.),
                            dzdzerror_pix_ = cms.untracked.double(8.0),
                            chi2_ = cms.untracked.double(12.)
                            )
                            




    
