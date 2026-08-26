from WMCore.Configuration import Configuration

config = Configuration()

config.section_("General")
config.General.requestName = "OniaTree_RawPrime0"
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "hioniaanalyzer_PbPbPrompt_DATA_cfg.py"
#config.JobType.maxMemoryMB = 2500         # request high memory machines.
config.JobType.numCores = 4
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
#config.JobType.maxJobRuntimeMin = 1000 #2750    # request longer runtime, ~48 hours.

config.section_("Data")
config.Data.inputDataset = '/HIPhysicsRawPrime0/HIRun2024A-PromptReco-v2/MINIAOD'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 50
#config.Data.totalUnits = -1
config.Data.splitting = "LumiBased"
config.Data.allowNonValidInputDataset = True

config.Data.outLFNDirBase = '/store/user/fdamas/%s' % (config.General.requestName)
config.Data.publication = False
config.Data.runRange = '387853-388784'
config.Data.lumiMask = 'https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions24HI/Cert_Collisions2024_HI_387853_388784_Muon.json'

config.section_("Site")
config.Site.storageSite = "T2_FR_GRIF_LLR"
#config.Site.whitelist = ["T2_CH_CERN"]
