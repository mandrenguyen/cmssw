import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras

#----------------------------------------------------------------------------

# Setup Settings for ONIA TREE: 2016 pPb data new reco

HLTProcess     = "HLT" # Name of HLT process
isMC           = False # if input is MONTECARLO: True or if it's DATA: False
muonSelection  = "Trk" # Single muon selection: All, Glb(isGlobal), GlbTrk(isGlobal&&isTracker), Trk(isTracker), GlbOrTrk, TwoGlbAmongThree (which requires two isGlobal for a trimuon, and one isGlobal for a dimuon) are available
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
keepExtraColl  = True # General Tracks + Stand Alone Muons + Converted Photon collections
miniAOD        = True # whether the input file is in miniAOD format (default is AOD)
UsePropToMuonSt = True # whether to use L1 propagated muons (works only for miniAOD now)
pdgId = 443 # J/Psi : 443, Y(1S) : 553
useMomFormat = "array" # default "array" for TClonesArray of TLorentzVector. Use "vector" for std::vector<float> of pt, eta, phi, M
addEventPlane = True
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
#print( "[INFO] OneMatchedHLTMu      = " + (OneMatchedHLTMu if OneMatchedHLTMu > -1 else "False") )
print( "[INFO] miniAOD              = " + ("True" if miniAOD else "False") )
print( "[INFO] UsePropToMuonSt      = " + ("True" if UsePropToMuonSt else "False") )
print( "[INFO] addEventPlane        = " + ("True" if addEventPlane else "False") )
print( " " )

# set up process
process = cms.Process("HIOnia", eras.Run2_2016_UPC)

# setup 'analysis'  options
options = VarParsing.VarParsing ('analysis')

# Input and Output File Name
options.outputFile = "Oniatree_pPbReReco.root"
options.secondaryOutputFile = "Jpsi_DataSet.root"

options.inputFiles =[
  '/store/hidata/PARun2016C/PADoubleMuon/MINIAOD/16Dec2024-v1/110000/0a0f46a2-0825-49a6-9b15-bab20614e193.root',
  '/store/hidata/PARun2016C/PADoubleMuon/MINIAOD/16Dec2024-v1/2820000/ff6d2c1b-c721-447c-a8bd-ad0882f4cf93.root',
  '/store/hidata/PARun2016C/PADoubleMuon/MINIAOD/16Dec2024-v1/110000/0875f5f5-565d-46f0-a2b8-477ec258d508.root',
  '/store/hidata/PARun2016C/PADoubleMuon/MINIAOD/16Dec2024-v1/110000/0a0f46a2-0825-49a6-9b15-bab20614e193.root',
  '/store/hidata/PARun2016C/PADoubleMuon/MINIAOD/16Dec2024-v1/110000/0b2ee483-bd1d-451e-8cbc-de962779d2d5.root',
  '/store/hidata/PARun2016C/PADoubleMuon/MINIAOD/16Dec2024-v1/110000/0bb0a3e1-01d8-4435-a82e-4803ac6c9c21.root'
]
options.maxEvents = -1 # -1 means all events

# Get and parse the command line arguments
options.parseArguments()

triggerList    = {
		# Double Muon Trigger List
		'DoubleMuonTrigger' : cms.vstring(
			      "HLT_PAL1DoubleMuOpen_v",
                  "HLT_PAL1DoubleMuOpen_OS_v",
                  "HLT_PAL1DoubleMuOpen_SS_v",
                  "HLT_PAL1DoubleMu0_v",
                  "HLT_PAL1DoubleMu0_MGT1_v",
                  "HLT_PAL1DoubleMu0_HighQ_v",
                  "HLT_PAL2DoubleMu0_v",
                  "HLT_PAL3DoubleMu0_v",
                  "HLT_PAL3DoubleMu0_HIon_v",
                  "HLT_PAL1DoubleMu10_v",
                  "HLT_PAL2DoubleMu10_v",
                  "HLT_PAL3DoubleMu10_v"
            ),
        # Single Muon Trigger List
        'SingleMuonTrigger' : cms.vstring(
          "HLT_PAL2Mu12_v",
          "HLT_PAL2Mu15_v",
          "HLT_PAL3Mu3_v",
          "HLT_PAL3Mu5_v",
          "HLT_PAL3Mu7_v",
          "HLT_PAL3Mu12_v",
          "HLT_PAL3Mu15_v"
			)
  }

# Global tag, see https://github.com/cms-sw/cmssw/blob/master/Configuration/AlCa/python/autoCond.py
if isMC:
  globalTag = 'auto:phase1_2024_realistic_ppRef5TeV' #for Run3 MC : phase1_2023_realistic
else:
  globalTag = '141X_dataRun2_v2'

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

process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
process.GlobalTag.toGet = cms.VPSet(
  cms.PSet(
    record = cms.string("HeavyIonRcd"),
    tag = cms.string("CentralityTable_HFtowersPlusTrunc200_EPOS5TeV_v80x01_mc"),
    connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
    label = cms.untracked.string("HFtowersPlusTruncEpos")
    )
  )
#----------------------------------------------------------------------------

# For OniaTree Analyzer
from HiAnalysis.HiOnia.oniaTreeAnalyzer_cff import oniaTreeAnalyzer
oniaTreeAnalyzer(process,
                 muonTriggerList=triggerList, #HLTProName=HLTProcess,
                 muonSelection=muonSelection, L1Stage=2, isMC=isMC, pdgID=pdgId, outputFileName=options.outputFile, doTrimu=doTrimuons#, OnlySingleMuons=True
)

process.onia2MuMuPatGlbGlb.dimuonSelection       = cms.string("mass > 2.4 && mass < 4 && charge==0 && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25")
process.onia2MuMuPatGlbGlb.lowerPuritySelection  = cms.string("pt > 1 && abs(eta) < 2.4 && isTrackerMuon && track.quality('highPurity')")
#process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("") ## No need to repeat lowerPuritySelection in there, already included
#if applyCuts:

process.onia2MuMuPatGlbGlb.LateDimuonSel         = cms.string("userFloat(\"vProb\")>0.005")

process.onia2MuMuPatGlbGlb.onlySoftMuons         = cms.bool(OnlySoftMuons)
process.hionia.minimumFlag      = cms.bool(False)           #for Reco_trk_*
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
process.hionia.checkTrigNames   = cms.bool(False)#change this to get the event-level trigger info in hStats output (but creates lots of warnings when fake trigger names are used)
process.hionia.mom4format       = cms.string(useMomFormat)
process.hionia.isHI = cms.untracked.bool(False)
process.hionia.isPA = cms.untracked.bool(True)

process.hionia.useEvtPlane      = cms.untracked.bool(addEventPlane)

process.oniaTreeAna.replace(process.hionia, process.centralityBin * process.hionia )

if addEventPlane:
  from RecoHI.HiEvtPlaneAlgos.HiEvtPlane_cfi import hiEvtPlane
  process.hiEvtPlane = hiEvtPlane.clone(
    vertexTag = "offlineSlimmedPrimaryVertices",
    trackTag = "packedPFCandidates",
    dzdzerror_pix = 40.,
    caloCentRef = -1,
    caloCentRefWidth = -1,
    cutEra = 0
  )

  from RecoHI.HiEvtPlaneAlgos.hiEvtPlaneFlat_cfi import hiEvtPlaneFlat
  process.hiEvtPlaneFlat = hiEvtPlaneFlat.clone(
    vertexTag = cms.InputTag("unpackedTracksAndVertices"),
    inputPlanesTag = cms.InputTag("hiEvtPlane"),
    trackTag = cms.InputTag("unpackedTracksAndVertices")
  )

  process.oniaTreeAna.replace(process.hionia, process.hiEvtPlane * process.hiEvtPlaneFlat * process.hionia )


if applyEventSel:
    # Offline event filters
    process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
    process.load('HeavyIonsAnalysis.EventAnalysis.hffilter_cfi')

    # HLT trigger firing events
    import HLTrigger.HLTfilters.hltHighLevel_cfi
    process.hltHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
    process.hltHI.HLTPaths = ["HLT_PAL*Mu*_v*"]
    process.hltHI.throw = False
    process.hltHI.andOr = True

    # Muon filtering
    SuperLooseMuonCut = "isTrackerMuon && pt > 1. && abs(eta) < 2.4 && track.quality('highPurity')"

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
                                    cut = cms.string("mass > 2.4 && mass < 4"),
                                    decay = cms.string("muonSelector@+ muonSelector@-")
                                    )

    process.atLeastOneDimuon = cms.EDFilter("CandViewCountFilter",
                                        src = cms.InputTag("dimuonSelection"),
                                        minNumber = cms.uint32(1)
                                        )

    
    process.oniaTreeAna.replace(process.patMuonSequence,process.muonSelector * process.atLeastTwoMuons * process.dimuonSelection * process.atLeastOneDimuon * process.primaryVertexFilter * process.patMuonSequence )

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

  if applyEventSel:
    process.oniaTreeAna.replace(process.hionia, process.beamScrapingFilter * process.hionia ) # must be called after unpacking

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
