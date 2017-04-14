# for the list of used tags please see:
# https://twiki.cern.ch/twiki/bin/view/CMS/Onia2MuMuSamples

import FWCore.ParameterSet.Config as cms

# set up process
process = cms.Process("Onia2MuMuPAT")
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

# Other statements
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run1_mc', '')

# produce missing l1extraParticles
process.L1Reco_step = cms.Path(process.l1extraParticles)

# HLT dimuon trigger
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltOniaHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltOniaHI.HLTPaths = ["HLT_HIL1DoubleMu0_HighQ_v1",
                              "HLT_HIL2Mu3_NHitQ_v1",
                              "HLT_HIL2Mu7_v1","HLT_HIL2Mu15_v1",
                              "HLT_HIL2DoubleMu0_NHitQ_v1",
                              "HLT_HIL2DoubleMu3_v1",
                              "HLT_HIL3Mu3_v1",
                              "HLT_HIL3DoubleMuOpen_v1","HLT_HIL3DoubleMuOpen_Mgt2_OS_NoCowboy_v1"
                              ]
process.hltOniaHI.throw = False
process.hltOniaHI.andOr = True

from HiSkim.HiOnia2MuMu.pp_onia2MuMuPAT_cff import *

onia2MuMuPAT(process, GlobalTag=process.GlobalTag.globaltag, MC=True, HLT="HLT", Filter=False)

process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = False
process.onia2MuMuPatGlbGlb.resolvePileUpAmbiguity = False
process.onia2MuMuPatGlbGlb.primaryVertexTag = 'offlinePrimaryVertices'

process.source.fileNames = cms.untracked.vstring(
    'file:/tmp/camelia/step3_ppRECO.root'#step3_RAW2DIGI_L1Reco_RECO.root'

    )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.outOnia2MuMu.fileName = cms.untracked.string( 'onia2MuMuPAT_MC.root' )
process.outTnP.fileName = cms.untracked.string( 'tnp_MC.root' )
process.genMuons.src = "genParticles"
# add event plane information
process.outTnP.outputCommands.extend(cms.untracked.vstring('keep *_generator_*_*'))

process.e = cms.EndPath(process.outOnia2MuMu)


process.patMuonSequence = cms.Sequence(process.genMuons *
                                       process.patMuonsWithTriggerSequence)

process.schedule = cms.Schedule(process.L1Reco_step,process.Onia2MuMuPAT,
process.e)


