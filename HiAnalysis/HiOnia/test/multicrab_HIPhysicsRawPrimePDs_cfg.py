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
config.JobType.psetName = "hioniaanalyzer_PbPbPrompt_DATA_cfg.py"
config.JobType.maxMemoryMB = 2400         # request high memory machines.
#config.JobType.numCores = 4
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
#config.JobType.maxJobRuntimeMin = 2000 # max = 2750

config.section_("Data")
config.Data.inputDBS = 'global'
#config.Data.totalUnits = -1
config.Data.splitting = "EventAwareLumiBased"
config.Data.unitsPerJob = 4000000

config.Data.allowNonValidInputDataset = True
config.Data.publication = False
config.Data.runRange = '387853-388784'
config.Data.lumiMask = 'https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions24HI/Cert_Collisions2024_HI_387853_388784_Muon.json'


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

# Submit the jobs: 60 HIRawPrime PDs, ~140k files each, average of 100k events/file
config.Data.outLFNDirBase = '/store/user/fdamas/PbPb2024/'


### First, HIRun2024A

# The first 15 HIPhysicsRawPrime PDs were promptly reconstructed
for i in range(15): # 0 -> 14
    config.General.requestName = f'EraA_RawPrime{i}'
    config.Data.inputDataset = f"/HIPhysicsRawPrime{i}/HIRun2024A-PromptReco-v1/MINIAOD"
    config.Data.outputDatasetTag = config.General.requestName

    print("Submitting CRAB job for: "+ config.Data.inputDataset)
    submit(config)

# The other 45 PDs were reconstructed later "offline"
for i in range(15, 60):

    # with a "glitch" for number 36 which is promoted to v2, see below...
    if i == 36:
        continue

    config.General.requestName = f'EraA_RawPrime{i}'
    config.Data.inputDataset = f"/HIPhysicsRawPrime{i}/HIRun2024A-OfflinePromptReco-v1/MINIAOD"
    config.Data.outputDatasetTag = config.General.requestName

    print("Submitting CRAB job for: "+ config.Data.inputDataset)
    submit(config)

# with a "glitch" for number 36 which is promoted to v2...
config.General.requestName = 'EraA_RawPrime36'
config.Data.inputDataset = "/HIPhysicsRawPrime36/HIRun2024A-OfflinePromptReco-v2/MINIAOD"
config.Data.outputDatasetTag = config.General.requestName

print("Submitting CRAB job for: "+ config.Data.inputDataset)
submit(config)



### HIRun2024B was completely reconstructed promptly, but there are two independent versions

for i in range(60):

    for k in range(1,3):
        config.General.requestName = f'EraB_RawPrime{i}_v{k}'
        config.Data.inputDataset = f"/HIPhysicsRawPrime{i}/HIRun2024B-PromptReco-v{k}/MINIAOD"
        config.Data.outputDatasetTag = config.General.requestName

        print("Submitting CRAB job for: "+ config.Data.inputDataset)
        submit(config)