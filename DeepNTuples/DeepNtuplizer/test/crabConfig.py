from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.requestName = 'DeepNtuples_nanFix_bjet_pThat80'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_TagInfo_DeepNtuplizer.py'
config.JobType.numCores = 1
config.JobType.maxMemoryMB = 2000
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles=['QGL_cmssw8020_v2.db']

config.section_('Data')
#config.Data.inputDataset ='/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-FixL1CaloGT_New_Release_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM'
#config.Data.inputDataset ='/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-FixL1CaloGT_New_Release_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM'
config.Data.inputDataset ='/Bjet_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/HINPbPbSpring21MiniAOD-FixL1CaloGT_New_Release_112X_upgrade2018_realistic_HI_v9-v1/MINIAODSIM'
#config.Data.inputDBS = 'phys03' 
#config.Data.splitting = 'FileBased'
config.Data.splitting = 'LumiBased'
#config.Data.splitting = 'Automatic'
config.Data.unitsPerJob = 2
#config.Data.unitsPerJob = 800
config.Data.totalUnits = -1
config.Data.outLFNDirBase = '/store/user/mnguyen/' 
config.Data.publication = True
config.Data.outputDatasetTag = config.General.requestName
#config.Data.userInputFiles = 'QGL_cmssw8020_v2.db'

config.section_('Site')
config.Data.ignoreLocality = False
config.Site.storageSite = 'T2_FR_GRIF_LLR'
config.Site.whitelist = ['T2_FR_*', 'T2_CH_*']
#config.Site.whitelist = ['T2_FR_GRIF_LLR']
