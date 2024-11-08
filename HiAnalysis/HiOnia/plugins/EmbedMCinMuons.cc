#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

namespace pat {

  class EmbedMCinMuons : public edm::stream::EDProducer<> {
  public:
    explicit EmbedMCinMuons(const edm::ParameterSet& iConfig):
      muonToken_(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"))),
      muonGenMatchToken_(consumes<edm::Association<reco::GenParticleCollection> >(iConfig.getParameter<edm::InputTag>("matchedGen"))) 
    {
      produces<pat::MuonCollection>();
    }
    ~EmbedMCinMuons() override{};

    void produce( edm::Event& iEvent, const edm::EventSetup& iSetup) override;

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    const edm::EDGetTokenT<pat::MuonCollection> muonToken_;
    const edm::EDGetTokenT<edm::Association<reco::GenParticleCollection> > muonGenMatchToken_;
  };

}  // namespace pat

void pat::EmbedMCinMuons::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // extract input information

  const auto& matches = iEvent.get(muonGenMatchToken_);
  const auto& muons = iEvent.getHandle(muonToken_);
  // initialize output muon collection
  auto output = std::make_unique<pat::MuonCollection>(*muons);

  // add gen information to muons
  for (size_t i = 0; i < muons->size(); i++) {
    const auto& gen = matches. get(muons.id(), i);
    if (gen.isNonnull()){
      (*output)[i].setGenParticle(*gen);
    }
  
  }
  iEvent.put(std::move(output));
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void pat::EmbedMCinMuons::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muons", edm::InputTag("unpackedMuons"))->setComment("muon input collection");
  desc.add<edm::InputTag>("matchedGen", edm::InputTag("muonMatch"))
      ->setComment("matches with gen muons input collection");
  descriptions.add("unpackedMuonsWithGenMatch", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
using namespace pat;
DEFINE_FWK_MODULE(EmbedMCinMuons);
