#ifndef __HIONIA__
#define __HIONIA__
// -*- C++ -*-
//
// Package:    HiOniaAnalyzer
// Class:      HiOniaAnalyzer
//
/**\class HiOniaAnalyzer HiOniaAnalyzer.cc UserCode/tdahms/HiAnalysis/HiOnia/plugins/HiOniaAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/

// system include files
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>

#include <TTree.h>
#include <TLorentzVector.h>
#include <TVector2.h>
#include <TClonesArray.h>
#include <TRegexp.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include <DataFormats/RecoCandidate/interface/RecoChargedCandidate.h>
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "DataFormats/HeavyIonEvent/interface/Centrality.h"
#include "DataFormats/HeavyIonEvent/interface/EvtPlane.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "HiAnalysis/HiOnia/interface/MyCommonHistoManager.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

class HiOniaAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources, edm::one::WatchRuns> {
public:
  explicit HiOniaAnalyzer(const edm::ParameterSet&);
  ~HiOniaAnalyzer() override;

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  void InitEvent();
  void InitTree();

  void makeCuts(bool keepSameSign);
  void makeBcCuts(bool keepWrongSign);
  void makeDimutrkCuts(bool keepWrongSign);
  bool checkCuts(const pat::CompositeCandidate* cand,
                 const pat::Muon* muon1,
                 const pat::Muon* muon2,
                 bool (HiOniaAnalyzer::*callFunc1)(const pat::Muon*),
                 bool (HiOniaAnalyzer::*callFunc2)(const pat::Muon*));
  bool checkBcCuts(const pat::CompositeCandidate* cand,
                   const pat::Muon* muon1,
                   const pat::Muon* muon2,
                   const pat::Muon* muon3,
                   bool (HiOniaAnalyzer::*callFunc1)(const pat::Muon*),
                   bool (HiOniaAnalyzer::*callFunc2)(const pat::Muon*),
                   bool (HiOniaAnalyzer::*callFunc3)(const pat::Muon*));
  bool checkDimuTrkCuts(const pat::CompositeCandidate* cand,
                        const pat::Muon* muon1,
                        const pat::Muon* muon2,
                        const reco::RecoChargedCandidate* trk,
                        bool (HiOniaAnalyzer::*callFunc1)(const pat::Muon*),
                        bool (HiOniaAnalyzer::*callFunc2)(const pat::Muon*),
                        bool (HiOniaAnalyzer::*callFunc3)(const reco::TrackRef));

  reco::GenParticleRef findDaughterRef(reco::GenParticleRef GenParticleDaughter, int GenParticlePDG);
  int IndexOfThisMuon(TLorentzVector* v1, bool isGen = false);
  int IndexOfThisTrack(TLorentzVector* v1, bool isGen = false);
  int IndexOfThisJpsi(int mu1_idx, int mu2_idx, int flipJpsi = 0);
  void fillGenInfo();
  void fillMuMatchingInfo();
  void fillQQMatchingInfo();
  void fillBcMatchingInfo();
  bool isAbHadron(int pdgID);
  bool isNeutrino(int pdgID);
  bool isAMixedbHadron(int pdgID, int momPdgID);
  std::pair<bool, reco::GenParticleRef> findBcMotherRef(reco::GenParticleRef GenParticleMother, int GenParticlePDG);
  bool isChargedTrack(int pdgId);
  std::vector<reco::GenParticleRef> GenBrothers(reco::GenParticleRef GenParticleMother, int GenJpsiPDG);
  reco::GenParticleRef findMotherRef(reco::GenParticleRef GenParticleMother, int GenParticlePDG);
  std::pair<std::vector<reco::GenParticleRef>, std::pair<float, float> > findGenMCInfo(const reco::GenParticle* genJpsi);
  std::pair<int, std::pair<float, float> > findGenBcInfo(reco::GenParticleRef genBc, const reco::GenParticle* genJpsi);

  void fillRecoMuons(int theCentralityBin);
  bool isMuonInAccept(const pat::Muon* aMuon, std::string muonType);
  bool isTrkInMuonAccept(TLorentzVector trk4mom, std::string muonType);

  bool isSoftMuonBase(const pat::Muon* aMuon);
  bool isHybridSoftMuon(const pat::Muon* aMuon);
  Short_t MuInSV(TLorentzVector v1, TLorentzVector v2, TLorentzVector v3);

  void fillRecoTracks();

  pair<unsigned int, const pat::CompositeCandidate*> theBestQQ();
  double CorrectMass(const reco::Muon& mu1, const reco::Muon& mu2, int mode);

  bool selGlobalMuon(const pat::Muon* aMuon);
  bool selTrackerMuon(const pat::Muon* aMuon);
  bool selGlobalOrTrackerMuon(const pat::Muon* aMuon);
  bool selAllMuon(const pat::Muon* aMuon);
  bool selTrk(const reco::TrackRef aTrk);
  bool PassMiniAODcut(const pat::Muon* aMuon);

  void fillRecoHistos();
  void fillRecoJpsi(int count, std::string trigName, std::string centName);
  void fillHistosAndDS(unsigned int theCat, const pat::CompositeCandidate* aJpsiCand);

  void fillTreeMuon(const pat::Muon* muon, int iType, ULong64_t trigBits);
  void fillTreeJpsi(int count);
  void fillTreeBc(int count);
  void fillTreeDimuTrk(int count);

  void checkTriggers(const pat::CompositeCandidate* aJpsiCand);
  void hltReport(const edm::Event& iEvent, const edm::EventSetup& iSetup);

  long int FloatToIntkey(float v);
  void beginRun(const edm::Run&, const edm::EventSetup&) override;
  void endRun(const edm::Run&, const edm::EventSetup&) override{};

  TLorentzVector lorentzMomentum(const reco::Candidate::LorentzVector& p);
  int muonIDmask(const pat::Muon* muon);

  // ----------member data ---------------------------
  enum StatBins { BIN_nEvents = 0 };

  enum dimuonCategories {
    GlbTrk_GlbTrk = 0,
    Glb_Glb = 1,
    Trk_Trk = 2,
    GlbOrTrk_GlbOrTrk = 3,
    TwoGlbAmongThree = 4,
    All_All = 5
  };

  enum muonCategories { GlbTrk = 0, Trk = 1, Glb = 2, GlbOrTrk = 3, All = 4 };

  std::vector<std::string> theRegions;
  std::vector<std::string> theCentralities;
  std::vector<std::string> theTriggerNames;
  std::vector<std::string> theSign;
  std::map<std::string, std::string> triggerNameMap;
  std::map<std::string, std::string> filterNameMap;

  HLTConfigProvider hltConfig;
  bool hltConfigInit;

  float etaMin;
  float etaMax;

  // TFileService
  edm::Service<TFileService> fs;

  // // TFile
  // TFile* fOut;

  // TTree
  TTree* myTree;

  TClonesArray* Reco_mu_4mom;
  TClonesArray* Reco_mu_L1_4mom;
  TClonesArray* Reco_QQ_4mom;
  TClonesArray* Reco_QQ_mumi_4mom;
  TClonesArray* Reco_QQ_mupl_4mom;
  TClonesArray* Reco_3mu_4mom;
  TClonesArray* Reco_QQ_vtx;
  TClonesArray* Reco_3mu_vtx;
  TClonesArray* Reco_trk_4mom;
  TClonesArray* Reco_trk_vtx;
  TClonesArray* Gen_Bc_4mom;
  TClonesArray* Gen_Bc_nuW_4mom;
  TClonesArray* Gen_3mu_4mom;

  TClonesArray* Gen_mu_4mom;
  TClonesArray* Gen_QQ_4mom;

  std::vector<float> Reco_mu_4mom_pt;
  std::vector<float> Reco_mu_L1_4mom_pt;
  std::vector<float> Reco_QQ_4mom_pt;
  std::vector<float> Reco_QQ_mumi_4mom_pt;
  std::vector<float> Reco_QQ_mupl_4mom_pt;
  std::vector<float> Reco_3mu_4mom_pt;
  std::vector<float> Reco_trk_4mom_pt;
  std::vector<float> Gen_Bc_4mom_pt;
  std::vector<float> Gen_Bc_nuW_4mom_pt;
  std::vector<float> Gen_3mu_4mom_pt;

  std::vector<float> Gen_mu_4mom_pt;
  std::vector<float> Gen_QQ_4mom_pt;

  std::vector<float> Reco_mu_4mom_eta;
  std::vector<float> Reco_mu_L1_4mom_eta;
  std::vector<float> Reco_QQ_4mom_eta;
  std::vector<float> Reco_QQ_mumi_4mom_eta;
  std::vector<float> Reco_QQ_mupl_4mom_eta;
  std::vector<float> Reco_3mu_4mom_eta;
  std::vector<float> Reco_trk_4mom_eta;
  std::vector<float> Gen_Bc_4mom_eta;
  std::vector<float> Gen_Bc_nuW_4mom_eta;
  std::vector<float> Gen_3mu_4mom_eta;

  std::vector<float> Gen_mu_4mom_eta;
  std::vector<float> Gen_QQ_4mom_eta;

  std::vector<float> Gen_QQ_4mom_y;
  std::vector<float> Reco_QQ_4mom_y;
  std::vector<float> Gen_Bc_4mom_y;
  std::vector<float> Gen_Bc_nuW_4mom_y;
  std::vector<float> Gen_3mu_4mom_y;
  std::vector<float> Reco_3mu_4mom_y;

  std::vector<float> Reco_mu_4mom_phi;
  std::vector<float> Reco_mu_L1_4mom_phi;
  std::vector<float> Reco_QQ_4mom_phi;
  std::vector<float> Reco_QQ_mumi_4mom_phi;
  std::vector<float> Reco_QQ_mupl_4mom_phi;
  std::vector<float> Reco_3mu_4mom_phi;
  std::vector<float> Reco_trk_4mom_phi;
  std::vector<float> Gen_Bc_4mom_phi;
  std::vector<float> Gen_Bc_nuW_4mom_phi;
  std::vector<float> Gen_3mu_4mom_phi;

  std::vector<float> Gen_mu_4mom_phi;
  std::vector<float> Gen_QQ_4mom_phi;

  std::vector<float> Reco_mu_4mom_m;
  std::vector<float> Reco_mu_L1_4mom_m;
  std::vector<float> Reco_QQ_4mom_m;
  std::vector<float> Reco_QQ_mumi_4mom_m;
  std::vector<float> Reco_QQ_mupl_4mom_m;
  std::vector<float> Reco_3mu_4mom_m;
  std::vector<float> Reco_trk_4mom_m;
  std::vector<float> Gen_Bc_4mom_m;
  std::vector<float> Gen_Bc_nuW_4mom_m;
  std::vector<float> Gen_3mu_4mom_m;

  std::vector<float> Gen_mu_4mom_m;
  std::vector<float> Gen_QQ_4mom_m;

  static const int Max_QQ_size = 10000;
  static const int Max_Bc_size = 10000;
  static const int Max_mu_size = 1000;
  static const int Max_trk_size = 10000;

  float Gen_weight;  // generator weight
  float Gen_pthat;   // ptHat scale of generated hard scattering

  Short_t Gen_QQ_size;               // number of generated Onia
  Short_t Gen_QQ_type[Max_QQ_size];  // Onia type: prompt, non-prompt, unmatched
  float Gen_QQ_ctau[Max_QQ_size];    // ctau: flight time
  float Gen_QQ_ctau3D[Max_QQ_size];  // ctau3D: 3D flight time
  int Gen_QQ_momId
      [Max_QQ_size];  // PDG ID of the generated mother of the Gen QQ, going back far enough in the geneaology to find a potential B mother
  float Gen_QQ_momPt[Max_QQ_size];       // Pt of mother particle of 2 muons
  Short_t Gen_QQ_mupl_idx[Max_QQ_size];  // index of the muon plus from Jpsi, in the full list of muons
  Short_t Gen_QQ_mumi_idx[Max_QQ_size];  // index of the muon minus from Jpsi, in the full list of muons
  Short_t Gen_QQ_whichRec
      [Max_QQ_size];  // index of the reconstructed Jpsi that was matched with this gen Jpsi. Is -1 if one of the 2 muons from Jpsi was not reconstructed. Is -2 if the two muons were reconstructed, but the dimuon was not selected
  Short_t Gen_QQ_Bc_idx[Max_QQ_size];  //Index of the Bc gen mother. -1 if there is no Bc mother

  Short_t Gen_Bc_size;                  // number of generated Bc
  float Gen_Bc_ctau[Max_Bc_size];       // ctau: flight time
  int Gen_Bc_pdgId[Max_Bc_size];        // pdgId of Bc
  Short_t Gen_Bc_QQ_idx[Max_Bc_size];   //Points to the number of the associated Jpsi daughter
  Short_t Gen_Bc_muW_idx[Max_Bc_size];  // index of the muon from W from Bc, in the full list of muons
  Short_t Gen_3mu_whichRec
      [Max_Bc_size];  // index of the reconstructed trimuon that was matched with this gen visible Bc. Is -1 if one muon of the Bc was not reconstructed

  Short_t Gen_mu_size;                 // number of generated muons
  Short_t Gen_mu_charge[Max_mu_size];  // muon charge
  Short_t Gen_mu_type[Max_mu_size];    // muon type: prompt, non-prompt, unmatched
  Short_t Gen_mu_whichRec
      [Max_mu_size];  // index of the reconstructed muon that was matched with this gen muon. Is -1 if the muon was not reconstructed
  float Gen_mu_MatchDeltaR[Max_mu_size];  // deltaR between reco and gen matched muons

  Short_t Reco_3mu_size;  // Number of reconstructed trimuons
  Short_t Reco_3mu_charge[Max_Bc_size];
  Short_t Reco_3mu_mupl_idx[Max_Bc_size];  // index of the muon plus from Jpsi, in the full list of muons
  Short_t Reco_3mu_mumi_idx[Max_Bc_size];  // index of the muon minus from Jpsi, in the full list of muons
  Short_t Reco_3mu_muW_idx[Max_Bc_size];   // index of the muon from W, in the full list of muons
  Short_t Reco_3mu_muW2_idx[Max_Bc_size];  // index of the muon from W, for the second possible OS dimuon
  //  Short_t Reco_3mu_muW_trkIdx[Max_Bc_size];    // index of the generated track closest to the reco muon from W, when the Jpsi is true and the muon is fake
  Short_t Reco_3mu_QQ1_idx[Max_Bc_size];   // index of a possible Jpsi from Bc, 1st of the two OS muon pairs
  Short_t Reco_3mu_QQ2_idx[Max_Bc_size];   // index of a possible Jpsi from Bc, 2nd of the two OS muon pairs
  Short_t Reco_3mu_QQss_idx[Max_Bc_size];  // index of a the same-sign muon pair from Bc
  Short_t Reco_3mu_whichGen
      [Max_Bc_size];  // index of the generated Bc that was matched with this rec Bc. Is -1 if one or more of the 3 muons from Bc was not reconstructed
  bool Reco_3mu_muW_isGenJpsiBro
      [Max_Bc_size];  // is this true or fake muon matched to a generated particle that is brother/nephew to the gen Jpsi (daughter of the gen B)
  int Reco_3mu_muW_trueId[Max_Bc_size];   //pdgId of the generated particle that the reco_muW is matched to
  float Reco_3mu_VtxProb[Max_Bc_size];    // chi2 probability of vertex fitting
  float Reco_3mu_KCVtxProb[Max_Bc_size];  // chi2 probability of kinematic constrained vertex fitting
  float Reco_3mu_ctau[Max_Bc_size];       // ctau: flight time
  float Reco_3mu_ctauErr[Max_Bc_size];    // error on ctau
  float Reco_3mu_cosAlpha
      [Max_QQ_size];  // cosine of angle between momentum of Bc and direction of PV--displaced vertex segment (in XY plane)
  float Reco_3mu_ctau3D[Max_Bc_size];     // ctau: flight time in 3D
  float Reco_3mu_ctauErr3D[Max_Bc_size];  // error on ctau in 3D
  float Reco_3mu_cosAlpha3D
      [Max_QQ_size];  // cosine of angle between momentum of Bc and direction of PV--displaced vertex segment (3D)
  float Reco_3mu_KCctau[Max_Bc_size];     // ctau: flight time
  float Reco_3mu_KCctauErr[Max_Bc_size];  // error on ctau
  float Reco_3mu_KCcosAlpha
      [Max_QQ_size];  // cosine of angle between momentum of Bc and direction of PV--displaced vertex segment (in XY plane)
  float Reco_3mu_KCctau3D[Max_Bc_size];     // ctau: flight time in 3D
  float Reco_3mu_KCctauErr3D[Max_Bc_size];  // error on ctau in 3D
  float Reco_3mu_KCcosAlpha3D
      [Max_QQ_size];  // cosine of angle between momentum of Bc and direction of PV--displaced vertex segment (3D)
  float Reco_3mu_MassErr[Max_Bc_size];
  float Reco_3mu_CorrM[Max_Bc_size];
  Short_t Reco_3mu_NbMuInSameSV[Max_Bc_size];
  float Reco_3mu_muW_dxy[Max_Bc_size];
  float Reco_3mu_muW_dz[Max_Bc_size];
  float Reco_3mu_mumi_dxy[Max_Bc_size];
  float Reco_3mu_mumi_dz[Max_Bc_size];
  float Reco_3mu_mupl_dxy[Max_Bc_size];
  float Reco_3mu_mupl_dz[Max_Bc_size];

  Short_t Reco_QQ_size;                   // Number of reconstructed Onia
  Short_t Reco_QQ_type[Max_QQ_size];      // Onia category: GG, GT, TT
  Short_t Reco_QQ_sign[Max_QQ_size];      /* Mu Mu combinations sign:
                             0 = +/- (signal)
                             1 = +/+
                             2 = -/- 
                          */
  Short_t Reco_QQ_mupl_idx[Max_QQ_size];  // index of the muon plus from Jpsi, in the full list of muons
  Short_t Reco_QQ_mumi_idx[Max_QQ_size];  // index of the muon minus from Jpsi, in the full list of muons
  Short_t Reco_QQ_whichGen
      [Max_QQ_size];  // index of the generated Jpsi that was matched with this rec Jpsi. Is -1 if one of the 2 muons from Jpsi was not reconstructed
  ULong64_t Reco_QQ_trig[Max_QQ_size];  // Vector of trigger bits matched to the Onia
  float Reco_QQ_VtxProb[Max_QQ_size];   // chi2 probability of vertex fitting
  float Reco_QQ_ctau[Max_QQ_size];      // ctau: flight time
  float Reco_QQ_ctauErr[Max_QQ_size];   // error on ctau
  float Reco_QQ_cosAlpha
      [Max_QQ_size];  // cosine of angle between momentum of Jpsi and direction of PV--displaced vertex segment (in XY plane)
  float Reco_QQ_ctau3D[Max_QQ_size];     // ctau: flight time in 3D
  float Reco_QQ_ctauErr3D[Max_QQ_size];  // error on ctau in 3D
  float Reco_QQ_cosAlpha3D
      [Max_QQ_size];  // cosine of angle between momentum of Jpsi and direction of PV--displaced vertex segment (3D)
  float Reco_QQ_dca[Max_QQ_size];
  float Reco_QQ_MassErr[Max_QQ_size];

  int Reco_QQ_NtrkPt02[Max_QQ_size];
  int Reco_QQ_NtrkPt03[Max_QQ_size];
  int Reco_QQ_NtrkPt04[Max_QQ_size];

  int Reco_QQ_NtrkDeltaR03[Max_QQ_size];
  int Reco_QQ_NtrkDeltaR04[Max_QQ_size];
  int Reco_QQ_NtrkDeltaR05[Max_QQ_size];

  float Reco_QQ_mupl_dxy[Max_QQ_size];  // dxy for plus inner track muons
  float Reco_QQ_mumi_dxy[Max_QQ_size];  // dxy for minus inner track muons
  float Reco_QQ_mupl_dz[Max_QQ_size];   // dz for plus inner track muons
  float Reco_QQ_mumi_dz[Max_QQ_size];   // dz for minus inner track muons
  Short_t Reco_QQ_flipJpsi[Max_QQ_size];

  Short_t Reco_mu_size;  // Number of reconstructed muons
  int Reco_mu_SelectionType[Max_mu_size];
  ULong64_t Reco_mu_trig[Max_mu_size];  // Vector of trigger bits matched to the muon
  Short_t Reco_mu_charge[Max_mu_size];  // Vector of charge of muons
  Short_t Reco_mu_type[Max_mu_size];    // Vector of type of muon (global=0, tracker=1, calo=2)
  Short_t Reco_mu_whichGen
      [Max_mu_size];  // index of the generated muon that was matched with this reco muon. Is -1 if the muon is not associated with a generated muon (fake, or very bad resolution)

  bool Reco_mu_highPurity[Max_mu_size];     // Vector of high purity flag
  bool Reco_mu_TrkMuArb[Max_mu_size];       // Vector of TrackerMuonArbitrated
  bool Reco_mu_TMOneStaTight[Max_mu_size];  // Vector of TMOneStationTight
  Short_t Reco_mu_candType
      [Max_mu_size];  // candidate type of muon. 0 (or not present): muon collection, 1: packedPFCandidate, 2: lostTrack collection
  bool Reco_mu_isPF[Max_mu_size];  // Vector of isParticleFlow muon
  bool Reco_mu_isTracker[Max_mu_size];
  bool Reco_mu_isGlobal[Max_mu_size];
  bool Reco_mu_isSoftCutBased[Max_mu_size];
  float Reco_mu_softMvaRun3Value[Max_mu_size];
  bool Reco_mu_isHybridSoft[Max_mu_size];
  bool Reco_mu_isMediumCutBased[Max_mu_size];
  bool Reco_mu_isTightCutBased[Max_mu_size];
  bool Reco_mu_InTightAcc[Max_mu_size];  // Is in the tight acceptance for global muons
  bool Reco_mu_InLooseAcc[Max_mu_size];  // Is in the loose acceptance for global muons

  int Reco_mu_nPixValHits[Max_mu_size];      // Number of valid pixel hits in sta muons
  int Reco_mu_nMuValHits[Max_mu_size];       // Number of valid muon hits in sta muons
  int Reco_mu_nTrkHits[Max_mu_size];         // track hits global muons
  int Reco_mu_nPixWMea[Max_mu_size];         // pixel layers with measurement for inner track muons
  int Reco_mu_nTrkWMea[Max_mu_size];         // track layers with measurement for inner track muons
  int Reco_mu_StationsMatched[Max_mu_size];  // number of stations matched for inner track muons
  float Reco_mu_segmentComp[Max_mu_size];
  float Reco_mu_kink[Max_mu_size];
  float Reco_mu_localChi2[Max_mu_size];
  float Reco_mu_normChi2_bestTracker[Max_mu_size];
  float Reco_mu_normChi2_inner[Max_mu_size];   // chi2/ndof for inner track muons
  float Reco_mu_normChi2_global[Max_mu_size];  // chi2/ndof for global muons
  float Reco_mu_dxy[Max_mu_size];              // dxy for inner track muons
  float Reco_mu_dxyErr[Max_mu_size];           // dxy error for inner track muons
  float Reco_mu_dz[Max_mu_size];               // dz for inner track muons
  float Reco_mu_dzErr[Max_mu_size];            // dz error for inner track muons
  float Reco_mu_pt_inner[Max_mu_size];         // pT for inner track muons
  float Reco_mu_pt_global[Max_mu_size];        // pT for global muons
  float Reco_mu_ptErr_inner[Max_mu_size];      // pT error for inner track muons
  float Reco_mu_ptErr_global[Max_mu_size];     // pT error for global muons
  float Reco_mu_pTrue[Max_mu_size];  // P of the associated generated muon, used to match the Reco_mu with the Gen_mu
  float Reco_mu_validFraction[Max_mu_size];
  int Reco_mu_simExtType[Max_Bc_size];  //

  Short_t muType;  // type of muon (GlbTrk=0, Trk=1, Glb=2, none=-1)
  std::vector<float>
      EtaOfWantedMuons;  //To know which single muons to fill, when fillSingleMuons=false and we want only the muons from selected dimuons
  std::vector<float>
      EtaOfWantedTracks;  //To know which tracks to fill (only tracks that are part of a selected dimuon-track candidate)

  Short_t Reco_trk_size;                  // Number of reconstructed tracks
  Short_t Reco_trk_charge[Max_trk_size];  // Vector of charge of tracks
  bool Reco_trk_highPurity[Max_trk_size];
  Short_t Reco_trk_whichGenmu[Max_trk_size];
  bool Reco_trk_InLooseAcc[Max_trk_size];
  bool Reco_trk_InTightAcc[Max_trk_size];
  float Reco_trk_dxyError[Max_trk_size];
  float Reco_trk_dzError[Max_trk_size];
  float Reco_trk_dxy[Max_trk_size];
  float Reco_trk_dz[Max_trk_size];
  float Reco_trk_ptErr[Max_trk_size];
  int Reco_trk_originalAlgo[Max_trk_size];
  int Reco_trk_nPixWMea[Max_trk_size];
  int Reco_trk_nTrkWMea[Max_trk_size];

  // histos
  TH1F* hGoodMuonsNoTrig = nullptr;
  TH1F* hGoodMuons = nullptr;
  TH1F* hL1DoubleMu0 = nullptr;

  MyCommonHistoManager* myRecoMuonHistos = nullptr;
  MyCommonHistoManager* myRecoGlbMuonHistos = nullptr;
  MyCommonHistoManager* myRecoTrkMuonHistos = nullptr;

  MyCommonHistoManager* myRecoJpsiHistos = nullptr;
  MyCommonHistoManager* myRecoJpsiGlbGlbHistos = nullptr;
  MyCommonHistoManager* myRecoJpsiGlbTrkHistos = nullptr;
  MyCommonHistoManager* myRecoJpsiTrkTrkHistos = nullptr;

  // event counters
  TH1F* hStats = nullptr;

  // centrality
  TH1F* hCent = nullptr;

  // number of primary vertices
  TH1F* hPileUp = nullptr;

  // z vertex distribution
  TH1F* hZVtx = nullptr;

  // centrality
  int centBin;
  int theCentralityBin;

  Short_t Npix, NpixelTracks, Ntracks;
  int NtracksPtCut, NtracksEtaCut, NtracksEtaPtCut;
  float SumET_HF, SumET_HFplus, SumET_HFminus, SumET_HFplusEta4, SumET_HFminusEta4, SumET_HFhit, SumET_HFhitPlus,
      SumET_HFhitMinus, SumET_EB, SumET_ET, SumET_EE, SumET_EEplus, SumET_EEminus, SumET_ZDC, SumET_ZDCplus,
      SumET_ZDCminus;

  // Event Plane variables
  int nEP;  // number of event planes
  //float *hiEvtPlane;
  float rpAng[50];
  float rpCos[50];
  float rpSin[50];
  float rpAng_origin[50];
  float rpCos_origin[50];
  float rpSin_origin[50];

  // handles
  edm::Handle<pat::CompositeCandidateCollection> collJpsi;
  edm::Handle<pat::CompositeCandidateCollection> collTrimuon;
  edm::Handle<pat::CompositeCandidateCollection> collDimutrk;
  edm::Handle<pat::MuonCollection> collMuon;
  edm::Handle<pat::MuonCollection> collMuonNoTrig;
  edm::Handle<reco::TrackCollection> collTracks;
  edm::Handle<reco::VertexCollection> SVs;

  edm::Handle<reco::GenParticleCollection> collGenParticles;
  edm::Handle<GenEventInfoProduct> genInfo;

  edm::Handle<edm::TriggerResults> collTriggerResults;

  // data members
  edm::EDGetTokenT<pat::MuonCollection> _patMuonToken;
  edm::EDGetTokenT<pat::MuonCollection> _patMuonNoTrigToken;
  edm::EDGetTokenT<pat::CompositeCandidateCollection> _patJpsiToken;
  edm::EDGetTokenT<pat::CompositeCandidateCollection> _patTrimuonToken;
  edm::EDGetTokenT<pat::CompositeCandidateCollection> _patDimuTrkToken;
  edm::EDGetTokenT<reco::TrackCollection> _recoTracksToken;
  edm::EDGetTokenT<reco::GenParticleCollection> _genParticleToken;
  edm::EDGetTokenT<GenEventInfoProduct> _genInfoToken;
  edm::EDGetTokenT<reco::VertexCollection> _thePVsToken;
  edm::EDGetTokenT<reco::VertexCollection> _SVToken;
  edm::EDGetTokenT<edm::TriggerResults> _tagTriggerResultsToken;
  edm::EDGetTokenT<reco::Centrality> _centralityTagToken;
  edm::EDGetTokenT<int> _centralityBinTagToken;
  edm::EDGetTokenT<reco::EvtPlaneCollection> _evtPlaneTagToken;
  std::string _histfilename;
  std::string _datasetname;
  std::string _mom4format;
  std::string _muonSel;

  std::vector<double> _centralityranges;
  std::vector<double> _ptbinranges;
  std::vector<double> _etabinranges;
  std::vector<string> _dblTriggerPathNames;
  std::vector<string> _sglTriggerPathNames;

  bool _onlythebest;
  bool _applycuts;
  bool _SofterSgMuAcceptance;
  bool _SumETvariables;
  bool _selTightGlobalMuon;
  bool _muonLessPrimaryVertex;
  bool _useSVfinder;
  bool _useBS;
  bool _useRapidity;
  bool _removeSignal;
  bool _removeMuons;
  bool _storeSs;
  bool _AtLeastOneCand;
  bool _combineCategories;
  bool _fillRooDataSet;
  bool _fillTree;
  bool _fillHistos;
  bool _theMinimumFlag;
  bool _fillSingleMuons;
  bool _onlySingleMuons;
  bool _fillRecoTracks;
  bool _isHI;
  bool _isPA;
  bool _isMC;
  bool _isPromptMC;
  bool _useEvtPlane;
  bool _useGeTracks;
  bool _doTrimuons;
  bool _doDimuTrk;
  int _flipJpsiDirection;
  bool _genealogyInfo;
  bool _miniAODcut;

  int _oniaPDG;
  int _BcPDG;
  int _OneMatchedHLTMu;
  bool _checkTrigNames;
  bool _genOnly;

  std::vector<unsigned int> _thePassedCats;
  std::vector<const pat::CompositeCandidate*> _thePassedCands;
  std::vector<unsigned int> _thePassedBcCats;
  std::vector<const pat::CompositeCandidate*> _thePassedBcCands;

  std::vector<reco::GenParticleRef> _Gen_QQ_MomAndTrkBro[Max_QQ_size];

  // number of events
  unsigned int nEvents;
  unsigned int passedCandidates;

  unsigned int runNb;
  unsigned int eventNb;
  unsigned int lumiSection;

  // limits
  float JpsiMassMin;
  float JpsiMassMax;
  float JpsiPtMin;   // SET BY
  float JpsiPtMax;   // DEFINITION
  float JpsiRapMin;  // OF BIN
  float JpsiRapMax;  // LIMITS
  float JpsiPDGMass = 3.09609;
  float BcPDGMass = 6.2745;

  math::XYZPoint RefVtx;
  float RefVtx_xError;
  float RefVtx_yError;
  float RefVtx_zError;
  float zVtx;
  Short_t nPV;

  // Trigger stuff
  // PUT HERE THE *LAST FILTERS* OF THE BITS YOU LIKE
  static const unsigned int sNTRIGGERS = 65;
  unsigned int NTRIGGERS;
  unsigned int NTRIGGERS_DBL;
  unsigned int nTrig;

  // MC 8E29
  bool isTriggerMatched[sNTRIGGERS];
  bool alreadyFilled[sNTRIGGERS];
  ULong64_t HLTriggers;
  int trigPrescale[sNTRIGGERS];

  std::map<std::string, int> mapTriggerNameToIntFired_;
  std::map<std::string, int> mapTriggerNameToPrescaleFac_;
  std::map<long int, int> mapMuonMomToIndex_;
  std::map<long int, int> mapGenMuonMomToIndex_;
  std::map<long int, int> mapTrkMomToIndex_;

  HLTPrescaleProvider hltPrescaleProvider;
  bool hltPrescaleInit;

  const edm::ParameterSet _iConfig;
};

#endif