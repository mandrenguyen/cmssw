import FWCore.ParameterSet.Config as cms
import sys

process = cms.Process("FlatCalib")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("RecoHI.HiEvtPlaneAlgos.HiEvtPlane_cfi")
process.load("HeavyIonsAnalysis.HiEvtPlaneCalib/evtplanecalibtree_cfi")
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.load("CondCore.DBCommon.CondDBCommon_cfi")
process.load('GeneratorInterface.HiGenCommon.HeavyIon_cff')
process.load("HeavyIonsAnalysis.Configuration.hfCoincFilter_cff")
process.load("HeavyIonsAnalysis.Configuration.analysisFilters_cff")
process.load("HeavyIonsAnalysis.Configuration.collisionEventSelection_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '75X_dataRun2_v13', '')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")

process.MessageLogger.cerr.FwkReport.reportEvery=1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)


process.GlobalTag.toGet.extend([
   cms.PSet(record = cms.string("HeavyIonRPRcd"),
      tag = cms.string("HeavyIonRPRcd_75x_v02_offline"),
      connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
   )
])

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
       'root://cmsxrootd.fnal.gov//store/hidata/HIRun2015/HIMinimumBias3/AOD/PromptReco-v1/000/263/155/00000/20B08F97-2EA7-E511-9125-02163E011DBD.root',
       'root://cmsxrootd.fnal.gov//store/hidata/HIRun2015/HIMinimumBias3/AOD/PromptReco-v1/000/263/156/00000/3ACB059C-2EA7-E511-96A3-02163E011F7E.root',
       'root://cmsxrootd.fnal.gov//store/hidata/HIRun2015/HIMinimumBias3/AOD/PromptReco-v1/000/263/158/00000/0C292F98-2EA7-E511-A23E-02163E01424C.root',
       'root://cmsxrootd.fnal.gov//store/hidata/HIRun2015/HIMinimumBias3/AOD/PromptReco-v1/000/263/211/00000/4A4D7A06-31A7-E511-BD16-02163E011936.root',
       'root://cmsxrootd.fnal.gov//store/hidata/HIRun2015/HIMinimumBias3/AOD/PromptReco-v1/000/263/212/00000/0E6B670A-31A7-E511-A765-02163E014153.root',
       'root://cmsxrootd.fnal.gov//store/hidata/HIRun2015/HIMinimumBias3/AOD/PromptReco-v1/000/263/212/00000/A2B5C70C-31A7-E511-B489-02163E014667.root',
       'root://cmsxrootd.fnal.gov//store/hidata/HIRun2015/HIMinimumBias3/AOD/PromptReco-v1/000/263/213/00000/528B1E2F-32A7-E511-A3EC-02163E0122F8.root'
    ),
                            duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
                            inputCommands=cms.untracked.vstring(
        'keep *',
        'drop *_hiEvtPlane_*_*'
        ),
                            dropDescendantsOfDroppedBranches=cms.untracked.bool(False)
                            )



process.TFileService = cms.Service("TFileService",
    fileName = cms.string("calib.root")
)
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.minBias = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.minBias.HLTPaths = [
                "HLT_HIL1MinimumBiasHF1AND_*",
                "HLT_HIL1MinimumBiasHF2AND_*"
]

process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
process.centralityBin.nonDefaultGlauberModel = cms.string("")
process.hiEvtPlane.loadDB_ = cms.untracked.bool(False)

process.dump = cms.EDAnalyzer("EventContentAnalyzer")


process.p = cms.Path(process.minBias*process.hfCoincFilter3*process.primaryVertexFilter*process.centralityBin*process.hiEvtPlane*process.evtPlaneCalibTree)


                        

