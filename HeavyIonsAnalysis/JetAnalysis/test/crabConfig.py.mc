from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
config.General.requestName = 'forestOnia_nonprompt_noreclustering'


config.General.transferOutputs = True
config.General.transferLogs = True

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
#config.JobType.psetName = 'runForestAOD_PbPb_MIX_75X.py'
config.JobType.psetName = 'runForestAOD_pp_MC_75X.py'
config.JobType.maxMemoryMB = 2400

config.section_('Data')



config.Data.inputDataset = '/BJpsiMM_5p02TeV_TuneCUETP8M1/HINppWinter16DR-75X_mcRun2_asymptotic_ppAt5TeV_v3-v1/AODSIM'


### central prod

## new tune
#config.Data.inputDataset = '/Pythia6_Dijet15_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet30_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet50_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet80_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet100_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet120_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet170_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR30_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR50_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR80_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR100_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR120_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR170_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet30_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet50_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet80_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet100_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet120_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet170_pp502_Hydjet_Cymbal_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v14-v1/AODSIM'

## old tune
## QCD
#config.Data.inputDataset = '/Pythia6_Dijet30_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet50_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet80_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet100_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet120_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_Dijet170_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'

## FCR
#config.Data.inputDataset = '/Pythia6_bJetFCR30_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR80_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR120_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJetFCR170_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'

## b-jet
#config.Data.inputDataset = '/Pythia6_bJet30_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet50_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet80_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet100_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet120_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'
#config.Data.inputDataset = '/Pythia6_bJet170_pp502_Hydjet_MB/HINPbPbWinter16DR-75X_mcRun2_HeavyIon_v13-v1/AODSIM'

### private prod
#config.Data.inputDataset = '/PyquenColl_Hydjet_5020GeV/mnguyen-PyquenColl_qcd100_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/PyquenColl_Hydjet_5020GeV/mnguyen-PyquenColl_bfcr100_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/PyquenColl_Hydjet_5020GeV/mnguyen-PyquenColl_bfcr120_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/PyquenColl_Hydjet_5020GeV/mnguyen-PyquenColl_qcd120_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'

#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_bfcr120_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_bfcr100_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_bfcr80_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_bfcr50_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_bfcr30_Hydjet_5020GeV_RECO_take2-031f3c4929f438d65ab71732c00a0cf4/USER'

#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr30_TuneZ2_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr50_TuneZ2_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr80_TuneZ2_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr100_TuneZ2_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr120_TuneZ2_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr30_TuneZ2_5020GeV_RECO_part2-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr50_TuneZ2_5020GeV_RECO_part2-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr80_TuneZ2_5020GeV_RECO_part2-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr100_TuneZ2_5020GeV_RECO_part2-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr120_TuneZ2_5020GeV_RECO_part2-0564587735dfa98972125c928a8975ef/USER'

#config.Data.inputDataset ='/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_qcd30_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_qcd50_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_qcd80_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_qcd100_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_qcd120_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_qcd80_TuneZ2_Hydjet_5020GeV_RECO_part2-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_qcd50_TuneZ2_Hydjet_5020GeV_RECO_part2-031f3c4929f438d65ab71732c00a0cf4/USER'

#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_bjet120_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_bjet80_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_bjet50_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_bjet30_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_bjet100_TuneZ2_Hydjet_5020GeV_RECO-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_bjet50_TuneZ2_Hydjet_5020GeV_RECO_part2-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_Hydjet_5020GeV/mnguyen-Pythia6_bjet80_TuneZ2_Hydjet_5020GeV_RECO_part2-031f3c4929f438d65ab71732c00a0cf4/USER'

#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd80_TuneZ2_5020GeV_RECO-eca985b12211699dc9125db438586ff6/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd50_TuneZ2_5020GeV_RECO-eca985b12211699dc9125db438586ff6/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd30_TuneZ2_5020GeV_RECO-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd120_TuneZ2_5020GeV_RECO-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd100_TuneZ2_5020GeV_RECO-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd50_TuneZ2_5020GeV_RECO_part2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd80_TuneZ2_5020GeV_RECO_part2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet80_TuneZ2_5020GeV_RECO_part2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet50_TuneZ2_5020GeV_RECO_part2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_bjet100_TuneZ2_5020GeV_GEN-SIM_part2/mnguyen-Pythia6_bjet100_TuneZ2_5020GeV_RECO_part2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet30_TuneZ2_5020GeV_RECO_v2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet50_TuneZ2_5020GeV_RECO_v2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet80_TuneZ2_5020GeV_RECO_v2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet100_TuneZ2_5020GeV_RECO-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet120_TuneZ2_5020GeV_RECO_v2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd65_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet65_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr65_5020GeV_RECO_take3-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bfcr15_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_qcd15_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet15_5020GeV_RECO-0564587735dfa98972125c928a8975ef/USER'

#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_qcd100_Signal_5020GeV_RECO_take9-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset ='/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_qcd100_Hydjet_5020GeV_RECO_take9-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_TuneZ2_qcd100_DoubleHydjet_5020GeV_RECO_take9_ccinp3_3p9GB-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/doubleHydjet_5020GeV/mnguyen-NuGun_Hydjet_5020GeV_RECO_take2-031f3c4929f438d65ab71732c00a0cf4/USER'
#config.Data.inputDataset = '/doubleHydjet_5020GeV/mnguyen-NuGun_DoubleHydjet_5020GeV_RECO_take2_ccinp3_3p9GB-031f3c4929f438d65ab71732c00a0cf4/USER'


#config.Data.inputDBS = 'phys03'
config.Data.unitsPerJob = 1
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = '/store/user/%s/forestMC75X' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = config.General.requestName

config.section_('Site')
config.Site.storageSite = 'T2_FR_GRIF_LLR'
#config.Site.whitelist = ['T2_US_MIT']
