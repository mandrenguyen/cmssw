import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing


#----------------------------------------------------------------------------

# Setup Settings for ONIA TREE:
   
isMC           = False    # if input is MONTECARLO: True or if it's DATA: False
applyMuonCuts  = False    # Apply muon ID quality cuts
muonSelection  = "Trk"    # Single muon selection: Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker) are availale
genPDG         = 443      # Generated Particle PDG ID (only needed for MC), Jpsi: 443 , Psi(2S): 100443, Upsilon(1S): 553 , Upsilon(2S): 100553 , Upsilon(2S): 200553

#----------------------------------------------------------------------------

# Print Onia Analysis settings:
print( " " )
print( "[INFO] Settings used for ONIA TREE DATA: " )
print( "[INFO] isMC          = " + ("True" if isMC else "False") )
print( "[INFO] applyMuonCuts = " + ("True" if applyMuonCuts else "False") )
print( "[INFO] muonSelection = " + muonSelection )
print( "[INFO] genPDG        = " + str(genPDG) )
print( " " )


# set up process
process = cms.Process("HIOnia")

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Names
options.outputFile = "OniaTree.root"
options.secondaryOutputFile = "Jpsi_DataSet.root"
options.inputFiles =  'file:/home/llr/cms/stahl/TEST/CMSSW_8_0_26_patch2/src/HiSkim/HiOnia2MuMu/test/onia2MuMuPAT_DATA_80X.root'
options.maxEvents = -1 # -1 means all events

# Get and parse the command line arguments
options.parseArguments()
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories.extend(["GetManyWithoutRegistration","GetByLabelWithoutRegistration"])
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.MessageLogger.categories.extend(["HiOnia2MuMuPAT_muonLessSizeORpvTrkSize"])
process.MessageLogger.cerr.HiOnia2MuMuPAT_muonLessSizeORpvTrkSize = cms.untracked.PSet( limit = cms.untracked.int32(5) )


# Global Tag:
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

process.GlobalTag.toGet = cms.VPSet(
  cms.PSet(
    record = cms.string("HeavyIonRcd"),
    tag = cms.string("CentralityTable_HFtowersPlusTrunc200_EPOS5TeV_v80x01_mc"),
    connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
    label = cms.untracked.string("HFtowersPlusTruncEpos")
    )
  )


### For Centrality
process.load("RecoHI.HiEvtPlaneAlgos.HiEvtPlane_cfi")
process.load("RecoHI.HiEvtPlaneAlgos.hiEvtPlaneFlat_cfi")
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("pACentrality")
process.centralityBin.centralityVariable = cms.string("HFtowersPlusTrunc")
process.centralityBin.nonDefaultGlauberModel = cms.string("Epos")
process.hiEvtPlaneFlat.vertexTag = cms.InputTag("offlinePrimaryVertices")
process.hiEvtPlaneFlat.centralityTag = cms.InputTag("pACentrality")
process.hiEvtPlaneFlat.centralityBinTag = cms.InputTag("centralityBin","HFtowersPlusTrunc")
process.hiEvtPlaneFlat.centralityVariable = cms.string("HFtowersPlusTrunc")
process.hiEvtPlaneFlat.nonDefaultGlauberModel = cms.string("Epos")
process.hiEvtPlane.vertexTag = cms.InputTag("offlinePrimaryVertices")
process.hiEvtPlane.centralityBinTag = cms.InputTag("centralityBin","HFtowersPlusTrunc")
process.hiEvtPlane.centralityVariable = cms.string("HFtowersPlusTrunc")
process.hiEvtPlane.nonDefaultGlauberModel = cms.string("Epos")


process.EventAna_step = cms.Path( process.centralityBin * process.hiEvtPlane * process.hiEvtPlaneFlat )

process.hionia = cms.EDAnalyzer('HiOniaAnalyzer',
                                #-- Collections
                                srcMuon             = cms.InputTag("patMuonsWithTrigger"),     # Name of PAT Muon Collection
                                srcMuonNoTrig       = cms.InputTag("patMuonsWithoutTrigger"),  # Name of PAT Muon Without Trigger Collection
                                src                 = cms.InputTag("onia2MuMuPatGlbGlb"),      # Name of Onia Skim Collection
                                EvtPlane            = cms.InputTag("hiEvtPlane",""),           # Name of Event Plane Collection. For RECO use: hiEventPlane,recoLevel

                                triggerResultsLabel = cms.InputTag("TriggerResults","","HLT"), # Label of Trigger Results

                                #-- Reco Details
                                useBeamSpot = cms.bool(False),  
                                useRapidity = cms.bool(True),
                                
                                #--
                                maxAbsZ = cms.double(24.0),
                                
                                pTBinRanges      = cms.vdouble(0.0, 6.0, 8.0, 9.0, 10.0, 12.0, 15.0, 40.0),
                                etaBinRanges     = cms.vdouble(0.0, 2.5),
                                centralityRanges = cms.vdouble(20,40,100),

                                onlyTheBest        = cms.bool(False),
                                applyCuts          = cms.bool(applyMuonCuts),
                                selTightGlobalMuon = cms.bool(False),
                                storeEfficiency    = cms.bool(False),
                      
                                removeSignalEvents = cms.untracked.bool(False),  # Remove/Keep signal events
                                removeTrueMuons    = cms.untracked.bool(False),  # Remove/Keep gen Muons
                                storeSameSign      = cms.untracked.bool(True),   # Store/Drop same sign dimuons
                                
                                #-- Gen Details
                                oniaPDG = cms.int32(genPDG),
                                muonSel = cms.string(muonSelection),
                                isHI = cms.untracked.bool(False),
                                isPA = cms.untracked.bool(True),
                                isMC = cms.untracked.bool(isMC),
                                isPromptMC = cms.untracked.bool(True),
                                useEvtPlane = cms.untracked.bool(True),
                                useGeTracks = cms.untracked.bool(False),
                                runVersionChange = cms.untracked.uint32(182133),

                                #-- Histogram configuration
                                combineCategories = cms.bool(False),
                                fillRooDataSet    = cms.bool(False),
                                fillTree          = cms.bool(True),
                                fillHistos        = cms.bool(False),
                                minimumFlag       = cms.bool(False),
                                fillSingleMuons   = cms.bool(True),
                                fillRecoTracks    = cms.bool(False),
                                histFileName      = cms.string(options.outputFile),		
                                dataSetName       = cms.string(options.secondaryOutputFile),
                                    
                                # HLT pPb MENU 2016

                                dblTriggerPathNames = cms.vstring("HLT_PAL1DoubleMuOpen_v1",
                                                                  "HLT_PAL1DoubleMuOpen_OS_v1",
                                                                  "HLT_PAL1DoubleMuOpen_SS_v1",
                                                                  "HLT_PAL1DoubleMu0_v1",
                                                                  "HLT_PAL1DoubleMu0_MGT1_v1",
                                                                  "HLT_PAL1DoubleMu0_HighQ_v1",
                                                                  "HLT_PAL2DoubleMu0_v1",
                                                                  "HLT_PAL3DoubleMu0_v1",
                                                                  "HLT_PAL3DoubleMu0_HIon_v1",
                                                                  "HLT_PAL1DoubleMu10_v1",
                                                                  "HLT_PAL2DoubleMu10_v1",
                                                                  "HLT_PAL3DoubleMu10_v1"),

                                dblTriggerFilterNames = cms.vstring("hltL1fL1sDoubleMuOpenBptxANDL1Filtered0",
                                                                    "hltL1fL1sDoubleMuOpenOSBptxANDL1Filtered0",
                                                                    "hltL1fL1sDoubleMuOpenSSBptxANDL1Filtered0",
                                                                    "hltL1fL1sDoubleMu0BptxANDL1Filtered0",
                                                                    "hltL1fL1sDoubleMu0MassGT1BptxANDL1Filtered0",
                                                                    "hltL1fL1sDoubleMu0BptxANDL1HighQFiltered0",
                                                                    "hltL2fL1sDoubleMuOpenBptxANDL1f0L2Filtered0",
                                                                    "hltL3fL1sDoubleMuOpenBptxANDL1f0L2f0L3Filtered0",
                                                                    "hltHIL3fL1sDoubleMuOpenBptxANDL1f0L2f0L3Filtered0",
                                                                    "hltL1fL1sDoubleMu10BptxANDL1Filtered0",
                                                                    "hltL2fL1sDoubleMu10BptxANDL1f0L2Filtered10",
                                                                    "hltL3fL1sDoubleMu10BptxANDL1f0L2f10L3Filtered10"),

                                sglTriggerPathNames = cms.vstring("HLT_PAL2Mu12_v1",
                                                                  "HLT_PAL2Mu15_v1",
                                                                  "HLT_PAL3Mu3_v1",
                                                                  "HLT_PAL3Mu5_v3",
                                                                  "HLT_PAL3Mu7_v1",
                                                                  "HLT_PAL3Mu12_v1",
                                                                  "HLT_PAL3Mu15_v1"),

                                sglTriggerFilterNames = cms.vstring("hltL2fL1sSingleMu7BptxANDL1f0L2Filtered12",
                                                                    "hltL2fL1sSingleMu7BptxANDL1f0L2Filtered15",
                                                                    "hltL3fL1sSingleMu3BptxANDL1f0L2f0L3Filtered3",
                                                                    "hltL3fL1sSingleMu5BptxANDL1f0L2f0L3Filtered5",
                                                                    "hltL3fL1sSingleMu5BptxANDL1f0L2f0L3Filtered7",
                                                                    "hltL3fL1sSingleMu7BptxANDL1f0L2f0L3Filtered12",
                                                                    "hltL3fL1sSingleMu7BptxANDL1f0L2f0L3Filtered15")
                                )

process.hionia.primaryVertexTag = cms.InputTag("offlinePrimaryVertices")
process.hionia.genParticles     = cms.InputTag("genParticles")
process.hionia.muonLessPV       = cms.bool(True)
process.hionia.EvtPlane         = cms.InputTag("hiEvtPlaneFlat","")
process.hionia.CentralitySrc    = cms.InputTag("pACentrality")
process.hionia.CentralityBinSrc = cms.InputTag("centralityBin","HFtowersPlusTrunc")
process.hionia.srcTracks        = cms.InputTag("generalTracks")
      
#Options:
process.source    = cms.Source("PoolSource",
                               fileNames = cms.untracked.vstring( options.inputFiles )
                               )
process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string( options.outputFile )
                                   )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.options   = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.p         = cms.Path(process.hionia)
process.schedule  = cms.Schedule( process.EventAna_step, process.p )
