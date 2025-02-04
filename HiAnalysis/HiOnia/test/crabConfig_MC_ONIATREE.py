from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
config.General.requestName = 'BcToJpsiMuNu_BCVEGPY_PYTHIA8_pp5TeV_22012019_2_ONIATREE_NoCuts'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'HiAnalysis/HiOnia/test/hioniaanalyzer_ppPrompt_trimuons_94X_MC_cfg.py'
config.JobType.outputFiles = ['Oniatree_MC_trimuons_22012019.root']
config.JobType.maxMemoryMB = 2500

config.section_('Data')
config.Data.inputDataset ='/BcToJpsiMuNu/gfalmagn-BcToJpsiMuNu_BCVEGPY_PYTHIA8_pp5TeV_22012019_2_reco_NoCuts-f2dad0906efe35dd2b4ba4e0d7abc43c/USER'
config.Data.inputDBS = 'phys03'
config.Data.unitsPerJob = 10
config.Data.splitting = 'FileBased'

config.Data.outLFNDirBase = '/store/user/%s/Bc_analysis/MC/%s' % (getUsernameFromSiteDB(), config.General.requestName)
config.Data.publication = True
config.Data.outputDatasetTag = config.General.requestName

config.section_('Site')
#config.Data.ignoreLocality = True
config.Site.storageSite = 'T2_FR_GRIF_LLR'
