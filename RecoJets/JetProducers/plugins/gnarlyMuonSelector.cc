#include <memory>

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/FwdPtr.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"


namespace pat {

  class gnarlyMuonSelector : public edm::stream::EDProducer<> {
  public:
    explicit gnarlyMuonSelector(const edm::ParameterSet& iConfig)
      :   constTag_(consumes<std::vector<edm::FwdPtr<pat::PackedCandidate> >  >(iConfig.getParameter<edm::InputTag>("constTag"))){
      //produces<std::vector<edm::FwdPtr<pat::PackedCandidate> > > ("muons");
      //produces<std::vector<edm::FwdPtr<pat::PackedCandidate> > > ("notMuons");
      //produces<std::vector<edm::FwdPtr<reco::PFCandidate> > > ("muons");
      //produces<std::vector<edm::FwdPtr<reco::PFCandidate> > > ("notMuons");
      produces<std::vector<reco::PFCandidate> >  ("muons");
      produces<std::vector<reco::PFCandidate> >  ("notMuons");
    }
    ~gnarlyMuonSelector() override = default;
    
    void produce(edm::Event&, const edm::EventSetup&) override;
    
    static void fillDescriptions(edm::ConfigurationDescriptions&);
    
  private:
    edm::EDGetTokenT<std::vector<edm::FwdPtr<pat::PackedCandidate> > > constTag_;
  };

}  // namespace pat

void pat::gnarlyMuonSelector::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  

  // input
  edm::Handle< std::vector<edm::FwdPtr<pat::PackedCandidate> > > constituents;
  iEvent.getByToken(constTag_, constituents);

  //output
  //std::unique_ptr<std::vector<edm::FwdPtr<pat::PackedCandidate> > >   myMuons = std::make_unique< std::vector<edm::FwdPtr<pat::PackedCandidate> > >();
  //std::unique_ptr<std::vector<edm::FwdPtr<pat::PackedCandidate> > >   myNotMuons = std::make_unique< std::vector<edm::FwdPtr<pat::PackedCandidate> > >();
  //std::unique_ptr<std::vector<edm::FwdPtr<reco::PFCandidate> > >   myMuons = std::make_unique< std::vector<edm::FwdPtr<reco::PFCandidate> > >();
  //std::unique_ptr<std::vector<edm::FwdPtr<reco::PFCandidate> > >   myNotMuons = std::make_unique< std::vector<edm::FwdPtr<reco::PFCandidate> > >();
  std::unique_ptr<std::vector<reco::PFCandidate> >    myMuons = std::make_unique< std::vector<reco::PFCandidate> > ();
  std::unique_ptr<std::vector<reco::PFCandidate> >    myNotMuons = std::make_unique< std::vector<reco::PFCandidate> > ();

  static const reco::PFCandidate dummySinceTranslateIsNotStatic;

  for (size_t i = 0; i < constituents->size(); ++i) {
    //std::cout<<" gnarly! "<<(*constituents)[i]->pdgId()<<std::endl;
    const edm::FwdPtr<pat::PackedCandidate> myPackedCand = (*constituents)[i];
    
    auto id = dummySinceTranslateIsNotStatic.translatePdgIdToType(myPackedCand->pdgId());    
    reco::PFCandidate pCand(reco::PFCandidate(myPackedCand->charge(), myPackedCand->p4(), id));
    math::XYZTLorentzVector pVec;
    pVec.SetPxPyPzE(myPackedCand->px(), myPackedCand->py(), myPackedCand->pz(), myPackedCand->energy());
    pCand.setP4(pVec);
    pCand.setSourceCandidatePtr(myPackedCand->sourceCandidatePtr(0));

    if(myPackedCand->pdgId()==13) myMuons->push_back(pCand);
    else myNotMuons->push_back(pCand);

  }
  
  
  iEvent.put(std::move(myMuons), "muons");
  iEvent.put(std::move(myNotMuons),"notMuons");
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void pat::gnarlyMuonSelector::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("candidates", edm::InputTag("packedPFCandidates"))
    ->setComment("packed PF candidate FwdPtr input collection");
}

#include "FWCore/Framework/interface/MakerMacros.h"
using namespace pat;
DEFINE_FWK_MODULE(gnarlyMuonSelector);
