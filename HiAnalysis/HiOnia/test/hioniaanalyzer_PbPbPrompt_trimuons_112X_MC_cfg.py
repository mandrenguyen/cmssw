import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing


#----------------------------------------------------------------------------

# Setup Settings for ONIA TREE: PbPb 2018

HLTProcess     = "HLT" # Name of HLT process 
isMC           = True # if input is MONTECARLO or DATA
muonSelection  = "TwoGlbAmongThree" # Single muon selection: Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker), TwoGlbAmongThree (which requires two isGlobal for a trimuon, and one isGlobal for a dimuon) are available
applyEventSel  = True # Only apply Event Selection if the required collections are present
OnlySoftMuons  = True # Keep only isSoftMuon's (with highPurity because this is pp config) from the beginning of HiSkim. In any case, if applyCuts=True, isSoftMuon is required at HiAnalysis level for muons of selected dimuons.
applyCuts      = False # At HiAnalysis level, apply kinematic acceptance cuts + identification cuts (isSoftMuon or isTightMuon, depending on TightGlobalMuon flag) for muons from selected di(tri)muons + hard-coded cuts on the di(tri)muon that you would want to add (but recommended to add everything in LateDimuonSelection, applied at the end of HiSkim)
SumETvariables = False  # Whether to write out SumET-related variables
SofterSgMuAcceptance = True # Whether to accept muons with a softer acceptance cuts than the usual (pt>3.5GeV at central eta, pt>1.8 at high |eta|). Applies when applyCuts=True
doTrimuons     = True # Make collections of trimuon candidates in addition to dimuons, and keep only events with >0 trimuons
doDimuonTrk    = False # Make collections of Jpsi+track candidates in addition to dimuons
atLeastOneCand = False # Keep only events that have one selected dimuon (or at least one trimuon if doTrimuons = true). BEWARE this can cause trouble in .root output if no event is selected by onia2MuMuPatGlbGlbFilter!
OneMatchedHLTMu = -1 # Keep only di(tri)muons of which the one(two) muon(s) are matched to the HLT Filter of this number. You can get the desired number in the output of oniaTree. Set to -1 for no matching. WARNING: it is the trigger bit+1 !
muonLessPV     = False  # Recalculate the PV without the two muons from the selected dimuon
#############################################################################
keepExtraColl  = False # General Tracks + Stand Alone Muons + Converted Photon collections
useSVfinder    = False # External SV finder to check if the muons are from a resolved SV
saveHLT        = False # whether to save the HLT trees
miniAOD        = False # whether the input file is in miniAOD format (default is AOD) 
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
process = cms.Process("HIOnia")

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Names
options.outputFile = "Oniatree_MC.root"
options.secondaryOutputFile = "Jpsi_DataSet.root"
options.inputFiles =['/store/user/gfalmagn/Bc_analysis/MC/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/BcToJpsiMuNu/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/190722_175113/0000/Bc_reco_123.root'
#'/store/user/gfalmagn/Bc_analysis/MC/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/BcToJpsiMuNu/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/190722_175113/0000/Bc_reco_1.root'
#'/store/user/gfalmagn/Bc_analysis/MC/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/BcToJpsiMuNu/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/190722_175113/0000/Bc_reco_123.root'
#'/store/himc/HINPbPbAutumn18DR/BToJpsi_pThat-2_TuneCP5-EvtGen_HydjetDrumMB_5p02TeV_pythia8/AODSIM/mva98_103X_upgrade2018_realistic_HI_v11-v1/260000/E3D89BFC-9594-5545-9A27-194298ADB540.root',
#'/store/user/gfalmagn/Bc_analysis/MC/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/BcToJpsiMuNu/BcToJpsiMuNu_BCVEGPY_PYTHIA8_2018PbPb5TeV_18072019_2_reco/190722_175113/0000/Bc_reco_99.root'
]
options.maxEvents = 2000 # -1 means all events

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
  globalTag = '103X_upgrade2018_realistic_HI_v11'
else:
  globalTag = '103X_dataRun2_Prompt_v3'

#----------------------------------------------------------------------------

# load the Geometry and Magnetic Field
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

# Global Tag:
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, globalTag, '')
### For Centrality
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
print('\n\033[31m~*~ USING CENTRALITY TABLE FOR HYDJET DRUM5EV8 TUNE~*~\033[0m\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_HydjetDrum5F_v1032x02_mc"),
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        label = cms.untracked.string("HFtowers")
        ),
    ])

#----------------------------------------------------------------------------

# For OniaTree Analyzer
from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process, 
                 muonTriggerList=triggerList, #HLTProName=HLTProcess, 
                 muonSelection=muonSelection, L1Stage=2, isMC=isMC, outputFileName=options.outputFile, muonlessPV=muonLessPV, doTrimu=doTrimuons)

process.onia2MuMuPatGlbGlb.dimuonSelection       = cms.string("2.5 < mass && mass < 3.6 && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25")
process.onia2MuMuPatGlbGlb.trimuonSelection      = cms.string("2.9 < mass && mass < 7.8 && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25")
process.onia2MuMuPatGlbGlb.lowerPuritySelection  = cms.string("(isGlobalMuon || isTrackerMuon) && abs(innerTrack.dxy)<4 && abs(innerTrack.dz)<25 && abs(eta) < 2.4 && ((abs(eta) < 1. && pt >= 3.3) || (1. <= abs(eta) && abs(eta) < 2. && p >= 2.9) || (2. <= abs(eta) && pt >= 0.8))")#tracker muon acceptance
#process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("") ## No need to repeat lowerPuritySelection in there, already included
if applyCuts:
  process.onia2MuMuPatGlbGlb.LateDimuonSel         = cms.string("userFloat(\"vProb\")>0.002")
  process.onia2MuMuPatGlbGlb.LateTrimuonSel        = cms.string("userFloat(\"vProb\")>0.005 && userFloat(\"ppdlPV3D\")>0 && userFloat(\"cosAlpha3D\")>0.4")
process.onia2MuMuPatGlbGlb.onlySoftMuons         = cms.bool(OnlySoftMuons)
process.hionia.minimumFlag      = cms.bool(keepExtraColl)           #for Reco_trk_*
process.hionia.useGeTracks      = cms.untracked.bool(keepExtraColl) #for Reco_trk_*
process.hionia.fillRecoTracks   = cms.bool(keepExtraColl)           #for Reco_trk_*
process.hionia.CentralitySrc    = cms.InputTag("hiCentrality")
process.hionia.CentralityBinSrc = cms.InputTag("centralityBin","HFtowers")
process.hionia.srcTracks        = cms.InputTag("generalTracks")
process.hionia.primaryVertexTag = cms.InputTag("offlinePrimaryVertices")
process.hionia.genParticles     = cms.InputTag("genParticles")
process.hionia.SofterSgMuAcceptance = cms.bool(SofterSgMuAcceptance)
process.hionia.SumETvariables   = cms.bool(SumETvariables)
process.hionia.applyCuts        = cms.bool(applyCuts)
process.hionia.AtLeastOneCand   = cms.bool(atLeastOneCand)
process.hionia.OneMatchedHLTMu  = cms.int32(OneMatchedHLTMu)
process.hionia.useSVfinder      = cms.bool(useSVfinder)
process.hionia.checkTrigNames   = cms.bool(False)
process.hionia.genealogyInfo    = cms.bool(True)

process.oniaTreeAna.replace(process.hionia, process.centralityBin * process.hionia )

process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVerticesRecovery_cfi")
if applyEventSel:
  process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
  process.load('HeavyIonsAnalysis.EventAnalysis.clusterCompatibilityFilter_cfi')
  process.load('HeavyIonsAnalysis.Configuration.hfCoincFilter_cff')
  process.oniaTreeAna.replace(process.hionia, process.hfCoincFilter2Th4 * process.primaryVertexFilter * process.clusterCompatibilityFilter * process.hionia )

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

if saveHLT:
  # For HLTBitAnalyzer
  process.load("HLTrigger.HLTanalyzers.HLTBitAnalyser_cfi")
  process.hltbitanalysis.HLTProcessName              = HLTProcess
  process.hltbitanalysis.hltresults                  = cms.InputTag("TriggerResults","",HLTProcess)
  process.hltbitanalysis.l1tAlgBlkInputTag           = cms.InputTag("hltGtStage2Digis","",HLTProcess)
  process.hltbitanalysis.l1tExtBlkInputTag           = cms.InputTag("hltGtStage2Digis","",HLTProcess)
  process.hltbitanalysis.gObjectMapRecord            = cms.InputTag("hltGtStage2ObjectMap","",HLTProcess)
  process.hltbitanalysis.gmtStage2Digis              = cms.string("hltGtStage2Digis")
  process.hltbitanalysis.caloStage2Digis             = cms.string("hltGtStage2Digis")
  process.hltbitanalysis.UseL1Stage2                 = cms.untracked.bool(True)
  process.hltbitanalysis.getPrescales                = cms.untracked.bool(False)
  process.hltbitanalysis.getL1InfoFromEventSetup     = cms.untracked.bool(False)
  process.hltbitanalysis.UseTFileService             = cms.untracked.bool(True)
  process.hltbitanalysis.RunParameters.HistogramFile = cms.untracked.string(options.outputFile)
  process.hltbitanalysis.RunParameters.isData        = cms.untracked.bool(not isMC)
  process.hltbitanalysis.RunParameters.Monte         = cms.bool(isMC)
  process.hltbitanalysis.RunParameters.GenTracks     = cms.bool(False)
  if (HLTProcess == "HLT") :
    process.hltbitanalysis.l1tAlgBlkInputTag = cms.InputTag("gtStage2Digis","","RECO")
    process.hltbitanalysis.l1tExtBlkInputTag = cms.InputTag("gtStage2Digis","","RECO")
    process.hltbitanalysis.gmtStage2Digis    = cms.string("gtStage2Digis")
    process.hltbitanalysis.caloStage2Digis   = cms.string("gtStage2Digis")

  # For HLTObject Analyzer
  process.load("HeavyIonsAnalysis.EventAnalysis.hltobject_cfi")
  process.hltobject.processName = cms.string(HLTProcess)
  process.hltobject.treeName = cms.string(options.outputFile)
  process.hltobject.loadTriggersFromHLT = cms.untracked.bool(False)
  process.hltobject.triggerNames = triggerList['DoubleMuonTrigger'] + triggerList['SingleMuonTrigger']
  process.hltobject.triggerResults = cms.InputTag("TriggerResults","",HLTProcess)
  process.hltobject.triggerEvent   = cms.InputTag("hltTriggerSummaryAOD","",HLTProcess)

  #include in analysis chain
  process.oniaTreeAna = cms.Path( process.hltbitanalysis * process.hltobject * process.oniaTreeAna )

if useSVfinder:
    from RecoVertex.AdaptiveVertexFinder.inclusiveVertexFinder_cfi import *
    from RecoVertex.AdaptiveVertexFinder.vertexMerger_cfi import *
    from RecoVertex.AdaptiveVertexFinder.trackVertexArbitrator_cfi import *
    
    process.inclusiveVertexFinderLoose = inclusiveVertexFinder.clone(
        vertexMinDLen2DSig = 1.5,
        vertexMinDLenSig = 1.25,
        vertexMinAngleCosine = 0.001,
        maximumLongitudinalImpactParameter = 0.6, #default = 0.3
        maxNTracks = 10, #default = 30
        minPt = 1.2, #following muon acceptance
        #useVertexReco = False,
        #fitterSigmacut = 3.,
        clusterizer = cms.PSet(
            seedMax3DIPSignificance = cms.double(9999.),#default
            seedMax3DIPValue = cms.double(9999.),#default
            seedMin3DIPSignificance = cms.double(1.6), # default=1.2
            seedMin3DIPValue = cms.double(0.005), # default = 0.005
            clusterMaxDistance = cms.double(0.05),#default = 0.05
            clusterMaxSignificance = cms.double(3.),#default = 4.5
            distanceRatio = cms.double(10.),#default = 20
            clusterMinAngleCosine = cms.double(0.001), # default = 0.5
            maxTimeSignificance = cms.double(3.5),#default
        ),
    )
    
    process.vertexMergerLoose = vertexMerger.clone(
        secondaryVertices = "inclusiveVertexFinderLoose"
    )
    process.trackVertexArbitratorLoose = trackVertexArbitrator.clone(
        secondaryVertices = cms.InputTag("vertexMergerLoose")
    )
    process.inclusiveSecondaryVerticesLoose = vertexMerger.clone(
        secondaryVertices = "trackVertexArbitratorLoose",
        maxFraction = 0.2, # default 0.7 - 0.2
        minSignificance = 3. # default 2 - 10
    )
    process.inclusiveVertexingTask = cms.Task(
        process.inclusiveVertexFinderLoose,
        process.vertexMergerLoose,
        process.trackVertexArbitratorLoose,
        process.inclusiveSecondaryVerticesLoose
    )
    process.inclusiveVertexing = cms.Sequence(process.inclusiveVertexingTask)
    process.oniaTreeAna.replace(process.hionia, process.inclusiveVertexing*process.hionia)
    
    from RecoBTag.SecondaryVertex.secondaryVertexTagInfos_cfi import *
    inclusiveSecondaryVertexFinderLooseTagInfos = secondaryVertexTagInfos.clone()
    # use external SV collection made from IVF
    inclusiveSecondaryVertexFinderLooseTagInfos.extSVCollection     = cms.InputTag('inclusiveSecondaryVerticesLoose')
    inclusiveSecondaryVertexFinderLooseTagInfos.useExternalSV = cms.bool(True)

process.oniaTreeAna = cms.Path(process.offlinePrimaryVerticesRecovery * process.oniaTreeAna)
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

################ Offline Primary Vertices Recovery
from HLTrigger.Configuration.CustomConfigs import MassReplaceInputTag
process = MassReplaceInputTag(process,"offlinePrimaryVertices","offlinePrimaryVerticesRecovery")
process.offlinePrimaryVerticesRecovery.oldVertexLabel = "offlinePrimaryVertices"
