#ifndef PFCandCompositeProducer_h
#define PFCandCompositeProducer_h


// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Math/interface/deltaR.h"


#include "TMath.h"
#include "TRandom.h"



class PFCandCompositeProducer : public edm::EDProducer {
 public:
  explicit PFCandCompositeProducer(const edm::ParameterSet&);
  ~PFCandCompositeProducer();
  
 private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  // ----------member data ---------------------------

  edm::EDGetTokenT<reco::PFCandidateCollection> pfCandToken_;
  edm::EDGetTokenT<pat::CompositeCandidateCollection> compositeToken_;

 
  
};
#endif
