from WMCore.Configuration import Configuration
config = Configuration()

#from CRABClient.UserUtilities import config, getUsernameFromSiteDB
#config = config()

### General ###
config.section_('General')
config.General.requestName = 'HIOniaForest_DATA_pp5TeV_YMM_removeCuts_part2'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

#### JobType ####
config.section_('JobType')
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "runOniaForestAOD_pp_DATA_94X.py"
config.JobType.maxMemoryMB = 4000
config.JobType.allowUndistributedCMSSW = True

#### Data ####
config.section_("Data")
config.Data.inputDataset = "/DoubleMuon/Run2017G-17Nov2017-v1/AOD"
#config.Data.lumiMask = 'Cert_306546-306826_5TeV_EOY2017ReReco_Collisions17_JSON.txt'
config.Data.lumiMask = 'crab_projects/crab_HIOniaForest_DATA_pp5TeV_YMM_removeCuts/results/notFinishedLumis.json'
config.Data.inputDBS = "global"
config.Data.splitting = "LumiBased"
config.Data.unitsPerJob = 1
#config.Data.totalUnits = 
config.Data.publication = False
config.Data.ignoreLocality = True

config.Data.outLFNDirBase = '/store/user/mnguyen/DATA_pp5TeV/%s' % (config.General.requestName)
config.Data.outputDatasetTag = config.General.requestName
#crabCommand('submit', config = config)

#### Site ####
config.section_('Site')
#config.Site.whitelist = ['T2_FR_GRIF_*',"T2_CH_CERN"]
config.Site.whitelist = ['T2_FR_*',"T2_CH_*","T2_BE_*"]
config.Site.storageSite = 'T2_FR_GRIF_LLR'   
