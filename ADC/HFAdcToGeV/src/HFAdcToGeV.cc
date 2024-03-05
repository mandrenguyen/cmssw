
// system include files
#include <memory>
#include <iostream>
#include <fstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"
#include "CalibFormats/HcalObjects/interface/HcalCalibrations.h"
#include "CondFormats/HcalObjects/interface/HcalQIEShape.h"

#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalElectronicsId.h"
#include "DataFormats/HcalDigi/interface/HcalQIESample.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

#include "Geometry/HcalCommonData/interface/HcalDDDRecConstants.h"
#include "Geometry/CaloTopology/interface/HcalTopology.h"

#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDefs.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "TTree.h"

// class declaration
//

class HFAdcToGeV : public edm::one::EDAnalyzer</*edm::one::WatchRuns, edm::one::WatchLuminosityBlocks*/> {
public:
  explicit HFAdcToGeV(const edm::ParameterSet&);
  ~HFAdcToGeV();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  edm::EDGetTokenT<QIE10DigiCollection> tok_hfQIE10_;
  edm::EDGetTokenT<CaloTowerCollection> towers_;

  bool minimized_;
  bool fillhf_;

  int nampl_;
  std::vector<int> ieta_;
  std::vector<int> depth_;
  std::vector<int> iphi_;
  std::vector<int> subdet_;

  std::vector<double> charge_;
  std::vector<double> charge_ped_;
  std::vector<double> energy_;
  std::vector<double> energy_ped_;

  std::vector<int> ampl_;
  int mMaxL1HFAdcPlus_;
  int mMaxL1HFAdcMinus_;
  int mMaxietaPlus_, mMaxietaMinus_;
  int mMaxiphiPlus_, mMaxiphiMinus_;
  int mMaxdepthPlus_, mMaxdepthMinus_;

  // hf information
  int nhfp_;
  int nhfn_;
  float hft_;
  float hftp_;
  float hftm_;

private:
  // L1GtUtils m_l1GtUtils;
  edm::Service<TFileService> fs;
  TTree *root;
  
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;
  
  // void beginRun(const edm::Run& run, const edm::EventSetup& iSetup) override;
  // void endRun(const edm::Run& run, const edm::EventSetup& iSetup) override;
  // void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  // void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------

  edm::ESGetToken<HcalDbService, HcalDbRecord> tok_conditions_;
  edm::ESGetToken<HcalDDDRecConstants, HcalRecNumberingRecord> tok_hcaldd_;

  void fill_hf(const edm::Event& iEvent);

  const int nchannel = 3456;
  int maxDepth_[5]; // 0:any, 1:HB, 2:HE, 3:HO, 4:HF

  std::string color_red = "\033[31;1m";
  std::string color_yellow = "\033[33;1m";
  std::string color_nc = "\033[0m";

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
HFAdcToGeV::HFAdcToGeV(const edm::ParameterSet& iConfig) :
  tok_conditions_(esConsumes<HcalDbService, HcalDbRecord>()),
  tok_hcaldd_(esConsumes<HcalDDDRecConstants, HcalRecNumberingRecord>())
  // m_l1GtUtils(iConfig, consumesCollector(), true)//this is important for 80x to compile
{
  const edm::InputTag hcalDigis("hcalDigis");
  tok_hfQIE10_ = consumes<QIE10DigiCollection>(iConfig.getUntrackedParameter<edm::InputTag>("digiLabel", hcalDigis));
  minimized_ = iConfig.getUntrackedParameter<bool>("minimized", false);
  fillhf_ = iConfig.getParameter<bool>("fillhf");
  const edm::InputTag towerMaker("towerMaker");
  if (fillhf_)
    towers_ = consumes<CaloTowerCollection>(iConfig.getUntrackedParameter<edm::InputTag>("tower_tag", towerMaker));
}


HFAdcToGeV::~HFAdcToGeV()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}

//
// member functions
//

// ------------ method called for each event  ------------
void HFAdcToGeV::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // using namespace edm;
  
  ieta_.clear();
  iphi_.clear();
  depth_.clear();
  subdet_.clear();

  charge_.clear();
  charge_ped_.clear();
  energy_.clear();
  energy_ped_.clear();

  ampl_.clear();  

  edm::ESHandle<HcalDDDRecConstants> pHRNDC = iSetup.getHandle(tok_hcaldd_);
  const HcalDDDRecConstants *hcons = &(*pHRNDC);
  maxDepth_[1] = hcons->getMaxDepth(0); // HB
  maxDepth_[2] = hcons->getMaxDepth(1); // HE
  maxDepth_[3] = hcons->getMaxDepth(3); // HO
  maxDepth_[4] = hcons->getMaxDepth(2); // HF
  maxDepth_[0] = (maxDepth_[1] > maxDepth_[2] ? maxDepth_[1] : maxDepth_[2]);
  maxDepth_[0] = (maxDepth_[0] > maxDepth_[3] ? maxDepth_[0] : maxDepth_[3]);
  maxDepth_[0] = (maxDepth_[0] > maxDepth_[4] ? maxDepth_[0] : maxDepth_[4]); // any of HB/HE/HO/HF

  // iSetup.get<HcalDbRecord>().get(conditions);
  edm::ESHandle<HcalDbService> conditions = iSetup.getHandle(tok_conditions_);
  edm::Handle<QIE10DigiCollection> digi;
  bool getdigitag = iEvent.getByToken(tok_hfQIE10_, digi);
  if(!getdigitag) std::cout << color_red+"HFAdcToGeV::analyze : invalid digiLabel ( - tok_hfQIE10_)"+color_nc << std::endl; 
  // else std::cout << color_yellow+"HFAdcToGeV::analyze : good digiLabel ( - tok_hfQIE10_)"+color_nc << std::endl;
  CaloSamples tool;

  const unsigned inputSize = digi->size();
  // std::cout << color_yellow+"HFAdcToGeV::analyze : digi->size() = " << inputSize << color_nc <<std::endl;
  nampl_ = inputSize;
  if ( inputSize )
    {
      mMaxL1HFAdcPlus_ = -1;
      mMaxL1HFAdcMinus_ = -1;
      mMaxietaPlus_ = -1;
      mMaxietaMinus_ = -1;
      mMaxiphiPlus_ = -1;
      mMaxiphiMinus_ = -1;
      mMaxdepthPlus_ = -1;
      mMaxdepthMinus_ = -1;
      for ( auto& it : *digi ) // QIE10DigiCollection::const_iterator
        {
          const QIE10DataFrame& frame(it);
          const HcalDetId cell(frame.id());
          int ieta = cell.ieta();
          int depth = cell.depth();
          int iphi = cell.iphi();
          int sub = cell.subdet();

          if(sub != HcalSubdetector::HcalForward) continue; // DataFormats/HcalDetId/interface/HcalSubdetector.h
          if(depth > maxDepth_[sub])
            {
              edm::LogWarning("HcalDetId") << color_red+"HcalDetID presents conflicting information. "
                                           << "Depth: " << depth << ", iphi: " << iphi << ", ieta: " << ieta 
                                           << ". Max depth from geometry is: " << maxDepth_[sub] << color_nc;
              continue;
            }

          HcalCalibrations calibrations = conditions->getHcalCalibrations(cell);
          const HcalQIECoder* channelCoder = conditions->getHcalCoder(cell);
          const HcalQIEShape* shape = conditions->getHcalShape(channelCoder);
          HcalCoderDb coder(*channelCoder, *shape);
          coder.adc2fC(frame, tool);
 
          if(frame.samples() != tool.size()) { edm::LogError("HFAdcToGeV") << "frame.samples() != tool.size()"; break; }

          int soi = tool.presamples();
          // int lastbin = tool.size() - 1;
          double charge = 0.;
          double charge_ped = 0;
          double energy = 0;
          double energy_ped = 0;
          int ampl = 0;
          for (int ii = 0; ii < tool.size(); ii++) 
            {
              QIE10DataFrame::Sample sam = frame[ii];
              if( (sam.soi() && ii != soi) || (!sam.soi() && ii == soi) ) { edm::LogError("HFAdcToGeV") << "Wrong soi information"; break; }
              int capid = sam.capid();
              int adc = sam.adc();
              if( ii != soi) continue;
              // if( ii < soi || ii > lastbin ) continue;
              charge += (tool[ii] - calibrations.pedestal(capid));
              energy += charge * calibrations.respcorrgain(capid);
              charge_ped += tool[ii];
              energy_ped += charge_ped * calibrations.respcorrgain(capid);
              ampl += adc;
            }

          if(ieta > 0) {
            if (ampl > mMaxL1HFAdcPlus_) {
              mMaxL1HFAdcPlus_ = ampl;
              mMaxietaPlus_ = ieta;
              mMaxiphiPlus_ = iphi;
              mMaxdepthPlus_ = depth;
            }
          }
          else {
            if (ampl > mMaxL1HFAdcMinus_) {
              mMaxL1HFAdcMinus_ = ampl;
              mMaxietaMinus_ = ieta;
              mMaxiphiMinus_ = iphi;
              mMaxdepthMinus_ = depth;
            }
          }
          if(!minimized_)
            {
              ieta_.push_back(ieta);
              iphi_.push_back(iphi);
              depth_.push_back(depth);
              subdet_.push_back(sub);

              charge_.push_back(charge);
              charge_ped_.push_back(charge_ped);
              energy_.push_back(energy);
              energy_ped_.push_back(energy_ped);

              ampl_.push_back(ampl);
            }
        }
    }

  if(fillhf_) fill_hf(iEvent);

  root->Fill();
}

void HFAdcToGeV::fill_hf(const edm::Event& iEvent) {
  edm::Handle<CaloTowerCollection> ts;
  iEvent.getByToken(towers_, ts);
  const CaloTowerCollection* towers = ts.product();

  int nhfn = 0;
  int nhfp = 0;

  float hftp = 0;
  float hftm = 0;

  for (const auto& cal : *towers) {
    if (cal.ietaAbs() < 30) { continue; }

    if (cal.zside() > 0) {
      if (cal.energy() > 4.) { nhfp++; }
      hftp += cal.pt();
    } else {
      if (cal.energy() > 4.) { nhfn++; }
      hftm += cal.pt();
    }
  }

  nhfp_ = nhfp;
  nhfn_ = nhfn;

  hft_ = hftp + hftm;
  hftp_ = hftp;
  hftm_ = hftm;
}

// ------------ method called once each job just before starting event loop  ------------

void HFAdcToGeV::beginJob()
{
  root = fs->make<TTree>("adc","adc");

  root->Branch("mMaxL1HFAdcPlus", &mMaxL1HFAdcPlus_, "mMaxL1HFAdcPlus/I");
  root->Branch("mMaxietaPlus", &mMaxietaPlus_, "mMaxietaPlus/I");
  root->Branch("mMaxiphiPlus", &mMaxiphiPlus_, "mMaxiphiPlus/I");
  root->Branch("mMaxdepthPlus", &mMaxdepthPlus_, "mMaxdepthPlus/I");
  root->Branch("mMaxL1HFAdcMinus", &mMaxL1HFAdcMinus_, "mMaxL1HFAdcMinus/I");
  root->Branch("mMaxietaMinus", &mMaxietaMinus_, "mMaxietaMinus/I");
  root->Branch("mMaxiphiMinus", &mMaxiphiMinus_, "mMaxiphiMinus/I");
  root->Branch("mMaxdepthMinus", &mMaxdepthMinus_, "mMaxdepthMinus/I");

  if(!minimized_)
    {
      root->Branch("nampl", &nampl_, "nampl/I");
      root->Branch("ieta", &ieta_);
      root->Branch("iphi", &iphi_);
      root->Branch("depth", &depth_);

      root->Branch("charge", &charge_);
      root->Branch("charge_ped", &charge_ped_);
      root->Branch("energy", &energy_);
      root->Branch("energy_ped", &energy_ped_);

      root->Branch("ampl", &ampl_);
    }

  if(fillhf_)
    {
      root->Branch("nhfp", &nhfp_);
      root->Branch("nhfn", &nhfn_);
      root->Branch("hft", &hft_);
      root->Branch("hftp", &hftp_);
      root->Branch("hftm", &hftm_);
    }
}

// ------------ method called once each job just after ending the event loop  ------------
void HFAdcToGeV::endJob() {}

// ------------ method called when starting to processes a run  ------------
// void HFAdcToGeV::beginRun(const edm::Run& run, const edm::EventSetup& iSetup) {}

// ------------ method called when ending the processing of a run  ------------
// void HFAdcToGeV::endRun(const edm::Run& run, const edm::EventSetup& iSetup) {}

// ------------ method called when starting to processes a luminosity block  ------------
// void HFAdcToGeV::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

// ------------ method called when ending the processing of a luminosity block  ------------
// void HFAdcToGeV::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HFAdcToGeV::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFAdcToGeV);
