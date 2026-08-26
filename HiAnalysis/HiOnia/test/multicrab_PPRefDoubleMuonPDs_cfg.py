from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException

# We want to put all the CRAB project directories from the tasks we submit here into one common directory.
# That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).
from CRABClient.UserUtilities import config
config = config()

config.section_("General")
#config.General.requestName = "HIPhysicsRawPrime5_PromptReco_v2"
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "hioniaanalyzer_ppRef2024_PromptReco_cfg.py"
config.JobType.maxMemoryMB = 2000         # request high memory machines.
config.JobType.numCores = 2 # must match the number of threads enabled in the hioniaanalyzer config file!
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
config.JobType.maxJobRuntimeMin = 2000 # max = 2750

config.section_("Data")
config.Data.inputDBS = 'global'
#config.Data.totalUnits = -1
config.Data.splitting = "EventAwareLumiBased"
config.Data.unitsPerJob = 5000000

config.Data.allowNonValidInputDataset = True
config.Data.publication = False
config.Data.runRange = '387474-387721'
config.Data.lumiMask = 'https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions24/Cert_Collisions2024_ppref_387474_387721_Muon.json'

config.section_("Site")
config.Site.storageSite = "T3_CH_CERNBOX"
config.Site.whitelist = ["T2_US_*","T2_CH_CERN","T1_US_*"]

# Multi crab part

def submit(config):
    try:
        crabCommand('submit', config = config, dryrun=False)
    except HTTPException as hte:
        print("Failed submitting task: %s" % (hte.headers))
    except ClientException as cle:
        print("Failed submitting task: %s" % (cle))

# Submit the jobs: 4 PPRefDoubleMuon PDs, ~1.5k files each, average of 150k events/file

for i in range(4):
    config.General.requestName = f'PPRefDoubleMuon{i}'
    config.Data.inputDataset = f"/PPRefDoubleMuon{i}/Run2024J-PromptReco-v1/MINIAOD"
    config.Data.outputDatasetTag = config.General.requestName
    config.Data.outLFNDirBase = '/store/user/fdamas/PPRef2024/'

    print("Submitting CRAB job for: "+ config.Data.inputDataset)
    submit(config)
