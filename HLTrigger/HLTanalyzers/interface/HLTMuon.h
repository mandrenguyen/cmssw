#ifndef HLTMUON_H
#define HLTMUON_H

#include "TVector3.h"
#include "TTree.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/MuonSeed/interface/L3MuonTrajectorySeed.h"
#include "DataFormats/MuonSeed/interface/L3MuonTrajectorySeedCollection.h"
#include "DataFormats/MuonSeed/interface/L2MuonTrajectorySeed.h" 
#include "DataFormats/MuonSeed/interface/L2MuonTrajectorySeedCollection.h" 
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "HLTrigger/HLTanalyzers/interface/JetUtil.h"


typedef std::vector<std::string> MyStrings;

/** \class HLTMuon
  *  
  * $Date: November 2006
  * $Revision: 
  * \author P. Bargassa - Rice U.
  */
class HLTMuon {
public:
  HLTMuon(); 

  void setup(const edm::ParameterSet& pSet, TTree* tree);

  /** Analyze the Data */
  void analyze(const edm::Event & event,
               const edm::Handle<reco::MuonCollection>                 & muon,
	       const edm::Handle<BXVector<l1t::Muon> >                 & mucands1, 
	       const edm::Handle<reco::RecoChargedCandidateCollection>      & mucands2,
	       const edm::Handle<reco::RecoChargedCandidateCollection>      & mucands3,
               const edm::Handle< std::vector<reco::Track> >                  & L3TkTracksFromL2OIState,
               const edm::Handle< std::vector<reco::Track> >                  & L3TkTracksFromL2OIHit,
               const std::vector< edm::Handle<trigger::TriggerFilterObjectWithRefs> > & muonFilterCollections,
	       const edm::ESHandle<MagneticField> & theMagField,
               const edm::Handle<reco::BeamSpot> & recoBeamSpotHandle,
	       TTree* tree);


private:

  int validChambers(const reco::TrackRef & track);
               
  // Tree variables
  TVector3 hltOnlineBeamSpot;
  float *muonReco_pt, *muonReco_phi, *muonReco_eta, *muonReco_et, *muonReco_e, *muonReco_chi2NDF, *muonReco_D0;
  int   *muonReco_charge, *muonReco_type, *muonReco_NValidTrkHits, *muonReco_NValidMuonHits;
  float *muonL1_pt, *muonL1_eta, *muonL1_phi;
  int   *muonL1_charge, *muonL1_bx, *muonL1_GMTMuonQuality, *muonL1_tfMuonIndex, *muonL1_tfRegion;
  float *muonL2_pt, *muonL2_eta, *muonL2_phi, *muonL2_dr, *muonL2_drsign, *muonL2_dz, *muonL2_vtxz;
  float *muonL2_L1dr;
  float *muonL3_pt, *muonL3_ptLx, *muonL3_eta, *muonL3_phi, *muonL3_dr, *muonL3_dz, *muonL3_vtxz, *muonL3_normchi2; 
  float *muonL3_globalpt, *muonL3_globaleta, *muonL3_globalphi, *muonL3_globalDxy, *muonL3_globalDxySig, *muonL3_globaldz, *muonL3_globalvtxz;
  float *muonL2_pterr, *muonL3_pterr;
  float *muonL3_L2dr, *muonL3_L1dr, *muonL3_TrackL2dr; 
  int nmuonReco, nmuonL1, nmuonL2, nmuonL3, nDiMuonL1, nDiMuonL2, nDiMuonL3, nL3TkL2OIHit, nL3TkL2OIState;
  int *muonL2_charge, *muonL2_iso, *muonL2_nhits, *muonL2_nchambers, *muonL2_nstat, *muonL2_ndtcscstat, *muonL3_charge, *muonL3_iso, *muonL3_L2idx, *muonL3_nhits, *muonL2_L1idx, *muonL3_global2idx, *muonL3_globalchg;
  int *muonL3_npixelhits, *muonL3_ntrackerhits, *muonL3_nmuonhits, *muonL3_trk10iso;

  float *dimuonL1_invmass, *dimuonL1_pt, *dimuonL1_rap, *dimuonL1_dR, *dimuonL1_phi;
  int *dimuonL1_charge, *dimuonL1_Mu1idx, *dimuonL1_Mu2idx;

  float *dimuonL2_invmass, *dimuonL2_pt, *dimuonL2_rap, *dimuonL2_dca, *dimuonL2_dPt, *dimuonL2_dPhi, *dimuonL2_angle;
  int *dimuonL2_charge, *dimuonL2_Mu1idx, *dimuonL2_Mu2idx;

  float *dimuonL3_invmass, *dimuonL3_pt, *dimuonL3_rap, *dimuonL3_dca, *dimuonL3_dPt, *dimuonL3_dPhi;
  int *dimuonL3_charge, *dimuonL3_isCowboy, *dimuonL3_Mu1idx, *dimuonL3_Mu2idx;

  float *L3TkL2OIHit_pt, *L3TkL2OIHit_phi, *L3TkL2OIHit_eta, *L3TkL2OIHit_dr, *L3TkL2OIHit_drError, *L3TkL2OIHit_dz, *L3TkL2OIHit_normchi2;               
  int *L3TkL2OIHit_charge, *L3TkL2OIHit_nhits, *L3TkL2OIHit_npixelhits, *L3TkL2OIHit_ntrackerhits, *L3TkL2OIHit_L3idx, *L3TkL2OIHit_L2idx; 

  float *L3TkL2OIState_pt, *L3TkL2OIState_phi, *L3TkL2OIState_eta, *L3TkL2OIState_dr, *L3TkL2OIState_drError, *L3TkL2OIState_dz, *L3TkL2OIState_normchi2;
  int *L3TkL2OIState_charge, *L3TkL2OIState_nhits, *L3TkL2OIState_npixelhits, *L3TkL2OIState_ntrackerhits, *L3TkL2OIState_L3idx, *L3TkL2OIState_L2idx; 

  ULong64_t  *muonL1_trig, *muonL2_trig, *muonL3_trig, *dimuonL1_trig, *dimuonL2_trig, *dimuonL3_trig; 

  // input variables
  bool _Monte,_Debug;
               
  int evtCounter;
               
  static float etaBarrel() { return 1.4; }

};


#endif
