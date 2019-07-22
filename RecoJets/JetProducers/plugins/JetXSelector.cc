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
    cut_( params.getParameter<std::string>("cut") ),
    filter_(false),
    selector_( cut_ )
  {
    produces< JetsOutput >();
    produces< ConstituentsOutput > ("constituents");
  }

  virtual ~JetXSelector() {}

  virtual void beginJob() override {}
  virtual void endJob() override {}

  virtual bool filter(edm::Event& iEvent, const edm::EventSetup& iSetup) override {

    //std::auto_ptr< JetsOutput > jets ( new std::vector<T>() );
    auto jets = std::make_unique< std::vector<T>>();
    //std::auto_ptr< ConstituentsOutput > candsOut( new ConstituentsOutput  );
    auto candsOut = std::make_unique< ConstituentsOutput>();

    edm::Handle< typename edm::View<T> > h_jets;
    iEvent.getByToken( srcToken_, h_jets );

    // Now set the Ptrs with the orphan handles.
    for ( typename edm::View<T>::const_iterator ibegin = h_jets->begin(),
	    iend = h_jets->end(), ijet = ibegin;
	  ijet != iend; ++ijet ) {

      // Check the selection
      if ( selector_(*ijet) ) {
	// Check if jet contains an "X"
	bool isJetPlusX = false;
	
	for ( unsigned int ida = 0; ida < ijet->numberOfDaughters(); ++ida ) {
	  
	  const reco::PFCandidate* daughter = dynamic_cast<const reco::PFCandidate*>(ijet->daughterPtr(ida).get());
	  reco::PFCandidate::ParticleType pfCandidateType = daughter->particleId();
	  if(pfCandidateType == reco::PFCandidate::h_HF )	  {
	    //std::cout<<" hooray!  "<<daughter->pt()<<std::endl;
	    isJetPlusX = true;
	    break;
	  }
	}

	if(!isJetPlusX) continue;
	// Add the jets that pass to the output collection
	jets->push_back( *ijet );
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
  std::string                    cut_;
  bool                           filter_;
  StringCutObjectSelector<T>   selector_;

};

typedef JetXSelector<reco::PFJet> PFJetXSelector;
typedef JetXSelector<pat::Jet, std::vector< edm::FwdPtr<pat::PackedCandidate> > > PatJetXSelector;

DEFINE_FWK_MODULE( PFJetXSelector );
DEFINE_FWK_MODULE( PatJetXSelector );
