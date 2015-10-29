import FWCore.ParameterSet.Config as cms

from Configuration.EventContent.EventContentHeavyIons_cff import FEVTEventContent, RECOEventContent, AODEventContent

skimFEVTContent = FEVTEventContent.clone()
skimFEVTContent.outputCommands.append("drop *_MEtoEDMConverter_*_*")
skimFEVTContent.outputCommands.append("drop *_*_*_SKIM")

skimRECOContent = RECOEventContent.clone()
skimRECOContent.outputCommands.append("drop *_MEtoEDMConverter_*_*")
skimRECOContent.outputCommands.append("drop *_*_*_SKIM")

skimAODContent = AODEventContent.clone()
skimAODContent.outputCommands.append("drop *_MEtoEDMConverter_*_*")
skimAODContent.outputCommands.append("drop *_*_*_SKIM")

#####################

from HeavyIonsAnalysis.Configuration.HI_PhotonSkim_cff import *
photonSkimPath = cms.Path( photonSkimSequence )
SKIMStreamPhoton = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'Photon',
    paths = (photonSkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

zEEPhotonSkimPath = cms.Path( zEEPhotonSkimSequence )
SKIMStreamZEEPhoton = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'ZEEPHOTON',
    paths = (zEEPhotonSkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

#####################

from HeavyIonsAnalysis.Configuration.HI_ZEESkim_cff import *
zEESkimPath = cms.Path( zEESkimSequence )
SKIMStreamZEE = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'ZEE',
    paths = (zEESkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

#####################

from HeavyIonsAnalysis.Configuration.HI_ZMMSkim_cff import *
zMMSkimPath = cms.Path( zMMSkimSequence )
SKIMStreamZMM = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'ZMM',
    paths = (zMMSkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

#####################

from HeavyIonsAnalysis.Configuration.HI_BJetSkim_cff import *
bJetSkimPath = cms.Path( bJetSkimSequence )
SKIMStreamBJet = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'BJET',
    paths = (bJetSkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

#####################

from HeavyIonsAnalysis.Configuration.HI_D0MesonSkim_cff import *
d0MesonSkimPath = cms.Path( d0MesonSkimSequence )
SKIMStreamD0Meson = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'D0MESON',
    paths = (d0MesonSkimPath),
    content = skimRECOContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('RECO')
    )

#####################

from HeavyIonsAnalysis.Configuration.HI_HighPtJetSkim_cff import *
highPtJetSkimPath = cms.Path( highPtJetSkimSequence )
SKIMStreamHighPtJet = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'HIGHPTJET',
    paths = (highPtJetSkimPath),
    content = skimRECOContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('RECO')
    )

#####################

from HeavyIonsAnalysis.Configuration.HI_OniaCentralSkim_cff import *
oniaCentralSkimPath = cms.Path( oniaCentralSkimSequence )
SKIMStreamOniaCentral = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'ONIACENTRAL',
    paths = (oniaCentralSkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

#####################     

from HeavyIonsAnalysis.Configuration.HI_OniaPeripheralSkim_cff import *
oniaPeripheralSkimPath = cms.Path( oniaPeripheralSkimSequence )
SKIMStreamOniaPeripheral = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'ONIAPERIPHERAL',
    paths = (oniaPeripheralSkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

#####################     

from HeavyIonsAnalysis.Configuration.HI_SingleTrackSkim_cff import *
singleTrackSkimPath = cms.Path( singleTrackSkimSequence )
SKIMStreamSingleTrack = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'ONIACENTRAL',
    paths = (singleTrackSkimPath),
    content = skimAODContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('AOD')
    )

#####################     
