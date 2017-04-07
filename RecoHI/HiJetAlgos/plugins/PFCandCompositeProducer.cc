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
     
     // apply some selections on the j/psi candidates here (FIXME)     
     if(cand->pt()>6. && cand->mass() > 2.){
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


	 /*  // I wish this worked:
	 if(muonTrack1 == pfTrack || muonTrack2 == pfTrack) {
	   writeCand= false;
	   replacedCands++;
	 }
	 */


       }
     }
   
     if(writeCand) prod->push_back(*pfOut);
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







    //define this as a plug-in
DEFINE_FWK_MODULE(PFCandCompositeProducer);
