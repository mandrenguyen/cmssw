// system include files
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/TriggerNamesService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/EmptyGroupDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDefs.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

// ROOT files
#include "TTree.h"

struct HFt {
  int run;
  int lumi;
  int event;
  int bx;
  
  int nhf4p;
  int nhf4n;
  float hft;
  float hftp;
  float hftm;
};

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class MiniHFtowers : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit MiniHFtowers(const edm::ParameterSet&);
  ~MiniHFtowers();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  // ------------ member data ------------
  void fill_hf(const edm::Event&);

  edm::EDGetTokenT<CaloTowerCollection> towers_;

  HFt hf_;

  edm::Service<TFileService> fs_;
  TTree* thf_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

MiniHFtowers::MiniHFtowers(const edm::ParameterSet& iConfig) {
  // now do what ever initialization is needed
  usesResource("TFileService");

  towers_ = consumes<CaloTowerCollection>(iConfig.getParameter<edm::InputTag>("tower_tag"));
}

MiniHFtowers::~MiniHFtowers() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

void MiniHFtowers::fill_hf(const edm::Event& iEvent) {
  edm::Handle<CaloTowerCollection> ts;
  iEvent.getByToken(towers_, ts);
  const CaloTowerCollection* towers = ts.product();

  int nhf4n = 0;
  int nhf4p = 0;

  float hftp = 0;
  float hftm = 0;

  for (const auto& cal : *towers) {
    if (cal.ietaAbs() < 30) { continue; }

    // https://github.com/cms-sw/cmssw/blob/master/DataFormats/Candidate/interface/LeafCandidate.h#L124-L125
    if (cal.zside() > 0) {
      if (cal.energy() > 4.) { nhf4p++; }
      hftp += cal.pt();
    } else {
      if (cal.energy() > 4.) { nhf4n++; }
      hftm += cal.pt();
    }
  }

  hf_.nhf4p = nhf4p;
  hf_.nhf4n = nhf4n;

  hf_.hft = hftp + hftm;
  hf_.hftp = hftp;
  hf_.hftm = hftm;
}

// ------------ method called for each event ------------
void MiniHFtowers::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  hf_.run = (int)iEvent.id().run();
  hf_.lumi = (int)iEvent.luminosityBlock();
  hf_.event = (int)iEvent.id().event();
  hf_.bx = (int)iEvent.bunchCrossing();

  fill_hf(iEvent);

  thf_->Fill();
}

// ------------ method called once each job just before starting event loop ------------
void MiniHFtowers::beginJob() {
  thf_ = fs_->make<TTree>("HFtree", "hfs");

  thf_->Branch("run", &hf_.run, "run/I");
  thf_->Branch("lumi", &hf_.lumi, "lumi/I");
  thf_->Branch("event", &hf_.event, "event/I");
  thf_->Branch("bx", &hf_.bx, "bx/I");

  thf_->Branch("nhf4p", &hf_.nhf4p, "nhf4p/I");
  thf_->Branch("nhf4n", &hf_.nhf4n, "nhf4n/I");
  thf_->Branch("hft", &hf_.hft, "hft/F");
  thf_->Branch("hftp", &hf_.hftp, "hftp/F");
  thf_->Branch("hftm", &hf_.hftm, "hftm/F");
}

// ------------ method called once each job just after ending the event loop ------------
void MiniHFtowers::endJob() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module ------------
void MiniHFtowers::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  // edm::ParameterSetDescription desc;

  // edm::ParameterDescription<edm::InputTag>("tower_tag", edm::InputTag("towerMaker"), true);
  // descriptions.add("minihftowers", desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(MiniHFtowers);
