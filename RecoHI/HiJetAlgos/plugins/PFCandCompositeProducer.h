#ifndef PFCandCompositeProducer_h
#define PFCandCompositeProducer_h


// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h" 

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Math/interface/deltaR.h"


#include "TMath.h"
#include "TRandom.h"
#include <string>



class PFCandCompositeProducer : public edm::stream::EDProducer<> {
 public:
  explicit PFCandCompositeProducer(const edm::ParameterSet&);
  ~PFCandCompositeProducer();
  
 private:
  void beginStream(edm::StreamID) override;
  void produce(edm::Event&, const edm::EventSetup&) override;
  void endStream() override;
  bool seld0Cand(const pat::CompositeCandidate d0Cand) const;
  bool selJpsiCand(const pat::CompositeCandidate jpsiCand) ;
  bool selMuonCand(const pat::CompositeCandidate jpsiCand, const char* muonName) ;
  bool checkDupTrack(const pat::CompositeCandidate cand1, const pat::CompositeCandidate cand2) ;
  bool checkDupMuon(const pat::CompositeCandidate cand1, const pat::CompositeCandidate cand2) ;
  
  // ----------member data ---------------------------
  const float d0mass2 = 3.4776282; //1.86484* 1.86484;
  const float jPsimass2 = 3.096916*3.096916;

  edm::EDGetTokenT<pat::PackedCandidateCollection> pfCandToken_;
  edm::EDGetTokenT<pat::CompositeCandidateCollection> compositeToken_;
  std::string jpsiTriggFilter_;
  bool isHI_;
  bool replaceJMM_;
  bool replaceDKPi_;
  
  struct ComparePt {
    bool operator()( pat::CompositeCandidate t1, pat::CompositeCandidate t2) const { return t1.pt() > t2.pt(); }
  };
  ComparePt ptComparator;
 
  
};
#endif


