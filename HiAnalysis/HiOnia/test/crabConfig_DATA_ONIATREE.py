from WMCore.Configuration import Configuration

config = Configuration()

config.section_("General")
config.General.requestName = "HIDoubleMuonPsiPeri_Run2018A_AOD_OniaTree_Run_327123_327564_flippedJpsi_BcTrimuon_27122019"#"HIDoubleMuonPsiPeri_Run2018A_AOD_OniaTree_Run_327151_327564_flippedJpsi_BcTrimuon_23122019"#"HIDoubleMuonPsiPeri_Run2018A_AOD_OniaTree_Run_327123_327564_BcTrimuon_16122019"#"HIDoubleMuon_Run2018A_AOD_OniaTree_Run_326381_327122_BcTrimuon_16122019"
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "HiAnalysis/HiOnia/test/hioniaanalyzer_PbPbPrompt_trimuons_flipJpsiDir_103X_DATA_cfg.py"#"HiAnalysis/HiOnia/test/hioniaanalyzer_PbPbPrompt_trimuons_103X_DATA_cfg.py"
#config.JobType.maxMemoryMB = 2500         # request high memory machines.
#config.JobType.numCores = 4
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
config.JobType.maxJobRuntimeMin = 1000 #2750    # request longer runtime, ~48 hours.

config.section_("Data")
config.Data.inputDataset = '/HIDoubleMuonPsiPeri/HIRun2018A-04Apr2019-v1/AOD'#'/HIDoubleMuonPsiPeri/HIRun2018A-04Apr2019-v1/AOD' #'/HIDoubleMuon/HIRun2018A-04Apr2019-v1/AOD'#/HIDoubleMuonPsiPeri/HIRun2018A-04Apr2019-v1/AOD    #'/HIDoubleMuon/HIRun2018A-PromptReco-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 70000
#config.Data.totalUnits = -1
config.Data.splitting = "EventAwareLumiBased"
config.Data.allowNonValidInputDataset = True

config.Data.outLFNDirBase = '/store/user/gfalmagn/Bc_analysis/data/%s' % (config.General.requestName)
config.Data.publication = False
config.Data.runRange = '327123-327564'#'327123-327564'#'326381-327122' or 326382 or 327564
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/HI/PromptReco/Cert_326381-327564_HI_PromptReco_Collisions18_JSON_HF_and_MuonPhys.txt'

config.section_("Site")
config.Site.storageSite = "T2_FR_GRIF_LLR"
#config.Site.whitelist = ["T2_CH_CERN"]
