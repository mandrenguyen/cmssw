// -*- C++ -*-
//
// Package:    PFCandCompositeProducer
// Class:      PFCandCompositeProducer
// 
/**\class PFCandCompositeProducer PFCandCompositeProducer RecoHI/PFCandCompositeProducer/src/PFCandCompositeProducer

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Yetkin Yilmaz,32 4-A08,+41227673039,
//         Created:  Thu Jan 20 19:53:58 CET 2011
//
//


// system include files
#include <memory>


// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoHI/HiJetAlgos/plugins/PFCandCompositeProducer.h"

#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "TMath.h"




//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
PFCandCompositeProducer::PFCandCompositeProducer(const edm::ParameterSet& iConfig)
{
  //register your products  
  pfCandToken_ = consumes<reco::PFCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCandTag"));
  compositeToken_ = consumes<pat::CompositeCandidateCollection>(iConfig.getParameter<edm::InputTag>("compositeTag"));
  //PI = TMath::Pi();
  jpsiTriggFilter_ = iConfig.getParameter<std::string>("jpsiTrigFilter");
  isHI_ = iConfig.getParameter<bool>("isHI");
  
  produces<reco::PFCandidateCollection>();

}


PFCandCompositeProducer::~PFCandCompositeProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
PFCandCompositeProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   
   edm::Handle<reco::PFCandidateCollection> pfCands;
   iEvent.getByToken(pfCandToken_, pfCands);
   
   edm::Handle<pat::CompositeCandidateCollection> composites;
   iEvent.getByToken(compositeToken_, composites);
      
   if (not composites.isValid()) {
     std::cout << "Warning: no meson candidates ..." << std::endl;
     return;
   }

   std::vector<bool> selectedComposites; 
   int nSelComposites=0;

   std::auto_ptr<reco::PFCandidateCollection> prod(new reco::PFCandidateCollection());
   
   // first pass over composite candidates, apply selections and check for presence in PF candidates 

   for (std::vector<pat::CompositeCandidate>::const_iterator it=composites->begin();
	it!=composites->end(); ++it) {
     
     const pat::CompositeCandidate* cand = &(*it);
     
     // apply some selections on the j/psi candidates here
     if( selJpsiCand(cand) && selMuonCand(cand,"muon1") && selMuonCand(cand,"muon2") ){
       selectedComposites.push_back(true);
       nSelComposites++;
     }
     else selectedComposites.push_back(false);


     // in case the muons are needed as well
	/*
     // cout << "Now checking candidate of type " << theJpsiCat << " with pt = " << cand->pt() << endl;
     const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(cand->daughter("muon1"));
     const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(cand->daughter("muon2"));
     
     reco::TrackRef muonTrack1 = muon1->innerTrack(); 
     reco::TrackRef muonTrack2 = muon2->innerTrack(); 
	*/
     
     
     // add composites as PF candidates     
     reco::Particle::LorentzVector p4(cand->px(),cand->py(),cand->pz(),cand->energy());
     // charge, LorentzVector, type (reco::PFCandidate::ParticleType::X )
     reco::PFCandidate newPFCand(1,p4,reco::PFCandidate::ParticleType::mu);
     prod->push_back(newPFCand);     
          
     
   }
   
   int replacedCands = 0;

   // now loop over PF candidates and remove ones that are part of composites
   for(reco::PFCandidateCollection::const_iterator ci  = pfCands->begin(); ci!=pfCands->end(); ++ci)  {
     
     const reco::PFCandidate& particle = *ci;

     reco::PFCandidate *pfOut = particle.clone();
     
     bool writeCand = true;
     

     
     if(particle.trackRef().isNonnull()){
       
       reco::TrackRef pfTrack = particle.trackRef();
       
       
       int i = 0;
       for (std::vector<pat::CompositeCandidate>::const_iterator it=composites->begin();
	    it!=composites->end(); ++it, i++) {
	 
	 if(!selectedComposites[i]) continue;
	 
	 const pat::CompositeCandidate* cand = &(*it);
	 // cout << "Now checking candidate of type " << theJpsiCat << " with pt = " << cand->pt() << endl;
	 const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(cand->daughter("muon1"));
	 const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(cand->daughter("muon2"));
	 
	 reco::TrackRef muonTrack1 = muon1->innerTrack(); 
	 reco::TrackRef muonTrack2 = muon2->innerTrack(); 
	 
	 //std::cout<<" PF track, pT = "<<pfTrack->pt()<<", eta "<<pfTrack->eta()<<", phi "<<pfTrack->eta()<<std::endl;
	 //std::cout<<" muon track 1, pT = "<<muonTrack1->pt()<<", eta "<<muonTrack1->eta()<<", phi "<<muonTrack1->eta()<<std::endl;
	 //std::cout<<" muon track 2, pT = "<<muonTrack2->pt()<<", eta "<<muonTrack2->eta()<<", phi "<<muonTrack2->eta()<<std::endl;
	 
	 double eps = 0.0001;
	 if((fabs(pfTrack->pt()-muonTrack1->pt() ) < eps && fabs(pfTrack->eta()-muonTrack1->eta() ) < eps && fabs(pfTrack->phi()-muonTrack1->phi() ) < eps )  ||
	    
	    (fabs(pfTrack->pt()-muonTrack2->pt() ) < eps && fabs(pfTrack->eta()-muonTrack2->eta() ) < eps && fabs(pfTrack->phi()-muonTrack2->phi() ) < eps ) ){
	   writeCand= false;                                                                                                                                            
           replacedCands++; 
	   
	 }	   

	 //delete muon1;
	 //delete muon2;
	 /*  // I wish this worked:
	 if(muonTrack1 == pfTrack || muonTrack2 == pfTrack) {
	   writeCand= false;
	   replacedCands++;
	 }
	 */


       }
     }
   
     if(writeCand) prod->push_back(*pfOut);
     delete pfOut;
   }
   
   
   if(nSelComposites>0) std::cout<<" # of selected composites "<<nSelComposites<<" replaced candidates "<<replacedCands<<std::endl;
   
   
   iEvent.put(prod);
   
   
}

// ------------ method called once each job just before starting event loop  ------------
void 
PFCandCompositeProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PFCandCompositeProducer::endJob() {
}


bool
PFCandCompositeProducer::selJpsiCand(const pat::CompositeCandidate* jpsiCand){
  bool isSelected = true;
  
  double vtxProb = jpsiCand->userFloat("vProb");
  
  const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(jpsiCand->daughter("muon1"));
  const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(jpsiCand->daughter("muon2"));
  
  bool isOS = (muon1->charge() == muon2->charge()) ? true : false;
  
  isSelected = isSelected && (jpsiCand->pt() > 6. && fabs(jpsiCand->rapidity()) < 2.4); // Kinematic cuts
  isSelected = isSelected && (jpsiCand->mass() > 2. && jpsiCand->mass() < 4.); // Mass cuts
  isSelected = isSelected && (vtxProb > 0.01); // Vertex probability cut
  isSelected = isSelected && isOS; // Opposite sign cut
  
  //delete muon1;
  //delete muon2;
  return isSelected;
}

bool
PFCandCompositeProducer::selMuonCand(const pat::CompositeCandidate* jpsiCand, const char* muonName){
  bool isSelected = true;
  
  const pat::Muon* muon = dynamic_cast<const pat::Muon*>(jpsiCand->daughter(muonName));
  
  math::XYZPoint RefVtx;
  if (isHI_) RefVtx = (*jpsiCand->userData<reco::Vertex>("PVwithmuons")).position();
  else RefVtx = (*jpsiCand->userData<reco::Vertex>("muonlessPV")).position();
  
  reco::TrackRef iTrack = muon->innerTrack();
  
  //const pat::TriggerObjectStandAloneCollection muHLTMatchesFilter = muon->triggerObjectMatchesByFilter(jpsiTriggFilter_);
  
  //bool isTriggerMatched = muHLTMatchesFilter.size() > 0;
  bool isGoodMuon = muon::isGoodMuon(*muon, muon::TMOneStationTight);
//  bool isGlobalMuon = muon->isGlobalMuon();
  bool isTrackerMuon = muon->isTrackerMuon();
  int nTrkWMea = iTrack->hitPattern().trackerLayersWithMeasurement();
  int nPixWMea = iTrack->hitPattern().pixelLayersWithMeasurement();
  double dxy = fabs(iTrack->dxy(RefVtx));
  double dz = fabs(iTrack->dz(RefVtx));
  /*
  double eta = muon->eta();
  double pt = muon->pt();
  bool isMuonInAcc = (fabs(eta) < 2.4 && ((fabs(eta) < 1.2 && pt >= 3.5) ||
                     (1.2 <= fabs(eta) && fabs(eta) < 2.1 && pt >= 5.77-1.89*fabs(eta)) ||
                     (2.1 <= fabs(eta) && pt >= 1.8)));
  
  isSelected = isSelected && isMuonInAcc; // Acceptance cut
  */
  isSelected = isSelected && isTrackerMuon; // Is tracker muon
//  isSelected = isSelected && isGlobalMuon; // Is global muon
  //isSelected = isSelected && isTriggerMatched; // Trigger matching
  isSelected = isSelected && isGoodMuon; // Is good muon
  isSelected = isSelected && (nTrkWMea > 5); // Minimum tracking layers with measurement
  isSelected = isSelected && (nPixWMea > 0); // Minimum pixel layers with measurement
  isSelected = isSelected && (dxy < 0.3); // Maximum distance to PV in xy
  isSelected = isSelected && (dz < 20); // Maximum distance to PV in z

  return isSelected;
}

    //define this as a plug-in
DEFINE_FWK_MODULE(PFCandCompositeProducer);
