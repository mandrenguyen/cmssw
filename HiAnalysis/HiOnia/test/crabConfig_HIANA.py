from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
config.General.requestName = 'Run2015E-v1_Run_262163_262174_OniaTree_20151121'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'hioniaanalyzer_cfg.py'
config.JobType.maxMemoryMB = 2400
config.JobType.outputFiles = ['OniaTree.root']               

config.section_('Data')
config.Data.inputDataset =''  # SET INPUT DATASET
config.Data.inputDBS = 'phys03'
config.Data.unitsPerJob = 1
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = '/store/user/%s/ExpressStream/%s' % (getUsernameFromSiteDB(), config.General.requestName)
config.Data.publication = False


config.section_('Site')
config.Site.storageSite = 'T2_FR_GRIF_LLR'


