import FWCore.ParameterSet.Config as cms

hltobject = cms.EDAnalyzer(
    "TriggerObjectAnalyzer",
    processName = cms.string("HLT"),
    treeName = cms.string("JetTriggers"),
    triggerNames = cms.vstring(
        "HLT_HISinglePhoton10_Eta1p5ForPPRef_v",
        "HLT_HISinglePhoton15_Eta1p5ForPPRef_v",
        "HLT_HISinglePhoton20_Eta1p5ForPPRef_v",
        "HLT_HISinglePhoton30_Eta1p5ForPPRef_v",
        "HLT_HISinglePhoton40_Eta1p5ForPPRef_v",
        "HLT_HISinglePhoton50_Eta1p5ForPPRef_v",
        "HLT_HISinglePhoton60_Eta1p5ForPPRef_v",
        "HLT_HISinglePhoton10_Eta3p1ForPPRef_v",
        "HLT_HISinglePhoton15_Eta3p1ForPPRef_v",
        "HLT_HISinglePhoton20_Eta3p1ForPPRef_v",
        "HLT_HISinglePhoton30_Eta3p1ForPPRef_v",
        "HLT_HISinglePhoton40_Eta3p1ForPPRef_v",
        "HLT_HISinglePhoton50_Eta3p1ForPPRef_v",
        "HLT_HISinglePhoton60_Eta3p1ForPPRef_v",
        "HLT_HIL1DoubleMuOpen_v",
        "HLT_HIL1DoubleMuOpen_OS_v",
        "HLT_HIL1DoubleMuOpen_SS_v",
        "HLT_HIL1DoubleMu0_v",
        "HLT_HIL1DoubleMu0_HighQ_v",
        "HLT_HIL1DoubleMu10_v",
        "HLT_HIL2DoubleMu0_v",
        "HLT_HIL2DoubleMu10_v",
        "HLT_HIL3DoubleMu0_v",
        "HLT_HIL3DoubleMu10_v",
        "HLT_HIL1Mu12_v",
        "HLT_HIL1Mu16_v",
        "HLT_HIL2Mu7_v",
        "HLT_HIL2Mu12_v",
        "HLT_HIL2Mu15_v",
        "HLT_HIL2Mu20_v",
        "HLT_HIL3Mu7_v",
        "HLT_HIL3Mu12_v",
        "HLT_HIL3Mu15_v",
        "HLT_HIL3Mu20_v",
        "HLT_HIL3Mu3_v",
        "HLT_HIL2Mu5_NHitQ10_v",
        "HLT_HIL3Mu3_NHitQ10_v",
        "HLT_HIL3Mu3_Track1_v",
        "HLT_HIL3Mu3_Track1_Jpsi_v",
        "HLT_HIL3Mu5_v",
        "HLT_HIL2Mu3_NHitQ10_v",
        "HLT_HIL3Mu5_NHitQ10_v",
        "HLT_HIL3Mu5_Track1_v",
        "HLT_HIL3Mu5_Track1_Jpsi_v",
        "HLT_HIDiPFJet25_FBEta3_NoCaloMatched_v",
        "HLT_HIDiPFJet15_FBEta3_NoCaloMatched_v",
        "HLT_HIDiPFJet25_NoCaloMatched_v",
        "HLT_HIDiPFJet15_NoCaloMatched_v",
        "HLT_HIDiPFJetAve35_HFJEC_v",
        "HLT_HIDiPFJetAve25_HFJEC_v",
        "HLT_HIDiPFJetAve15_HFJEC_v",
        "HLT_HIDmesonPPTrackingGlobal_Dpt8_v",
        "HLT_HIDmesonPPTrackingGlobal_Dpt15_v",
        "HLT_HIDmesonPPTrackingGlobal_Dpt30_v",
        "HLT_HIDmesonPPTrackingGlobal_Dpt40_v",
        "HLT_HIDmesonPPTrackingGlobal_Dpt50_v",
        "HLT_HIDmesonPPTrackingGlobal_Dpt60_v",
        "HLT_HIDijet16And8_MidEta2p7_Dpt8_v",
        "HLT_HIDijet20And8_MidEta2p7_Dpt8_v",
        "HLT_HIDijet16And12_MidEta2p7_Dpt10_v",
        "HLT_HIDijet20And12_MidEta2p7_Dpt10_v",
        "HLT_HIDijet28And16_MidEta2p7_Dpt15_v",
        "HLT_HIFullTracks_HighPt18_v",
        "HLT_HIFullTracks_HighPt24_v",
        "HLT_HIFullTracks_HighPt34_v",
        "HLT_HIFullTracks_HighPt45_v",
        "HLT_HIHT80_Beamspot_ppRef5TeV_v",
        "HLT_HIAK4PFJet15_v",
        "HLT_HIAK4PFJet30_v",
        "HLT_HIAK4PFJet40_v",
        "HLT_HIAK4PFJet60_v",
        "HLT_HIAK4PFJet80_v",
        "HLT_HIAK4PFJet100_v",
        "HLT_HIAK4PFJet120_v",
        "HLT_HIAK4PFJet30FWD_v",
        "HLT_HIAK4PFJet40FWD_v",
        "HLT_HIAK4PFJet60FWD_v",
        "HLT_HIAK4PFJet80FWD_v",
        "HLT_HIAK4CaloJet15_v",
        "HLT_HIAK4CaloJet30_v",
        "HLT_HIAK4CaloJet40_v",
        "HLT_HIAK4CaloJet60_v",
        "HLT_HIAK4CaloJet80_v",
        "HLT_HIAK4CaloJet100_v",
        "HLT_HIAK4CaloJet120_v",
        "HLT_HIAK4CaloJet30FWD_v",
        "HLT_HIAK4CaloJet40FWD_v",
        "HLT_HIAK4CaloJet60FWD_v",
        "HLT_HIAK4CaloJet80FWD_v",
        "HLT_HIPhoton20_HoverELoose_v",
        "HLT_HIPhoton30_HoverELoose_v",
        "HLT_HIPhoton40_HoverELoose_v",
        "HLT_HIPhoton50_HoverELoose_v",
        "HLT_HIPhoton60_HoverELoose_v",
        "HLT_HIEle10_WPLoose_Gsf_v",
        "HLT_HIEle15_WPLoose_Gsf_v",
        "HLT_HIEle20_WPLoose_Gsf_v",
        "HLT_HIEle30_WPLoose_Gsf_v",
        "HLT_HIEle40_WPLoose_Gsf_v",
        "HLT_HIEle20_eta2p1_WPTight_Gsf_CentralPFJet15_EleCleaned_v",
        "HLT_HIEle20_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v",
        "HLT_HIDoublePhoton15_Eta3p1ForPPRef_Mass50to1000_v",
        "HLT_HIAK4PFJet30_bTag_v",
        "HLT_HIAK4PFJet40_bTag_v",
        "HLT_HIAK4PFJet60_bTag_v",
        "HLT_HIAK4PFJet80_bTag_v",
        "HLT_HIEle17_WPLoose_Gsf_v",
        "HLT_HIEle15_Ele8_CaloIdL_TrackIdL_IsoVL_v",
        "HLT_HIEle15_CaloIdL_TrackIdL_IsoVL_PFJet30_v",
        "HLT_HIMu17_v",
        "HLT_HIMu15_v",
        "HLT_HIMu12_v",
        "HLT_HIMu7p5_L2Mu2_Jpsi_v",
        "HLT_HIMu7p5_L2Mu2_Upsilon_v",
        "HLT_HIMu7p5_Track2_Jpsi_v",
        "HLT_HIMu7p5_Track2_Upsilon_v",
        "HLT_HIDimuon0_Jpsi_v",
        "HLT_HIDimuon0_Jpsi_NoVertexing_v",
        "HLT_HIDimuon0_Upsilon_NoVertexing_v",
        "HLT_HIPFJet25_v",
        "HLT_HIPFJet140_v",
        "HLT_HIPFJetFwd15_v",
        "HLT_HIPFJetFwd25_v",
        "HLT_HIPFJetFwd140_v",
        "HLT_HIAK8PFJetFwd15_v",
        "HLT_HIAK8PFJetFwd25_v",
        "HLT_HIAK8PFJetFwd40_v",
        "HLT_HIAK8PFJetFwd60_v",
        "HLT_HIAK8PFJetFwd80_v",
        "HLT_HIAK8PFJetFwd140_v",
        "HLT_HIAK8PFJet15_v",
        "HLT_HIAK8PFJet25_v",
        "HLT_HIAK8PFJet40_v",
        "HLT_HIAK8PFJet60_v",
        "HLT_HIAK8PFJet80_v",
        "HLT_HIAK8PFJet140_v",
        "HLT_HIL3Mu5_AK4PFJet30_v",
        "HLT_HIL3Mu5_AK4PFJet40_v",
        "HLT_HIL3Mu5_AK4PFJet60_v",
        ),
    triggerResults = cms.InputTag("TriggerResults", "", "HLT"),
    triggerEvent   = cms.InputTag("hltTriggerSummaryAOD", "", "HLT")
    )
