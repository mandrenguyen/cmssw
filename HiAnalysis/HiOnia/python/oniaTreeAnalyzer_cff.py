import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.helpers import *

def oniaTreeAnalyzer(process, isPbPb=False, isMC=False, applyEventSel=True, muonSelection="Trk", pdgID=443, outputFileName="OniaJetTree.root"):

    HLTProName = "HLT"
    process.MessageLogger.categories.extend(["HiOnia2MuMuPAT_muonLessSizeORpvTrkSize"])
    process.MessageLogger.cerr.HiOnia2MuMuPAT_muonLessSizeORpvTrkSize = cms.untracked.PSet( limit = cms.untracked.int32(5) )

    process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
    # load the Modules for the PATMuonsWithTrigger
    process.load('RecoMuon.Configuration.RecoMuon_cff')
    process.load('RecoTracker.Configuration.RecoTracker_cff')
    # load the Modules for the TransientTrackBuilder
    process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

###################### Onia Skim Producer #################################################

    import HLTrigger.HLTfilters.hltHighLevel_cfi
    process.hltOniaHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()

    if isPbPb:
        # HLT PbPbP MENU:  /cdaq/physics/Run2015HI/HeavyIon/500Bunches/v8.0/HLT/V7
        process.hltOniaHI.HLTPaths =  [
            "HLT_HIL1DoubleMu0_v1",
            "HLT_HIL1DoubleMu0_part1_v1",
            "HLT_HIL1DoubleMu0_part2_v1",
            "HLT_HIL1DoubleMu0_part3_v1",
            "HLT_HIL1DoubleMu0_2HF_v1",
            "HLT_HIL1DoubleMu0_2HF0_v1",
            "HLT_HIL1DoubleMu10_v1",
            "HLT_HIL2DoubleMu0_NHitQ_v2",
            "HLT_HIL2DoubleMu0_NHitQ_2HF_v1",
            "HLT_HIL2DoubleMu0_NHitQ_2HF0_v1",
            "HLT_HIL1DoubleMu0_2HF_Cent30100_v1",
            "HLT_HIL1DoubleMu0_2HF0_Cent30100_v1",
            "HLT_HIL2DoubleMu0_2HF_Cent30100_NHitQ_v1",
            "HLT_HIL1DoubleMu0_Cent30_v1",
            "HLT_HIL2DoubleMu0_2HF0_Cent30100_NHitQ_v1",
            "HLT_HIL2DoubleMu0_Cent30_NHitQ_v1",
            "HLT_HIL2DoubleMu0_Cent30_OS_NHitQ_v1",
            "HLT_HIL3DoubleMu0_Cent30_v1",
            "HLT_HIL3DoubleMu0_Cent30_OS_m2p5to4p5_v1",
            "HLT_HIL3DoubleMu0_Cent30_OS_m7to14_v1",
            "HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
            "HLT_HIL3DoubleMu0_OS_m7to14_v1",
            "HLT_HIL2Mu3_NHitQ10_2HF_v1",
            "HLT_HIL2Mu3_NHitQ10_2HF0_v1",
            "HLT_HIL3Mu3_NHitQ15_2HF_v1",
            "HLT_HIL3Mu3_NHitQ15_2HF0_v1",
            "HLT_HIL2Mu5_NHitQ10_2HF_v1",
            "HLT_HIL2Mu5_NHitQ10_2HF0_v1",
            "HLT_HIL3Mu5_NHitQ15_2HF_v1",
            "HLT_HIL3Mu5_NHitQ15_2HF0_v1",
            "HLT_HIL2Mu7_NHitQ10_2HF_v1",
            "HLT_HIL2Mu7_NHitQ10_2HF0_v1",
            "HLT_HIL3Mu7_NHitQ15_2HF_v1",
            "HLT_HIL3Mu7_NHitQ15_2HF0_v1",
            "HLT_HIL2Mu15_v2",
            "HLT_HIL2Mu15_2HF_v1",
            "HLT_HIL2Mu15_2HF0_v1",
            "HLT_HIL3Mu15_v1",
            "HLT_HIL3Mu15_2HF_v1",
            "HLT_HIL3Mu15_2HF0_v1",
            "HLT_HIL2Mu20_v1",
            "HLT_HIL2Mu20_2HF_v1",	
            "HLT_HIL2Mu20_2HF0_v1",
            "HLT_HIL3Mu20_v1",
            "HLT_HIL3Mu20_2HF_v1",
            "HLT_HIL3Mu20_2HF0_v1"
            ]
    else:
        # HLT PP MENU: /users/HiMuonTrigDev/pp5TeV/NovDev/V4
        process.hltOniaHI.HLTPaths = [
            "HLT_HIL1DoubleMu0_v1",
            "HLT_HIL1DoubleMu10_v1",
            "HLT_HIL2DoubleMu0_NHitQ_v1",
            "HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
            "HLT_HIL3DoubleMu0_OS_m7to14_v1",
            "HLT_HIL2Mu3_NHitQ10_v1",
            "HLT_HIL3Mu3_NHitQ15_v1",
            "HLT_HIL2Mu5_NHitQ10_v1",
            "HLT_HIL3Mu5_NHitQ15_v1",
            "HLT_HIL2Mu7_NHitQ10_v1",
            "HLT_HIL3Mu7_NHitQ15_v1",
            "HLT_HIL2Mu15_v1",
            "HLT_HIL3Mu15_v1",
            "HLT_HIL2Mu20_v1",
            "HLT_HIL3Mu20_v1"
            ]
    process.hltOniaHI.throw = False
    process.hltOniaHI.andOr = True
    process.hltOniaHI.TriggerResultsTag = cms.InputTag("TriggerResults","",HLTProName)

    from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import onia2MuMuPAT
    onia2MuMuPAT(process, GlobalTag=process.GlobalTag.globaltag, MC=isMC, HLT=HLTProName, Filter=False)

### Temporal fix for the PAT Trigger prescale warnings.
    process.patTriggerFull.l1GtReadoutRecordInputTag = cms.InputTag("gtDigis","","RECO")
###

##### Onia2MuMuPAT input collections/options
    process.onia2MuMuPatGlbGlb.dimuonSelection          = cms.string("mass > 0")
    if isPbPb:
        process.onia2MuMuPatGlbGlb.srcTracks                = cms.InputTag("hiGeneralTracks")
        process.onia2MuMuPatGlbGlb.primaryVertexTag         = cms.InputTag("hiSelectedVertex")
        process.patMuonsWithoutTrigger.pvSrc                = cms.InputTag("hiSelectedVertex")
        process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = False
        process.onia2MuMuPatGlbGlb.resolvePileUpAmbiguity   = False
    else: # ispp
        process.onia2MuMuPatGlbGlb.srcTracks                = cms.InputTag("generalTracks")
        process.onia2MuMuPatGlbGlb.primaryVertexTag         = cms.InputTag("offlinePrimaryVertices")
        process.patMuonsWithoutTrigger.pvSrc                = cms.InputTag("offlinePrimaryVertices")
    # Adding muonLessPV gives you lifetime values wrt. muonLessPV only
        process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = True
        process.onia2MuMuPatGlbGlb.resolvePileUpAmbiguity   = True
    if isMC:
        process.genMuons.src = "genParticles"
        process.onia2MuMuPatGlbGlb.genParticles = "genParticles"

##### Dimuon pair selection
    commonP1 = "|| (innerTrack.isNonnull && genParticleRef(0).isNonnull)"
    commonP2 = " && abs(innerTrack.dxy)<4 && abs(innerTrack.dz)<35"
    if muonSelection == "Glb":
        highP = "isGlobalMuon"; # At least one muon must pass this selection
        process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("("+highP+commonP1+")"+commonP2)
        lowP = "isGlobalMuon"; # BOTH muons must pass this selection
        process.onia2MuMuPatGlbGlb.lowerPuritySelection = cms.string("("+lowP+commonP1+")"+commonP2)
    elif muonSelection == "GlbTrk":
        highP = "(isGlobalMuon && isTrackerMuon)";
        process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("("+highP+commonP1+")"+commonP2)
        lowP = "(isGlobalMuon && isTrackerMuon)";
        process.onia2MuMuPatGlbGlb.lowerPuritySelection = cms.string("("+lowP+commonP1+")"+commonP2)
    elif muonSelection == "Trk":
        highP = "isTrackerMuon";
        process.onia2MuMuPatGlbGlb.higherPuritySelection = cms.string("("+highP+commonP1+")"+commonP2)
        lowP = "isTrackerMuon";
        process.onia2MuMuPatGlbGlb.lowerPuritySelection = cms.string("("+lowP+commonP1+")"+commonP2)
    else:
        print "ERROR: Incorrect muon selection " + muonSelection + " . Valid options are: Glb, Trk, GlbTrk"

##### Event Selection
    if applyEventSel:
        if isPbPb:
            process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
            process.load('HeavyIonsAnalysis.EventAnalysis.HIClusterCompatibilityFilter_cfi')
            process.clusterCompatibilityFilter.clusterPars = cms.vdouble(0.0,0.006)
            process.patMuonSequence.replace(process.hltOniaHI , process.hltOniaHI * process.hfCoincFilter3 * process.primaryVertexFilter * process.clusterCompatibilityFilter )
        else:
            process.PAprimaryVertexFilter = cms.EDFilter("VertexSelector",
                                                         src = cms.InputTag("offlinePrimaryVertices"),
                                                         cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2 && tracksSize >= 2"),
                                                         filter = cms.bool(True),
                                                         )
            process.NoScraping = cms.EDFilter("FilterOutScraping",
                                              applyfilter = cms.untracked.bool(True),
                                              debugOn = cms.untracked.bool(False),
                                              numtrack = cms.untracked.uint32(10),
                                              thresh = cms.untracked.double(0.25),
                                              )
            process.patMuonSequence.replace(process.hltOniaHI , process.hltOniaHI * process.PAprimaryVertexFilter * process.NoScraping )

##### Remove few paths for MC
    ##if isMC:
    ##    process.patMuonSequence.remove(process.hltOniaHI)

###################### HiOnia Analyzer #################################################

    process.hionia = cms.EDAnalyzer('HiOniaAnalyzer',
                                    #-- Collections
                                    srcMuon             = cms.InputTag("patMuonsWithTrigger"),     # Name of PAT Muon Collection
                                    srcMuonNoTrig       = cms.InputTag("patMuonsWithoutTrigger"),  # Name of PAT Muon Without Trigger Collection
                                    src                 = cms.InputTag("onia2MuMuPatGlbGlb"),      # Name of Onia Skim Collection
                                    EvtPlane            = cms.InputTag("hiEvtPlane",""),           # Name of Event Plane Collection. For RECO use: hiEventPlane,recoLevel
                                    genParticles        = cms.InputTag("genParticles"),
                                    triggerResultsLabel = cms.InputTag("TriggerResults","",HLTProName), # Label of Trigger Results

                                    #-- Reco Details
                                    useBeamSpot = cms.bool(False),
                                    useRapidity = cms.bool(True),

                                    #--
                                    maxAbsZ = cms.double(24.0),

                                    pTBinRanges      = cms.vdouble(0.0, 6.0, 8.0, 9.0, 10.0, 12.0, 15.0, 40.0),
                                    etaBinRanges     = cms.vdouble(0.0, 2.5),
                                    centralityRanges = cms.vdouble(20,40,100),

                                    onlyTheBest        = cms.bool(False),
                                    applyCuts          = cms.bool(False),
                                    selTightGlobalMuon = cms.bool(False),
                                    storeEfficiency    = cms.bool(False),

                                    removeSignalEvents = cms.untracked.bool(False),  # Remove/Keep signal events
                                    removeTrueMuons    = cms.untracked.bool(False),  # Remove/Keep gen Muons
                                    storeSameSign      = cms.untracked.bool(True),   # Store/Drop same sign dimuons

                                    #-- Gen Details
                                    oniaPDG = cms.int32(pdgID),
                                    muonSel = cms.string(muonSelection),
                                    isHI = cms.untracked.bool(isPbPb),
                                    isPA = cms.untracked.bool(False),
                                    isMC = cms.untracked.bool(isMC),
                                    isPromptMC = cms.untracked.bool(True),
                                    useEvtPlane = cms.untracked.bool(False),
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
                                    histFileName      = cms.string(outputFileName),
                                    dataSetName       = cms.string("Jpsi_DataSet.root"),

                                    dblTriggerPathNames = cms.vstring("HLT_PAL1DoubleMuOpen_v1",
                                                                      "HLT_PAL1DoubleMu0_HighQ_v1",
                                                                      "HLT_PAL2DoubleMu3_v1"),
                                    
                                    dblTriggerFilterNames = cms.vstring("hltL1fL1sPAL1DoubleMuOpenL1Filtered0",
                                                                        "hltL1fL1sPAL1DoubleMu0HighQL1FilteredHighQ",
                                                                        "hltL2fL1sPAL2DoubleMu3L2Filtered3"),
                                    
                                    sglTriggerPathNames = cms.vstring("HLT_PAMu3_v1",
                                                                      "HLT_PAMu7_v1",
                                                                      "HLT_PAMu12_v1"),
                                    
                                    sglTriggerFilterNames = cms.vstring("hltL3fL2sMu3L3Filtered3",
                                                                        "hltL3fL2sMu7L3Filtered7",
                                                                        "hltL3fL2sMu12L3Filtered12")

                                    )

    process.hionia.l1GtReadoutRecordInputTag = cms.InputTag("gtDigis","","RECO")

    if isPbPb:           
        process.hionia.primaryVertexTag = cms.InputTag("hiSelectedVertex")
        process.hionia.EvtPlane         = cms.InputTag("hiEvtPlane","")
        process.hionia.CentralitySrc    = cms.InputTag("hiCentrality") 
        process.hionia.CentralityBinSrc = cms.InputTag("centralityBin","HFtowers")
        process.hionia.srcTracks        = cms.InputTag("hiGeneralTracks")
        process.hionia.muonLessPV       = cms.bool(False)      

        # HLT PbPbP MENU:  /online/collisions/2015/HeavyIons/v1.0/HLT/V6
                                
        process.hionia.dblTriggerPathNames   = cms.vstring("HLT_HIL1DoubleMu0_v1",
                                                           "HLT_HIL1DoubleMu0_2HF_v1",
                                                           "HLT_HIL1DoubleMu0_2HF0_v1",
                                                           "HLT_HIL1DoubleMu10_v1",
                                                           "HLT_HIL2DoubleMu0_NHitQ_v2",
                                                           "HLT_HIL2DoubleMu0_NHitQ_2HF_v1",
                                                           "HLT_HIL2DoubleMu0_NHitQ_2HF0_v1",
                                                           "HLT_HIL1DoubleMu0_2HF_Cent30100_v1",
                                                           "HLT_HIL1DoubleMu0_2HF0_Cent30100_v1",
                                                           "HLT_HIL2DoubleMu0_2HF_Cent30100_NHitQ_v1",
                                                           "HLT_HIL1DoubleMu0_Cent30_v1",
                                                           "HLT_HIL2DoubleMu0_2HF0_Cent30100_NHitQ_v1",
                                                           "HLT_HIL2DoubleMu0_Cent30_NHitQ_v1",
                                                           "HLT_HIL2DoubleMu0_Cent30_OS_NHitQ_v1",
                                                           "HLT_HIL3DoubleMu0_Cent30_v1",
                                                           "HLT_HIL3DoubleMu0_Cent30_OS_m2p5to4p5_v1",
                                                           "HLT_HIL3DoubleMu0_Cent30_OS_m7to14_v1",
                                                           "HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
                                                           "HLT_HIL3DoubleMu0_OS_m7to14_v1")

        process.hionia.sglTriggerPathNames   = cms.vstring("HLT_HIL2Mu3_NHitQ10_2HF_v1",
                                                           "HLT_HIL2Mu3_NHitQ10_2HF0_v1",
                                                           "HLT_HIL3Mu3_NHitQ15_2HF_v1",
                                                           "HLT_HIL3Mu3_NHitQ15_2HF0_v1",
                                                           "HLT_HIL2Mu5_NHitQ10_2HF_v1",
                                                           "HLT_HIL2Mu5_NHitQ10_2HF0_v1",
                                                           "HLT_HIL3Mu5_NHitQ15_2HF_v1",
                                                           "HLT_HIL3Mu5_NHitQ15_2HF0_v1",
                                                           "HLT_HIL2Mu7_NHitQ10_2HF_v1",
                                                           "HLT_HIL2Mu7_NHitQ10_2HF0_v1",
                                                           "HLT_HIL3Mu7_NHitQ15_2HF_v1",
                                                           "HLT_HIL3Mu7_NHitQ15_2HF0_v1",
                                                           "HLT_HIL2Mu15_v2",
                                                           "HLT_HIL2Mu15_2HF_v1",
                                                           "HLT_HIL2Mu15_2HF0_v1",
                                                           "HLT_HIL3Mu15_v1",
                                                           "HLT_HIL3Mu15_2HF_v1",
                                                           "HLT_HIL3Mu15_2HF0_v1",
                                                           "HLT_HIL2Mu20_v1",
                                                           "HLT_HIL2Mu20_2HF_v1",	
                                                           "HLT_HIL2Mu20_2HF0_v1",
                                                           "HLT_HIL3Mu20_v1",
                                                           "HLT_HIL3Mu20_2HF_v1",
                                                           "HLT_HIL3Mu20_2HF0_v1")

        process.hionia.dblTriggerFilterNames = cms.vstring("hltHIDoubleMu0L1Filtered",
                                                           "hltHIDoubleMu0MinBiasL1Filtered",
                                                           "hltHIDoubleMu0HFTower0Filtered",
                                                           "hltHIDoubleMu10L1Filtered",
                                                           "hltHIL2DoubleMu0NHitQFiltered",
                                                           "hltHIL2DoubleMu0NHitQ2HFFiltered",
                                                           "hltHIL2DoubleMu0NHitQ2HF0Filtered",
                                                           "hltHIDoubleMu0MinBiasCent30to100L1Filtered",
                                                           "hltHIDoubleMu0HFTower0Cent30to100L1Filtered",
                                                           "hltHIL2DoubleMu02HFcent30100NHitQFiltered",
                                                           "hltHIDoubleMu0MinBiasCent30L1Filtered",
                                                           "hltHIL2DoubleMu02HF0cent30100NHitQFiltered",
                                                           "hltHIL2DoubleMu0cent30NHitQFiltered",
                                                           "hltHIL2DoubleMu0cent30OSNHitQFiltered",
                                                           "hltHIDimuonOpenCentrality30L3Filter",
                                                           "hltHIDimuonOpenCentrality30OSm2p5to4p5L3Filter",
                                                           "hltHIDimuonOpenCentrality30OSm7to14L3Filter",
                                                           "hltHIDimuonOpenOSm2p5to4p5L3Filter",
                                                           "hltHIDimuonOpenOSm7to14L3Filter")
        
        process.hionia.sglTriggerFilterNames = cms.vstring("hltHIL2Mu3N10HitQ2HFL2Filtered",
                                                           "hltHIL2Mu3N10HitQ2HF0L2Filtered",
                                                           "hltHISingleMu3NHit152HFL3Filtered",
                                                           "hltHISingleMu3NHit152HF0L3Filtered",
                                                           "hltHIL2Mu5N10HitQ2HFL2Filtered",
                                                           "hltHIL2Mu5N10HitQ2HF0L2Filtered",
                                                           "hltHISingleMu5NHit152HFL3Filtered",
                                                           "hltHISingleMu5NHit152HF0L3Filtered",
                                                           "hltHIL2Mu7N10HitQ2HFL2Filtered",
                                                           "hltHIL2Mu7N10HitQ2HF0L2Filtered",
                                                           "hltHISingleMu7NHit152HFL3Filtered",
                                                           "hltHISingleMu7NHit152HF0L3Filtered",
                                                           "hltHIL2Mu15L2Filtered",
                                                           "hltHIL2Mu152HFFiltered",
                                                           "hltHIL2Mu15N10HitQ2HF0L2Filtered",
                                                           "hltHISingleMu15L3Filtered",
                                                           "hltHISingleMu152HFL3Filtered",
                                                           "hltHISingleMu152HF0L3Filtered",
                                                           "hltHIL2Mu20L2Filtered",
                                                           "hltHIL2Mu202HFL2Filtered",
                                                           "hltHIL2Mu202HF0L2Filtered",
                                                           "hltHIL3SingleMu20L3Filtered",
                                                           "hltHISingleMu202HFL3Filtered",
                                                           "hltHISingleMu202HF0L3Filtered")

    else: 
        process.hionia.primaryVertexTag = cms.InputTag("offlinePrimaryVertices")
        process.hionia.EvtPlane         = cms.InputTag("")
        process.hionia.CentralitySrc    = cms.InputTag("")
        process.hionia.CentralityBinSrc = cms.InputTag("")
        process.hionia.srcTracks        = cms.InputTag("generalTracks")       
        process.hionia.muonLessPV       = cms.bool(True)
  
        # HLT PP MENU: /users/HiMuonTrigDev/pp5TeV/NovDev/V4
  
        process.hionia.dblTriggerPathNames   = cms.vstring("HLT_HIL1DoubleMu0_v1",
                                                           "HLT_HIL1DoubleMu10_v1",
                                                           "HLT_HIL2DoubleMu0_NHitQ_v1",
                                                           "HLT_HIL3DoubleMu0_OS_m2p5to4p5_v1",
                                                           "HLT_HIL3DoubleMu0_OS_m7to14_v1")

        process.hionia.sglTriggerPathNames   = cms.vstring("HLT_HIL2Mu3_NHitQ10_v1",
                                                           "HLT_HIL3Mu3_NHitQ15_v1",
                                                           "HLT_HIL2Mu5_NHitQ10_v1",
                                                           "HLT_HIL3Mu5_NHitQ15_v1",
                                                           "HLT_HIL2Mu7_NHitQ10_v1",
                                                           "HLT_HIL3Mu7_NHitQ15_v1",
                                                           "HLT_HIL2Mu15_v1",
                                                           "HLT_HIL3Mu15_v1",
                                                           "HLT_HIL2Mu20_v1",
                                                           "HLT_HIL3Mu20_v1")
        
        process.hionia.dblTriggerFilterNames = cms.vstring("hltHIDoubleMu0L1Filtered",
                                                           "hltHIDoubleMu10MinBiasL1Filtered",
                                                           "hltHIL2DoubleMu0NHitQFiltered",
                                                           "hltHIDimuonOpenOSm2p5to4p5L3Filter",
                                                           "hltHIDimuonOpenOSm7to14L3Filter")
        
        process.hionia.sglTriggerFilterNames = cms.vstring("hltHIL2Mu3N10HitQL2Filtered",
                                                           "hltHISingleMu3NHit15L3Filtered",
                                                           "hltHIL2Mu5N10HitQL2Filtered",
                                                           "hltHISingleMu5NHit15L3Filtered",
                                                           "hltHISingleMu5NHit15L3Filtered",
                                                           "hltHISingleMu7NHit15L3Filtered",
                                                           "hltHIL2Mu15L2Filtered",
                                                           "hltHISingleMu15L3Filtered",
                                                           "hltHIL2Mu20L2Filtered",
                                                           "hltHIL3SingleMu20L3Filtered")

    process.oniaTreeAna = cms.Sequence(process.patMuonSequence * process.onia2MuMuPatGlbGlb * process.hionia )

