import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras

#----------------------------------------------------------------------------

# Setup Settings for ONIA TREE: 2024 PbPb data, ZMu skim (RAW-RECO format!)

HLTProcess     = "HLT" # Name of HLT process
isMC           = False # if input is MONTECARLO: True or if it's DATA: False
muonSelection  = "Glb" # Single muon selection: All, Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker), GlbOrTrk, TwoGlbAmongThree (which requires two isGlobal for a trimuon, and one isGlobal for a dimuon) are available
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
miniAOD        = False # whether the input file is in miniAOD format (default is AOD)
UsePropToMuonSt = False # whether to use L1 propagated muons (works only for miniAOD now)
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
#print( "[INFO] OneMatchedHLTMu      = " + ("True" if OneMatchedHLTMu > -1 else "False") )
print( "[INFO] miniAOD              = " + ("True" if miniAOD else "False") )
print( "[INFO] UsePropToMuonSt      = " + ("True" if UsePropToMuonSt else "False") )
print( " " )

# set up process
process = cms.Process("HIOnia", eras.Run3_pp_on_PbPb_2024)

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Name

options.inputFiles = [
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime0/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/886/00000/5821b1f9-8f24-4353-89bb-86e58c9aefa9.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime0/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/53059e13-c77d-4673-bd4d-4d9663a642fb.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime0/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/2dddaa28-629e-4a98-a036-01923f0ebbbc.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime1/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/886/00000/e96d08ca-558a-469a-add5-3c7779eed879.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime1/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/03e06ad5-c737-4a56-9135-bbb05f455b38.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime1/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/6a0904dc-02c0-4112-ab58-26c27168c6b2.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime1/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/908/00000/7df6ff7d-8479-4314-a5fa-633ce08b419c.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime10/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/426517ff-ecde-4c4b-9088-f088e8b75b6b.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime10/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/41657e9d-29e6-4d5c-8421-bcc7453de851.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime11/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/d5ef5442-4b71-4e54-ad1b-afb73fe01dfe.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime11/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/fe35b8a8-f0a4-4d23-a35e-e77241dd240e.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime11/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/908/00000/77ca55b0-9d87-4e9f-9f51-e6e6c1e3cacd.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime4/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/84148e4a-f808-473b-b50e-20fffc815c7c.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime4/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/5015b5b9-d54e-4347-b103-cb117ca40552.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime4/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/886/00000/301c42ae-b77e-424b-af6a-524864ed4293.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime8/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/aeca7f4a-0919-4cfd-b247-0d7ffc7d259a.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime8/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/0872709c-4d82-4414-a9a9-54b7578cc909.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime8/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/908/00000/c4c21da2-6f66-4ef3-ad97-cc5a90baef29.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime9/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/2bfca2dd-d413-4f79-8313-4081a9c85d7c.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime9/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/298d1755-3b8d-4aed-8238-22fadefe41a8.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime9/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/908/00000/c8cea453-460f-4f4d-a944-eae206a0a020.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime6/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/035a3257-9ef2-42a7-a958-f4d33859b702.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime6/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/858f6b54-0f94-45ae-97a1-173e364c82ff.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime6/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/886/00000/9ddb49b2-1615-44ed-83f3-7cedf18f9ef0.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime5/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/a2f2b17d-c78e-4a69-8429-54f39528cbcb.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime5/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/e77bbb26-e1b4-4628-9790-8cf4ec6eea12.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime3/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/4874baeb-88ea-4e4b-83db-6a15a18ed5f5.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime3/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/75152bd6-68d6-4750-8424-52fc854fe20e.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime3/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/908/00000/7a817179-1a4a-4adb-8223-992a75ef3ff5.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime7/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/886/00000/0c2feb04-ac16-488a-aa91-0189dabb800a.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime7/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/892/00000/fd6b9e1d-c970-47f3-a8cb-5cc31884fb7a.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime7/RAW-RECO/PbPbZMu-PromptReco-v1/000/387/907/00000/18081696-c1a9-49ef-8116-0d3d52f73254.root',
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime8/RAW-RECO/PbPbZMu-PromptReco-v1/000/388/095/00000/4ce5fad1-1c11-465b-ad3e-240191588c92.root'
]

options.outputFile = 'Oniatree_ZMuSkim_PbPb2024PromptReco_141X.root'
options.secondaryOutputFile = "Jpsi_Dataset.root"

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
                        "HLT_HIL2DoubleMu0_SQ_v",#4
                        "HLT_HIL2DoubleMuOpen_Centrality40to100_v",#5
                        ),
                # Single Muon Trigger List
                'SingleMuonTrigger' : cms.vstring(
                        "HLT_HIL1SingleMu0_Open_v",#6
                        "HLT_HIL1SingleMu0_v",#7
                        "HLT_HIL1SingleMu0_Centrality40to100_v",#8
                        "HLT_HIL1SingleMu0_Centrality30to100_v",#9
                        "HLT_HIL1SingleMuOpen_Centrality30to100_v",#10
                        "HLT_HIL2SingleMu3_Open_v",#11
                        "HLT_HIL2SingleMu5_v",#12
                        "HLT_HIL2SingleMu7_v",#13
                        "HLT_HIL2SingleMu12_v",#14
                        "HLT_HIMinimumBiasHF1AND_v", #15
                        "HLT_HIMinimumBiasHF1ANDZDC1nOR_v", #16
			)
                }

# Global tag, see https://github.com/cms-sw/cmssw/blob/master/Configuration/AlCa/python/autoCond.py
if isMC:
  globalTag = 'auto:phase1_2024_realistic_hi' #for Run3 MC : phase1_2023_realistic_hi
else:
  globalTag = '141X_dataRun3_Prompt_v3'

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
print('\n\033[31m~*~ USING NOMINAL CENTRALITY TABLE FOR 2024 PbPb DATA ~*~\033[0m\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run3v140x01_offline_Nominal"),
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

process.onia2MuMuPatGlbGlb.dimuonSelection       = cms.string("mass > 60")
process.onia2MuMuPatGlbGlb.lowerPuritySelection  = cms.string("pt > 15. && isPFMuon && isGlobalMuon && abs(eta) < 2.4 && (globalTrack().normalizedChi2() < 10) && (globalTrack().hitPattern().numberOfValidMuonHits()>0) && (numberOfMatchedStations() > 1) && (innerTrack().hitPattern().numberOfValidPixelHits() > 0)")

process.onia2MuMuPatGlbGlb.LateDimuonSel = cms.string("userFloat(\"vProb\")>0.001")

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

process.hionia.storeSameSgin = cms.bool(True)

process.oniaTreeAna.replace(process.hionia, process.centralityBin * process.hionia )

if applyEventSel:
  # Offline event filters
  process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
  process.load('HeavyIonsAnalysis.EventAnalysis.hffilterPF_cfi')
  #process.oniaTreeAna.replace(process.hionia, process.phfCoincFilter2Th4 * process.primaryVertexFilter * process.clusterCompatibilityFilter * process.hionia )

  # Muon filtering
  SuperLooseMuonCut = "isGlobalMuon && pt > 15. && abs(eta) < 2.4 && isPFMuon && (globalTrack().hitPattern().numberOfValidMuonHits()>0)"

  MUONCUT = SuperLooseMuonCut
  
  process.muonSelector = cms.EDFilter("MuonSelector",
                                        src = cms.InputTag("muons"),
                                        cut = cms.string(MUONCUT),
                                        filter = cms.bool(True)
  )

  process.atLeastTwoMuons = cms.EDFilter("MuonCountFilter",
                                 src = cms.InputTag("muons"),
                                 minNumber = cms.uint32(2)
                                 )

  # PV and trigger filters already applied to the PbPbZMu skim
  process.oniaTreeAna.replace(process.patMuonSequence, process.muonSelector * process.atLeastTwoMuons * process.phfCoincFilterPF2Th4 * process.patMuonSequence )

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

#process.options.numberOfThreads = 2

process.schedule  = cms.Schedule( process.oniaTreeAna )
