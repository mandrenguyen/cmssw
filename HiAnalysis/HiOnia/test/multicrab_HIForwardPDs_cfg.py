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
config.JobType.psetName = "hioniaanalyzer_UPC2024_PromptReco_cfg.py"
config.JobType.maxMemoryMB = 2000         # request high memory machines.
config.JobType.numCores = 2
config.JobType.allowUndistributedCMSSW = True #Problems with slc7
#config.JobType.maxJobRuntimeMin = 2000 # max = 2750

config.section_("Data")
config.Data.inputDBS = 'global'
#config.Data.totalUnits = -1
config.Data.splitting = "EventAwareLumiBased"
config.Data.unitsPerJob = 100000000

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

# Submit the jobs: 20 HIForward PDs, ~140k files each, average of 100k events/file

config.Data.outLFNDirBase = '/store/user/fdamas/PbPb2024/UPC/'

eras = "AB"

for i in range(20):

    # for each era
    for era in eras:

        # and for the two versions
        for k in range(1,2):
            config.General.requestName = f'Era{era}_Forward{i}_v{k}'
            config.Data.inputDataset = f"/HIForward{i}/HIRun2024{era}-PromptReco-v{k}/MINIAOD"
            config.Data.outputDatasetTag = config.General.requestName
    

            print("Submitting CRAB job for: "+ config.Data.inputDataset)
            submit(config)
