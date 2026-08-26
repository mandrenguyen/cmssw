import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras
import os

#----------------------------------------------------------------------------

# Setup Settings for ONIA TREE: 2024 PbPb data, PAG's reco of streamer files

HLTProcess     = "HLT" # Name of HLT process
isMC           = False # if input is MONTECARLO: True or if it's DATA: False
muonSelection  = "GlbOrTrk" # Single muon selection: All, Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker), GlbOrTrk, TwoGlbAmongThree (which requires two isGlobal for a trimuon, and one isGlobal for a dimuon) are available
applyEventSel  = True # Only apply Event Selection if the required collections are present
OnlySoftMuons  = False # Keep only isSoftMuon's (without highPurity, and without isGlobal which should be put in 'muonSelection' parameter) from the beginning of HiSkim. If you want the full SoftMuon selection, set this flag false and add 'isSoftMuon' in lowerPuritySelection. In any case, if applyCuts=True, isSoftMuon is required at HiAnalysis level for muons of selected dimuons.
applyCuts      = False # At HiAnalysis level, apply kinematic acceptance cuts + identification cuts (isSoftMuon (without highPurity) or isTightMuon, depending on TightGlobalMuon flag) for muons from selected di(tri)muons + hard-coded cuts on the di(tri)muon that you would want to add (but recommended to add everything in LateDimuonSelection, applied at the end of HiSkim)
SumETvariables = True  # Whether to write out SumET-related variables
SofterSgMuAcceptance = False # Whether to accept muons with a softer acceptance cuts than the usual (pt>3.5GeV at central eta, pt>1.5 at high |eta|). Applies when applyCuts=True
doTrimuons     = False # Make collections of trimuon candidates in addition to dimuons, and keep only events with >0 trimuons (if atLeastOneCand)
doDimuonTrk    = False # Make collections of Jpsi+track candidates in addition to dimuons
atLeastOneCand = False # Keep only events that have one selected dimuon (or at least one trimuon if doTrimuons = true). BEWARE this can cause trouble in .root output if no event is selected by onia2MuMuPatGlbGlbFilter!
OneMatchedHLTMu = -1   # Keep only di(tri)muons of which the one(two) muon(s) are matched to the HLT Filter of this number. You can get the desired number in the output of oniaTree. Set to -1 for no matching.
#############################################################################
keepExtraColl  = False # General Tracks + Stand Alone Muons + Converted Photon collections
miniAOD        = True # whether the input file is in miniAOD format (default is AOD)
UsePropToMuonSt = True # whether to use L1 propagated muons (works only for miniAOD now)
pdgId = 443 # J/Psi : 443, Y(1S) : 553
useMomFormat = "vector" # default "array" for TClonesArray of TLorentzVector. Use "vector" for std::vector<float> of pt, eta, phi, M
#----------------------------------------------------------------------------

# Print Onia Tree settings:
print( " " )
print( "[INFO] Settings used for ONIA TREE: " )
print( "[INFO] isMC                 = " + ("True" if isMC else "False") )
print( "[INFO] applyEventSel        = " + ("True" if applyEventSel else "False") )
print( "[INFO] applyCuts            = " + ("True" if applyCuts else "False") )
print( "[INFO] keepExtraColl        = " + ("True" if keepExtraColl else "False") )
print( "[INFO] SumETvariables       = " + ("True" if SumETvariables else "False") )
print( "[INFO] SofterSgMuAcceptance = " + ("True" if SofterSgMuAcceptance else "False") )
print( "[INFO] muonSelection        = " + muonSelection )
print( "[INFO] onlySoftMuons        = " + ("True" if OnlySoftMuons else "False") )
print( "[INFO] doTrimuons           = " + ("True" if doTrimuons else "False") )
print( "[INFO] doDimuonTrk          = " + ("True" if doDimuonTrk else "False") )
print( "[INFO] atLeastOneCand       = " + ("True" if atLeastOneCand else "False") )
print( "[INFO] OneMatchedHLTMu      = " + ("True" if OneMatchedHLTMu > -1 else "False") )
print( "[INFO] miniAOD              = " + ("True" if miniAOD else "False") )
print( "[INFO] UsePropToMuonSt      = " + ("True" if UsePropToMuonSt else "False") )
print( " " )

# set up process
process = cms.Process("HIOnia", eras.Run3_pp_on_PbPb_2024)

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Name

runNb = 387973

PDname = 'HIPhysicsRawPrime0'

recoFormat = 'mini' # simply '' for AOD

inputPath = f'/eos/cms/store/group/phys_heavyions/vavladim/RECO2024/CRAB_UserFiles/crab_{recoFormat}AOD_Physics{PDname}_{runNb}/241109_092437/0000/'

print(inputPath)

fileList = [f"file:{os.path.join(inputPath, f)}" for f in os.listdir(inputPath) if f.endswith(".root")]


options.inputFiles = fileList


options.maxEvents = -1 # -1 means all events


outputName = f'Oniatree_{PDname}_Run{runNb}_{recoFormat}AOD'

options.outputFile = f"/eos/cms/store/group/phys_heavyions/dileptons/Data2024/PbPb/FastOniatrees/{outputName}.root"

options.maxEvents = -1 # -1 means all events

# Get and parse the command line arguments
options.parseArguments()

triggerList    = {
		# Double Muon Trigger List
		'DoubleMuonTrigger' : cms.vstring(
                        "HLT_HIL1DoubleMu0_MaxDr3p5_Open_v",#0
                        "HLT_HIL1DoubleMu0_v",#1
                        "HLT_HIL1DoubleMu0_SQ_v",#2
                        "HLT_HIL2DoubleMu0_Open_v",#3
                        "HLT_HIL2DoubleMu0_M1p5to6_Open_v",#4
                        "HLT_HIL2DoubleMu2p8_M1p5to6_Open_v",#5
                        "HLT_HIL2DoubleMu0_M7to15_Open_v",#6
                        "HLT_HIL2DoubleMu3_M7to15_Open_v",#7
                        "HLT_HIL2DoubleMu0_SQ_v",#8
                        "HLT_HIL2DoubleMuOpen_Centrality40to100_v",#9
                        "HLT_HIL2DoubleMuOpen_OS_v",#10
                        "HLT_HIL2DoubleMuOpen_SS_v",#11
                        #"HLT_HIL3DoubleMu0_M0toInf_Open_v",#8
                        #"HLT_HIL3DoubleMu0_Quarkonia_Open_v",#9
                        #"HLT_HIL3DoubleMu2_Quarkonia_Open_v",#10
                        #"HLT_HIL3DoubleMu0_M2to4p5_Open_v",#11
                        #"HLT_HIL3DoubleMu2_M2to4p5_Open_v",#12
                        #"HLT_HIL3DoubleMu0_M7to15_Open_v",#13
                        #"HLT_HIL3DoubleMu2_M7to15_Open_v",#14
                        ),
                # Single Muon Trigger List
                'SingleMuonTrigger' : cms.vstring(
                        "HLT_HIL1SingleMu0_Open_v",#12
                        "HLT_HIL1SingleMu0_v",#13
                        "HLT_HIL1SingleMu0_Centrality40to100_v",#14
                        "HLT_HIL1SingleMu0_Centrality30to100_v",#15
                        "HLT_HIL1SingleMuOpen_Centrality30to100_v",#16
                        "HLT_HIL2SingleMu3_Open_v",#17
                        "HLT_HIL2SingleMu5_v",#18
                        "HLT_HIL2SingleMu7_v",#19
                        "HLT_HIL2SingleMu12_v",#20
                        "HLT_HIL2SingleMu0_Centrality40to100_v",#21
                        "HLT_HIL2SingleMu0_Centrality30to100_v",#22
                        "HLT_HIL2SingleMuOpen_Centrality30to100_v",#23
                        #"HLT_HIL3SingleMu3_Open_v",#24
                        #"HLT_HIL3SingleMu5_v",#25
                        #"HLT_HIL3SingleMu7_v",#26
                        #"HLT_HIL3SingleMu12_v",#27
                        "HLT_HIMinimumBiasHF1AND_v", #28
                        "HLT_HIMinimumBiasHF1ANDZDC2nOR_v", #29
                        "HLT_HIMinimumBiasHF1ANDZDC1nOR_v", #30
			)
                }

# Global tag, see https://github.com/cms-sw/cmssw/blob/master/Configuration/AlCa/python/autoCond.py
if isMC:
  globalTag = 'auto:phase1_2024_realistic_hi' #for Run3 MC : phase1_2023_realistic_hi
else:
  globalTag = '141X_dataRun3_Express_v3'

#----------------------------------------------------------------------------

# load the Geometry and Magnetic Field
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

# Global Tag:
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, globalTag, '')

### For Centrality
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
print('\n\033[31m~*~ USING NOMINAL CENTRALITY TABLE FOR 2023 PbPb DATA ~*~\033[0m\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_Run3v1302x04_Nominal_Offline"),
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        label = cms.untracked.string("HFtowers")
        ),
    ])

#----------------------------------------------------------------------------

# For OniaTree Analyzer
from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process,
                 muonTriggerList=triggerList, HLTProName=HLTProcess,
                 muonSelection=muonSelection, L1Stage=2, isMC=isMC, pdgID=pdgId, outputFileName=options.outputFile, doTrimu=doTrimuons,
                 OnlySingleMuons=False
)

#if applyCuts:
process.onia2MuMuPatGlbGlb.LateDimuonSel = cms.string("userFloat(\"vProb\")>0.005")
process.onia2MuMuPatGlbGlb.onlySoftMuons = cms.bool(OnlySoftMuons)
process.hionia.minimumFlag      = cms.bool(keepExtraColl)           #for Reco_trk_*
process.hionia.useGeTracks      = cms.untracked.bool(keepExtraColl) #for Reco_trk_*
process.hionia.fillRecoTracks   = cms.bool(keepExtraColl)           #for Reco_trk_*
process.hionia.CentralitySrc    = cms.InputTag("hiCentrality")
process.hionia.CentralityBinSrc = cms.InputTag("centralityBin","HFtowers")
process.hionia.SofterSgMuAcceptance = cms.bool(SofterSgMuAcceptance)
process.hionia.SumETvariables   = cms.bool(SumETvariables)
process.hionia.applyCuts        = cms.bool(applyCuts)
process.hionia.AtLeastOneCand   = cms.bool(atLeastOneCand)
process.hionia.OneMatchedHLTMu  = cms.int32(OneMatchedHLTMu)
process.hionia.checkTrigNames   = cms.bool(False)#change this to get the event-level trigger info in hStats output (but creates lots of warnings when fake trigger names are used)
process.hionia.mom4format       = cms.string(useMomFormat)

process.oniaTreeAna.replace(process.hionia, process.centralityBin * process.hionia )

if applyEventSel:
  # Offline event filters
  process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
  process.load('HeavyIonsAnalysis.EventAnalysis.hffilter_cfi')
  #process.oniaTreeAna.replace(process.hionia, process.phfCoincFilter2Th4 * process.primaryVertexFilter * process.clusterCompatibilityFilter * process.hionia )

  # HLT trigger firing events
  import HLTrigger.HLTfilters.hltHighLevel_cfi
  process.hltHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
  process.hltHI.HLTPaths = ["HLT_HIL*SingleMu*_v*", "HLT_HIL*DoubleMu*_v*", "HLT_HIMinimumBiasHF1AND*_v*"]
  process.hltHI.throw = False
  process.hltHI.andOr = True

  # Muon filtering
  SuperLooseMuonCut = "(isTrackerMuon || isGlobalMuon) && pt > 1. && abs(eta) < 2.4"

  MUONCUT = SuperLooseMuonCut
  
  process.muonSelector = cms.EDFilter("PATMuonRefSelector",
                                        src = cms.InputTag("slimmedMuons"),
                                        cut = cms.string(MUONCUT),
                                        filter = cms.bool(True)
  )

  process.atLeastTwoMuons = cms.EDFilter("MuonRefPatCount",
                                 src = cms.InputTag("slimmedMuons"),
                                  cut = cms.string(MUONCUT),
                                 minNumber = cms.uint32(2)
                                 )

  process.dimuonSelection = cms.EDProducer("CandViewShallowCloneCombiner",
                                    checkCharge = cms.bool(True),
                                    cut = cms.string("mass > 2.5"),
                                    decay = cms.string("muonSelector@+ muonSelector@-")
                                    )

  process.atLeastOneDimuon = cms.EDFilter("CandViewCountFilter",
                                        src = cms.InputTag("dimuonSelection"),
                                        minNumber = cms.uint32(1)
                                        )
  
  process.oniaTreeAna.replace(process.patMuonSequence,process.muonSelector * process.atLeastTwoMuons * process.dimuonSelection * process.atLeastOneDimuon * process.phfCoincFilter2Th4 * process.primaryVertexFilter * process.clusterCompatibilityFilter * process.patMuonSequence )

if atLeastOneCand:
  if doTrimuons:
      process.oniaTreeAna.replace(process.onia2MuMuPatGlbGlb, process.onia2MuMuPatGlbGlb * process.onia2MuMuPatGlbGlbFilterTrimu)
      process.oniaTreeAna.replace(process.patMuonSequence, process.filter3mu * process.pseudoDimuonFilterSequence * process.patMuonSequence)
  elif doDimuonTrk:
      process.oniaTreeAna.replace(process.onia2MuMuPatGlbGlb, process.onia2MuMuPatGlbGlb * process.onia2MuMuPatGlbGlbFilterDimutrk)
      process.oniaTreeAna.replace(process.patMuonSequence, process.pseudoDimuonFilterSequence * process.patMuonSequence)
  else:
      process.oniaTreeAna.replace(process.onia2MuMuPatGlbGlb, process.onia2MuMuPatGlbGlb * process.onia2MuMuPatGlbGlbFilter)
      #BEWARE, pseudoDimuonFilterSequence asks for opposite-sign dimuon in given mass range. But saves a lot of time by filtering before running PAT muons
      process.oniaTreeAna.replace(process.patMuonSequence, process.pseudoDimuonFilterSequence * process.patMuonSequence)

process.oniaTreeAna = cms.Path(process.oniaTreeAna)
if miniAOD:
  from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import changeToMiniAOD
  changeToMiniAOD(process)
  process.unpackedMuons.addPropToMuonSt = cms.bool(UsePropToMuonSt)

#----------------------------------------------------------------------------
#Options:
process.source = cms.Source("PoolSource",
#process.source = cms.Source("NewEventStreamFileReader", # for streamer data
		fileNames = cms.untracked.vstring( options.inputFiles ),
		)
process.TFileService = cms.Service("TFileService",
		fileName = cms.string( options.outputFile )
		)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.options   = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.options.numberOfThreads = 4

process.schedule  = cms.Schedule( process.oniaTreeAna )
