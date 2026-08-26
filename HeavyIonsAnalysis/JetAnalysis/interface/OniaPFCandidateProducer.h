#ifndef OniaPFCandidateProducer_h
#define OniaPFCandidateProducer_h

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

// #include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "TMath.h"
#include "TRandom.h"
#include <string>

class OniaPFCandidateProducer : public edm::stream::EDProducer<> {
public:
  explicit OniaPFCandidateProducer(const edm::ParameterSet &);
  ~OniaPFCandidateProducer();

private:
  virtual void beginJob();
  virtual void produce(edm::Event &, const edm::EventSetup &);
  virtual void endJob();
  bool seld0Cand(const pat::CompositeCandidate d0Cand);
  bool selOniaCand(const pat::CompositeCandidate oniaCand);
  bool selMuonCand(const pat::CompositeCandidate jpsiCand,
                   const char *muonName);
  bool checkDupTrack(const pat::CompositeCandidate cand1,
                     const pat::CompositeCandidate cand2);
  bool checkDupMuon(const pat::CompositeCandidate cand1,
                    const pat::CompositeCandidate cand2);

  // ----------member data ---------------------------

  //  edm::EDGetTokenT<reco::PFCandidateCollection> pfCandToken_;
  edm::EDGetTokenT<pat::PackedCandidateCollection> pfCandToken_;
  edm::EDGetTokenT<pat::CompositeCandidateCollection> compositeToken_;
  std::string jpsiTriggFilter_;
  bool isHI_;
  bool replaceOniaMM_;
  double massOnia_;
  double massOnia2_ = 0.0;
  bool replaceDKPi_;
  bool checkOnia_;
  std::string oniaMuonID_;
  bool applyMuonAcceptance_;
  bool assignClosestOniaMass_ = false;
  double cutMassOnia_ = 0.0;

  struct ComparePt {
    bool operator()(pat::CompositeCandidate t1,
                    pat::CompositeCandidate t2) const {
      return t1.pt() > t2.pt();
    }
  };
  ComparePt ptComparator;
};
#endif
