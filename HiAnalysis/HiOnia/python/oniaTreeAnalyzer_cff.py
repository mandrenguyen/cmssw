import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.helpers import *

def oniaTreeAnalyzer(process, muonTriggerList=[[],[],[],[]], HLTProName='HLT', muonSelection="Trk", useL1Stage2=True, isMC=True, pdgID=443, outputFileName="OniaTree.root"):

    #process.load("FWCore.MessageService.MessageLogger_cfi")
    #process.MessageLogger.categories.extend(["GetManyWithoutRegistration","GetByLabelWithoutRegistration"])
    #process.MessageLogger.destinations = ['cout', 'cerr']
    #process.MessageLogger.cerr.FwkReport.reportEvery = 1000
    #process.MessageLogger.categories.extend(["HiOnia2MuMuPAT_muonLessSizeORpvTrkSize"])
    #process.MessageLogger.cerr.HiOnia2MuMuPAT_muonLessSizeORpvTrkSize = cms.untracked.PSet( limit = cms.untracked.int32(5) )
    
    process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
    # load the Modules for the PATMuonsWithTrigger
    process.load('RecoMuon.Configuration.RecoMuon_cff')
    process.load('RecoTracker.Configuration.RecoTracker_cff')
    # load the Modules for the TransientTrackBuilder
    process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

###################### Onia Skim Producer #################################################
    '''
    import HLTrigger.HLTfilters.hltHighLevel_cfi
    process.hltOniaHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
    '''
    from HiSkim.HiOnia2MuMu.onia2MuMuPAT_cff import onia2MuMuPAT
    onia2MuMuPAT(process, GlobalTag=process.GlobalTag.globaltag, MC=isMC, HLT=HLTProName, Filter=False, useL1Stage2=useL1Stage2)

### Temporal fix for the PAT Trigger prescale warnings.
    if (HLTProName == 'HLT') :
        process.patTriggerFull.l1GtReadoutRecordInputTag = cms.InputTag("gtDigis","","RECO")
        process.patTriggerFull.l1tAlgBlkInputTag = cms.InputTag("gtStage2Digis","","RECO")
        process.patTriggerFull.l1tExtBlkInputTag = cms.InputTag("gtStage2Digis","","RECO")
    else :
        process.patTriggerFull.l1GtReadoutRecordInputTag = cms.InputTag("hltGtDigis","",HLTProName)
        process.patTriggerFull.l1tAlgBlkInputTag = cms.InputTag("hltGtStage2Digis","",HLTProName)
        process.patTriggerFull.l1tExtBlkInputTag = cms.InputTag("hltGtStage2Digis","",HLTProName)
###

##### Onia2MuMuPAT input collections/options
    process.onia2MuMuPatGlbGlb.dimuonSelection          = cms.string("mass > 0")
    process.onia2MuMuPatGlbGlb.resolvePileUpAmbiguity   = True
    process.onia2MuMuPatGlbGlb.srcTracks                = cms.InputTag("generalTracks")
    process.onia2MuMuPatGlbGlb.primaryVertexTag         = cms.InputTag("offlinePrimaryVertices")
    process.patMuonsWithoutTrigger.pvSrc                = cms.InputTag("offlinePrimaryVertices")
# Adding muonLessPV gives you lifetime values wrt. muonLessPV only
    process.onia2MuMuPatGlbGlb.addMuonlessPrimaryVertex = False

    if isMC:
        process.genMuons.src = "genParticles"
        process.onia2MuMuPatGlbGlb.genParticles = "genParticles"
        
    #process.patMuonSequence.remove(process.hltOniaHI)

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

###################### HiOnia Analyzer #################################################

    process.hionia = cms.EDAnalyzer('HiOniaAnalyzer',
                                    #-- Collections
                                    srcMuon             = cms.InputTag("patMuonsWithTrigger"),     # Name of PAT Muon Collection
                                    srcMuonNoTrig       = cms.InputTag("patMuonsWithoutTrigger"),  # Name of PAT Muon Without Trigger Collection
                                    src                 = cms.InputTag("onia2MuMuPatGlbGlb"),      # Name of Onia Skim Collection
                                    EvtPlane            = cms.InputTag("hiEvtPlane",""),           # Name of Event Plane Collection. For RECO use: hiEventPlane,recoLevel
                                    
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
                                    isHI = cms.untracked.bool(False),
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
                                    
                                    dblTriggerPathNames = cms.vstring(muonTriggerList['DoubleMuonTrigger']),
                                    
                                    dblTriggerFilterNames = cms.vstring(muonTriggerList['DoubleMuonFilter']),
                                    
                                    sglTriggerPathNames = cms.vstring(muonTriggerList['SingleMuonTrigger']),

                                    sglTriggerFilterNames = cms.vstring(muonTriggerList['SingleMuonFilter']),
                                    )

    process.hionia.primaryVertexTag = cms.InputTag("offlinePrimaryVertices")
    process.hionia.genParticles     = cms.InputTag("genParticles")
    process.hionia.muonLessPV       = cms.bool(False)
    process.hionia.CentralitySrc    = cms.InputTag("")
    process.hionia.CentralityBinSrc = cms.InputTag("")
    process.hionia.srcTracks        = cms.InputTag("generalTracks")       
    
    #process.oniaTreeAna = cms.EndPath(process.patMuonSequence * process.onia2MuMuPatGlbGlb * process.hionia )

