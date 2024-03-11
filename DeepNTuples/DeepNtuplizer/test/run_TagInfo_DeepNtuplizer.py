import FWCore.ParameterSet.Config as cms


process = cms.Process("DNNFiller")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic_hi','')# ', '')
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2018_realistic_hi','')# ', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, "112X_mc2017_realistic_v6", '')  #pp
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC103X_2018PbPb_v4"),                               
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
         )
])
print('\n\033[31m~*~ USING CENTRALITY TABLE FOR PbPb 2018 DATA ~*~\033[0m\n')
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
             tag = cms.string("CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run2v1033p1x01_offline"),
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
             label = cms.untracked.string("HFtowers")
        ),
])

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(2007) )

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
#process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.options = cms.untracked.PSet(
   allowUnscheduled = cms.untracked.bool(True),  
   wantSummary=cms.untracked.bool(False)
)

process.source = cms.Source('PoolSource',
                            #fileNames=cms.untracked.vstring('/store/himc/RunIISummer20UL17pp5TeVMiniAODv2/QCD_pThat-15_Dijet_TuneCP5_5p02TeV-pythia8/MINIAODSIM/106X_mc2017_realistic_forppRef5TeV_v3-v3/50000/ED89F8C3-56E9-AF48-9D64-7D9CF448BE0C.root'),
                            #fileNames=cms.untracked.vstring('/store/himc/HINPbPbSpring21MiniAOD/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/MINIAODSIM/FixL1CaloGT_New_Release_112X_upgrade2018_realistic_HI_v9-v1/260000/6700a2b8-9c0d-4e1a-a774-2463e1e57785.root')
                            fileNames=cms.untracked.vstring('file:./HIN-HINPbPbSpring21MiniAOD-00187.root')
                            #eventsToProcess = cms.untracked.VEventRange("1:30282084-1:30282084"),
                    )
#process.source.skipEvents = cms.untracked.uint32(18)

process.load("DeepNTuples.DeepNtuplizer.extraJets_cff")
process.hiPuRho.reMiniAODBugFix = True
from DeepNTuples.DeepNtuplizer.clusterJetsFromMiniAOD_cff import setupHeavyIonJets
process.jetsR4 = cms.Sequence()
setupHeavyIonJets('akCs0PF', process.jetsR4, process, isMC = 1, radius = 0.40, JECTag = 'AK4PF')
process.akCs0PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
process.akCs0PFpatJets.useLegacyJetMCFlavour = True
process.akCs0PFpatJetPartons.particles = "hiSignalGenParticles"
process.akCs0PFpatJetPartonAssociationLegacy.partons = 'allPartons'
process.akCs0PFpatJetCorrFactors.useNPV = False
process.akCs0PFpatJetCorrFactors.useRho = False
process.akCs0PFpatJets.addJetCharge = False
process.akCs0PFJets.jetPtMin = 10.
process.akCs0PFpatJets.discriminatorSources = []
#process.load("DeepNTuples.DeepNtuplizer.candidateBtaggingMiniAOD_cff")

#process.load("PhysicsTools.PatAlgos.slimming.primaryVertexAssociation_cfi")
#process.primaryVertexAssociation.jets = "akCs0PFJets"
#process.primaryVertexAssociation.particles = "packedPFCandidates"

process.reclusterJets = cms.Path(
        process.extraJetsMC *
        process.jetsR4 #*
        #process.primaryVertexAssociation 
)

from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask
patAlgosToolsTask = getPatAlgosToolsTask(process)

bTagInfos = [
        'pfImpactParameterTagInfos',
        'pfInclusiveSecondaryVertexFinderTagInfos',
        'pfSecondaryVertexTagInfos',
        'pfDeepCSVTagInfos',
        #'pfInclusiveSecondaryVertexFinderNegativeTagInfos',
        #'pfDeepCSVNegativeTagInfos',
        #'pfNegativeDeepFlavourTagInfos',
        'pfParticleNetAK4TagInfos'
]


from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

updateJetCollection(
   process,
        #labelName = "DeepFlavour",
        labelName = "",
        #jetSource = cms.InputTag('slimmedJets'),
        jetSource = cms.InputTag('akCs0PFpatJets'),
        #pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'), #pp
        pvSource = cms.InputTag('offlineSlimmedPrimaryVerticesRecovery'),
        #svSource = cms.InputTag('slimmedSecondaryVertices'),
        svSource = cms.InputTag('inclusiveCandidateSecondaryVertices'),
        jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
        btagInfos = bTagInfos,
        btagDiscriminators = [
                'pfJetProbabilityBJetTags',
                'pfJetBProbabilityBJetTags',
                'pfCombinedInclusiveSecondaryVertexV2BJetTags',
                'pfCombinedSecondaryVertexV2BJetTags',
                #'pfDeepCSVJetTags:probudsg', 
                'pfDeepCSVJetTags:probb', 
                #'pfDeepCSVJetTags:probc', 
                'pfDeepCSVJetTags:probbb', 
                #'pfDeepCSVJetTags:probcc' 
                'pfDeepCSVDiscriminatorsJetTags:BvsAll',
                #'pfDeepFlavourJetTags:probg',
                #'pfDeepFlavourJetTags:probuds',
                'pfDeepFlavourJetTags:probb',
                #'pfDeepFlavourJetTags:probc',
                'pfDeepFlavourJetTags:probbb',
                'pfDeepFlavourJetTags:problepb'
                #, 'pfNegativeDeepFlavourJetTags:probb'
                #, 'pfNegativeDeepFlavourJetTags:probbb'
                #, 'pfNegativeDeepFlavourJetTags:problepb'
                #, 'pfNegativeDeepFlavourJetTags:probc'
                #, 'pfNegativeDeepFlavourJetTags:probuds'
                #, 'pfNegativeDeepFlavourJetTags:probg'
                #, 'pfParticleNetAK4JetTags:probpu'
                , 'pfParticleNetAK4JetTags:probb'
                #, 'pfParticleNetAK4JetTags:probg'
                #, 'pfParticleNetAK4JetTags:probcc'
                , 'pfParticleNetAK4JetTags:probbb' 
                #, 'pfParticleNetAK4JetTags:probc'
                #, 'pfParticleNetAK4JetTags:probuds'
                #, 'pfParticleNetAK4JetTags:probundef'
                , 'pfParticleNetAK4DiscriminatorsJetTags:BvsAll'
                #, 'pfParticleNetAK4DiscriminatorsJetTags:CvsL'
                #, 'pfParticleNetAK4DiscriminatorsJetTags:QvsG' 
                #, 'pfParticleNetAK4DiscriminatorsJetTags:CvsB'
        ],
)


bTagDiscriminators = [
        #'combinedSecondaryVertexV2BJetTags',
        #'softPFMuonBJetTags',
        #'softPFElectronBJetTags',
        'pfJetBProbabilityBJetTags',
        'pfJetProbabilityBJetTags',
        'pfCombinedInclusiveSecondaryVertexV2BJetTags',
        'pfCombinedSecondaryVertexV2BJetTags',
        #'pfDeepCSVJetTags:probudsg', #to be fixed with new names
        'pfDeepCSVJetTags:probb',
        #'pfDeepCSVJetTags:probc',
        'pfDeepCSVJetTags:probbb',
        #'pfDeepCSVJetTags:probcc',
        'pfDeepCSVDiscriminatorsJetTags:BvsAll',
        # DeepFlavour                                                                                                                      
        'pfDeepFlavourJetTags:probb'
        , 'pfDeepFlavourJetTags:probbb'
        , 'pfDeepFlavourJetTags:problepb'
        #, 'pfDeepFlavourJetTags:probc'
        #, 'pfDeepFlavourJetTags:probuds'
        #, 'pfDeepFlavourJetTags:probg'
        #, 'pfNegativeDeepFlavourJetTags:probb'
        #, 'pfNegativeDeepFlavourJetTags:probbb'
        #, 'pfNegativeDeepFlavourJetTags:problepb'
        #, 'pfNegativeDeepFlavourJetTags:probc'
        #, 'pfNegativeDeepFlavourJetTags:probuds'
        #, 'pfNegativeDeepFlavourJetTags:probg'
        #, 'pfParticleNetAK4JetTags:probpu'
        , 'pfParticleNetAK4JetTags:probb'
        #, 'pfParticleNetAK4JetTags:probg'
        #, 'pfParticleNetAK4JetTags:probcc'
        , 'pfParticleNetAK4JetTags:probbb' 
        #, 'pfParticleNetAK4JetTags:probc'
        #, 'pfParticleNetAK4JetTags:probuds'
        #, 'pfParticleNetAK4JetTags:probundef'
        , 'pfParticleNetAK4DiscriminatorsJetTags:BvsAll'
        #, 'pfParticleNetAK4DiscriminatorsJetTags:CvsL'
        #, 'pfParticleNetAK4DiscriminatorsJetTags:QvsG' 
        #, 'pfParticleNetAK4DiscriminatorsJetTags:CvsB'
]

if hasattr(process,'updatedPatJetsTransientCorrected'):
        process.updatedPatJetsTransientCorrected.addTagInfos = cms.bool(True)
        process.updatedPatJetsTransientCorrected.addBTagInfo = cms.bool(True)

process.pfDeepFlavourTagInfos.puppi_value_map = ""
process.pfDeepFlavourTagInfos.fallback_puppi_weight = True
process.pfDeepFlavourTagInfos.fallback_vertex_association = True
process.pfDeepFlavourTagInfos.vertex_associator = ''

'''
jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')

from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
updateJetCollection(
        process,
        labelName = "DeepFlavour",
        jetSource = cms.InputTag('slimmedJets'),#'ak4Jets'
        jetCorrections = jetCorrectionsAK4,
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),

        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        explicitJTA = False
)

if hasattr(process,'updatedPatJetsTransientCorrectedDeepFlavour'):
	process.updatedPatJetsTransientCorrectedDeepFlavour.addTagInfos = cms.bool(True) 
	process.updatedPatJetsTransientCorrectedDeepFlavour.addBTagInfo = cms.bool(True)
else:
	raise ValueError('I could not find updatedPatJetsTransientCorrectedDeepFlavour to embed the tagInfos, please check the cfg')
'''

# QGLikelihood
process.load("DeepNTuples.DeepNtuplizer.QGLikelihood_cfi")
process.es_prefer_jec = cms.ESPrefer("PoolDBESSource", "QGPoolDBESSource")
process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = cms.InputTag("selectedUpdatedPatJets")
process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')


from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
process.ak4GenJetsWithNu = ak4GenJets.clone(src = 'hiSignalGenParticles')
 
 ## Filter out neutrinos from packed GenParticles
#process.ak0GenJetsNoNu.src = 'packedGenParticles'  #pp
process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedGenParticles"), cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16"))
 ## Define GenJets
process.ak4GenJetsRecluster = ak4GenJets.clone(src = 'packedGenParticlesForJetsNoNu')
 
 
process.patGenJetMatchWithNu = cms.EDProducer("GenJetMatcher",  # cut on deltaR; pick best by deltaR           
    src         = cms.InputTag("selectedUpdatedPatJets"),      # RECO jets (any View<Jet> is ok) 
    matched     = cms.InputTag("ak4GenJetsWithNu"),        # GEN jets  (must be GenJetCollection)              
    #matched     = cms.InputTag("slimmedGenJets"),        # GEN jets  (must be GenJetCollection)              
    mcPdgId     = cms.vint32(),                      # n/a   
    mcStatus    = cms.vint32(),                      # n/a   
    checkCharge = cms.bool(False),                   # n/a   
    maxDeltaR   = cms.double(0.4),                   # Minimum deltaR for the match   
    #maxDPtRel   = cms.double(3.0),                  # Minimum deltaPt/Pt for the match (not used in GenJetMatcher)                     
    resolveAmbiguities    = cms.bool(True),          # Forbid two RECO objects to match to the same GEN object 
    resolveByMatchQuality = cms.bool(False),         # False = just match input in order; True = pick lowest deltaR pair first          
)

process.patGenJetMatchRecluster = cms.EDProducer("GenJetMatcher",  # cut on deltaR; pick best by deltaR           
    src         = cms.InputTag("selectedUpdatedPatJets"),      # RECO jets (any View<Jet> is ok) 
    matched     = cms.InputTag("ak4GenJetsRecluster"),        # GEN jets  (must be GenJetCollection)              
    #matched     = cms.InputTag("slimmedGenJets"),        # GEN jets  (must be GenJetCollection)              
    mcPdgId     = cms.vint32(),                      # n/a   
    mcStatus    = cms.vint32(),                      # n/a   
    checkCharge = cms.bool(False),                   # n/a   
    maxDeltaR   = cms.double(0.4),                   # Minimum deltaR for the match   
    #maxDPtRel   = cms.double(3.0),                  # Minimum deltaPt/Pt for the match (not used in GenJetMatcher)                     
    resolveAmbiguities    = cms.bool(True),          # Forbid two RECO objects to match to the same GEN object 
    resolveByMatchQuality = cms.bool(False),         # False = just match input in order; True = pick lowest deltaR pair first          
)

process.genJetSequence = cms.Sequence(process.packedGenParticlesForJetsNoNu*process.ak4GenJetsWithNu*process.ak4GenJetsRecluster*process.patGenJetMatchWithNu*process.patGenJetMatchRecluster)
#process.genJetSequence = cms.Sequence(process.patGenJetMatchWithNu*process.patGenJetMatchRecluster)
 

process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string("deepNtuple.root"))

# DeepNtuplizer
process.load("DeepNTuples.DeepNtuplizer.DeepNtuplizer_cfi")
process.deepntuplizer.centralityBinCollectionName    = cms.InputTag("centralityBin","HFtowers")
#process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
#process.centralityBin.Centrality = cms.InputTag("centralityBin")
#process.centralityBin.centralityVariable = cms.string("HFtowers")
    
process.updatedPatJetsTransientCorrected.addTagInfos = True

process.deepntuplizer.jets = cms.InputTag('selectedUpdatedPatJets');
process.deepntuplizer.bDiscriminators = bTagDiscriminators 
process.deepntuplizer.tagInfoName = cms.string('pfDeepCSV')

process.load("PhysicsTools.PatAlgos.producersHeavyIons.heavyIonUnsubtractedJets_cff")
process.load("PhysicsTools.PatAlgos.producersHeavyIons.heavyIonInclusiveVertices_cff")
process.pfInclusiveSecondaryVertexFinderTagInfos.extSVCollection = 'inclusiveCandidateSecondaryVertices'
process.deepntuplizer.secVertices  = 'inclusiveCandidateSecondaryVertices'

#process.deepntuplizer.vertices = "offlineSlimmedPrimaryVertices" #pp

process.inclusiveVertices = cms.Path(
        process.inclusiveCandidateVertexFinder *
        process.candidateVertexMerger *
        process.candidateVertexArbitrator *
        process.inclusiveCandidateSecondaryVertices #*
        #process.inclusiveCandidateNegativeVertexFinder *
        #process.candidateNegativeVertexMerger *
        #process.candidateNegativeVertexArbitrator *
        #process.inclusiveCandidateNegativeSecondaryVertices 
        #process.slimmedSecondaryVertices
)

process.unsubtractedJets = cms.Path(
        process.ak4PFJets*
        process.hiSignalGenParticles *
        process.selectedHadronsAndPartons * 
        process.ak4PFparton *
        process.ak4JetFlavourInfos *
        process.unsubPatJets
)

# this peels off the JEC.  Make sure it doesn't fuck with the UE subtraction
process.patJetCorrFactors.useRho= False
process.patJetCorrFactors.useNPV= False
process.patJetCorrFactors.payload = 'AK4PF'

process.patJetUpdate = cms.Path(
        process.patJetCorrFactors*
        process.updatedPatJets
)



process.pfParticleNetAK4TagInfos.jets = "updatedPatJets"
process.pfParticleNetAK4TagInfos.pf_candidates = "packedPFCandidates"
process.pfParticleNetAK4TagInfos.puppi_value_map = ''
process.pfParticleNetAK4TagInfos.vertex_associator = ""
process.pfParticleNetAK4TagInfos.vertices = "offlineSlimmedPrimaryVerticesRecovery"
#process.pfParticleNetAK4TagInfos.vertices = "offlineSlimmedPrimaryVertices" #pp

process.pfParticleNetAK4JetTags.src = "pfParticleNetAK4TagInfos"

#process.akCs0PFpfImpactParameterTagInfos.primaryVertex  = "offlineSlimmedPrimaryVertices" #pp

process.bTagging = cms.Path(
        process.pfImpactParameterTagInfos*
        process.pfInclusiveSecondaryVertexFinderTagInfos*
        process.pfDeepCSVTagInfos*
        process.pfSecondaryVertexTagInfos*
        process.pfCombinedInclusiveSecondaryVertexV2BJetTags*
        process.pfDeepCSVJetTags*
        process.pfDeepCSVDiscriminatorsJetTags*
        process.pfJetProbabilityBJetTags*
        process.pfJetBProbabilityBJetTags*
        process.pfCombinedSecondaryVertexV2BJetTags*
        process.pfDeepFlavourTagInfos*
        process.pfDeepFlavourJetTags*
        #process.pfInclusiveSecondaryVertexFinderNegativeTagInfosDeepFlavour*
        #process.pfDeepCSVNegativeTagInfosDeepFlavour*
        #process.pfNegativeDeepFlavourTagInfosDeepFlavour*
        #process.pfNegativeDeepFlavourJetTagsDeepFlavour*
        process.pfParticleNetAK4TagInfos*
        process.pfParticleNetAK4JetTags*
        process.pfParticleNetAK4DiscriminatorsJetTags
)


process.patJetCorrFactorsTransientCorrected.levels = [
        'L2Relative', 
        'L3Absolute'
]
process.patJetCorrFactorsTransientCorrected.payload = 'AK4PF'
process.patJetCorrFactorsTransientCorrected.useNPV = False
process.patJetCorrFactorsTransientCorrected.useRho = False


process.patJetTransient = cms.Path(
        process.patJetCorrFactorsTransientCorrected*
        process.updatedPatJetsTransientCorrected*
        process.selectedUpdatedPatJets
)
process.QGTagger.srcJets = "unsubPatJets"

process.p = cms.Path(
        process.QGTagger * 
        process.genJetSequence* 
        #process.candidateBtagging*
        #process.centralityBin*
        process.deepntuplizer
)

#test with unsubtracted jets
'''
process.akCs0PFpatJets.jetSource = 'ak4PFJets'
process.akCs0PFpatJetPartonMatch.src = 'ak4PFJets'
process.akCs0PFpatJetPartonAssociationLegacy.jets = 'ak4PFJets'
process.akCs0PFpfImpactParameterTagInfos.jets  = 'ak4PFJets'
process.akCs0PFpatJetGenJetMatch.src = 'ak4PFJets'
process.akCs0PFpatJetPartonMatch.src = 'ak4PFJets'
process.akCs0PFpatJetCorrFactors.src = 'ak4PFJets'
'''
#process.akCs0PFJets.writeJetsWithConst = False
