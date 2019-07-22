#include "HiSkim/HiOnia2MuMu/interface/HiOnia2MuMuPAT.h"

//Headers for the data items
#include <DataFormats/TrackReco/interface/TrackFwd.h>
#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/MuonReco/interface/MuonFwd.h>
#include <DataFormats/MuonReco/interface/Muon.h>
#include <DataFormats/Common/interface/View.h>
#include <DataFormats/HepMCCandidate/interface/GenParticle.h>
#include <DataFormats/PatCandidates/interface/Muon.h>
#include <DataFormats/VertexReco/interface/VertexFwd.h>

//Headers for services and tools
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "RecoVertex/PrimaryVertexProducer/interface/PrimaryVertexProducer.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"     
#include "TMath.h"
#include "Math/VectorUtil.h"
#include "TVector3.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"

HiOnia2MuMuPAT::HiOnia2MuMuPAT(const edm::ParameterSet& iConfig):
  muonsToken_(consumes< edm::View<pat::Muon> >(iConfig.getParameter<edm::InputTag>("muons"))),
  thebeamspotToken_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpotTag"))),
  thePVsToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("primaryVertexTag"))),
  recoTracksToken_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("srcTracks"))),
  theGenParticlesToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticles"))),
  higherPuritySelection_(iConfig.getParameter<std::string>("higherPuritySelection")),
  lowerPuritySelection_(iConfig.getParameter<std::string>("lowerPuritySelection")),
  dimuonSelection_(iConfig.existsAs<std::string>("dimuonSelection") ? iConfig.getParameter<std::string>("dimuonSelection") : ""),
  trimuonSelection_(iConfig.existsAs<std::string>("trimuonSelection") ? iConfig.getParameter<std::string>("trimuonSelection") : ""),
  LateDimuonSel_(iConfig.existsAs<std::string>("LateDimuonSel") ? iConfig.getParameter<std::string>("LateDimuonSel") : ""),
  LateTrimuonSel_(iConfig.existsAs<std::string>("LateTrimuonSel") ? iConfig.getParameter<std::string>("LateTrimuonSel") : ""),
  addCommonVertex_(iConfig.getParameter<bool>("addCommonVertex")),
  addMuonlessPrimaryVertex_(iConfig.getParameter<bool>("addMuonlessPrimaryVertex")),
  resolveAmbiguity_(iConfig.getParameter<bool>("resolvePileUpAmbiguity")),
  onlySoftMuons_(iConfig.getParameter<bool>("onlySoftMuons")),
  doTrimuons_(iConfig.getParameter<bool>("doTrimuons"))
{  
  produces<pat::CompositeCandidateCollection>("");
  produces<pat::CompositeCandidateCollection>("trimuon");
}


HiOnia2MuMuPAT::~HiOnia2MuMuPAT()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

bool
HiOnia2MuMuPAT::isSoftMuon(const pat::Muon* aMuon) {
  return (
          muon::isGoodMuon(*aMuon, muon::TMOneStationTight) &&
          aMuon->innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5   &&
          aMuon->innerTrack()->hitPattern().pixelLayersWithMeasurement()   > 0   &&
          //aMuon->innerTrack()->quality(reco::TrackBase::highPurity) && 
          fabs(aMuon->innerTrack()->dxy(RefVtx)) < 0.3 &&
          fabs(aMuon->innerTrack()->dz(RefVtx)) < 20.
          );
}

// ------------ method called to produce the data  ------------
void
HiOnia2MuMuPAT::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{  
  using namespace edm;
  using namespace std;
  using namespace reco;
  typedef Candidate::LorentzVector LorentzVector;

  vector<double> muMasses, muMasses3;
  muMasses.push_back( 0.1056583715 );  muMasses.push_back( 0.1056583715 );
  muMasses3.push_back( 0.1056583715 );  muMasses3.push_back( 0.1056583715 );  muMasses3.push_back( 0.1056583715 );

  std::unique_ptr<pat::CompositeCandidateCollection> oniaOutput(new pat::CompositeCandidateCollection);
  std::unique_ptr<pat::CompositeCandidateCollection> trimuOutput(new pat::CompositeCandidateCollection);
  
  Vertex thePrimaryV;
  Vertex theBeamSpotV; 

  ESHandle<MagneticField> magneticField;
  iSetup.get<IdealMagneticFieldRecord>().get(magneticField);

  // get the stored reco BS, and copy its position in a Vertex object (theBeamSpotV)
  Handle<BeamSpot> theBeamSpot;
  iEvent.getByToken(thebeamspotToken_,theBeamSpot);
  BeamSpot bs = *theBeamSpot;
  theBeamSpotV = Vertex(bs.position(), bs.covariance3D());

  Handle<VertexCollection> priVtxs;
  iEvent.getByToken(thePVsToken_, priVtxs);
  if ( priVtxs->begin() != priVtxs->end() ) {
    thePrimaryV = Vertex(*(priVtxs->begin()));
  }
  else {
    thePrimaryV = Vertex(bs.position(), bs.covariance3D());
  }

  RefVtx = thePrimaryV.position();
 
  // // ---------------------------
  Handle< View<pat::Muon> > muons;
  iEvent.getByToken(muonsToken_ , muons);

  edm::ESHandle<TransientTrackBuilder> theTTBuilder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theTTBuilder);
  KalmanVertexFitter vtxFitter(true);
  TrackCollection muonLess; // track collection related to PV, minus the 2 muons (if muonLessPV option is activated)

  Handle<reco::TrackCollection> collTracks;
  iEvent.getByToken(recoTracksToken_,collTracks);
  int Ntrk = -1; 
  if ( collTracks.isValid() ) {
    Ntrk = 0;
    for(std::vector<reco::Track>::const_iterator it=collTracks->begin(); it!=collTracks->end(); ++it) {
      const reco::Track* track = &(*it);        
      if ( track->qualityByName("highPurity") ) { Ntrk++; }
    }
  }

  std::vector<pat::Muon> ourMuons;
  for(View<pat::Muon>::const_iterator it = muons->begin(), itend = muons->end(); it != itend; ++it){
    if ( lowerPuritySelection_(*it) && (!onlySoftMuons_ || isSoftMuon(&(*it))) ){
      ourMuons.push_back(*it);}
  }
  int ourMuNb = ourMuons.size();
  //std::cout<<"number of soft muons = "<<ourMuNb<<std::endl;

  int Bcnb=0;
  //  int Jpsinb=0;
  // JPsi candidates only from muons
  for(int i=0; i<ourMuNb; i++){
    const pat::Muon& it = ourMuons[i];
    for(int j=i+1; j<ourMuNb; j++){
      const pat::Muon& it2 = ourMuons[j];
      // one muon must pass tight quality
      if (!(higherPuritySelection_(it) || higherPuritySelection_(it2))) continue;

      pat::CompositeCandidate myCand;
      // ---- no explicit order defined ----
      myCand.addDaughter(it, "muon1");
      myCand.addDaughter(it2,"muon2"); 

      // ---- define and set candidate's 4momentum  ----  
      LorentzVector jpsi = it.p4() + it2.p4();
      myCand.setP4(jpsi);
      myCand.setCharge(it.charge()+it2.charge());

      std::map< std::string, int > userInt;
      std::map< std::string, float > userFloat;
      std::map< std::string, reco::Vertex > userVertex;
      Vertex theOriginalPV;

      // ---- fit vertex using Tracker tracks (if they have tracks) ----
      if (it.track().isNonnull() && it2.track().isNonnull()) {
        
        //build the dimuon secondary vertex      

        vector<TransientTrack> t_tks;
        t_tks.push_back(theTTBuilder->build(*it.track()));  // pass the reco::Track, not  the reco::TrackRef (which can be transient)
        t_tks.push_back(theTTBuilder->build(*it2.track())); // otherwise the vertex will have transient refs inside.
        TransientVertex myVertex = vtxFitter.vertex(t_tks);

        CachingVertex<5> VtxForInvMass = vtxFitter.vertex( t_tks );
        Measurement1D MassWErr = massCalculator.invariantMass( VtxForInvMass, muMasses );
        userFloat["MassErr"] = MassWErr.error();

        if (myVertex.isValid()) {

          if (resolveAmbiguity_) {
            float minDz = 999999.;
            
            TwoTrackMinimumDistance ttmd;
            bool status = ttmd.calculate( GlobalTrajectoryParameters(
                                                                     GlobalPoint(myVertex.position().x(), myVertex.position().y(), myVertex.position().z()),
                                                                     GlobalVector(myCand.px(),myCand.py(),myCand.pz()),TrackCharge(0),&(*magneticField)),
                                          GlobalTrajectoryParameters(
                                                                     GlobalPoint(bs.position().x(), bs.position().y(), bs.position().z()),
                                                                     GlobalVector(bs.dxdz(), bs.dydz(), 1.),TrackCharge(0),&(*magneticField)));
            float extrapZ=-9E20;
            if (status) extrapZ=ttmd.points().first.z();
            
            for(VertexCollection::const_iterator itv = priVtxs->begin(), itvend = priVtxs->end(); itv != itvend; ++itv) {
              // only consider good vertices
              if ( itv->isFake() || fabs(itv->position().z()) > 25 || itv->position().Rho() > 2 || itv->tracksSize() < 2) continue;
              float deltaZ = fabs(extrapZ - itv->position().z()) ;
              if ( deltaZ < minDz ) {
                minDz = deltaZ;    
                thePrimaryV = Vertex(*itv);
              }
            }
          }//if resolve ambiguity

          theOriginalPV = thePrimaryV;

	  // ---- apply the dimuon cut --- This selection is done only here because "resolvePileUpAmbiguity" info is needed for later Trimuon 
	  if(!dimuonSelection_(myCand)) 
	    {goto TrimuonCand;}

          float vChi2 = myVertex.totalChiSquared();
          float vNDF  = myVertex.degreesOfFreedom();
          float vProb(TMath::Prob(vChi2,(int)vNDF));
          
          userFloat["vNChi2"] = (vChi2/vNDF);
          userFloat["vProb"] = vProb;

          TVector3 vtx, vtx3D;
          TVector3 pvtx, pvtx3D;
          VertexDistanceXY vdistXY;
          VertexDistance3D vdistXYZ;

          vtx.SetXYZ(myVertex.position().x(),myVertex.position().y(),0);
          TVector3 pperp(jpsi.px(), jpsi.py(), 0);
          AlgebraicVector3 vpperp(pperp.x(), pperp.y(), 0.);

          vtx3D.SetXYZ(myVertex.position().x(),myVertex.position().y(),myVertex.position().z());
          TVector3 pxyz(jpsi.px(), jpsi.py(), jpsi.pz());
          AlgebraicVector3 vpxyz(pxyz.x(), pxyz.y(), pxyz.z());


          muonLess.clear();
          muonLess.reserve(thePrimaryV.tracksSize());
          if( addMuonlessPrimaryVertex_ && thePrimaryV.tracksSize()>2) {
            // Primary vertex matched to the dimuon, now refit it removing the two muons
            //edm::LogWarning("HiOnia2MuMuPAT_addMuonlessPrimaryVertex") << "If muonLessPV is turned on, ctau is calculated with muonLessPV only.\n" ;

            // I need to go back to the reco::Muon object, as the TrackRef in the pat::Muon can be an embedded ref.
            const reco::Muon *rmu1 = dynamic_cast<const reco::Muon *>(it.originalObject());
            const reco::Muon *rmu2 = dynamic_cast<const reco::Muon *>(it2.originalObject());
            if( thePrimaryV.hasRefittedTracks() ) {
              // Need to go back to the original tracks before taking the key
              std::vector<reco::Track>::const_iterator itRefittedTrack   = thePrimaryV.refittedTracks().begin();
              std::vector<reco::Track>::const_iterator refittedTracksEnd = thePrimaryV.refittedTracks().end();
              for( ; itRefittedTrack != refittedTracksEnd; ++itRefittedTrack ) {
                if( thePrimaryV.originalTrack(*itRefittedTrack).key() == rmu1->track().key() ) continue;
                if( thePrimaryV.originalTrack(*itRefittedTrack).key() == rmu2->track().key() ) continue;

                const reco::Track & recoTrack = *(thePrimaryV.originalTrack(*itRefittedTrack));
                muonLess.push_back(recoTrack);
              }
            }// PV has refitted tracks
            else 
	      {
		std::vector<reco::TrackBaseRef>::const_iterator itPVtrack = thePrimaryV.tracks_begin();
		for( ; itPVtrack != thePrimaryV.tracks_end(); ++itPVtrack ) if (itPVtrack->isNonnull()) {
		    if( itPVtrack->key() == rmu1->track().key() ) continue;
		    if( itPVtrack->key() == rmu2->track().key() ) continue;
		    muonLess.push_back(**itPVtrack);
		  }
	      }// take all tracks associated with the vtx
	    
            if (muonLess.size()>1 && muonLess.size() < thePrimaryV.tracksSize()) {
              // find the new vertex, from which the 2 munos were removed
              // need the transient tracks corresponding to the new track collection
              std::vector<reco::TransientTrack> t_tks; 
              t_tks.reserve(muonLess.size());

              for (reco::TrackCollection::const_iterator it = muonLess.begin(), ed = muonLess.end(); it != ed; ++it) {
                t_tks.push_back((*theTTBuilder).build(*it));
                t_tks.back().setBeamSpot(bs);
              }
              AdaptiveVertexFitter* theFitter=new AdaptiveVertexFitter();
              TransientVertex pvs = theFitter->vertex(t_tks, bs);  // if you want the beam constraint

              if (pvs.isValid()) {
                reco::Vertex muonLessPV = Vertex(pvs);
                thePrimaryV = muonLessPV;
              } else {
                edm::LogWarning("HiOnia2MuMuPAT_FailingToRefitMuonLessVtx") << 
                "TransientVertex re-fitted is not valid!! You got still the 'old vertex'" << "\n";
              }
            } else {
              if ( muonLess.size()==thePrimaryV.tracksSize() ){
                edm::LogWarning("HiOnia2MuMuPAT_muonLessSizeORpvTrkSize") << 
                 "Still have the original PV: the refit was not done 'cose it is already muonless" << "\n";
              } else if ( muonLess.size()<=1 ){
                edm::LogWarning("HiOnia2MuMuPAT_muonLessSizeORpvTrkSize") << 
                  "Still have the original PV: the refit was not done 'cose there are not enough tracks to do the refit without the muon tracks" << "\n";
              } else {
                edm::LogWarning("HiOnia2MuMuPAT_muonLessSizeORpvTrkSize") << 
                  "Still have the original PV: Something weird just happened, muonLess.size()=" << muonLess.size() << " and thePrimaryV.tracksSize()=" << thePrimaryV.tracksSize() << " ." << "\n";
              }
            }
          }// refit vtx without the muon tracks

          // count the number of high Purity tracks with pT > 900 MeV attached to the chosen vertex      
          // this makes sense only in case of pp reconstruction
          double vertexWeight = -1., sumPTPV = -1.;
          int countTksOfPV = -1;
         
          EDConsumerBase::Labels thePVsLabel;
          EDConsumerBase::labelsForToken(thePVsToken_, thePVsLabel);
          if(thePVsLabel.module==(std::string)("offlinePrimaryVertices")) {
            const reco::Muon *rmu1 = dynamic_cast<const reco::Muon *>(it.originalObject());
            const reco::Muon *rmu2 = dynamic_cast<const reco::Muon *>(it2.originalObject());
            try {
              for(reco::Vertex::trackRef_iterator itVtx = theOriginalPV.tracks_begin(); itVtx != theOriginalPV.tracks_end(); itVtx++) if(itVtx->isNonnull()) {

                const reco::Track& track = **itVtx;
                if(!track.quality(reco::TrackBase::highPurity)) continue;
                if(track.pt() < 0.5) continue; //reject all rejects from counting if less than 900 MeV

                TransientTrack tt = theTTBuilder->build(track);
                pair<bool,Measurement1D> tkPVdist = IPTools::absoluteImpactParameter3D(tt,theOriginalPV);

                if (!tkPVdist.first) continue;
                if (tkPVdist.second.significance()>3) continue;
                if (track.ptError()/track.pt()>0.1) continue;

                // do not count the two muons
                if (rmu1 != 0 && rmu1->innerTrack().key() == itVtx->key()) continue;
                if (rmu2 != 0 && rmu2->innerTrack().key() == itVtx->key()) continue;

                vertexWeight += theOriginalPV.trackWeight(*itVtx);
                if(theOriginalPV.trackWeight(*itVtx) > 0.5){
                  countTksOfPV++;
                  sumPTPV += track.pt();
                }
              }
            } catch (std::exception & err) {std::cout << " Counting tracks from PV, fails! " << std::endl; return ; }
          }
          userInt["countTksOfPV"] = countTksOfPV;
          userFloat["vertexWeight"] = (float) vertexWeight;
          userFloat["sumPTPV"] = (float) sumPTPV;

          ///DCA
          TrajectoryStateClosestToPoint mu1TS = t_tks[0].impactPointTSCP();
          TrajectoryStateClosestToPoint mu2TS = t_tks[1].impactPointTSCP();
          float dca = 1E20;
          if (mu1TS.isValid() && mu2TS.isValid()) {
            ClosestApproachInRPhi cApp;
            cApp.calculate(mu1TS.theState(), mu2TS.theState());
            if (cApp.status() ) dca = cApp.distance();
          }
          userFloat["DCA"] = dca;
          ///end DCA
         
          if (addMuonlessPrimaryVertex_) {
            userVertex["muonlessPV"] = thePrimaryV;
            userVertex["PVwithmuons"] = theOriginalPV;
          } else {
            userVertex["PVwithmuons"] = thePrimaryV;
          }
          
          // lifetime using PV
          pvtx.SetXYZ(thePrimaryV.position().x(),thePrimaryV.position().y(),0);
          TVector3 vdiff = vtx - pvtx;
          double cosAlpha = vdiff.Dot(pperp)/(vdiff.Perp()*pperp.Perp());
          Measurement1D distXY = vdistXY.distance(Vertex(myVertex), thePrimaryV);
          double ctauPV = distXY.value()*cosAlpha*3.096916/pperp.Perp();
          GlobalError v1e = (Vertex(myVertex)).error();
          GlobalError v2e = thePrimaryV.error();
          AlgebraicSymMatrix33 vXYe = v1e.matrix()+ v2e.matrix();
          double ctauErrPV = sqrt(ROOT::Math::Similarity(vpperp,vXYe))*3.096916/(pperp.Perp2());

          userFloat["ppdlPV"] = ctauPV;
          userFloat["ppdlErrPV"] = ctauErrPV;
          userFloat["cosAlpha"] = cosAlpha;

          pvtx3D.SetXYZ(thePrimaryV.position().x(),thePrimaryV.position().y(),thePrimaryV.position().z());
          TVector3 vdiff3D = vtx3D - pvtx3D;
          double cosAlpha3D = vdiff3D.Dot(pxyz)/(vdiff3D.Mag()*pxyz.Mag());
          Measurement1D distXYZ = vdistXYZ.distance(Vertex(myVertex), thePrimaryV);
          double ctauPV3D = distXYZ.value()*cosAlpha3D*3.096916/pxyz.Mag();
          double ctauErrPV3D = sqrt(ROOT::Math::Similarity(vpxyz,vXYe))*3.096916/(pxyz.Mag2());
          
          userFloat["ppdlPV3D"] = ctauPV3D;
          userFloat["ppdlErrPV3D"] = ctauErrPV3D;
          userFloat["cosAlpha3D"] = cosAlpha3D;

          if (addMuonlessPrimaryVertex_) {
	    // lifetime using Original PV
	    pvtx.SetXYZ(theOriginalPV.position().x(),theOriginalPV.position().y(),0);
	    vdiff = vtx - pvtx;
	    double cosAlphaOrigPV = vdiff.Dot(pperp)/(vdiff.Perp()*pperp.Perp());
	    distXY = vdistXY.distance(Vertex(myVertex), theOriginalPV);
	    double ctauOrigPV = distXY.value()*cosAlphaOrigPV*3.096916/pperp.Perp();
	    GlobalError v1eOrigPV = (Vertex(myVertex)).error();
	    GlobalError v2eOrigPV = theOriginalPV.error();
	    AlgebraicSymMatrix33 vXYeOrigPV = v1eOrigPV.matrix()+ v2eOrigPV.matrix();
	    double ctauErrOrigPV = sqrt(ROOT::Math::Similarity(vpperp,vXYeOrigPV))*3.096916/(pperp.Perp2());

	    userFloat["ppdlOrigPV"] = ctauOrigPV;
	    userFloat["ppdlErrOrigPV"] = ctauErrOrigPV;

	    pvtx3D.SetXYZ(theOriginalPV.position().x(), theOriginalPV.position().y(), theOriginalPV.position().z());
	    vdiff3D = vtx3D - pvtx3D;
	    double cosAlphaOrigPV3D = vdiff3D.Dot(pxyz)/(vdiff3D.Mag()*pxyz.Mag());
	    distXYZ = vdistXYZ.distance(Vertex(myVertex), theOriginalPV);
	    double ctauOrigPV3D = distXYZ.value()*cosAlphaOrigPV3D*3.096916/pxyz.Mag();
	    double ctauErrOrigPV3D = sqrt(ROOT::Math::Similarity(vpxyz,vXYeOrigPV))*3.096916/(pxyz.Mag2());
          
	    userFloat["ppdlOrigPV3D"] = ctauOrigPV3D;
	    userFloat["ppdlErrOrigPV3D"] = ctauErrOrigPV3D;
	  }
	  else {
	    userFloat["ppdlOrigPV"] = ctauPV;
	    userFloat["ppdlErrOrigPV"] = ctauErrPV;
	    userFloat["ppdlOrigPV3D"] = ctauPV3D;
	    userFloat["ppdlErrOrigPV3D"] = ctauErrPV3D;
	  }

          // lifetime using BS
          pvtx.SetXYZ(theBeamSpotV.position().x(),theBeamSpotV.position().y(),0);
          vdiff = vtx - pvtx;
          cosAlpha = vdiff.Dot(pperp)/(vdiff.Perp()*pperp.Perp());
          distXY = vdistXY.distance(Vertex(myVertex), theBeamSpotV);
          double ctauBS = distXY.value()*cosAlpha*3.096916/pperp.Perp();
          GlobalError v1eB = (Vertex(myVertex)).error();
          GlobalError v2eB = theBeamSpotV.error();
          AlgebraicSymMatrix33 vXYeB = v1eB.matrix()+ v2eB.matrix();
          double ctauErrBS = sqrt(ROOT::Math::Similarity(vpperp,vXYeB))*3.096916/(pperp.Perp2());
          
          userFloat["ppdlBS"] = ctauBS;
          userFloat["ppdlErrBS"] = ctauErrBS;
          pvtx3D.SetXYZ(theBeamSpotV.position().x(),theBeamSpotV.position().y(),theBeamSpotV.position().z());
          vdiff3D = vtx3D - pvtx3D;
          cosAlpha3D = vdiff3D.Dot(pxyz)/(vdiff3D.Mag()*pxyz.Mag());
          distXYZ = vdistXYZ.distance(Vertex(myVertex), theBeamSpotV);
          double ctauBS3D = distXYZ.value()*cosAlpha3D*3.096916/pxyz.Mag();
          double ctauErrBS3D = sqrt(ROOT::Math::Similarity(vpxyz,vXYeB))*3.096916/(pxyz.Mag2());

          userFloat["ppdlBS3D"] = ctauBS3D;
          userFloat["ppdlErrBS3D"] = ctauErrBS3D;
          
          if (addCommonVertex_) {
            userVertex["commonVertex"] = Vertex(myVertex);
          }
        } else {
          userFloat["vNChi2"] = -1;
          userFloat["vProb"] = -1;
          userFloat["vertexWeight"] = -100;
          userFloat["sumPTPV"] = -100;
          userFloat["DCA"] = -10;
          userFloat["ppdlPV"] = -100;
          userFloat["ppdlErrPV"] = -100;
          userFloat["cosAlpha"] = -10;
          userFloat["ppdlBS"] = -100;
          userFloat["ppdlErrBS"] = -100;
          userFloat["ppdlOrigPV"] = -100;
          userFloat["ppdlErrOrigPV"] = -100;
          userFloat["ppdlPV3D"] = -100;
          userFloat["ppdlErrPV3D"] = -100;
          userFloat["cosAlpha3D"] = -10;
          userFloat["ppdlBS3D"] = -100;
          userFloat["ppdlErrBS3D"] = -100;
          userFloat["ppdlOrigPV3D"] = -100;
          userFloat["ppdlErrOrigPV3D"] = -100;

          userInt["countTksOfPV"] = -1;

          if (addCommonVertex_) {
            userVertex["commonVertex"] = Vertex();
          }
          if (addMuonlessPrimaryVertex_) {
            userVertex["muonlessPV"] = Vertex();
            userVertex["PVwithmuons"] = Vertex();
          } else {
            userVertex["PVwithmuons"] = Vertex();
          }
        }
      } else {
        userFloat["vNChi2"] = -1;
        userFloat["vProb"] = -1;
        userFloat["vertexWeight"] = -100;
        userFloat["sumPTPV"] = -100;
        userFloat["DCA"] = -10;
        userFloat["ppdlPV"] = -100;
        userFloat["ppdlErrPV"] = -100;
        userFloat["cosAlpha"] = -10;
        userFloat["ppdlBS"] = -100;
        userFloat["ppdlErrBS"] = -100;
        userFloat["ppdlOrigPV"] = -100;
        userFloat["ppdlErrOrigPV"] = -100;
        userFloat["ppdlPV3D"] = -100;
        userFloat["ppdlErrPV3D"] = -100;
        userFloat["cosAlpha3D"] = -10;
        userFloat["ppdlBS3D"] = -100;
        userFloat["ppdlErrBS3D"] = -100;
        userFloat["ppdlOrigPV3D"] = -100;
        userFloat["ppdlErrOrigPV3D"] = -100;
        
        userInt["countTksOfPV"] = -1;
        
        if (addCommonVertex_) {
          userVertex["commonVertex"] = Vertex();
        }
        if (addMuonlessPrimaryVertex_) {
          userVertex["muonlessPV"] = Vertex();
          userVertex["PVwithmuons"] = Vertex();
        } else {
          userVertex["PVwithmuons"] = Vertex();
        }
      }

      userInt["Ntrk"] = Ntrk;

      for (std::map<std::string, int>::iterator i = userInt.begin(); i != userInt.end(); i++) { myCand.addUserInt(i->first , i->second); }
      for (std::map<std::string, float>::iterator i = userFloat.begin(); i != userFloat.end(); i++) { myCand.addUserFloat(i->first , i->second); }
      for (std::map<std::string, reco::Vertex>::iterator i = userVertex.begin(); i != userVertex.end(); i++) { myCand.addUserData(i->first , i->second); }

      if(!LateDimuonSel_(myCand)) continue;
      // ---- Push back output ----  
      oniaOutput->push_back(myCand);
      //**********************************************************
      
    TrimuonCand:
      if(!doTrimuons_) continue;
      // ---- Create all trimuon combinations (Bc candidates) ----
      for(int k=j+1; k<ourMuNb; k++){
      	const pat::Muon& it3 = ourMuons[k];
    	// Two must pass tight quality  (includes |eta|<2.4)
    	if (!( (higherPuritySelection_(it) && higherPuritySelection_(it2))
	       || (higherPuritySelection_(it) && higherPuritySelection_(it3))
	       || (higherPuritySelection_(it2) && higherPuritySelection_(it3)) )) continue;

    	pat::CompositeCandidate BcCand;
    	// ---- no explicit order defined ----
    	BcCand.addDaughter(it, "muon1");
    	BcCand.addDaughter(it2,"muon2"); 
    	BcCand.addDaughter(it3,"muon3"); 
 
    	// ---- define and set candidate's 4momentum  ----  
    	LorentzVector bc = it.p4() + it2.p4() + it3.p4();
    	BcCand.setP4(bc);
    	BcCand.setCharge(it.charge()+it2.charge()+it3.charge());

    	std::map< std::string, int > userBcInt;
    	std::map< std::string, float > userBcFloat;
    	std::map< std::string, reco::Vertex > userBcVertex;

    	// ---- Redefine the three possible Jpsi's, to apply dimuon cuts to one of them ----
    	pat::CompositeCandidate myCand2;
    	myCand2.addDaughter(it,"muon1"); 
    	myCand2.addDaughter(it3,"muon2"); 
    	LorentzVector jpsi2 = it.p4() + it3.p4();
    	myCand2.setP4(jpsi2);

    	pat::CompositeCandidate myCand3;
    	myCand3.addDaughter(it2,"muon1"); 
    	myCand3.addDaughter(it3,"muon2"); 
    	LorentzVector jpsi3 = it2.p4() + it3.p4();
    	myCand3.setP4(jpsi3);

	// ---- apply the trimuon cut ----
	if(!( trimuonSelection_(BcCand)
	      && (dimuonSelection_(myCand) || dimuonSelection_(myCand2) || dimuonSelection_(myCand3)) )
	   ) continue;
	
    	// ---- fit vertex using Tracker tracks (if they have tracks) ----
    	if (it.track().isNonnull() && it2.track().isNonnull() && it3.track().isNonnull()) {
	  
    	  //build the trimuon secondary vertex
	  
    	  vector<TransientTrack> t_tks;
    	  t_tks.push_back(theTTBuilder->build(*it.track()));  // pass the reco::Track, not  the reco::TrackRef (which can be transient)
    	  t_tks.push_back(theTTBuilder->build(*it2.track())); // otherwise the vertex will have transient refs inside.
    	  t_tks.push_back(theTTBuilder->build(*it3.track()));
    	  TransientVertex TrimuVertex = vtxFitter.vertex(t_tks);
	  
    	  CachingVertex<5> VtxForInvMass = vtxFitter.vertex( t_tks );
    	  Measurement1D MassWErr = massCalculator.invariantMass( VtxForInvMass, muMasses3 );
    	  userBcFloat["MassErr"] = MassWErr.error();

    	  if (TrimuVertex.isValid()) {
    	    float vChi2 = TrimuVertex.totalChiSquared();
    	    float vNDF  = TrimuVertex.degreesOfFreedom();
    	    float vProb(TMath::Prob(vChi2,(int)vNDF));

    	    Bcnb+=1;
    	    
	    userBcFloat["vNChi2"] = (vChi2/vNDF);
    	    userBcFloat["vProb"] = vProb;

    	    TVector3 vtx, vtx3D;
    	    TVector3 pvtx, pvtx3D;
    	    VertexDistanceXY vdistXY;
    	    VertexDistance3D vdistXYZ;

    	    vtx.SetXYZ(TrimuVertex.position().x(),TrimuVertex.position().y(),0);
    	    TVector3 pperp(bc.px(), bc.py(), 0);
    	    AlgebraicVector3 vpperp(pperp.x(), pperp.y(), 0.);

    	    vtx3D.SetXYZ(TrimuVertex.position().x(),TrimuVertex.position().y(),TrimuVertex.position().z());
    	    TVector3 pxyz(bc.px(), bc.py(), bc.pz());
    	    AlgebraicVector3 vpxyz(pxyz.x(), pxyz.y(), pxyz.z());

    	    //The "resolvePileUpAmbiguity" (looking for the PV that is the closest in z to the displaced vertex) has already been done with the dimuon, we keep this PV as such
    	    Vertex thePrimaryV = theOriginalPV;

    	    muonLess.clear();
    	    muonLess.reserve(thePrimaryV.tracksSize());
    	    if( addMuonlessPrimaryVertex_ && thePrimaryV.tracksSize()>2) {
    	      // Primary vertex matched to the dimuon, now refit it removing the three muons
    	      // I need to go back to the reco::Muon object, as the TrackRef in the pat::Muon can be an embedded ref.
    	      const reco::Muon *rmu1 = dynamic_cast<const reco::Muon *>(it.originalObject());
    	      const reco::Muon *rmu2 = dynamic_cast<const reco::Muon *>(it2.originalObject());
    	      const reco::Muon *rmu3 = dynamic_cast<const reco::Muon *>(it3.originalObject());
    	      if( thePrimaryV.hasRefittedTracks() ) {
    		// Need to go back to the original tracks before taking the key
    		std::vector<reco::Track>::const_iterator itRefittedTrack   = thePrimaryV.refittedTracks().begin();
    		std::vector<reco::Track>::const_iterator refittedTracksEnd = thePrimaryV.refittedTracks().end();
    		for( ; itRefittedTrack != refittedTracksEnd; ++itRefittedTrack ) {
    		  if( thePrimaryV.originalTrack(*itRefittedTrack).key() == rmu1->track().key() ) continue;
    		  if( thePrimaryV.originalTrack(*itRefittedTrack).key() == rmu2->track().key() ) continue;
    		  if( thePrimaryV.originalTrack(*itRefittedTrack).key() == rmu3->track().key() ) continue;

    		  const reco::Track & recoTrack = *(thePrimaryV.originalTrack(*itRefittedTrack));
    		  muonLess.push_back(recoTrack);
    		}
    	      }// PV has refitted tracks
    	      else 
    		{
    		  std::vector<reco::TrackBaseRef>::const_iterator itPVtrack = thePrimaryV.tracks_begin();
    		  for( ; itPVtrack != thePrimaryV.tracks_end(); ++itPVtrack ) if (itPVtrack->isNonnull()) {
    		      if( itPVtrack->key() == rmu1->track().key() ) continue;
    		      if( itPVtrack->key() == rmu2->track().key() ) continue;
    		      if( itPVtrack->key() == rmu3->track().key() ) continue;
    		      muonLess.push_back(**itPVtrack);
    		    }
    		}// take all tracks associated with the vtx

    	      if (muonLess.size()>1 && muonLess.size() < thePrimaryV.tracksSize()) {
    		// find the new vertex, from which the 2 muons were removed
    		// need the transient tracks corresponding to the new track collection
    		std::vector<reco::TransientTrack> t_tks; 
    		t_tks.reserve(muonLess.size());

    		for (reco::TrackCollection::const_iterator it = muonLess.begin(), ed = muonLess.end(); it != ed; ++it) {
    		  t_tks.push_back((*theTTBuilder).build(*it));
    		  t_tks.back().setBeamSpot(bs);
    		}
    		AdaptiveVertexFitter* theFitter=new AdaptiveVertexFitter();
    		TransientVertex pvs = theFitter->vertex(t_tks, bs);  // if you want the beam constraint

    		if (pvs.isValid()) {
    		  reco::Vertex muonLessPV = Vertex(pvs);
    		  thePrimaryV = muonLessPV;
    		} else {
    		  edm::LogWarning("HiOnia2MuMuPAT_FailingToRefitMuonLessVtx") << 
    		    "TransientVertex re-fitted is not valid!! You got still the 'old vertex'" << "\n";
    		}
    	      } else {
    		if ( muonLess.size()==thePrimaryV.tracksSize() ){
    		  //edm::LogWarning("HiOnia2MuMuPAT_muonLessSizeORpvTrkSize") << 
    		  //  "Still have the original PV: the refit was not done 'cose it is already muonless" << "\n";
    		} else if ( muonLess.size()<=1 ){
    		  //edm::LogWarning("HiOnia2MuMuPAT_muonLessSizeORpvTrkSize") << 
    		  //  "Still have the original PV: the refit was not done 'cose there are not enough tracks to do the refit without the muon tracks" << "\n";
    		} else {
    		  edm::LogWarning("HiOnia2MuMuPAT_muonLessSizeORpvTrkSize") << 
    		    "Still have the original PV: Something weird just happened, muonLess.size()=" << muonLess.size() << " and thePrimaryV.tracksSize()=" << thePrimaryV.tracksSize() << " ." << "\n";
    		}
    	      }
    	    }// refit vtx without the muon tracks


    	    // count the number of high Purity tracks with pT > 900 MeV attached to the chosen vertex      
    	    // this makes sense only in case of pp reconstruction
    	    double vertexWeight = -1., sumPTPV = -1.;
    	    int countTksOfPV = -1;
         
    	    EDConsumerBase::Labels thePVsLabel;
    	    EDConsumerBase::labelsForToken(thePVsToken_, thePVsLabel);
    	    if(thePVsLabel.module==(std::string)("offlinePrimaryVertices")) {
    	      const reco::Muon *rmu1 = dynamic_cast<const reco::Muon *>(it.originalObject());
    	      const reco::Muon *rmu2 = dynamic_cast<const reco::Muon *>(it2.originalObject());
    	      const reco::Muon *rmu3 = dynamic_cast<const reco::Muon *>(it3.originalObject());
    	      try {
    	    	for(reco::Vertex::trackRef_iterator itVtx = theOriginalPV.tracks_begin(); itVtx != theOriginalPV.tracks_end(); itVtx++) if(itVtx->isNonnull()) {

    	    	    const reco::Track& track = **itVtx;
    	    	    if(!track.quality(reco::TrackBase::highPurity)) continue;
    	    	    if(track.pt() < 0.5) continue; //reject all rejects from counting if less than 900 MeV

    	    	    TransientTrack tt = theTTBuilder->build(track);
    	    	    pair<bool,Measurement1D> tkPVdist = IPTools::absoluteImpactParameter3D(tt,theOriginalPV);

    	    	    if (!tkPVdist.first) continue;
    	    	    if (tkPVdist.second.significance()>3) continue;
    	    	    if (track.ptError()/track.pt()>0.1) continue;

    	    	    // do not count the three muons
    	    	    if (rmu1 != 0 && rmu1->innerTrack().key() == itVtx->key()) continue;
    	    	    if (rmu2 != 0 && rmu2->innerTrack().key() == itVtx->key()) continue;
    	    	    if (rmu3 != 0 && rmu3->innerTrack().key() == itVtx->key()) continue;

    	    	    vertexWeight += theOriginalPV.trackWeight(*itVtx);
    	    	    if(theOriginalPV.trackWeight(*itVtx) > 0.5){
    	    	      countTksOfPV++;
    	    	      sumPTPV += track.pt();
    	    	    }
    	    	  }
    	      } catch (std::exception & err) {std::cout << " Counting tracks from PV, fails! " << std::endl; return ; }
    	    }
    	    userBcInt["countTksOfPV"] = countTksOfPV;
    	    userBcFloat["vertexWeight"] = (float) vertexWeight;
    	    userBcFloat["sumPTPV"] = (float) sumPTPV;
         
    	    if (addMuonlessPrimaryVertex_) {
    	      userBcVertex["muonlessPV"] = thePrimaryV;
    	      userBcVertex["PVwithmuons"] = theOriginalPV;
    	    } else {
    	      userBcVertex["PVwithmuons"] = thePrimaryV;
    	    }

    	    // lifetime using PV
    	    pvtx.SetXYZ(thePrimaryV.position().x(),thePrimaryV.position().y(),0);
    	    TVector3 vdiff = vtx - pvtx;
    	    double cosAlpha = vdiff.Dot(pperp)/(vdiff.Perp()*pperp.Perp());
    	    Measurement1D distXY = vdistXY.distance(Vertex(TrimuVertex), thePrimaryV);
    	    double ctauPV = distXY.value()*cosAlpha*6.276/pperp.Perp();
    	    GlobalError v1e = (Vertex(TrimuVertex)).error();
    	    GlobalError v2e = thePrimaryV.error();
    	    AlgebraicSymMatrix33 vXYe = v1e.matrix()+ v2e.matrix();
    	    double ctauErrPV = sqrt(ROOT::Math::Similarity(vpperp,vXYe))*6.276/(pperp.Perp2());

    	    userBcFloat["ppdlPV"] = ctauPV;
    	    userBcFloat["ppdlErrPV"] = ctauErrPV;
    	    userBcFloat["cosAlpha"] = cosAlpha;

    	    pvtx3D.SetXYZ(thePrimaryV.position().x(),thePrimaryV.position().y(),thePrimaryV.position().z());
    	    TVector3 vdiff3D = vtx3D - pvtx3D;
    	    double cosAlpha3D = vdiff3D.Dot(pxyz)/(vdiff3D.Mag()*pxyz.Mag());
    	    Measurement1D distXYZ = vdistXYZ.distance(Vertex(TrimuVertex), thePrimaryV);
    	    double ctauPV3D = distXYZ.value()*cosAlpha3D*6.276/pxyz.Mag();
    	    double ctauErrPV3D = sqrt(ROOT::Math::Similarity(vpxyz,vXYe))*6.276/(pxyz.Mag2());
          
    	    userBcFloat["ppdlPV3D"] = ctauPV3D;
    	    userBcFloat["ppdlErrPV3D"] = ctauErrPV3D;
    	    userBcFloat["cosAlpha3D"] = cosAlpha3D;

	    // if (addMuonlessPrimaryVertex_) {
	    //   // lifetime using Original PV
	    //   pvtx.SetXYZ(theOriginalPV.position().x(),theOriginalPV.position().y(),0);
	    //   vdiff = vtx - pvtx;
	    //   double cosAlphaOrigPV = vdiff.Dot(pperp)/(vdiff.Perp()*pperp.Perp());
	    //   distXY = vdistXY.distance(Vertex(TrimuVertex), theOriginalPV);
	    //   double ctauOrigPV = distXY.value()*cosAlphaOrigPV*6.276/pperp.Perp();
	    //   GlobalError v1eOrigPV = (Vertex(TrimuVertex)).error();
	    //   GlobalError v2eOrigPV = theOriginalPV.error();
	    //   AlgebraicSymMatrix33 vXYeOrigPV = v1eOrigPV.matrix()+ v2eOrigPV.matrix();
	    //   double ctauErrOrigPV = sqrt(ROOT::Math::Similarity(vpperp,vXYeOrigPV))*6.276/(pperp.Perp2());

	    //   userBcFloat["ppdlOrigPV"] = ctauOrigPV;
	    //   userBcFloat["ppdlErrOrigPV"] = ctauErrOrigPV;

	    //   pvtx3D.SetXYZ(theOriginalPV.position().x(), theOriginalPV.position().y(), theOriginalPV.position().z());
	    //   vdiff3D = vtx3D - pvtx3D;
	    //   double cosAlphaOrigPV3D = vdiff3D.Dot(pxyz)/(vdiff3D.Mag()*pxyz.Mag());
	    //   distXYZ = vdistXYZ.distance(Vertex(TrimuVertex), theOriginalPV);
	    //   double ctauOrigPV3D = distXYZ.value()*cosAlphaOrigPV3D*6.276/pxyz.Mag();
	    //   double ctauErrOrigPV3D = sqrt(ROOT::Math::Similarity(vpxyz,vXYeOrigPV))*6.276/(pxyz.Mag2());
          
	    //   userBcFloat["ppdlOrigPV3D"] = ctauOrigPV3D;
	    //   userBcFloat["ppdlErrOrigPV3D"] = ctauErrOrigPV3D;
	    // }
	    // else{
	    //   userBcFloat["ppdlOrigPV"] = ctauPV;
	    //   userBcFloat["ppdlErrOrigPV"] = ctauErrPV;
	    //   userBcFloat["ppdlOrigPV3D"] = ctauPV3D;
	    //   userBcFloat["ppdlErrOrigPV3D"] = ctauErrPV3D;	      
	    // }

    	    // // lifetime using BS
    	    // pvtx.SetXYZ(theBeamSpotV.position().x(),theBeamSpotV.position().y(),0);
    	    // vdiff = vtx - pvtx;
    	    // cosAlpha = vdiff.Dot(pperp)/(vdiff.Perp()*pperp.Perp());
    	    // distXY = vdistXY.distance(Vertex(TrimuVertex), theBeamSpotV);
    	    // double ctauBS = distXY.value()*cosAlpha*6.276/pperp.Perp();
    	    // GlobalError v1eB = (Vertex(TrimuVertex)).error();
    	    // GlobalError v2eB = theBeamSpotV.error();
    	    // AlgebraicSymMatrix33 vXYeB = v1eB.matrix()+ v2eB.matrix();
    	    // double ctauErrBS = sqrt(ROOT::Math::Similarity(vpperp,vXYeB))*6.276/(pperp.Perp2());
          
    	    // userBcFloat["ppdlBS"] = ctauBS;
    	    // userBcFloat["ppdlErrBS"] = ctauErrBS;
    	    // pvtx3D.SetXYZ(theBeamSpotV.position().x(),theBeamSpotV.position().y(),theBeamSpotV.position().z());
    	    // vdiff3D = vtx3D - pvtx3D;
    	    // cosAlpha3D = vdiff3D.Dot(pxyz)/(vdiff3D.Mag()*pxyz.Mag());
    	    // distXYZ = vdistXYZ.distance(Vertex(TrimuVertex), theBeamSpotV);
    	    // double ctauBS3D = distXYZ.value()*cosAlpha3D*6.276/pxyz.Mag();
    	    // double ctauErrBS3D = sqrt(ROOT::Math::Similarity(vpxyz,vXYeB))*6.276/(pxyz.Mag2());

    	    // userBcFloat["ppdlBS3D"] = ctauBS3D;
    	    // userBcFloat["ppdlErrBS3D"] = ctauErrBS3D;
          
    	    if (addCommonVertex_) {
    	      userBcVertex["commonVertex"] = Vertex(TrimuVertex);
    	    }

    	  } else {
    	    userBcFloat["vNChi2"] = -1;
    	    userBcFloat["vProb"] = -1;
    	    userBcFloat["vertexWeight"] = -100;
    	    userBcFloat["sumPTPV"] = -100;
    	    userBcFloat["ppdlPV"] = -100;
    	    userBcFloat["ppdlErrPV"] = -100;
    	    userBcFloat["cosAlpha"] = -100;
    	    userBcFloat["ppdlBS"] = -100;
    	    userBcFloat["ppdlErrBS"] = -100;
    	    userBcFloat["ppdlOrigPV"] = -100;
    	    userBcFloat["ppdlErrOrigPV"] = -100;
    	    userBcFloat["ppdlPV3D"] = -100;
    	    userBcFloat["ppdlErrPV3D"] = -100;
    	    userBcFloat["cosAlpha3D"] = -100;
    	    userBcFloat["ppdlBS3D"] = -100;
    	    userBcFloat["ppdlErrBS3D"] = -100;
    	    userBcFloat["ppdlOrigPV3D"] = -100;
    	    userBcFloat["ppdlErrOrigPV3D"] = -100;

    	    userBcInt["countTksOfPV"] = -1;

    	    if (addCommonVertex_) {
    	      userBcVertex["commonVertex"] = Vertex();
    	    }
    	    if (addMuonlessPrimaryVertex_) {
    	      userBcVertex["muonlessPV"] = Vertex();
    	      userBcVertex["PVwithmuons"] = Vertex();
    	    } else {
    	      userBcVertex["PVwithmuons"] = Vertex();
    	    } 
    	  }
    	} else {
    	  userBcFloat["vNChi2"] = -1;
    	  userBcFloat["vProb"] = -1;
    	  userBcFloat["vertexWeight"] = -100;
    	  userBcFloat["sumPTPV"] = -100;
    	  userBcFloat["ppdlPV"] = -100;
    	  userBcFloat["ppdlErrPV"] = -100;
    	  userBcFloat["cosAlpha"] = -100;
    	  userBcFloat["ppdlBS"] = -100;
    	  userBcFloat["ppdlErrBS"] = -100;
    	  userBcFloat["ppdlOrigPV"] = -100;
    	  userBcFloat["ppdlErrOrigPV"] = -100;
    	  userBcFloat["ppdlPV3D"] = -100;
    	  userBcFloat["ppdlErrPV3D"] = -100;
    	  userBcFloat["cosAlpha3D"] = -100;
    	  userBcFloat["ppdlBS3D"] = -100;
    	  userBcFloat["ppdlErrBS3D"] = -100;
    	  userBcFloat["ppdlOrigPV3D"] = -100;
    	  userBcFloat["ppdlErrOrigPV3D"] = -100;
        
    	  userBcInt["countTksOfPV"] = -1;
        
    	  if (addCommonVertex_) {
    	    userBcVertex["commonVertex"] = Vertex();
    	  }
    	  if (addMuonlessPrimaryVertex_) {
    	    userBcVertex["muonlessPV"] = Vertex();
    	    userBcVertex["PVwithmuons"] = Vertex();
    	  } else {
    	    userBcVertex["PVwithmuons"] = Vertex();
    	  }
    	}

    	for (std::map<std::string, int>::iterator i = userBcInt.begin(); i != userBcInt.end(); i++) { BcCand.addUserInt(i->first , i->second); }
    	for (std::map<std::string, float>::iterator i = userBcFloat.begin(); i != userBcFloat.end(); i++) { BcCand.addUserFloat(i->first , i->second); }
    	for (std::map<std::string, reco::Vertex>::iterator i = userBcVertex.begin(); i != userBcVertex.end(); i++) { BcCand.addUserData(i->first , i->second); }

	if(!LateTrimuonSel_(BcCand)) continue;	
    	// ---- Push back output ----  
    	trimuOutput->push_back(BcCand);

      }//it3 muon
    }//it2 muon
  }//it muon

  //  std::sort(oniaOutput->begin(),oniaOutput->end(),pTComparator_);
  std::sort(oniaOutput->begin(),oniaOutput->end(),vPComparator_);
  iEvent.put(std::move(oniaOutput),"");

  if(doTrimuons_){
    std::sort(trimuOutput->begin(),trimuOutput->end(),vPComparator_);
    iEvent.put(std::move(trimuOutput),"trimuon");
  }
}

// ------------ method called once each job just before starting event loop  ------------
void 
HiOnia2MuMuPAT::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HiOnia2MuMuPAT::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(HiOnia2MuMuPAT);
