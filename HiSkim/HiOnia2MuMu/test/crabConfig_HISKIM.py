from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
config.General.requestName = 'Run2015E-v1_Run_262163_262254_OniaSKIM_20151121'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'onia2MuMuPATHI_7xy_cfg.py'
config.JobType.maxMemoryMB = 2400

config.section_('Data')
config.Data.inputDataset ='/ExpressPhysics/Run2015E-Express-v1/FEVT'  
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 50
config.Data.splitting = 'LumiBased'
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/HI/DCSOnly/json_DCSONLY.txt'
config.Data.runRange = '262163-262254'
config.Data.outLFNDirBase = '/store/user/%s/ExpressStream/%s' % (getUsernameFromSiteDB(), config.General.requestName)
config.Data.publication = False
#config.Data.outputDatasetTag = config.General.requestName

config.section_('Site')
config.Site.storageSite = 'T2_FR_GRIF_LLR'
