// File: HLTAnalyzer.cc
// Description:  Example of Analysis driver originally from Jeremy Mans, 
// Date:  13-October-2006

#include <boost/foreach.hpp>

#include "HLTrigger/HLTanalyzers/interface/HLTAnalyzer.h"
#include "HLTMessages.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

typedef std::pair<const char *, const edm::InputTag *> MissingCollectionInfo;

template <class T>
static inline
bool getCollection(const edm::Event & event, std::vector<MissingCollectionInfo> & missing, edm::Handle<T> & handle, const edm::InputTag & name, const edm::EDGetTokenT<T> token, const char * description) 
{
  event.getByToken(token, handle);
  bool valid = handle.isValid();
  if (not valid) {
    missing.push_back( std::make_pair(description, & name) );
    handle.clear();
    //	std::cout << "not valid "<< description << " " << name << std::endl;
  }
  return valid;
}

// Boiler-plate constructor definition of an analyzer module:
HLTAnalyzer::HLTAnalyzer(edm::ParameterSet const& conf) :

  hlt_analysis_(conf, consumesCollector(), *this) {

    // If your module takes parameters, here is where you would define
    // their names and types, and access them to initialize internal
    // variables. Example as follows:
    std::cout << " Beginning HLTAnalyzer Analysis " << std::endl;

    getSimL1_ = conf.getUntrackedParameter<bool>("getSimL1", false);
    muonFilterCollections_    = conf.getParameter< std::vector< edm::InputTag > > ("muonFilters");
      
    muon_               = conf.getParameter<edm::InputTag> ("muon");

    firstLumi_          = conf.getUntrackedParameter<int> ("firstLumi",0);
    lastLumi_           = conf.getUntrackedParameter<int> ("lastLumi",-1);
       
    // keep this separate from l1extramc_ as needed by FastSim:
    //    This is purposefully done this way to allow FastSim to run with OpenHLT: 
    //    The {FastSim+OpenHLT} package runs on the head of HLTrigger/HLTanalyzers 
    //    where there is purposefully this duplication because FastSim does the 
    //    simulation of muons seperately, and needs the same collection.
 
    gmtstage2_      = conf.getParameter<std::string>   ("gmtStage2Digis");
    calostage2_     = conf.getParameter<std::string>   ("caloStage2Digis");
    if (getSimL1_) {
      m_l1stage2mu      = edm::InputTag(gmtstage2_,  "");
      m_l1stage2eg      = edm::InputTag(calostage2_);
      m_l1stage2jet     = edm::InputTag(calostage2_);
      m_l1stage2tau     = edm::InputTag(calostage2_);
      m_l1stage2ets     = edm::InputTag(calostage2_);
    } else {
      m_l1stage2mu      = edm::InputTag(gmtstage2_,  "Muon");
      m_l1stage2eg      = edm::InputTag(calostage2_, "EGamma");
      m_l1stage2jet     = edm::InputTag(calostage2_, "Jet");
      m_l1stage2tau     = edm::InputTag(calostage2_, "Tau");
      m_l1stage2ets     = edm::InputTag(calostage2_, "EtSum");
    }

    hltresults_       = conf.getParameter<edm::InputTag> ("hltresults");
    gObjectMapRecord_ = conf.getParameter<edm::InputTag> ("gObjectMapRecord");
    
    MuCandTag2_          = conf.getParameter<edm::InputTag> ("MuCandTag2");
    MuCandTag3_          = conf.getParameter<edm::InputTag> ("MuCandTag3");

    L3TkTracksFromL2OIStateTag_  = conf.getParameter<edm::InputTag> ("L3TkTracksFromL2OIStateTag");
    L3TkTracksFromL2OIHitTag_    = conf.getParameter<edm::InputTag> ("L3TkTracksFromL2OIHitTag");

    m_file = 0;   // set to null
    errCnt = 0;
    
    // read run parameters with a default value 
    edm::ParameterSet runParameters = conf.getParameter<edm::ParameterSet>("RunParameters");
    _HistName = runParameters.getUntrackedParameter<std::string>("HistogramFile", "test.root");
    _EtaMin   = runParameters.getUntrackedParameter<double>("EtaMin", -5.2);
    _EtaMax   = runParameters.getUntrackedParameter<double>("EtaMax",  5.2);

    _UseTFileService = conf.getUntrackedParameter<bool>("UseTFileService",false);


    // Define all consumed products  

    for (unsigned int i=0; i<muonFilterCollections_.size() ; i++) { 
      muonFilterTokens_.push_back( consumes<trigger::TriggerFilterObjectWithRefs>( muonFilterCollections_.at(i) ) );
    }

    BSProducer_ =  edm::InputTag("hltOnlineBeamSpot");
    BSProducerToken_ = consumes<reco::BeamSpot>(BSProducer_);

    muonToken_ = consumes<reco::MuonCollection>(muon_);

    hltresultsToken_ = consumes<edm::TriggerResults>(hltresults_);
    l1stage2muToken_   = consumes< BXVector<l1t::Muon> >(m_l1stage2mu);
    l1stage2egToken_   = consumes< BXVector<l1t::EGamma> >(m_l1stage2eg);
    l1stage2jetToken_  = consumes< BXVector<l1t::Jet> >(m_l1stage2jet);
    l1stage2tauToken_  = consumes< BXVector<l1t::Tau> >(m_l1stage2tau);
    l1stage2etsToken_  = consumes< BXVector<l1t::EtSum> >(m_l1stage2ets);

    gObjectMapRecordToken_ = consumes<GlobalObjectMapRecord>(gObjectMapRecord_);

    MuCandTag2Token_ = consumes<reco::RecoChargedCandidateCollection>(MuCandTag2_);
    MuCandTag3Token_ = consumes<reco::RecoChargedCandidateCollection>(MuCandTag3_);

    L3TkTracksFromL2OIStateToken_  = consumes<std::vector<reco::Track>>(L3TkTracksFromL2OIStateTag_);
    L3TkTracksFromL2OIHitToken_    = consumes<std::vector<reco::Track>>(L3TkTracksFromL2OIHitTag_);
    
    // open the tree file and initialize the tree
    if(_UseTFileService){
      edm::Service<TFileService> fs;
      HltTree = fs->make<TTree>("HltTree", "");
    }else{
      m_file = new TFile(_HistName.c_str(), "RECREATE");
      if (m_file)
        m_file->cd();
      HltTree = new TTree("HltTree", "");
    }

    treeWeight=xSection_*filterEff_;
    std::cout << "\n Setting HltTree weight to " << treeWeight << " = " << xSection_ << "*" << filterEff_ << " (cross section * gen filter efficiency)\n" << std::endl;
    
    // Setup the different analysis
    muon_analysis_.setup(conf, HltTree);
    hlt_analysis_.setup(conf, HltTree);
    evt_header_.setup(consumesCollector(), HltTree);
}

void HLTAnalyzer::beginRun(const edm::Run& run, const edm::EventSetup& c){ 
  
  hlt_analysis_.beginRun(run, c);
}

// Boiler-plate "analyze" method declaration for an analyzer module.
void HLTAnalyzer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) {
    
    // To get information from the event setup, you must request the "Record"
    // which contains it and then extract the object you need
    //edm::ESHandle<CaloGeometry> geometry;
    //iSetup.get<IdealGeometryRecord>().get(geometry);
    
    //int iLumi = iEvent.luminosityBlock();
    // if (iLumi<firstLumi_) return;
    //if (lastLumi_ != -1 && iLumi>lastLumi_) return;
    
    // These declarations create handles to the types of records that you want
    // to retrieve from event "iEvent".

    std::vector< edm::Handle<trigger::TriggerFilterObjectWithRefs> > muonFilterCollections;

    edm::Handle<edm::TriggerResults>                  hltresults;
    edm::Handle<GlobalObjectMapRecord>                l1GoMR;
    edm::Handle< BXVector<l1t::EGamma> >              l1stage2eg;
    edm::Handle< BXVector<l1t::Muon> >                l1stage2mu;
    edm::Handle< BXVector<l1t::Jet> >                 l1stage2jet;
    edm::Handle< BXVector<l1t::Tau> >                 l1stage2tau;
    edm::Handle< BXVector<l1t::EtSum> >               l1stage2ets;

    edm::Handle<reco::MuonCollection>                 muon;
    
    edm::Handle<reco::RecoChargedCandidateCollection> mucands2, mucands3;
    edm::Handle< std::vector<reco::Track> > L3TkTracksFromL2OIState, L3TkTracksFromL2OIHit;
    
    // Reco vertex collection
    edm::Handle<reco::VertexCollection> recoVertexsHLT;
    edm::Handle<reco::VertexCollection> recoVertexsOffline0;

    edm::ESHandle<MagneticField>                theMagField;
    iSetup.get<IdealMagneticFieldRecord>().get(theMagField);
    
    edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
    
    // extract the collections from the event, check their validity and log which are missing
    std::vector<MissingCollectionInfo> missing;
    
    
    for (unsigned int i=0; i<muonFilterCollections_.size() ; i++) { 
      edm::Handle<trigger::TriggerFilterObjectWithRefs> handle;
      iEvent.getByToken(muonFilterTokens_.at(i), handle);
      muonFilterCollections.push_back(handle);
    }

    //get the BeamSpot
    getCollection( iEvent, missing, recoBeamSpotHandle,   BSProducer_ ,   BSProducerToken_ ,   "Beam Spot handle");
    // gets its position
    reco::BeamSpot::Point BSPosition(0,0,0);
    BSPosition = recoBeamSpotHandle->position();
    getCollection( iEvent, missing, muon,            muon_,              muonToken_,              kMuon );
    getCollection( iEvent, missing, hltresults,      hltresults_,        hltresultsToken_,        kHltresults );
    getCollection( iEvent, missing, l1GoMR,          gObjectMapRecord_,  gObjectMapRecordToken_,  "L1 Stage2 Global Object Map Record" );
    getCollection( iEvent, missing, l1stage2eg,      m_l1stage2eg,       l1stage2egToken_,        "L1 Stage2 EGamma objects" );
    getCollection( iEvent, missing, l1stage2mu,      m_l1stage2mu,       l1stage2muToken_,        "L1 Stage2 Muon objects" );
    getCollection( iEvent, missing, l1stage2jet,     m_l1stage2jet,      l1stage2jetToken_,       "L1 Stage2 Jet objects" );
    getCollection( iEvent, missing, l1stage2tau,     m_l1stage2tau,      l1stage2tauToken_,       "L1 Stage2 Tau objects" );
    getCollection( iEvent, missing, l1stage2ets,     m_l1stage2ets,      l1stage2etsToken_,       "L1 Stage2 EtSum objects" );
    getCollection( iEvent, missing, mucands2,        MuCandTag2_,        MuCandTag2Token_,        kMucands2 );
    getCollection( iEvent, missing, mucands3,        MuCandTag3_,        MuCandTag3Token_,        kMucands3 );
    getCollection( iEvent, missing, L3TkTracksFromL2OIState, L3TkTracksFromL2OIStateTag_, L3TkTracksFromL2OIStateToken_, "L3TkTracksFromL2OIState" );
    getCollection( iEvent, missing, L3TkTracksFromL2OIHit, L3TkTracksFromL2OIHitTag_, L3TkTracksFromL2OIHitToken_, "L3TkTracksFromL2OIHit" );

    // print missing collections
    if (not missing.empty() and (errCnt < errMax())) {
        errCnt++;
        std::stringstream out;       
        out <<  "OpenHLT analyser - missing collections (This message is for information only. RECO collections will always be missing when running on RAW, MC collections will always be missing when running on data):";
        BOOST_FOREACH(const MissingCollectionInfo & entry, missing)
        out << "\n\t" << entry.first << ": " << entry.second->encode();
        edm::LogPrint("OpenHLT") << out.str() << std::endl; 
        if (errCnt == errMax())
            edm::LogWarning("OpenHLT") << "Maximum error count reached -- No more messages will be printed.";
    }
    
    
    muon_analysis_.analyze(
                           iEvent,
                           muon,
                           l1stage2mu,
                           mucands2,
                           mucands3,
                           L3TkTracksFromL2OIState,
                           L3TkTracksFromL2OIHit,
                           muonFilterCollections,
			   theMagField,
                           recoBeamSpotHandle,
                           HltTree);

    // run the analysis, passing required event fragments
    hlt_analysis_.analyze(
                          hltresults,
                          l1stage2eg,
                          l1stage2mu,
                          l1stage2jet,
                          l1stage2tau,
                          l1stage2ets,
                          l1GoMR,
                          iSetup,
                          iEvent,
                          HltTree);
    
    evt_header_.analyze(iEvent, HltTree);
    
    
    // std::cout << " Ending Event Analysis" << std::endl;
    // After analysis, fill the variables tree
    if (m_file)
        m_file->cd();
    HltTree->Fill();
}

// "endJob" is an inherited method that you may implement to do post-EOF processing and produce final output.
void HLTAnalyzer::endJob() {

  if(!_UseTFileService){    
    if (m_file)
      m_file->cd();
    
    const edm::ParameterSet &thepset = edm::getProcessParameterSetContainingModule(moduleDescription());
    TList *list = HltTree->GetUserInfo();   
    list->Add(new TObjString(thepset.dump().c_str()));   
    
    //HltTree->SetWeight(treeWeight);
    HltTree->Write();
    delete HltTree;
    HltTree = 0;
    
    if (m_file) {         // if there was a tree file...
      m_file->Write();    // write out the branches
      delete m_file;      // close and delete the file
      m_file = 0;         // set to zero to clean up
    }
  }
    
}
