import FWCore.ParameterSet.Config as cms

from RecoHI.HiJetAlgos.HiGenJets_cff import *
from RecoJets.Configuration.GenJetParticles_cff import *
from RecoHI.HiJetAlgos.HiSignalParticleProducer_cfi import hiSignalGenParticles
from HeavyIonsAnalysis.JetAnalysis.akSoftDrop4GenJets_cfi import akSoftDrop4GenJets

genParticlesForJetsSignal = genParticlesForJets.clone(src = cms.InputTag("hiSignalGenParticles"))
akSoftDrop4GenJets.src            = cms.InputTag("genParticlesForJetsSignal")

akSoftDrop1GenJets = akSoftDrop4GenJets.clone(rParam = cms.double(0.1) , R0 = cms.double(0.1))
akSoftDrop2GenJets = akSoftDrop4GenJets.clone(rParam = cms.double(0.2) , R0 = cms.double(0.2))
akSoftDrop3GenJets = akSoftDrop4GenJets.clone(rParam = cms.double(0.3) , R0 = cms.double(0.3))
akSoftDrop5GenJets = akSoftDrop4GenJets.clone(rParam = cms.double(0.5) , R0 = cms.double(0.5))
akSoftDrop6GenJets = akSoftDrop4GenJets.clone(rParam = cms.double(0.6) , R0 = cms.double(0.6))

akHiGenJets = cms.Sequence(
    genParticlesForJets +
    hiSignalGenParticles +
    genParticlesForJetsSignal +
    ak1HiGenJets +
    ak2HiGenJets +
    ak3HiGenJets +
    ak4HiGenJets +
    ak5HiGenJets +
    ak6HiGenJets +
    akSoftDrop4GenJets +
    akSoftDrop5GenJets
)
