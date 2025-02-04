#include <iostream>

#include "HLTrigger/HLTanalyzers/interface/HLTMCtruth.h"
#include "HLTrigger/HLTanalyzers/interface/HLTMuon.h"
#include "HLTrigger/HLTanalyzers/interface/HLTStage2Info.h"
#include "HLTrigger/HLTanalyzers/interface/EventHeader.h"
#include "HLTrigger/HLTanalyzers/interface/RECOVertex.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/Registry.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"  

#include "CondFormats/DataRecord/interface/L1CaloGeometryRecord.h"  

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1TGlobal/interface/GlobalObjectMapRecord.h"
#include "DataFormats/L1TGlobal/interface/GlobalObjectMap.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

/** \class HLTAnalyzer
  *  
  * $Date: November 2006
  * $Revision: 
  * \author P. Bargassa - Rice U.
  */

class HLTAnalyzer : public edm::EDAnalyzer {
public:
  explicit HLTAnalyzer(edm::ParameterSet const& conf);
  virtual void analyze(edm::Event const& e, edm::EventSetup const& iSetup);
  virtual void beginRun(const edm::Run& , const edm::EventSetup& );
  virtual void endJob();

  //  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions); 

  // Analysis tree to be filled
  TTree *HltTree;

private:
  // variables persistent across events should be declared here.
  //
  ///Default analyses

  EventHeader evt_header_;
  HLTMuon     muon_analysis_;
  HLTMCtruth  mct_analysis_;
  HLTStage2Info     hlt_analysis_;
  RECOVertex  vrt_analysisHLT_;
  RECOVertex  vrt_analysisOffline0_;

  int firstLumi_, lastLumi_;
  double xSection_, filterEff_, treeWeight; 

  //
  // All tokens needed to access products in the event
  //
  std::vector< edm::EDGetTokenT<trigger::TriggerFilterObjectWithRefs> >  muonFilterTokens_;
  std::vector<int> L1Bits_;

  edm::EDGetTokenT< BXVector<GlobalAlgBlk> >             algToken_;
  edm::EDGetTokenT<reco::BeamSpot>                       BSProducerToken_;
  edm::EDGetTokenT<reco::MuonCollection>                 muonToken_;
  edm::EDGetTokenT<edm::TriggerResults>                  hltresultsToken_;
  edm::EDGetTokenT<GlobalObjectMapRecord>                gObjectMapRecordToken_;
  edm::EDGetTokenT< BXVector<l1t::Muon> >                l1stage2muToken_;
  edm::EDGetTokenT< BXVector<l1t::EGamma> >              l1stage2egToken_;
  edm::EDGetTokenT< BXVector<l1t::Jet> >                 l1stage2jetToken_; 
  edm::EDGetTokenT< BXVector<l1t::Tau> >                 l1stage2tauToken_;
  edm::EDGetTokenT< BXVector<l1t::EtSum> >               l1stage2etsToken_;
    
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> MuCandTag2Token_, MuCandTag3Token_;
  edm::EDGetTokenT<std::vector<reco::Track>> L3TkTracksFromL2OIStateToken_, L3TkTracksFromL2OIHitToken_;
              
  // Reco vertex collection
  edm::EDGetTokenT<reco::VertexCollection> VertexHLTToken_;
  edm::EDGetTokenT<reco::VertexCollection> VertexOffline0Token_;

  //
  // All input tags
  //
  bool getSimL1_;
  std::vector< edm::InputTag > muonFilterCollections_;

  edm::InputTag BSProducer_;

  edm::InputTag algInputTag_;
  edm::InputTag hltresults_;
  edm::InputTag gObjectMapRecord_;
  edm::InputTag muon_;
  edm::InputTag m_l1stage2mu;
  edm::InputTag m_l1stage2eg;
  edm::InputTag m_l1stage2jet;
  edm::InputTag m_l1stage2tau;
  edm::InputTag m_l1stage2ets;
  edm::InputTag m_l1stage2ct;

  edm::InputTag MuCandTag2_,MuCandTag3_;
  edm::InputTag L3TkTracksFromL2OIStateTag_, L3TkTracksFromL2OIHitTag_;
  
  // Reco vertex collection
  edm::InputTag VertexTagHLT_;
  edm::InputTag VertexTagOffline0_;

  std::string gmtstage2_, calostage2_;

  int errCnt;
  static int errMax() { return 5; }

  std::string _HistName; // Name of histogram file
  double _EtaMin,_EtaMax;
    double _MinPtChargedHadrons, _MinPtGammas;
  TFile* m_file; // pointer to Histogram file

  bool _UseTFileService;
};
