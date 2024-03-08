### HiForest Configuration
# Input: miniAOD
# Type: mc

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_pp_on_PbPb_2023_cff import Run3_pp_on_PbPb_2023
process = cms.Process('HiForest', Run3_pp_on_PbPb_2023)

###############################################################################

# HiForest info
process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 132X, mc")

###############################################################################




# input files
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/n/ncrepet/work/public/root4Matt/cjet/reco_RAW2DIGI_L1Reco_RECO_PAT_inMINIAODSIM_gg2cc_10GevCut_bias.root'
    ),
                            secondaryFileNames = cms.untracked.vstring(
                                'file:/afs/cern.ch/user/n/ncrepet/work/public/root4Matt/cjet/digi_DIGI_L1_DIGI2RAW_HLT_PU_gg2cc_10GevCut_bias.root'
                            ),
                            dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
                            inputCommands = cms.untracked.vstring(
                                'keep *',
                                'drop *_hltGtStage2Digis_*_HLT',
                                'drop *_gtStage2Digis_*_RECO',
                            )
)



# number of events to process, set to -1 to process all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
    )

###############################################################################

# load Global Tag, geometry, etc.
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

#for L1 ########################
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.options = cms.untracked.PSet(
    FailPath = cms.untracked.vstring(),
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    SkipEvent = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

#END for L1 ########################


from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2023_realistic_hi', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC103X_2018PbPb_v4"),
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
         )
])


###############################################################################

# root output
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestMiniAOD.root"))

# # edm output for debugging purposes
# process.output = cms.OutputModule(
#     "PoolOutputModule",
#     fileName = cms.untracked.string('HiForestEDM.root'),
#     outputCommands = cms.untracked.vstring(
#         'keep *',
#         )
#     )

# process.output_path = cms.EndPath(process.output)

###############################################################################

#############################
# Gen Analyzer
#############################
process.load('HeavyIonsAnalysis.EventAnalysis.HiGenAnalyzer_cfi')

# event analysis
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_mc_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

#from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_mc
#process.hltobject.triggerNames = trigger_list_mc

################################
# electrons, photons, muons
process.load('HeavyIonsAnalysis.EGMAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizer.doGenParticles = cms.bool(True)
process.ggHiNtuplizer.doMuons = cms.bool(False)
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
################################
# jet reco sequence
process.load('HeavyIonsAnalysis.JetAnalysis.akCs4PFJetSequence_pponPbPb_mc_cff')
################################
# tracks
process.load("HeavyIonsAnalysis.TrackAnalysis.TrackAnalyzers_cff")
#muons
process.load("HeavyIonsAnalysis.MuonAnalysis.unpackedMuons_cfi")
process.load("HeavyIonsAnalysis.MuonAnalysis.muonAnalyzer_cfi")
process.muonAnalyzer.doGen = cms.bool(True)

###############################################################################

# ZDC analyzer
process.load('HeavyIonsAnalysis.ZDCAnalysis.QWZDC2018Producer_cfi')
process.load('HeavyIonsAnalysis.ZDCAnalysis.QWZDC2018RecHit_cfi')
process.load('HeavyIonsAnalysis.ZDCAnalysis.zdcanalyzer_cfi')

process.zdcanalyzer.doZDCRecHit = False
process.zdcanalyzer.doZDCDigi = True
process.zdcanalyzer.zdcRecHitSrc = cms.InputTag("QWzdcreco")
process.zdcanalyzer.zdcDigiSrc = cms.InputTag("hcalDigis", "ZDC")
process.zdcanalyzer.calZDCDigi = False
process.zdcanalyzer.verbose = False

###############################################################################
# main forest sequence
process.forest = cms.Path(
    process.HiForestInfo +
    process.hltanalysis +
#    process.hltobject +
#    process.l1object +
    process.trackSequencePbPb +
#    process.particleFlowAnalyser +
    process.hiEvtAnalyzer +
    process.HiGenParticleAna +
    process.ggHiNtuplizer +
#    process.zdcdigi +
#    process.QWzdcreco +
    process.zdcanalyzer #+
#    process.unpackedMuons +
#    process.muonAnalyzer
    )

#customisation

addR3Jets = False
addR3FlowJets = False
addR4Jets = False
addR4FlowJets = True
matchJets = True             # Enables q/g and heavy flavor jet identification in MC
addCandidateTagging = False
doHIJetID = True             # Fill jet ID and composition information branches
doWTARecluster = False        # Add jet phi and eta for WTA axis

if addR3Jets or addR3FlowJets or addR4Jets or addR4FlowJets :
    process.load("HeavyIonsAnalysis.JetAnalysis.extraJets_cff")
    from HeavyIonsAnalysis.JetAnalysis.clusterJetsFromMiniAOD_cff import setupHeavyIonJets
    process.load("HeavyIonsAnalysis.JetAnalysis.candidateBtaggingMiniAOD_cff")

    if addR3Jets :
        process.jetsR3 = cms.Sequence()
        jetName = 'akCs3PF'
        setupHeavyIonJets(jetName, process.jetsR3, process, isMC = 1, radius = 0.30, JECTag = 'AK3PF', doFlow = False, matchJets = matchJets)
        process.akCs3PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akCs3PFJetAnalyzer = process.akCs4PFJetAnalyzer.clone(jetTag = jetName + "patJets", jetName = jetName, genjetTag = "ak3GenJetsNoNu", matchJets = matchJets, matchTag = "ak3PFMatchingFor" + jetName + "patJets", doHiJetID = doHIJetID, doWTARecluster = doWTARecluster)
        process.forest += process.extraJetsMC * process.jetsR3 * process.akCs3PFJetAnalyzer

    if addR3FlowJets :
        process.jetsR3flow = cms.Sequence()
        jetName = 'akCs3PFFlow'
        setupHeavyIonJets(jetName, process.jetsR3flow, process, isMC = 1, radius = 0.30, JECTag = 'AK3PF', doFlow = True, matchJets = matchJets)
        process.akCs3PFFlowpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akFlowPuCs3PFJetAnalyzer = process.akCs4PFJetAnalyzer.clone(jetTag = jetName + "patJets", jetName = jetName, genjetTag = "ak3GenJetsNoNu", matchJets = matchJets, matchTag = "ak3PFMatchingFor" + jetName + "patJets", doHiJetID = doHIJetID, doWTARecluster = doWTARecluster)
        process.forest += process.extraFlowJetsMC * process.jetsR3flow * process.akFlowPuCs3PFJetAnalyzer

    if addR4Jets :
        # Recluster using an alias "0" in order not to get mixed up with the default AK4 collections
        process.jetsR4 = cms.Sequence()
        jetName = 'akCs0PF'
        setupHeavyIonJets(jetName, process.jetsR4, process, isMC = 1, radius = 0.40, JECTag = 'AK4PF', doFlow = False, matchJets = matchJets)
        process.akCs0PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akCs4PFJetAnalyzer.jetTag = jetName + 'patJets'
        process.akCs4PFJetAnalyzer.jetName = jetName
        process.akCs4PFJetAnalyzer.matchJets = matchJets
        process.akCs4PFJetAnalyzer.matchTag = 'ak4PFMatchingFor' + jetName + 'patJets'
        process.akCs4PFJetAnalyzer.doHiJetID = doHIJetID
        process.akCs4PFJetAnalyzer.doWTARecluster = doWTARecluster
        process.forest += process.extraJetsMC * process.jetsR4 * process.akCs4PFJetAnalyzer

    if addR4FlowJets :
        process.jetsR4flow = cms.Sequence()
        jetName = 'akCs4PFFlow'
        setupHeavyIonJets(jetName, process.jetsR4flow, process, isMC = 1, radius = 0.40, JECTag = 'AK4PF', doFlow = True, matchJets = matchJets)
        process.akCs4PFFlowpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akFlowPuCs4PFJetAnalyzer.jetTag = jetName + 'patJets'
        process.akFlowPuCs4PFJetAnalyzer.jetName = jetName
        process.akFlowPuCs4PFJetAnalyzer.matchJets = matchJets
        process.akFlowPuCs4PFJetAnalyzer.matchTag = 'ak4PFMatchingFor' + jetName + 'patJets'
        process.akFlowPuCs4PFJetAnalyzer.doHiJetID = doHIJetID
        process.akFlowPuCs4PFJetAnalyzer.doWTARecluster = doWTARecluster
        process.forest += process.extraFlowJetsMC * process.jetsR4flow * process.akFlowPuCs4PFJetAnalyzer 


if addCandidateTagging:
    process.load("HeavyIonsAnalysis.JetAnalysis.candidateBtaggingMiniAOD_cff")

    from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
    updateJetCollection(
        process,
        jetSource = cms.InputTag('slimmedJets'),
        jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
        btagDiscriminators = ['pfCombinedSecondaryVertexV2BJetTags', 'pfDeepCSVDiscriminatorsJetTags:BvsAll', 'pfDeepCSVDiscriminatorsJetTags:CvsB', 'pfDeepCSVDiscriminatorsJetTags:CvsL'], ## to add discriminators,
        btagPrefix = 'TEST',
    )

    process.updatedPatJets.addJetCorrFactors = False
    process.updatedPatJets.discriminatorSources = cms.VInputTag(
        cms.InputTag('pfDeepCSVJetTags:probb'),
        cms.InputTag('pfDeepCSVJetTags:probc'),
        cms.InputTag('pfDeepCSVJetTags:probudsg'),
        cms.InputTag('pfDeepCSVJetTags:probbb'),
    )

    process.akCs4PFJetAnalyzer.jetTag = "updatedPatJets"

    process.forest.insert(1,process.candidateBtagging*process.updatedPatJets)


#########################
# Event Selection -> add the needed filters here
#########################

################ for L1

process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
process.pclusterCompatibilityFilter = cms.Path(process.clusterCompatibilityFilter)
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)
process.load('HeavyIonsAnalysis.EventAnalysis.hffilter_cfi')
process.pphfCoincFilter2Th4 = cms.Path(process.phfCoincFilter2Th4)
process.pAna = cms.EndPath(process.skimanalysis)


process.raw2digi_step = cms.Path(process.RawToDigi)
process.endjob_step = cms.EndPath(process.endOfProcess)

# Schedule definition
process.schedule = cms.Schedule(process.forest,process.raw2digi_step,process.endjob_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
#MassReplaceInputTag(process, new="rawDataMapperByLabel", old="rawDataCollector")

# customisation of the process.                                                                                                                                                                                  

# Automatic addition of the customisation function from L1Trigger.Configuration.customiseReEmul                                                                                                                  
from L1Trigger.Configuration.customiseReEmul import L1TReEmulFromRAW

#call to customisation function L1TReEmulFromRAW imported from L1Trigger.Configuration.customiseReEmul                                                                                                           
process = L1TReEmulFromRAW(process)

# Automatic addition of the customisation function from L1Trigger.L1TNtuples.customiseL1Ntuple                                                                                                                   
from L1Trigger.L1TNtuples.customiseL1Ntuple import L1NtupleRAWEMU

#call to customisation function L1NtupleRAWEMU imported from L1Trigger.L1TNtuples.customiseL1Ntuple                                                                                                              
process = L1NtupleRAWEMU(process)

# Automatic addition of the customisation function from L1Trigger.Configuration.customiseSettings                                                                                                                
from L1Trigger.Configuration.customiseSettings import L1TSettingsToCaloParamsHI_2023_v0_4_2

#call to customisation function L1TSettingsToCaloParamsHI_2023_v0_4_2 imported from L1Trigger.Configuration.customiseSettings                                                                                    
process = L1TSettingsToCaloParamsHI_2023_v0_4_2(process)

# Automatic addition of the customisation function from L1Trigger.Configuration.customiseUtils                                                                                                                   
from L1Trigger.Configuration.customiseUtils import L1TGlobalMenuXML

#call to customisation function L1TGlobalMenuXML imported from L1Trigger.Configuration.customiseUtils                                                                                                            
process = L1TGlobalMenuXML(process)

# End of customisation functions                                                                                                                                                                                 


# Customisation from command line                                                                                                                                                                                

# Add early deletion of temporary data products to reduce peak memory need                                                                                                                                       
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion                                                                                                                                                                                      

process.HFAdcana = cms.EDAnalyzer("HFAdcToGeV",
    digiLabel = cms.untracked.InputTag("hcalDigis"),
    minimized = cms.untracked.bool(True),
    fillhf = cms.bool(False)
)

process.HFAdc = cms.Path(process.HFAdcana)
process.schedule.append(process.HFAdc)

