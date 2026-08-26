from WMCore.Configuration import Configuration

config = Configuration()

config.section_("General")
config.General.requestName = "OniaTree_pPb"
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "hioniaanalyzer_2016pPb_UPCreco_DATA_cfg.py"
#config.JobType.maxMemoryMB = 2500         # request high memory machines.
config.JobType.numCores = 4
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
#config.JobType.maxJobRuntimeMin = 1000 #2750    # request longer runtime, ~48 hours.

config.section_("Data")
config.Data.inputDataset = '/PADoubleMuon/PARun2016C-16Dec2024-v1/MINIAOD'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 50
#config.Data.totalUnits = -1
config.Data.splitting = "LumiBased"
config.Data.allowNonValidInputDataset = True

config.Data.outLFNDirBase = '/store/user/fdamas/%s' % (config.General.requestName)
config.Data.publication = False

# for pPb
config.Data.runRange = '285479-285832'
config.Data.lumiMask = 'https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions16/HI/Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T_MuonPhys.txt'

# for Pbp
#config.Data.runRange = '285952-286496'
#config.Data.lumiMask = 'https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions16/HI/Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T_MuonPhys.txt'

config.section_("Site")
config.Site.storageSite = "T2_FR_GRIF_LLR"
#config.Site.whitelist = ["T2_CH_CERN"]
