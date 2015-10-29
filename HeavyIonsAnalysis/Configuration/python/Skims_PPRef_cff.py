import FWCore.ParameterSet.Config as cms

from Configuration.EventContent.EventContent_cff import RECOEventContent

skimRECOContent = RECOEventContent.clone()
skimRECOContent.outputCommands.append("drop *_MEtoEDMConverter_*_*")
skimRECOContent.outputCommands.append("drop *_*_*_SKIM")

#####################

from HeavyIonsAnalysis.Configuration.PP_HighPtJetSkim_cff import *
highPtJetSkimPath = cms.Path( highPtJetSkimSequence )
SKIMStreamHighPtJet = cms.FilteredStream(
    responsible = 'HI PAG',
    name = 'HIGHPTJET',
    paths = (highPtJetSkimPath),
    content = skimRECOContent.outputCommands,
    selectEvents = cms.untracked.PSet(),
    dataTier = cms.untracked.string('RECO')
    )

