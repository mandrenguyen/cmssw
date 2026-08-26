from WMCore.Configuration import Configuration

config = Configuration()

config.section_("General")
config.General.requestName = "DoubleMu_Run2017G_AOD_Run_306546_306826_trimuonSkim_05082019"#306826
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "HiSkim/HiOnia2MuMu/test/SimpleTrimuonSkim.py"
#config.JobType.maxMemoryMB = 2500         # request high memory machines.
#config.JobType.maxJobRuntimeMin = 2750    # request longer runtime, ~48 hours.

config.section_("Data")
config.Data.inputDataset = '/DoubleMuon/Run2017G-17Nov2017-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 100
#config.Data.totalUnits = -1
config.Data.splitting = 'LumiBased'
config.Data.outLFNDirBase = '/store/user/gfalmagn/AOD/%s' % (config.General.requestName)
#config.Data.outLFNDirBase = '/store/group/phys_heavyions/dileptons/Data2018/PbPb502TeV/TTrees/PromptAOD'
config.Data.publication = True
config.Data.runRange = '306546-306826'#'306546-306826'
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/5TeV/ReReco/Cert_306546-306826_5TeV_EOY2017ReReco_Collisions17_JSON_MuonPhys.txt'

config.section_('Site')
config.Site.storageSite = 'T2_FR_GRIF_LLR'

