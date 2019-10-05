// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

//
// class declaration
//

class HiBadParticleFilter : public edm::global::EDFilter<> {
public:
  explicit HiBadParticleFilter(const edm::ParameterSet&);
  ~HiBadParticleFilter() override;

private:
  bool filter(edm::StreamID iID, edm::Event&, const edm::EventSetup&) const override;

  // ----------member data ---------------------------

  edm::EDGetTokenT<edm::View<reco::PFCandidate> >   tokenPFCandidates_;
  edm::EDGetTokenT<edm::View<reco::Muon> >   tokenMuons_;

  const bool taggingMode_;
  const double          minMuonPt_;
  const double          minChargedHadronPt_;
  const double          minMuonTrackRelPtErr_;
  const double          maxMuonSeededDzSig_;
  const double          maxMuonSeededDxySig_;
  const double          minCaloCompatibility_;
  const double          minTrackRelPtErrLoose_;
  const double          minTrackRelPtErrTight_;
  const unsigned        minTrackNHitsLoose_;
  const unsigned        minTrackNHitsTight_;
};

//
// constructors and destructor
//
HiBadParticleFilter::HiBadParticleFilter(const edm::ParameterSet& iConfig)
  : tokenPFCandidates_ ( consumes<edm::View<reco::PFCandidate> >(iConfig.getParameter<edm::InputTag> ("PFCandidates")  ))
  , taggingMode_          ( iConfig.getParameter<bool>    ("taggingMode") )
  , minMuonPt_            ( iConfig.getParameter<double>  ("minMuonPt") )
  , minChargedHadronPt_   ( iConfig.getParameter<double>  ("minChargedHadronPt") )
  , minMuonTrackRelPtErr_ ( iConfig.getParameter<double>  ("minMuonTrackRelPtErr") )
  , maxMuonSeededDzSig_   ( iConfig.getParameter<double>  ("maxMuonSeededDzSig") )
  , maxMuonSeededDxySig_  ( iConfig.getParameter<double>  ("maxMuonSeededDxySig") )
  , minCaloCompatibility_ ( iConfig.getParameter<double>  ("minCaloCompatibility") )
  , minTrackRelPtErrLoose_( iConfig.getParameter<double>  ("minTrackRelPtErrLoose") )
  , minTrackRelPtErrTight_( iConfig.getParameter<double>  ("minTrackRelPtErrTight") )
  , minTrackNHitsLoose_   ( iConfig.getParameter<uint>  ("minTrackNHitsLoose") )
  , minTrackNHitsTight_   ( iConfig.getParameter<uint>  ("minTrackNHitsTight") )
{
  produces<bool>();
  produces<reco::PFCandidateCollection>();
}

HiBadParticleFilter::~HiBadParticleFilter() { }


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
HiBadParticleFilter::filter(edm::StreamID iID, edm::Event& iEvent, const edm::EventSetup& iSetup) const
{
  using namespace std;
  using namespace edm;

  typedef View<reco::PFCandidate> CandidateView;
  Handle<CandidateView> pfCandidates;
  iEvent.getByToken(tokenPFCandidates_,pfCandidates);

  auto pOutputCandidateCollection = std::make_unique<reco::PFCandidateCollection>();


  bool foundBadCandidate = false;


  for(unsigned j=0;j<pfCandidates->size();++j ) {
    const reco::PFCandidate & pfCandidate = (*pfCandidates)[j];
    

    if(abs(pfCandidate.particleId()) == 3)     // muon cleaning    
      {
	if(pfCandidate.pt() > minMuonPt_){
    
	if(!pfCandidate.muonRef()->isGlobalMuon() || !pfCandidate.muonRef()->isTrackerMuon() || !pfCandidate.trackRef().isNonnull())	
	  {
	    foundBadCandidate=true;
	    break;
	  }
	reco::TrackRef track = pfCandidate.trackRef();

	if(track->ptError()/track->pt()>minMuonTrackRelPtErr_){
	  foundBadCandidate=true;
	  break;	 
	}
	
	if(track->algo()==13 || track->algo()==14){
	  double dxySig = fabs(track->dxy());
	  double dxyErr = track->dxyError();
	  if(dxyErr>0) dxySig/=dxyErr;

	  double dzSig = fabs(track->dz());
	  double dzErr = track->dzError();
	  if(dzErr>0) dzSig/=dzErr;

	  if(dxySig > maxMuonSeededDxySig_ || dzSig > maxMuonSeededDzSig_){
	    foundBadCandidate=true;
	    break;	 
	  }	  	 
	}
      }
      }
    else if(abs(pfCandidate.particleId()) == 1)  //charged hadron cleaning
      {

	if(pfCandidate.pt() > minChargedHadronPt_){
	
	reco::TrackRef track = pfCandidate.trackRef();

	if(track->algo()==13 || track->algo()==14){
	  double dxySig = fabs(track->dxy());
	  double dxyErr = track->dxyError();
	  if(dxyErr>0) dxySig/=dxyErr;
	  
	  double dzSig = fabs(track->dz());
	  double dzErr = track->dzError();
	  if(dzErr>0) dzSig/=dzErr;
	  
	  if(dxySig > maxMuonSeededDxySig_ || dzSig > maxMuonSeededDzSig_){
	    foundBadCandidate=true;
	    break;	 
	  }	  	 
	  	  	  
	}

	double caloEnergy = pfCandidate.ecalEnergy() + pfCandidate.hcalEnergy();
	unsigned nHits = track->numberOfValidHits();
	double relError =track->ptError()/track->pt();

	
	if(caloEnergy < track->p()*minCaloCompatibility_) 	// tight selection if calo incompatible
	  {
	    if(relError > minTrackRelPtErrTight_  || nHits < minTrackNHitsTight_ ){
	      foundBadCandidate=true;
	      break;	 
	    }	     
	}
	else {
	    if(relError > minTrackRelPtErrLoose_  || nHits< minTrackNHitsLoose_ ){
	      foundBadCandidate=true;
	      break;	 
	    }	 
	}
	}
      }

    pOutputCandidateCollection->push_back(pfCandidate);


  } // end loop over pf candidates

  bool pass = !foundBadCandidate;

  iEvent.put(std::move(pOutputCandidateCollection) );

  iEvent.put( std::unique_ptr<bool>(new bool(pass)) );
    
  return taggingMode_ || pass;


}




//define this as a plug-in
DEFINE_FWK_MODULE(HiBadParticleFilter);
