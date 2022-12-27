/* \class PFJetSelector
 *
 * Selects jets with a configurable string-based cut,
 * and also writes out the constituents of the jet
 * into a separate collection.
 *
 * \author: Sal Rappoccio
 *
 *
 *  Addendum:  Selects jets that contain an X particle, which I'm using for compositeCandidates (Matt)
 *
 * for more details about the cut syntax, see the documentation
 * page below:
 *
 *   https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePhysicsCutParser
 *
 *
 */


#include "FWCore/Framework/interface/EDFilter.h"

#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/JetReco/interface/CaloJet.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "FWCore/Framework/interface/Event.h"

template < class T, typename C = std::vector<typename T::ConstituentTypeFwdPtr> >
class JetXSelector : public edm::EDFilter {

public:

  typedef std::vector<T> JetsOutput;
  typedef C ConstituentsOutput;

  JetXSelector ( edm::ParameterSet const & params ) :
    srcToken_( consumes< typename edm::View<T> >( params.getParameter<edm::InputTag>("src") ) ),
    offPV_( consumes< std::vector<reco::Vertex> >( params.getParameter<edm::InputTag>("offPV") ) ),
    cut_( params.getParameter<std::string>("cut") ),
    dummy_ ( params.getParameter<bool>("dummy") ),
    filter_(false),
    selector_( cut_ )
  {
    produces< JetsOutput >();
    produces< ConstituentsOutput > ("constituents");
  }

  virtual ~JetXSelector() {}

  virtual void beginJob() override {}
  virtual void endJob() override {}

  /*
  // Default initialization is for edm::FwdPtr. Specialization (below) is for edm::Ptr.
  typename ConstituentsOutput::value_type const initptr(edm::Ptr<pat::PackedCandidate> const& dau) const {
    return typename ConstituentsOutput::value_type(dau, dau);
  }
  */

  virtual bool filter(edm::Event& iEvent, const edm::EventSetup& iSetup) override {

    auto jets = std::make_unique< std::vector<T>>();
    auto candsOut = std::make_unique< ConstituentsOutput>();

    edm::Handle< typename edm::View<T> > h_jets;
    iEvent.getByToken( srcToken_, h_jets );


    edm::Handle<reco::VertexCollection> h_pv;
    iEvent.getByToken( offPV_, h_pv );
    
    const reco::VertexCollection* recoVertices;
    recoVertices = h_pv.product();
    
    const reco::Vertex pv = (*recoVertices)[0];
    math::XYZPoint RefVtx(pv.position().x(),pv.position().y(),pv.position().z());
    
    // Now set the Ptrs with the orphan handles.
    for ( typename edm::View<T>::const_iterator ibegin = h_jets->begin(),
	      iend = h_jets->end(), ijet = ibegin;
	  ijet != iend; ++ijet ) {
	// Check the selection
      bool isJetPlusX = false;      
      if ( selector_(*ijet) ) {
	if(!dummy_) {	
	  
	  // Check if jet contains an "X"
	  for ( unsigned int ida = 0; ida < ijet->numberOfDaughters(); ++ida ) {
	    //const reco::PFCandidate* daughter = dynamic_cast<const reco::PFCandidate*>(ijet->daughterPtr(ida).get());
	    const pat::PackedCandidate* daughter = dynamic_cast<const pat::PackedCandidate*>(ijet->daughterPtr(ida).get());
	    if(daughter->pt()<5.) continue;
	    //reco::PFCandidate::ParticleType pfCandidateType = daughter->particleId();	  
	    
	    int pfCandidateType = daughter->pdgId();	  
	    //if(pfCandidateType == reco::PFCandidate::mu )	  {
	    if(abs(pfCandidateType) == 13 )	  {
	      //if(daughter->isMuon() ){ 
	      //const reco::MuonRef& muon = daughter->muonRef();
	      //if(!daughter->isTrackerMuon()) continue;
	      if(!daughter->isGlobalMuon()) continue;
	      //reco::TrackRef iTrack = muon->innerTrack();
	      const reco::Track *iTrack = daughter->bestTrack();
	      
	      if(fabs(iTrack->dz(RefVtx)) > 20) continue;
	      if(fabs(iTrack->dxy(RefVtx)) > 0.3) continue;
	      if(iTrack->hitPattern().trackerLayersWithMeasurement() < 6) continue;
	      if(iTrack->hitPattern().pixelLayersWithMeasurement() < 1) continue;
	      
	      
	      //std::cout<<" muon pt = "<<daughter->pt()<<std::endl;
	      isJetPlusX = true;
	      break;
	    }
	  }
	}
	if(!isJetPlusX && !dummy_) continue;
	// Add the jets that pass to the output collection
	//std::cout<<" found a muon-jet "<<std::endl;
	jets->push_back( *ijet );
	if(dummy_) std::cout<<" number of daughters "<<ijet->numberOfDaughters()<<std::endl;
	for ( unsigned int ida = 0; ida < ijet->numberOfDaughters(); ++ida ) {
	  candsOut->push_back( typename ConstituentsOutput::value_type( ijet->daughterPtr(ida), ijet->daughterPtr(ida) ) );
	}
      }
    }
  
    // put  in Event
    bool pass = jets->size() > 0;
    iEvent.put(std::move(jets));
    iEvent.put(std::move(candsOut), "constituents");

    if ( filter_ )
      return pass;
    else
      return true;
    
  }


protected:
  edm::EDGetTokenT< typename edm::View<T> >                  srcToken_;
  edm::EDGetTokenT< std::vector<reco::Vertex> >              offPV_;
  std::string                    cut_;
  bool                    dummy_;
  bool                           filter_;
  StringCutObjectSelector<T>   selector_;
  
};



//typedef JetXSelector<reco::PFJet, std::vector< edm::FwdPtr<reco::PFCandidate> > > PFJetXSelector;
//typedef JetXSelector<reco::PFJet, std::vector< edm::FwdPtr<pat::PackedCandidate> >> PFJetXSelector;
typedef JetXSelector<reco::PFJet, std::vector< edm::FwdPtr<pat::PackedCandidate> >> PFJetXSelector;
//typedef JetXSelector<reco::PFJet, std::vector< edm::Ptr<pat::PackedCandidate> > >  PFJetXSelectorPtr;
typedef JetXSelector<pat::Jet, std::vector< edm::FwdPtr<pat::PackedCandidate> > > PatJetXSelector;

DEFINE_FWK_MODULE( PFJetXSelector );
//DEFINE_FWK_MODULE( PFJetXSelectorPtr );
DEFINE_FWK_MODULE( PatJetXSelector );
