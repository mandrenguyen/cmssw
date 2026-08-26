import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras

#----------------------------------------------------------------------------

# Settings to analyze 2024 PbPb Prompt Reco data

HLTProcess     = "HLT" # Name of HLT process 
isMC           = False # if input is MONTECARLO: True or if it's DATA: False
muonSelection  = "TwoGlbAmongThree" # Single muon selection: Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker), TwoGlbAmongThree (which requires two isGlobal for a trimuon, and one isGlobal for a dimuon) are available
applyEventSel  = True # Only apply Event Selection if the required collections are present
OnlySoftMuons  = True # Keep only isSoftMuon's (with highPurity because this is pp config) from the beginning of HiSkim. In any case, if applyCuts=True, isSoftMuon is required at HiAnalysis level for muons of selected dimuons.
applyCuts      = True # At HiAnalysis level, apply kinematic acceptance cuts + identification cuts (isSoftMuon or isTightMuon, depending on TightGlobalMuon flag) for muons from selected di(tri)muons + hard-coded cuts on the di(tri)muon that you would want to add (but recommended to add everything in LateDimuonSelection, applied at the end of HiSkim)
SumETvariables = True  # Whether to write out SumET-related variables
SofterSgMuAcceptance = True # Whether to accept muons with a softer acceptance cuts than the usual (pt>3.5GeV at central eta, pt>1.8 at high |eta|). Applies when applyCuts=True
doTrimuons     = True # Make collections of trimuon candidates in addition to dimuons, and keep only events with >0 trimuons
flipJpsiDirection = 0 # (number of) Flip direction of Jpsi momentum and PV-SV, before combining it with a third muon
doDimuonTrk    = False # Make collections of Jpsi+track candidates in addition to dimuons
atLeastOneCand = False # Keep only events that have one selected dimuon (or at least one trimuon if doTrimuons = true). BEWARE this can cause trouble in .root output if no event is selected by onia2MuMuPatGlbGlbFilter!
OneMatchedHLTMu = -1   # Keep only di(tri)muons of which the one(two) muon(s) are matched to the HLT Filter of this number. You can get the desired number in the output of oniaTree. Set to -1 for no matching. WARNING: it is the trigger bit+1 !
muonLessPV     = False  # Recalculate the PV without the two muons from the selected dimuon
#############################################################################
keepExtraColl  = False # General Tracks + Stand Alone Muons + Converted Photon collections
useSVfinder    = False # External SV finder to check if the muons are from a resolved SV
miniAOD        = True # whether the input file is in miniAOD format (default is AOD)
UsePropToMuonSt = True # whether to use L1 propagated muons (works only for miniAOD now)
useMomFormat = "vector" # default "array" for TClonesArray of TLorentzVector. Use "vector" for std::vector<float> of pt, eta, phi, M
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
print( "[INFO] OneMatchedHLTMu      = " + ("True" if OneMatchedHLTMu > -1 else "False") )
print( " " )

# set up process
process = cms.Process("HIOnia", eras.Run3_pp_on_PbPb_2024)

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Names
options.outputFile = 'Oniatree_trimuons_2024PbPbPromptRecoData_141X_miniAOD.root'
options.secondaryOutputFile = "Jpsi_DataSet.root"
options.inputFiles =[
  '/store/hidata/HIRun2024A/HIPhysicsRawPrime0/MINIAOD/PromptReco-v1/000/387/939/00000/ccae3ab3-d57b-462b-b69b-f77fa16cbf72.root',
]
options.maxEvents = 100 # -1 means all events

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
                        "HLT_HIL2DoubleMuOpen_OS_v",#6
                        "HLT_HIL2DoubleMuOpen_SS_v",#7
                        ),
                # Single Muon Trigger List
                'SingleMuonTrigger' : cms.vstring(
                        "HLT_HIL1SingleMu0_Open_v",#8
                        "HLT_HIL1SingleMu0_v",#9
                        "HLT_HIL1SingleMu0_Centrality40to100_v",#10
                        "HLT_HIL1SingleMu0_Centrality30to100_v",#11
                        "HLT_HIL1SingleMuOpen_Centrality30to100_v",#12
                        "HLT_HIMinimumBiasHF1AND_v", #13
                        "HLT_HIMinimumBiasHF1ANDZDC1nOR_v", #14
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
                 muonSelection=muonSelection, L1Stage=2, isMC=isMC, outputFileName=options.outputFile, muonlessPV=muonLessPV, doTrimu=doTrimuons, doDimuTrk=doDimuonTrk, flipJpsiDir=flipJpsiDirection)

process.onia2MuMuPatGlbGlb.dimuonSelection       = cms.string("2.5 < mass && mass < 3.6 && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25")
process.onia2MuMuPatGlbGlb.trimuonSelection      = cms.string("2.9 < mass && mass < 7.8 && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25")
process.onia2MuMuPatGlbGlb.lowerPuritySelection  = cms.string("(isGlobalMuon || isTrackerMuon) && abs(innerTrack.dxy)<4 && abs(innerTrack.dz)<25 && abs(eta) < 2.4 && ((abs(eta) < 1. && pt >= 3.3) || (1. <= abs(eta) && abs(eta) < 2. && p >= 2.9) || (2. <= abs(eta) && pt >= 0.8))")#tracker muon acceptance
#process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("") ## No need to repeat lowerPuritySelection in there, already included
if applyCuts:
  process.onia2MuMuPatGlbGlb.LateDimuonSel         = cms.string("userFloat(\"vProb\")>0.002")
  process.onia2MuMuPatGlbGlb.LateTrimuonSel        = cms.string("userFloat(\"vProb\")>0.005 && userFloat(\"ppdlPV3D\")>0 && userFloat(\"ppdlPV\")>0 && userFloat(\"cosAlpha3D\")>0.4")
process.onia2MuMuPatGlbGlb.onlySoftMuons         = cms.bool(OnlySoftMuons)
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
process.hionia.useSVfinder      = cms.bool(useSVfinder)
process.hionia.checkTrigNames   = cms.bool(False)
process.hionia.mom4format       = cms.string(useMomFormat)

process.oniaTreeAna.replace(process.hionia, process.centralityBin * process.hionia )

if applyEventSel:
  # Offline event filters
  process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
  process.load('HeavyIonsAnalysis.EventAnalysis.hffilterPF_cfi')
  #process.oniaTreeAna.replace(process.hionia, process.phfCoincFilter2Th4 * process.primaryVertexFilter * process.clusterCompatibilityFilter * process.hionia )

  # HLT trigger firing events
  import HLTrigger.HLTfilters.hltHighLevel_cfi
  process.hltHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
  process.hltHI.HLTPaths = ["HLT_HIMinimumBiasHF1AND*_v*"]
  process.hltHI.throw = False
  process.hltHI.andOr = True
  
  process.oniaTreeAna.replace(process.patMuonSequence, process.phfCoincFilterPF2Th4 * process.primaryVertexFilter * process.hltHI * process.clusterCompatibilityFilter * process.patMuonSequence )

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
