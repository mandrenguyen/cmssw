from WMCore.Configuration import Configuration
config = Configuration()


### General ###
config.section_('General')
config.General.requestName = 'QCDPhoton_L1test_useParent'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

#### JobType ####
config.section_('JobType')
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "forestL1_miniAOD_run3_MC.py"
config.JobType.maxMemoryMB = 3000
config.JobType.allowUndistributedCMSSW = True
config.JobType.numCores = 1

#### Data ####
config.section_("Data")
config.Data.inputDataset = "/QCDPhoton_pThat15_Run3_HydjetEmbedded/mnguyen-QCDPhoton_pThat15_Run3_HydjetEmbedded_mAOD-04cb4abe7df7f58bc7ae99950237d9d9/USER"
config.Data.inputDBS = "phys03"
config.Data.splitting = "FileBased"
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.outLFNDirBase = '/store/user/mnguyen/l1test/' 
config.Data.outputDatasetTag = config.General.requestName
config.Data.allowNonValidInputDataset = True
config.Data.useParent = True
#config.Data.secondaryInputDataset = "/QCDPhoton_pThat15_Run3_HydjetEmbedded/mnguyen-QCDPhoton_pThat15_Run3_HydjetEmbedded_DIGI-752b0cc9d22f4a7f20eccfe0d5df0682/USER"

#### Site ####
config.section_('Site')
config.Site.storageSite = 'T2_FR_GRIF_LLR'
