from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
config.General.requestName = 'SingleMuPt050_Pythia8Gun_ONIATREE_20160730'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'hioniaproducer_TriggerStudy2016_pPb_SiMuMC_cfg.py'
config.JobType.outputFiles = ['OniaTree.root']
config.JobType.maxMemoryMB = 2500

config.section_('Data')
config.Data.inputDataset ='/SingleMuPythia8Gun/anstahll-SingleMuPt050_pythia8Gun_RECO_20160730-f00d99212c270a6815523bc4da29a3be/USER'
config.Data.inputDBS = 'phys03'
config.Data.unitsPerJob = 1
config.Data.splitting = 'FileBased'

config.Data.outLFNDirBase = '/store/user/%s/TriggerStudy2016/MC/%s' % (getUsernameFromSiteDB(), config.General.requestName)
config.Data.publication = False
config.Data.outputDatasetTag = config.General.requestName

config.section_('Site')
config.Data.ignoreLocality = True
config.Site.storageSite = 'T2_FR_GRIF_LLR'
