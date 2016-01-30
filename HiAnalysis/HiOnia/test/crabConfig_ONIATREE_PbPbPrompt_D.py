from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
config.General.requestName = 'HIOniaL1DoubleMu0D_HIRun2015-PromptReco-v1_Run_262548_263757_ONIATREE_20160128_noCUT'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'hioniaanalyzer_PbPbPrompt_D_cfg.py'
config.JobType.maxMemoryMB = 2500
config.JobType.outputFiles = ['OniaTree.root']

config.section_('Data')
config.Data.inputDataset = '/HIOniaL1DoubleMu0D/anstahll-HIOniaL1DoubleMu0D_HIRun2015-PromptReco-v1_Run_262548_263757_ONIASKIM_v2-4f1bd322b73cd234f35e9d53a49ca839/USER'
config.Data.inputDBS = 'phys03'
config.Data.unitsPerJob = 10
config.Data.splitting = 'LumiBased'
config.Data.runRange = '262548-263757'
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/HI/Cert_262548-263757_PromptReco_HICollisions15_JSON_MuonPhys.txt'
config.Data.outLFNDirBase = '/store/user/%s/HIPromptReco/%s' % (getUsernameFromSiteDB(), config.General.requestName)
config.Data.publication = False

config.section_('Site')
config.Site.whitelist = ["T2_FR_GRIF_LLR"]
config.Site.storageSite = 'T2_FR_GRIF_LLR'

# If your site is blacklisted by crab, use:
#config.Data.ignoreLocality = True
#config.Site.whitelist = ["T1_FR*","T2_FR*","T3_FR*"]
