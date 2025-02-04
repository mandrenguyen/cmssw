from WMCore.Configuration import Configuration

config = Configuration()

config.section_("General")
config.General.requestName = "DoubleMu_Run2017G_AOD_Run_306546_306826_OniaTree_TripleMuBc"#306826
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "HiAnalysis/HiOnia/test/hioniaanalyzer_ppPrompt_trimuons_94X_DATA_cfg.py"
config.JobType.maxMemoryMB = 2500         # request high memory machines.
#config.JobType.maxJobRuntimeMin = 2750    # request longer runtime, ~48 hours.

config.section_("Data")
config.Data.inputDataset = '/DoubleMuon/Run2017G-17Nov2017-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 1000000
config.Data.totalUnits = -1
config.Data.splitting = 'EventAwareLumiBased'
config.Data.outLFNDirBase = '/store/user/gfalmagn/PromptAOD/%s' % (config.General.requestName)
#config.Data.outLFNDirBase = '/store/group/phys_heavyions/dileptons/Data2018/PbPb502TeV/TTrees/PromptAOD'
config.Data.publication = False
config.Data.runRange = '306546-306826'#'306546-306826'

config.section_('Site')
config.Site.storageSite = 'T2_FR_GRIF_LLR'

