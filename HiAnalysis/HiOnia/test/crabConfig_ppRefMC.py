from WMCore.Configuration import Configuration

config = Configuration()

config.section_("General")
config.General.requestName = "PromptJpsi"
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "hioniaanalyzer_ppRef_MC_cfg.py"
config.JobType.maxMemoryMB = 2000         # request high memory machines.
#config.JobType.numCores = 4
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
config.JobType.maxJobRuntimeMin = 1200 #2750    # request longer runtime, ~48 hours.


config.section_("Data")
config.Data.inputDataset = '/PromptJPsiToMuMu_pThat-2_TuneCP5_5p36TeV_pythia8/RunIIIpp5p36Winter24MiniAOD-141X_mcRun3_2024_realistic_ppRef5TeV_v7-v2/MINIAODSIM'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 40
#config.Data.totalUnits = -1
config.Data.splitting = "FileBased"
config.Data.allowNonValidInputDataset = True
config.Data.outputDatasetTag = config.General.requestName

config.Data.outLFNDirBase = '/store/user/fdamas/PPRef2024/RunPrepMC/'
config.Data.publication = False

config.section_("Site")
config.Site.storageSite = "T3_CH_CERNBOX"
config.Site.whitelist = ["T2_US_*","T1_US_*","T2_CH_CERN","T2_FR_*"]
