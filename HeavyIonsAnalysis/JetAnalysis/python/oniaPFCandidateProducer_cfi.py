import FWCore.ParameterSet.Config as cms


oniaPFCandidates = cms.EDProducer(
    "OniaPFCandidateProducer",
    pfCandTag=cms.InputTag("packedPFCandidates"),
    compositeTag=cms.InputTag("onia2MuMuPatGlbGlb"),
    jpsiTrigFilter=cms.string(""),
    isHI=cms.bool(False),
    replaceOniaMM=cms.bool(True),
    massOnia=cms.double(3.096900),
    massOnia2=cms.double(3.686097),
    replaceDKPi=cms.bool(False),
    checkOnia=cms.bool(False),
    oniaMuonID=cms.string("Soft"),
    applyMuonAcceptance=cms.bool(True),
    assignClosestOniaMass=cms.bool(True),
    cutMassOnia=cms.double(3.4),
)
