import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras

#----------------------------------------------------------------------------

# Setup Settings for ONIA TREE: PbPb 2018

HLTProcess     = "HLT" # Name of HLT process
isMC           = False # if input is MONTECARLO: True or if it's DATA: False
muonSelection  = "Glb" # Single muon selection: All, Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker), GlbOrTrk, TwoGlbAmongThree (which requires two isGlobal for a trimuon, and one isGlobal for a dimuon) are available
applyEventSel  = True # Only apply Event Selection if the required collections are present
OnlySoftMuons  = False # Keep only isSoftMuon's (without highPurity, and without isGlobal which should be put in 'muonSelection' parameter) from the beginning of HiSkim. If you want the full SoftMuon selection, set this flag false and add 'isSoftMuon' in lowerPuritySelection. In any case, if applyCuts=True, isSoftMuon is required at HiAnalysis level for muons of selected dimuons.
applyCuts      = False # At HiAnalysis level, apply kinematic acceptance cuts + identification cuts (isSoftMuon (without highPurity) or isTightMuon, depending on TightGlobalMuon flag) for muons from selected di(tri)muons + hard-coded cuts on the di(tri)muon that you would want to add (but recommended to add everything in LateDimuonSelection, applied at the end of HiSkim)
SumETvariables = True  # Whether to write out SumET-related variables
SofterSgMuAcceptance = True # Whether to accept muons with a softer acceptance cuts than the usual (pt>3.5GeV at central eta, pt>1.5 at high |eta|). Applies when applyCuts=True
doTrimuons     = False # Make collections of trimuon candidates in addition to dimuons, and keep only events with >0 trimuons (if atLeastOneCand)
doDimuonTrk    = False # Make collections of Jpsi+track candidates in addition to dimuons
atLeastOneCand = False # Keep only events that have one selected dimuon (or at least one trimuon if doTrimuons = true). BEWARE this can cause trouble in .root output if no event is selected by onia2MuMuPatGlbGlbFilter!
OneMatchedHLTMu = -1   # Keep only di(tri)muons of which the one(two) muon(s) are matched to the HLT Filter of this number. You can get the desired number in the output of oniaTree. Set to -1 for no matching.
#############################################################################
keepExtraColl  = False # General Tracks + Stand Alone Muons + Converted Photon collections
useSVfinder    = False # External SV finder to check if the muons are from a resolved SV
miniAOD        = True # whether the input file is in miniAOD format (default is AOD)
miniAOD_muonCuts = False # Apply the cuts used in the muon collections of miniAOD. Only has an effect with AOD.
useL1MuonProp = False # whether use of eta, phi information at L1 propagated from PV. Only use for offline-online matching studies
doEvtPlane    = True # if using EvtPlane
#----------------------------------------------------------------------------

# Print Onia Tree settings:
print( " " )
print( "[INFO] Settings used for ONIA TREE: " )
print( "[INFO] isMC                 = " + ("True" if isMC else "False") )
print( "[INFO] applyEventSel        = " + ("True" if applyEventSel else "False") )
print( "[INFO] keepExtraColl        = " + ("True" if keepExtraColl else "False") )
print( "[INFO] SumETvariables       = " + ("True" if SumETvariables else "False") )
print( "[INFO] SofterSgMuAcceptance = " + ("True" if SofterSgMuAcceptance else "False") )
print( "[INFO] muonSelection        = " + muonSelection )
print( "[INFO] onlySoftMuons        = " + ("True" if OnlySoftMuons else "False") )
print( "[INFO] doTrimuons           = " + ("True" if doTrimuons else "False") )
print( " " )

# set up process
process = cms.Process("HIOnia", eras.Run2_2018)

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Names
options.outputFile = "Oniatree_miniAOD.root"
options.secondaryOutputFile = "Jpsi_DataSet.root"
options.inputFiles =[
  #'file:/eos/cms/store/group/phys_heavyions/mnguyen/miniAOD/reMiniAOD_DATA_PAT_HIDoubleMuon.root',
  #'file:/afs/cern.ch/user/a/anstahll/work/MiniAOD/OniaTree/TRY2/CMSSW_10_3_3_patch1/src/reMiniAOD_DATA_PAT_JPsi.root',
  #'file:/home/llr/cms/falmagne/miniAOD/CMSSW_10_3_3_patch1/src/reMiniAOD_DATA_PAT_JPsi_HiDoubleMuPsiPeri.root'
  '/store/hidata/HIRun2018A/HIDoubleMuon/MINIAOD/PbPb18_MiniAODv1-v1/120000/00137f14-f84b-4f1b-ad37-0717a01c981b.root'
  #'/store/hidata/HIRun2018A/HIDoubleMuon/AOD/04Apr2019-v1/310001/FC3997EF-7B96-7947-81B0-96EEEA53AA60.root',
  #'/store/hidata/HIRun2018A/HIDoubleMuon/AOD/04Apr2019-v1/310001/FE775A7B-0F67-A14E-AAEE-1E00D4EE0BC9.root',
  #'/store/hidata/HIRun2018A/HIDoubleMuon/AOD/04Apr2019-v1/310001/FECEBB19-DB3E-AB40-B807-458134ABEEA3.root',
  #'/store/hidata/HIRun2018A/HIDoubleMuon/AOD/04Apr2019-v1/310001/FED19720-0CE4-5B4D-91E0-DB230A5046EB.root'
  #'/store/hidata/HIRun2018A/HIDoubleMuonPsiPeri/AOD/04Apr2019-v1/270007/2DA8EBF0-C0E7-6C42-AE96-8830278F8532.root'
  #'file:/afs/cern.ch/user/a/anstahll/work/MiniAOD/OniaTree/TRY2/CMSSW_10_3_3_patch1/src/reMiniAOD_DATA_PAT_JPsi.root',
  #'/store/hidata/HIRun2018A/HIDoubleMuon/AOD/04Apr2019-v1/310001/FED19720-0CE4-5B4D-91E0-DB230A5046EB.root'
  #'/store/hidata/HIRun2018A/HIDoubleMuon/AOD/PromptReco-v1/000/326/859/00000/9D9FEF75-B31A-9645-9090-0F99D895AED9.root'
]
options.maxEvents = 100 # -1 means all events

# Get and parse the command line arguments
options.parseArguments()


triggerList    = {
		# Double Muon Trigger List
		'DoubleMuonTrigger' : cms.vstring(
			"HLT_HIL1DoubleMuOpen_v1",#0
			"HLT_HIL1DoubleMuOpen_OS_Centrality_40_100_v1", #1
			"HLT_HIL1DoubleMuOpen_Centrality_50_100_v1", #2
			"HLT_HIL1DoubleMu10_v1", #3
			"HLT_HIL2_L1DoubleMu10_v1",#4
			"HLT_HIL3_L1DoubleMu10_v1", #5
			"HLT_HIL2DoubleMuOpen_v1", #6
			"HLT_HIL3DoubleMuOpen_v1", #7
			"HLT_HIL3DoubleMuOpen_M60120_v1", #8
			"HLT_HIL3DoubleMuOpen_JpsiPsi_v1", #9
			"HLT_HIL3DoubleMuOpen_Upsi_v1", #10
			"HLT_HIL3Mu0_L2Mu0_v1", #11
			"HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1",#12
			"HLT_HIL3Mu2p5NHitQ10_L2Mu2_M7toinf_v1",#13
                        "HLT_HIL3Mu3_L1TripleMuOpen_v1",#14
                        "HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1_L1step",#15
                        "HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1_L2step",#16
                        "HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1_L3step",#17
                        ),
		# Double Muon Filter List
		'DoubleMuonFilter'  : cms.vstring(
			"hltL1fL1sL1DoubleMuOpenL1Filtered0",
			"hltL1fL1sL1DoubleMuOpenOSCentrality40100L1Filtered0",
			"hltL1fL1sL1DoubleMuOpenCentrality50100L1Filtered0",
			"hltL1fL1sL1DoubleMu10L1Filtered0",
			"hltL2fL1sL1DoubleMu10L1f0L2Filtered0",
			"hltDoubleMuOpenL1DoubleMu10Filtered",
			"hltL2fL1sL1DoubleMuOpenL1f0L2Filtered0",
			"hltL3fL1DoubleMuOpenL3Filtered0",
			"hltL3fL1DoubleMuOpenL3FilteredM60120",
			"hltL3fL1DoubleMuOpenL3FilteredPsi",
			"hltL3fL1DoubleMuOpenL3FilteredUpsi",
			"hltL3f0L3Mu0L2Mu0Filtered0",
                        "hltL3f0L3Mu0L2Mu0DR3p5FilteredNHitQ10M1to5",
			"hltL3f0L3Mu2p5NHitQ10L2Mu2FilteredM7toinf",
                        "hltL3fL1sL1DoubleMuOpenL1fN3L2f0L3Filtered3",
                        "hltL1fL1sL1DoubleMuOpenMAXdR3p5L1Filtered0",#L1 step for Jpsi trigger
                        "hltL2fDoubleMuOpenL2DR3p5PreFiltered0",#L2 step
                        "hltL3f0L3Mu0L2Mu0DR3p5FilteredNHitQ10M1to5"#"hltL3f0DR3p5L3FilteredNHitQ10"#L3 step
			),
                # Single Muon Trigger List
                'SingleMuonTrigger' : cms.vstring(
                        "HLT_HIL1MuOpen_Centrality_70_100_v1",
                        "HLT_HIL1MuOpen_Centrality_80_100_v1",
                        "HLT_HIL2Mu3_NHitQ15_v1",
                        "HLT_HIL2Mu5_NHitQ15_v1",
                        "HLT_HIL2Mu7_NHitQ15_v1",
                        "HLT_HIL3Mu3_NHitQ10_v1",
                        "HLT_HIL3Mu5_NHitQ10_v1",
                        "HLT_HIL3Mu7_NHitQ10_v1",
                        "HLT_HIL3Mu12_v1",
                        "HLT_HIL3Mu15_v1",
                        "HLT_HIL3Mu20_v1",
                        "HLT_HIL2Mu3_NHitQ15_v2",
                        "HLT_HIL2Mu5_NHitQ15_v2",
                        "HLT_HIL2Mu7_NHitQ15_v2",
                        "HLT_HIL3Mu3_NHitQ10_v2",
                        "HLT_HIL3Mu5_NHitQ10_v2",
                        "HLT_HIL3Mu7_NHitQ10_v2",
                        "HLT_HIL3Mu12_v2",
                        "HLT_HIL3Mu15_v2",
                        "HLT_HIL3Mu20_v2",
			),
	        # Single Muon Filter List
	        'SingleMuonFilter'  : cms.vstring(
                        "hltL1fL1sL1MuOpenCentrality70100L1Filtered0",
                        "hltL1fL1sL1MuOpenCentrality80100L1Filtered0",
                        "hltL2fL1sMuOpenL1f0L2Filtered3NHitQ15",
                        "hltL2fL1sMuOpenL1f0L2Filtered5NHitQ15",
                        "hltL2fL1sMuOpenL1f0L2Filtered7NHitQ15",
                        "hltL3fL1sL1SingleMuOpenL1f0L2f0L3Filtered3NHitQ10",
                        "hltL3fL1sL1SingleMuOpenL1f0L2f0L3Filtered5NHitQ10",
                        "hltL3fL1sL1SingleMuOpenL1f0L2f0L3Filtered7NHitQ10",
                        "hltL3fL1sL1SingleMuOpenL1f7L2f0L3Filtered12",
                        "hltL3fL1sL1SingleMuOpenL1f7L2f0L3Filtered15",
                        "hltL3fL1sL1SingleMuOpenL1f7L2f0L3Filtered20",
                        "hltL2fL1sMu3OpenL1f0L2Filtered3NHitQ15",
                        "hltL2fL1sMu3OpenL1f0L2Filtered5NHitQ15",
                        "hltL2fL1sMu3OpenL1f0L2Filtered7NHitQ15",
                        "hltL3fL1sL1SingleMu3OpenL1f0L2f0L3Filtered3NHitQ10",
                        "hltL3fL1sL1SingleMu3OpenL1f0L2f0L3Filtered5NHitQ10",
                        "hltL3fL1sL1SingleMu3OpenL1f0L2f0L3Filtered7NHitQ10",
                        "hltL3fL1sL1SingleMu3OpenL1f7L2f0L3Filtered12",
                        "hltL3fL1sL1SingleMu3OpenL1f7L2f0L3Filtered15",
                        "hltL3fL1sL1SingleMu3OpenL1f7L2f0L3Filtered20",
			)
                }

## Global tag
if isMC:
  globalTag = '112X_upgrade2018_realistic_HI_v9' 
else:
  globalTag = '112X_dataRun2_PromptLike_HI_v7'

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
print('\n\033[31m~*~ USING CENTRALITY TABLE FOR PbPb 2018 ~*~\033[0m\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_HydjetDrum5F_v1032x02_mc" if isMC else "CentralityTable_HFtowers200_DataPbPb_update2018HIrun_v0_offline"),
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        label = cms.untracked.string("HFtowers")
        ),
    ])

#----------------------------------------------------------------------------

# For OniaTree Analyzer
from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process,
                 muonTriggerList=triggerList, #HLTProName=HLTProcess,
                 muonSelection=muonSelection, L1Stage=2, isMC=isMC, outputFileName=options.outputFile, doTrimu=doTrimuons,
                 miniAOD=miniAOD, miniAODcuts=miniAOD_muonCuts#, OnlySingleMuons=False
)

#process.onia2MuMuPatGlbGlb.dimuonSelection       = cms.string("8 < mass && mass < 14 && charge==0 && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25")
#process.onia2MuMuPatGlbGlb.lowerPuritySelection  = cms.string("pt > 5 || isPFMuon || (pt>1.2 && (isGlobalMuon || isStandAloneMuon)) || (isTrackerMuon && track.quality('highPurity'))")
#process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("") ## No need to repeat lowerPuritySelection in there, already included
if applyCuts:
  process.onia2MuMuPatGlbGlb.LateDimuonSel         = cms.string("userFloat(\"vProb\")>0.01")
process.onia2MuMuPatGlbGlb.onlySoftMuons         = cms.bool(OnlySoftMuons)
process.hionia.minimumFlag      = cms.bool(keepExtraColl)           #for Reco_trk_*
process.hionia.useGeTracks      = cms.untracked.bool(keepExtraColl) #for Reco_trk_*
process.hionia.fillRecoTracks   = cms.bool(keepExtraColl)           #for Reco_trk_*
process.hionia.CentralitySrc    = cms.InputTag("hiCentrality")
process.hionia.CentralityBinSrc = cms.InputTag("centralityBin","HFtowers")
#process.hionia.muonLessPV       = cms.bool(False)
process.hionia.SofterSgMuAcceptance = cms.bool(SofterSgMuAcceptance)
process.hionia.SumETvariables   = cms.bool(SumETvariables)
process.hionia.applyCuts        = cms.bool(applyCuts)
process.hionia.AtLeastOneCand   = cms.bool(atLeastOneCand)
process.hionia.OneMatchedHLTMu  = cms.int32(OneMatchedHLTMu)
process.hionia.useSVfinder      = cms.bool(useSVfinder)
process.hionia.useL1MuonProp    = cms.bool(useL1MuonProp)
process.hionia.checkTrigNames   = cms.bool(False)#change this to get the event-level trigger info in hStats output (but creates lots of warnings when fake trigger names are used)

process.hionia.useEvtPlane      = cms.untracked.bool(doEvtPlane)

process.oniaTreeAna.replace(process.hionia, process.centralityBin * process.hionia )

if applyEventSel:
  process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
  process.load('HeavyIonsAnalysis.EventAnalysis.hffilter_cfi')
  process.oniaTreeAna.replace(process.hionia, process.clusterCompatibilityFilter * process.primaryVertexFilter * process.phfCoincFilter2Th4 * process.hionia )

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

process.schedule  = cms.Schedule( process.oniaTreeAna )

