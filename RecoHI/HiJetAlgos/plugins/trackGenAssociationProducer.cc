// WIP 

// -*- C++ -*-
//
// Package:    trackGenAssociationProducer
// Class:      trackGenAssociationProducer
//

/**\class trackGenAssociationProducer trackGenAssociationProducer.cc
* @brief Produce a mapping between the tracks and the gen particles in the event
* given the IPTagInfo and a list of final state generated particles
*
* The description of the run-time parameters can be found at fillDescriptions()
* The description of the products can be found at trackGenAssociationProducer()
*/

// Original Author:  Lida Kalipoliti, LLR


// system include files
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/EDPutToken.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

#include "CommonTools/CandUtils/interface/pdgIdUtils.h"
#include "PhysicsTools/JetMCUtils/interface/CandMCTag.h"
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/Common/interface/AssociationMap.h"
#include "AnalysisDataFormats/TrackInfo/interface/TrackGenAssociation.h"


class trackGenAssociationProducer : public edm::global::EDProducer<> {
public:
  explicit trackGenAssociationProducer(const edm::ParameterSet& cfg);
  ~trackGenAssociationProducer() override = default;

  void produce(edm::StreamID, edm::Event &, const edm::EventSetup &) const override;
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  // Definitions 
  typedef reco::TrackGenAssociationCollection MapType;
  typedef pat::PackedCandidate TrackType;
  typedef pat::PackedGenParticle GenType;

  // [DEBUG]
  typedef pat::Jet JetType;  
  // 

  // Configurables
  edm::EDGetTokenT<std::vector<GenType>> genParticlesToken_;
  edm::EDGetTokenT<std::vector<TrackType>> packedCandidatesToken_;

  // [DEBUG]
  edm::EDGetTokenT<std::vector<JetType>> inputJetsToken_;
  //

  double maxDR2_;
  double minRelPt_;
  double maxRelPt_;
};

trackGenAssociationProducer::trackGenAssociationProducer(const edm::ParameterSet& cfg) {
  // Initialize configurables
  genParticlesToken_ = consumes<std::vector<GenType>>(cfg.getParameter<edm::InputTag>("genParticleSrc"));
  packedCandidatesToken_ = consumes<std::vector<TrackType>>(cfg.getParameter<edm::InputTag>("pfCandidateSrc"));

  maxDR2_ = cfg.getUntrackedParameter<double>("maxDR2", 0.0004);
  minRelPt_ = cfg.getUntrackedParameter<double>("minRelPt", 0.8);
  maxRelPt_ = cfg.getUntrackedParameter<double>("maxRelPt", 1.2);

  produces<MapType>();
}


// ------------ method called to produce the data  ------------
void trackGenAssociationProducer::produce(edm::StreamID, edm::Event &evt, const edm::EventSetup &setup) const {
  std::cout << "trackGenAssociationProducer produce" << std::endl;

  // [DEBUG] Grab the jets
  edm::Handle<std::vector<JetType>> inputJets;
  evt.getByToken(inputJetsToken_, inputJets);
  //

  // Grab the pf candidates
  edm::Handle<std::vector<TrackType>> pfCandidates;
  evt.getByToken(packedCandidatesToken_, pfCandidates);

  // Grab the gen particles
  edm::Handle<std::vector<GenType>> genParticles;
  evt.getByToken(genParticlesToken_, genParticles);

  // Create output collection
  std::unique_ptr<MapType> trackGenMap = std::make_unique<MapType>(pfCandidates, genParticles);
 
  // Go over the charged PF candidates
  int countCharged = 0;
  for (size_t icand = 0; icand < pfCandidates->size(); icand++) {
    const pat::PackedCandidate& pfCand = pfCandidates->at(icand);
    if (pfCand.charge() == 0) continue;

    // Look for match in charged gen particles 
    double minDR2 = std::numeric_limits<double>::max();

    int imatch = -1;
    for (size_t igen = 0; igen < genParticles->size(); igen++) {
      const GenType& genParticle = genParticles->at(igen);
      if (genParticle.charge() == 0) continue;

      double DR2 = reco::deltaR2(pfCand, genParticle);
      if (DR2 > maxDR2_) continue;

      double relPt = pfCand.pt() / genParticle.pt();
      if (relPt < minRelPt_ || relPt > maxRelPt_) continue;

      if (DR2 < minDR2) {
        minDR2 = DR2;
        imatch = igen;
      }
    } // end gen particle loop

    if (imatch >= 0) {
      // get references
      pat::PackedCandidateRef pfCandRef(pfCandidates, icand);
      pat::PackedGenParticleRef matchedGenParticleRef(genParticles, imatch);

      trackGenMap->insert(pfCandRef, matchedGenParticleRef);
    }  
    countCharged++;
  }  
  evt.put(std::move(trackGenMap));
}

void trackGenAssociationProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setComment("track to gen particle association map");
  desc.add<edm::InputTag>("jetSrc", edm::InputTag("slimmedJets"));
  desc.add<edm::InputTag>("genParticleSrc", edm::InputTag("packedGenParticles"));
  desc.add<edm::InputTag>("pfCandidateSrc", edm::InputTag("packedPFCandidates"));

  desc.addUntracked<double>("maxDR2", 0.0004);
  desc.addUntracked<double>("minRelPt", 0.8);
  desc.addUntracked<double>("maxRelPt", 1.2);

  descriptions.add("trackGenAssociationProducer", desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(trackGenAssociationProducer);
