import FWCore.ParameterSet.Config as cms


from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
ak4PFJets.src = 'packedPFCandidates'

from RecoHI.HiJetAlgos.hiSignalParticleProducer_cfi import hiSignalParticleProducer as hiSignalGenParticles
hiSignalGenParticles.src = 'prunedGenParticles'

selectedHadronsAndPartons = cms.EDProducer('HadronAndPartonSelector',
     src = cms.InputTag("generator"),
     #particles = cms.InputTag("prunedGenParticles"),
     particles = cms.InputTag("hiSignalGenParticles"),
     partonMode = cms.string("Auto"),
     fullChainPhysPartons = cms.bool(True)
 )

ak4PFparton = cms.EDProducer("MCMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag("hiSignalGenParticles"),
    maxDPtRel = cms.double(3.0),
    maxDeltaR = cms.double(0.4),
    mcPdgId = cms.vint32(
        1, 2, 3, 4, 5, 
        21
    ),
    mcStatus = cms.vint32(3, 23),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(False),
    src = cms.InputTag("ak4PFJets")
)

from PhysicsTools.JetMCAlgos.AK4PFJetsMCFlavourInfos_cfi import ak4JetFlavourInfos

from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cfi import patJets as unsubPatJets
unsubPatJets.addAssociatedTracks = False
unsubPatJets.addBTagInfo = False
unsubPatJets.addDiscriminators = False
unsubPatJets.addGenJetMatch = False
unsubPatJets.addGenPartonMatch = True
unsubPatJets.addJetCharge = False
unsubPatJets.addJetCorrFactors = False
unsubPatJets.addJetFlavourInfo = True
unsubPatJets.embedGenJetMatch = False
unsubPatJets.embedGenPartonMatch = False
unsubPatJets.getJetMCFlavour = True
unsubPatJets.jetSource = 'ak4PFJets'
unsubPatJets.JetFlavourInfoSource = "ak4JetFlavourInfos"
unsubPatJets.genPartonMatch = "ak4PFparton"
