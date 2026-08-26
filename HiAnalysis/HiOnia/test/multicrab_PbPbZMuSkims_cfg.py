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
config.JobType.psetName = "Zanalyzer_PbPb2024_PromptReco_cfg.py"
config.JobType.maxMemoryMB = 1400         # request high memory machines.
#config.JobType.numCores = 4
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
config.JobType.maxJobRuntimeMin = 1200 # max = 2750

config.section_("Data")
config.Data.inputDBS = 'global'
#config.Data.totalUnits = -1
config.Data.splitting = "FileBased"
config.Data.unitsPerJob = 80

config.Data.allowNonValidInputDataset = True
config.Data.publication = False
config.Data.runRange = '387853-388784'
config.Data.lumiMask = 'https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions24HI/Cert_Collisions2024_HI_387853_388784_Muon.json'


config.section_("Site")
config.Site.storageSite = "T3_CH_CERNBOX"
#config.Site.whitelist = ["T2_US_*","T2_CH_CERN","T1_US_*"]

# Multi crab part

def submit(config):
    try:
        crabCommand('submit', config = config, dryrun=False)
    except HTTPException as hte:
        print("Failed submitting task: %s" % (hte.headers))
    except ClientException as cle:
        print("Failed submitting task: %s" % (cle))

# Submit the jobs: 32 HIRawPrime PDs, ~5.5k files each, average of 100k events/file

config.Data.outLFNDirBase = '/store/user/fdamas/PbPb2024/ZMuSkims/'


for i in range(15):

    config.General.requestName = f'EraA_ZMuRawPrime{i}'
    config.Data.inputDataset = f"/HIPhysicsRawPrime{i}/HIRun2024A-PbPbZMu-PromptReco-v1/RAW-RECO"
    config.Data.outputDatasetTag = 'EraA'

    print("Submitting CRAB job for: "+ config.Data.inputDataset)
    submit(config)

for i in range(60):

    config.General.requestName = f'EraB_ZMuRawPrime{i}'
    config.Data.inputDataset = f"/HIPhysicsRawPrime{i}/HIRun2024B-PbPbZMu-PromptReco-v1/RAW-RECO"
    config.Data.outputDatasetTag = 'EraB'

    print("Submitting CRAB job for: "+ config.Data.inputDataset)
    submit(config)
