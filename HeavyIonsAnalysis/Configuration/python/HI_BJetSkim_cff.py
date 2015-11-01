import FWCore.ParameterSet.Config as cms

# HLT jet trigger
import HLTrigger.HLTfilters.hltHighLevel_cfi
hltJetHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
hltJetHI.HLTPaths = ["HLT_PuAK4CalobJet80Eta2p1_v*"]
hltJetHI.throw = False
hltJetHI.andOr = True

# selection of valid vertex
primaryVertexFilterForBJets = cms.EDFilter("VertexSelector",
    src = cms.InputTag("hiSelectedVertex"),
    cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2"), 
    filter = cms.bool(True),   # otherwise it won't filter the events
    )

# jet energy correction (L2+L3)
from JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff import *
akPu4PFJetsL2L3 = cms.EDProducer('PFJetCorrectionProducer',
    src = cms.InputTag('akPu4PFJets'),
    correctors = cms.vstring('ak4PFL2L3')
    )

# leading jet E_T filter
jetEtFilter = cms.EDFilter("EtMinPFJetCountFilter",
    src = cms.InputTag("akPu4PFJetsL2L3"),
    etMin = cms.double(110.0),
    minNumber = cms.uint32(1)
    )

# Offline jet selection - not activated
leadingPFJet = cms.EDFilter( "LargestEtPFJetSelector",
    src = cms.InputTag( "akPu4PFJetsL2L3" ),
    filter = cms.bool( False ),
    maxNumber = cms.uint32( 1 )
    )

goodLeadingJet = cms.EDFilter("PFJetSelector",
    src = cms.InputTag("leadingPFJet"),
    cut = cms.string("et > 150")
    )

# dijet skim sequence
bJetSkimSequence = cms.Sequence(hltJetHI
                                 * primaryVertexFilterForBJets
                                 * jetEtFilter
                                 * akPu4PFJetsL2L3
                                 * jetEtFilter
                                 * leadingPFJet
                                 * goodLeadingJet
                                 )
