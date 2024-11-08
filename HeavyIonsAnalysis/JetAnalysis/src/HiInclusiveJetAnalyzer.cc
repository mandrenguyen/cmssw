/*
  Based on the jet response analyzer
  Modified by Matt Nguyen, November 2010
*/

#include "HeavyIonsAnalysis/JetAnalysis/interface/HiInclusiveJetAnalyzer.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "fastjet/contrib/Njettiness.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "AnalysisDataFormats/TrackInfo/interface/TrackToGenParticleMap.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "TRandom.h"

using namespace std;
using namespace edm;
using namespace reco;

HiInclusiveJetAnalyzer::HiInclusiveJetAnalyzer(const edm::ParameterSet& iConfig) {
  doMatch_ = iConfig.getUntrackedParameter<bool>("matchJets", false);
  jetTag_ = consumes<pat::JetCollection>(iConfig.getParameter<InputTag>("jetTag"));
  matchTag_ = consumes<pat::JetCollection>(iConfig.getUntrackedParameter<InputTag>("matchTag"));

  doMujets_ = iConfig.getUntrackedParameter<bool>("doMujets", false);
  if (doMujets_) mujetTag_ = consumes<pat::JetCollection>(iConfig.getParameter<InputTag>("mujetTag"));

  jetTagInfos_ = consumes<reco::JetTagCollection>(edm::InputTag("pfJetProbabilityBJetTags"));

  useQuality_ = iConfig.getUntrackedParameter<bool>("useQuality", true);
  trackQuality_ = iConfig.getUntrackedParameter<std::string>("trackQuality", "highPurity");

  jetName_ = iConfig.getUntrackedParameter<std::string>("jetName");
  doGenTaus_ = iConfig.getUntrackedParameter<bool>("doGenTaus", false);
  doGenSym_ = iConfig.getUntrackedParameter<bool>("doGenSym", false);
  doSubJets_ = iConfig.getUntrackedParameter<bool>("doSubJets", false);
  doSubJetsNew_ = iConfig.getUntrackedParameter<bool>("doSubJetsNew", false);
  doJetConstituents_ = iConfig.getUntrackedParameter<bool>("doJetConstituents", false);
  doGenSubJets_ = iConfig.getUntrackedParameter<bool>("doGenSubJets", false);

  doSvtx_ = iConfig.getUntrackedParameter<bool>("doSvtx", false);
  if (doSvtx_) {
    svTagInfoLabel_ = iConfig.getUntrackedParameter<std::string>("svTagInfoLabel");
  }

  //reWTA reclustering
  doWTARecluster_ = iConfig.getUntrackedParameter<bool>("doWTARecluster", false);

  //rho_ = consumes<double>(iConfig.getParameter<edm::InputTag>("rhoSrc"));

  if (doGenTaus_) {
    tokenGenTau1_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("genTau1"));
    tokenGenTau2_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("genTau2"));
    tokenGenTau3_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("genTau3"));
  }

  if (doGenSym_) {
    tokenGenSym_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("genSym"));
    tokenGenDroppedBranches_ = consumes<edm::ValueMap<int>>(iConfig.getParameter<edm::InputTag>("genDroppedBranches"));
  }

  isMC_ = iConfig.getUntrackedParameter<bool>("isMC", false);
  useHepMC_ = iConfig.getUntrackedParameter<bool>("useHepMC", false);
  fillGenJets_ = iConfig.getUntrackedParameter<bool>("fillGenJets", false);

  doHiJetID_ = iConfig.getUntrackedParameter<bool>("doHiJetID", false);
  doStandardJetID_ = iConfig.getUntrackedParameter<bool>("doStandardJetID", false);

  rParam = iConfig.getParameter<double>("rParam");
  hardPtMin_ = iConfig.getUntrackedParameter<double>("hardPtMin", 4);
  jetPtMin_ = iConfig.getParameter<double>("jetPtMin");
  jetAbsEtaMax_ = iConfig.getUntrackedParameter<double>("jetAbsEtaMax", 5.1);

  doJetTrueFlavour_ = iConfig.getUntrackedParameter<bool>("doJetTrueFlavour",true);

  if (isMC_) {
    genjetTag_ = consumes<edm::View<reco::GenJet>>(iConfig.getParameter<InputTag>("genjetTag"));
    if (useHepMC_) {
      eventInfoTag_ = consumes<HepMCProduct>(iConfig.getParameter<InputTag>("eventInfoTag"));
    }
    eventGenInfoTag_ = consumes<GenEventInfoProduct>(iConfig.getParameter<InputTag>("eventInfoTag"));

    //jetFlavourInfosToken_ = consumes<reco::JetFlavourInfoMatchingCollection>( iConfig.getParameter<edm::InputTag>("jetFlavourInfos") );
  }
  useRawPt_ = iConfig.getUntrackedParameter<bool>("useRawPt", true);

  doLegacyBtagging_ = iConfig.getUntrackedParameter<bool>("doLegacyBtagging", true);
  doCandidateBtagging_ = iConfig.getUntrackedParameter<bool>("doCandidateBtagging", true);

  pfCandidateLabel_ =
      consumes<edm::View<pat::PackedCandidate>>(iConfig.getUntrackedParameter<edm::InputTag>("pfCandidateLabel"));

  if (isMC_)
    genParticleSrc_ =
        consumes<std::vector<pat::PackedGenParticle>>(iConfig.getUntrackedParameter<edm::InputTag>("genParticles"));

  if (doLegacyBtagging_) {
    trackCHEBJetTags_ = "trackCountingHighEffBJetTags";
    trackCHPBJetTags_ = "trackCountingHighPurBJetTags";
    jetPBJetTags_ = "jetProbabilityBJetTags";
    jetBPBJetTags_ = "jetBProbabilityBJetTags";
    simpleSVHighEffBJetTags_ = "simpleSecondaryVertexHighEffBJetTags";
    simpleSVHighPurBJetTags_ = "simpleSecondaryVertexHighPurBJetTags";
    combinedSVV2BJetTags_ = "combinedSecondaryVertexV2BJetTags";
  }

  if (doCandidateBtagging_) {
    deepCSVJetTags_ = "pfDeepCSVJetTags:probb";
    pfJPJetTags_ = "pfJetProbabilityBJetTags";
    deepFlavourJetTags_ = "pfDeepFlavourJetTags";
    particleNetJetTags_ = "pfParticleNetAK4JetTags";
    particleNetDiscriminatorsJetTags_ = "pfParticleNetAK4DiscriminatorsJetTags";
  }
  doSubEvent_ = false;

  if (isMC_) {
    genPtMin_ = iConfig.getUntrackedParameter<double>("genPtMin", 10);
    doSubEvent_ = iConfig.getUntrackedParameter<bool>("doSubEvent", false);
  }

  if (doSubJetsNew_) {
    groomedJetsToken_ = consumes<edm::View<reco::Jet>>(iConfig.getUntrackedParameter<edm::InputTag>("groomedJets", edm::InputTag("slimmedJets")));
    // std::cout << "groomedJets have been consumed" << std::endl;
    if (isMC_) {
      groomedGenJetsToken_ = consumes<edm::View<reco::Jet>>(iConfig.getUntrackedParameter<edm::InputTag>("groomedGenJets", edm::InputTag("slimmedGenJets")));
      // std::cout << "groomedGenJets have been consumed" << std::endl;
    }
  }

  doTracks_ = iConfig.getUntrackedParameter<bool>("doTracks", false);
  if (doTracks_) {
    trkPtCut_ = iConfig.getUntrackedParameter<double>("trkPtCut", 1.);
    ipTagInfoLabel_ = iConfig.getUntrackedParameter<std::string>("ipTagInfoLabel");
    if (isMC_) {
      trackToGenParticleMapToken_ = consumes<reco::TrackToGenParticleMap>(iConfig.getUntrackedParameter<edm::InputTag>("trackToGenParticleMap", edm::InputTag("TrackToGenParticleMapProducer", "trackToGenParticleMap")));
      // std::cout << "Before consuming " << std::endl;
      droppedTracksToken_ = consumes<edm::View<reco::PFCandidate>>(iConfig.getUntrackedParameter<edm::InputTag>("droppedTracks", edm::InputTag("dynGroomedPFJets", "droppedTracks")));
      // std::cout << "Token is initialized " << std::endl;
    }
  }

  // [DEBUG]
  primaryVerticesToken_ = consumes<std::vector<reco::Vertex>>(iConfig.getUntrackedParameter<edm::InputTag>("primaryVertices", edm::InputTag("offlineSlimmedPrimaryVertices")));
  // primaryVerticesToken_ = consumes<std::vector<reco::Vertex>>(iConfig.getUntrackedParameter<edm::InputTag>("primaryVertices", edm::InputTag("unpackedTracksAndVertices")));

  
}

HiInclusiveJetAnalyzer::~HiInclusiveJetAnalyzer() {}

void HiInclusiveJetAnalyzer::beginRun(const edm::Run& run, const edm::EventSetup& es) {}

void HiInclusiveJetAnalyzer::beginJob() {
  string jetTagTitle = jetTagLabel_.label() + " Jet Analysis Tree";
  t = fs1->make<TTree>("t", jetTagTitle.c_str());

  t->Branch("run", &jets_.run, "run/I");
  t->Branch("evt", &jets_.evt, "evt/I");
  t->Branch("lumi", &jets_.lumi, "lumi/I");
  t->Branch("nref", &jets_.nref, "nref/I");
  t->Branch("rawpt", jets_.rawpt, "rawpt[nref]/F");
  t->Branch("jtpt", jets_.jtpt, "jtpt[nref]/F");
  t->Branch("jteta", jets_.jteta, "jteta[nref]/F");
  t->Branch("jty", jets_.jty, "jty[nref]/F");
  t->Branch("jtphi", jets_.jtphi, "jtphi[nref]/F");
  t->Branch("jtpu", jets_.jtpu, "jtpu[nref]/F");
  t->Branch("jtm", jets_.jtm, "jtm[nref]/F");
  t->Branch("jtarea", jets_.jtarea, "jtarea[nref]/F");
  /*
  t->Branch("jer_sf_nom", jets_.jer_sf_nom, "jer_sf_nom[nref]/F");
  t->Branch("jer_sf_up", jets_.jer_sf_up, "jer_sf_up[nref]/F");
  t->Branch("jer_sf_down", jets_.jer_sf_down, "jer_sf_down[nref]/F");
  t->Branch("jec_unc", jets_.jec_unc, "jec_unc[nref]/F");
  */
  t->Branch("nvtx", &jets_.nvtx, "nvtx/I");
  //t->Branch("rho", &jets_.rho, "rho/F");

  //for reWTA reclustering
  if (doWTARecluster_) {
    t->Branch("WTAeta", jets_.WTAeta, "WTAeta[nref]/F");
    t->Branch("WTAphi", jets_.WTAphi, "WTAphi[nref]/F");
  }

  t->Branch("jtPfCHF", jets_.jtPfCHF, "jtPfCHF[nref]/F");
  t->Branch("jtPfNHF", jets_.jtPfNHF, "jtPfNHF[nref]/F");
  t->Branch("jtPfCEF", jets_.jtPfCEF, "jtPfCEF[nref]/F");
  t->Branch("jtPfNEF", jets_.jtPfNEF, "jtPfNEF[nref]/F");
  t->Branch("jtPfMUF", jets_.jtPfMUF, "jtPfMUF[nref]/F");

  t->Branch("jtPfCHM", jets_.jtPfCHM, "jtPfCHM[nref]/I");
  t->Branch("jtPfNHM", jets_.jtPfNHM, "jtPfNHM[nref]/I");
  t->Branch("jtPfCEM", jets_.jtPfCEM, "jtPfCEM[nref]/I");
  t->Branch("jtPfNEM", jets_.jtPfNEM, "jtPfNEM[nref]/I");
  t->Branch("jtPfMUM", jets_.jtPfMUM, "jtPfMUM[nref]/I");
  /*
  t->Branch("jttau1", jets_.jttau1, "jttau1[nref]/F");
  t->Branch("jttau2", jets_.jttau2, "jttau2[nref]/F");
  t->Branch("jttau3", jets_.jttau3, "jttau3[nref]/F");
  */
  if (doSubJets_) {
    t->Branch("jtSubJetPt", &jets_.jtSubJetPt);
    t->Branch("jtSubJetEta", &jets_.jtSubJetEta);
    t->Branch("jtSubJetPhi", &jets_.jtSubJetPhi);
    t->Branch("jtSubJetM", &jets_.jtSubJetM);
    t->Branch("jtsym", jets_.jtsym, "jtsym[nref]/F");
    t->Branch("jtdroppedBranches", jets_.jtdroppedBranches, "jtdroppedBranches[nref]/I");
  }

  if (doSubJetsNew_) {
    //t->Branch("sjt1HasHF", jets_.sjt1HasHF, "sjt1HasHF[nref]/I");
    t->Branch("sjt1Pt", jets_.sjt1Pt, "sjt1Pt[nref]/F");
    t->Branch("sjt1Eta", jets_.sjt1Eta, "sjt1Eta[nref]/F");
    t->Branch("sjt1Phi", jets_.sjt1Phi, "sjt1Phi[nref]/F");
    t->Branch("sjt1E", jets_.sjt1E, "sjt1E[nref]/F");
    t->Branch("sjt1Y", jets_.sjt1Y, "sjt1Y[nref]/F");
    t->Branch("sjt1Pz", jets_.sjt1Pz, "sjt1Pz[nref]/F");
    t->Branch("sjt2Pt", jets_.sjt2Pt, "sjt2Pt[nref]/F");
    t->Branch("sjt2Eta", jets_.sjt2Eta, "sjt2Eta[nref]/F");
    t->Branch("sjt2Phi", jets_.sjt2Phi, "sjt2Phi[nref]/F");
    t->Branch("sjt2E", jets_.sjt2E, "sjt2E[nref]/F");
    t->Branch("sjt2Y", jets_.sjt2Y, "sjt2Y[nref]/F");
    t->Branch("sjt2Pz", jets_.sjt2Pz, "sjt2Pz[nref]/F");

    if (isMC_) {
      //t->Branch("rsjt1HasHF", jets_.rsjt1HasHF, "rsjt1HasHF[nref]/I");
      t->Branch("rsjt1Pt", jets_.rsjt1Pt, "rsjt1Pt[nref]/F");
      t->Branch("rsjt1Eta", jets_.rsjt1Eta, "rsjt1Eta[nref]/F");
      t->Branch("rsjt1Phi", jets_.rsjt1Phi, "rsjt1Phi[nref]/F");
      t->Branch("rsjt1E", jets_.rsjt1E, "rsjt1E[nref]/F");
      t->Branch("rsjt1Y", jets_.rsjt1Y, "rsjt1Y[nref]/F");
      t->Branch("rsjt1Pz", jets_.rsjt1Pz, "rsjt1Pz[nref]/F");
      t->Branch("rsjt2Pt", jets_.rsjt2Pt, "rsjt2Pt[nref]/F");
      t->Branch("rsjt2Eta", jets_.rsjt2Eta, "rsjt2Eta[nref]/F");
      t->Branch("rsjt2Phi", jets_.rsjt2Phi, "rsjt2Phi[nref]/F");
      t->Branch("rsjt2E", jets_.rsjt2E, "rsjt2E[nref]/F");
      t->Branch("rsjt2Y", jets_.rsjt2Y, "rsjt2Y[nref]/F");
      t->Branch("rsjt2Pz", jets_.rsjt2Pz, "rsjt2Pz[nref]/F");


    }
  }

  if (doJetConstituents_) {
    t->Branch("jtConstituentsId", &jets_.jtConstituentsId);
    t->Branch("jtConstituentsE", &jets_.jtConstituentsE);
    t->Branch("jtConstituentsPt", &jets_.jtConstituentsPt);
    t->Branch("jtConstituentsEta", &jets_.jtConstituentsEta);
    t->Branch("jtConstituentsPhi", &jets_.jtConstituentsPhi);
    t->Branch("jtConstituentsM", &jets_.jtConstituentsM);
    t->Branch("jtSDConstituentsId", &jets_.jtSDConstituentsId);
    t->Branch("jtSDConstituentsE", &jets_.jtSDConstituentsE);
    t->Branch("jtSDConstituentsPt", &jets_.jtSDConstituentsPt);
    t->Branch("jtSDConstituentsEta", &jets_.jtSDConstituentsEta);
    t->Branch("jtSDConstituentsPhi", &jets_.jtSDConstituentsPhi);
    t->Branch("jtSDConstituentsM", &jets_.jtSDConstituentsM);
  }
  // jet ID information, jet composition
  if (doHiJetID_) {
    t->Branch("trackMax", jets_.trackMax, "trackMax[nref]/F");
    t->Branch("trackSum", jets_.trackSum, "trackSum[nref]/F");
    t->Branch("trackN", jets_.trackN, "trackN[nref]/I");
    t->Branch("trackHardSum", jets_.trackHardSum, "trackHardSum[nref]/F");
    t->Branch("trackHardN", jets_.trackHardN, "trackHardN[nref]/I");

    t->Branch("chargedMax", jets_.chargedMax, "chargedMax[nref]/F");
    t->Branch("chargedSum", jets_.chargedSum, "chargedSum[nref]/F");
    t->Branch("chargedN", jets_.chargedN, "chargedN[nref]/I");
    t->Branch("chargedHardSum", jets_.chargedHardSum, "chargedHardSum[nref]/F");
    t->Branch("chargedHardN", jets_.chargedHardN, "chargedHardN[nref]/I");

    t->Branch("photonMax", jets_.photonMax, "photonMax[nref]/F");
    t->Branch("photonSum", jets_.photonSum, "photonSum[nref]/F");
    t->Branch("photonN", jets_.photonN, "photonN[nref]/I");
    t->Branch("photonHardSum", jets_.photonHardSum, "photonHardSum[nref]/F");
    t->Branch("photonHardN", jets_.photonHardN, "photonHardN[nref]/I");

    t->Branch("neutralMax", jets_.neutralMax, "neutralMax[nref]/F");
    t->Branch("neutralSum", jets_.neutralSum, "neutralSum[nref]/F");
    t->Branch("neutralN", jets_.neutralN, "neutralN[nref]/I");

    t->Branch("eMax", jets_.eMax, "eMax[nref]/F");
    t->Branch("eSum", jets_.eSum, "eSum[nref]/F");
    t->Branch("eN", jets_.eN, "eN[nref]/I");

    t->Branch("muMax", jets_.muMax, "muMax[nref]/F");
    t->Branch("muSum", jets_.muSum, "muSum[nref]/F");
    t->Branch("muN", jets_.muN, "muN[nref]/I");
  }

  if (doStandardJetID_) {
    t->Branch("fHPD", jets_.fHPD, "fHPD[nref]/F");
    t->Branch("fRBX", jets_.fRBX, "fRBX[nref]/F");
    t->Branch("n90", jets_.n90, "n90[nref]/I");
    t->Branch("fSubDet1", jets_.fSubDet1, "fSubDet1[nref]/F");
    t->Branch("fSubDet2", jets_.fSubDet2, "fSubDet2[nref]/F");
    t->Branch("fSubDet3", jets_.fSubDet3, "fSubDet3[nref]/F");
    t->Branch("fSubDet4", jets_.fSubDet4, "fSubDet4[nref]/F");
    t->Branch("restrictedEMF", jets_.restrictedEMF, "restrictedEMF[nref]/F");
    t->Branch("nHCAL", jets_.nHCAL, "nHCAL[nref]/I");
    t->Branch("nECAL", jets_.nECAL, "nECAL[nref]/I");
    t->Branch("apprHPD", jets_.apprHPD, "apprHPD[nref]/F");
    t->Branch("apprRBX", jets_.apprRBX, "apprRBX[nref]/F");
    t->Branch("n2RPC", jets_.n2RPC, "n2RPC[nref]/I");
    t->Branch("n3RPC", jets_.n3RPC, "n3RPC[nref]/I");
    t->Branch("nRPC", jets_.nRPC, "nRPC[nref]/I");

    t->Branch("fEB", jets_.fEB, "fEB[nref]/F");
    t->Branch("fEE", jets_.fEE, "fEE[nref]/F");
    t->Branch("fHB", jets_.fHB, "fHB[nref]/F");
    t->Branch("fHE", jets_.fHE, "fHE[nref]/F");
    t->Branch("fHO", jets_.fHO, "fHO[nref]/F");
    t->Branch("fLong", jets_.fLong, "fLong[nref]/F");
    t->Branch("fShort", jets_.fShort, "fShort[nref]/F");
    t->Branch("fLS", jets_.fLS, "fLS[nref]/F");
    t->Branch("fHFOOT", jets_.fHFOOT, "fHFOOT[nref]/F");
  }

  if (doTracks_) {
    t->Branch("jtNtrk", jets_.jtNtrk, "jtNtrk[nref]/I");
    t->Branch("ntrk", &jets_.ntrk, "ntrk/I");
    t->Branch("trkJetId", jets_.trkJetId, "trkJetId[ntrk]/I");
    t->Branch("trkSvtxId", jets_.trkSvtxId, "trkSvtxId[ntrk]/I");
    t->Branch("trkPt", jets_.trkPt, "trkPt[ntrk]/F");
    t->Branch("trkEta", jets_.trkEta, "trkEta[ntrk]/F");
    t->Branch("trkPhi", jets_.trkPhi, "trkPhi[ntrk]/F");
    t->Branch("trkIp3d", jets_.trkIp3d, "trkIp3d[ntrk]/F");
    t->Branch("trkIp3dSig", jets_.trkIp3dSig, "trkIp3dSig[ntrk]/F");
    t->Branch("trkIp2d", jets_.trkIp2d, "trkIp2d[ntrk]/F");
    t->Branch("trkIp2dSig", jets_.trkIp2dSig, "trkIp2dSig[ntrk]/F");
    t->Branch("trkDistToAxisSig", jets_.trkDistToAxisSig, "trkDistToAxisSig[ntrk]/F");
    t->Branch("trkDistToAxis", jets_.trkDistToAxis, "trkDistToAxis[ntrk]/F");
    t->Branch("trkIpProb3d", jets_.trkIpProb3d, "trkIpProb3d[ntrk]/F");
    t->Branch("trkIpProb2d", jets_.trkIpProb2d, "trkIpProb2d[ntrk]/F");
    t->Branch("trkDz", jets_.trkDz, "trkDz[ntrk]/F");
    t->Branch("trkPdgId", jets_.trkPdgId, "trkPdgId[ntrk]/I");
    t->Branch("trkMatchSta", jets_.trkMatchSta, "trkMatchSta[ntrk]/I");

    t->Branch("jtptCh", jets_.jtptCh, "jtptCh[nref]/F");
    if (isMC_) {
      t->Branch("refptCh", jets_.refptCh, "refptCh[nref]/F");
      t->Branch("refNtrk", jets_.refNtrk, "refNtrk[nref]/I");
    }
  }

  if (doSvtx_) {
    t->Branch("jtNsvtx", jets_.jtNsvtx, "jtNsvtx[nref]/I");
    t->Branch("nsvtx", &jets_.nsvtx, "nsvtx/I");
    t->Branch("svtxJetId", jets_.svtxJetId, "svtxJetId[nsvtx]/I");
    t->Branch("svtxNtrk", jets_.svtxNtrk, "svtxNtrk[nsvtx]/I");
    t->Branch("svtxdl", jets_.svtxdl, "svtxdl[nsvtx]/F");
    t->Branch("svtxdls", jets_.svtxdls, "svtxdls[nsvtx]/F");
    t->Branch("svtxdl2d", jets_.svtxdl2d, "svtxdl2d[nsvtx]/F");
    t->Branch("svtxdls2d", jets_.svtxdls2d, "svtxdls2d[nsvtx]/F");
    t->Branch("svtxm", jets_.svtxm, "svtxm[nsvtx]/F");
    t->Branch("svtxmcorr", jets_.svtxmcorr, "svtxmcorr[nsvtx]/F");
    t->Branch("svtxpt", jets_.svtxpt, "svtxpt[nsvtx]/F");
    t->Branch("svtxnormchi2", jets_.svtxnormchi2, "svtxnormchi2[nsvtx]/F");
    
    t->Branch("ntrkInSvtxNotInJet", &jets_.ntrkInSvtxNotInJet, "ntrkInSvtxNotInJet/I");
    t->Branch("trkInSvtxNotInJetSvId", jets_.trkInSvtxNotInJetSvId, "trkInSvtxNotInJetSvId[ntrkInSvtxNotInJet]/I");
    t->Branch("trkInSvtxNotInJetOtherJetId", jets_.trkInSvtxNotInJetOtherJetId, "trkInSvtxNotInJetOtherJetId[ntrkInSvtxNotInJet]/I");
    t->Branch("trkInSvtxNotInJetMatchSta", jets_.trkInSvtxNotInJetMatchSta, "trkInSvtxNotInJetMatchSta[ntrkInSvtxNotInJet]/I");
    t->Branch("trkInSvtxNotInJetPt", jets_.trkInSvtxNotInJetPt, "trkInSvtxNotInJetPt[ntrkInSvtxNotInJet]/F");
    t->Branch("trkInSvtxNotInJetEta", jets_.trkInSvtxNotInJetEta, "trkInSvtxNotInJetEta[ntrkInSvtxNotInJet]/F");
    t->Branch("trkInSvtxNotInJetPhi", jets_.trkInSvtxNotInJetPhi, "trkInSvtxNotInJetPhi[ntrkInSvtxNotInJet]/F");
  }

  // Jet ID
  if (doMatch_) {
    t->Branch("matchedPt", jets_.matchedPt, "matchedPt[nref]/F");
    t->Branch("matchedRawPt", jets_.matchedRawPt, "matchedRawPt[nref]/F");
    t->Branch("matchedPu", jets_.matchedPu, "matchedPu[nref]/F");
    t->Branch("matchedR", jets_.matchedR, "matchedR[nref]/F");
    if (isMC_) {
      t->Branch("matchedHadronFlavor", jets_.matchedHadronFlavor, "matchedHadronFlavor[nref]/I");
      t->Branch("matchedPartonFlavor", jets_.matchedPartonFlavor, "matchedPartonFlavor[nref]/I");
    }
  }

  // b-jet discriminators
  if (doLegacyBtagging_) {
    t->Branch("discr_ssvHighEff", jets_.discr_ssvHighEff, "discr_ssvHighEff[nref]/F");
    t->Branch("discr_ssvHighPur", jets_.discr_ssvHighPur, "discr_ssvHighPur[nref]/F");
    t->Branch("discr_csvV2", jets_.discr_csvV2, "discr_csvV2[nref]/F");
    t->Branch("discr_muByIp3", jets_.discr_muByIp3, "discr_muByIp3[nref]/F");
    t->Branch("discr_muByPt", jets_.discr_muByPt, "discr_muByPt[nref]/F");
    t->Branch("discr_prob", jets_.discr_prob, "discr_prob[nref]/F");
    t->Branch("discr_probb", jets_.discr_probb, "discr_probb[nref]/F");
    t->Branch("discr_tcHighEff", jets_.discr_tcHighEff, "discr_tcHighEff[nref]/F");
    t->Branch("discr_tcHighPur", jets_.discr_tcHighPur, "discr_tcHighPur[nref]/F");

    t->Branch("mue", jets_.mue, "mue[nref]/F");
    t->Branch("mupt", jets_.mupt, "mupt[nref]/F");
    t->Branch("mueta", jets_.mueta, "mueta[nref]/F");
    t->Branch("muphi", jets_.muphi, "muphi[nref]/F");
    t->Branch("mudr", jets_.mudr, "mudr[nref]/F");
    t->Branch("muptrel", jets_.muptrel, "muptrel[nref]/F");
    t->Branch("muchg", jets_.muchg, "muchg[nref]/I");
  }
  if(doCandidateBtagging_){
    t->Branch("discr_deepCSV", jets_.discr_deepCSV, "discr_deepCSV[nref]/F");
    t->Branch("discr_pfJP", jets_.discr_pfJP, "discr_pfJP[nref]/F");
    t->Branch("discr_deepFlavour_b", jets_.discr_deepFlavour_b, "discr_deepFlavour_b[nref]/F");
    t->Branch("discr_deepFlavour_bb", jets_.discr_deepFlavour_bb, "discr_deepFlavour_bb[nref]/F");
    t->Branch("discr_deepFlavour_lepb", jets_.discr_deepFlavour_lepb, "discr_deepFlavour_lepb[nref]/F");
    t->Branch("discr_deepFlavour_c", jets_.discr_deepFlavour_c, "discr_deepFlavour_c[nref]/F");
    t->Branch("discr_particleNet_bb", jets_.discr_particleNet_bb, "discr_particleNet_bb[nref]/F");
    t->Branch("discr_particleNet_pu", jets_.discr_particleNet_pu, "discr_particleNet_pu[nref]/F");
    t->Branch("discr_particleNet_cc", jets_.discr_particleNet_cc, "discr_particleNet_cc[nref]/F");
    t->Branch("discr_particleNet_undef", jets_.discr_particleNet_undef, "discr_particleNet_undef[nref]/F");
    t->Branch("discr_particleNet_c", jets_.discr_particleNet_c, "discr_particleNet_c[nref]/F");
    t->Branch("discr_particleNet_b", jets_.discr_particleNet_b, "discr_particleNet_b[nref]/F");
    t->Branch("discr_particleNet_uds", jets_.discr_particleNet_uds, "discr_particleNet_uds[nref]/F");
    t->Branch("discr_particleNet_g", jets_.discr_particleNet_g, "discr_particleNet_g[nref]/F");
    t->Branch("discr_particleNet_BvsAll", jets_.discr_particleNet_BvsAll, "discr_particleNet_BvsAll[nref]/F");
    t->Branch("discr_particleNet_CvsB", jets_.discr_particleNet_CvsB, "discr_particleNet_CvsB[nref]/F");
    t->Branch("discr_particleNet_CvsL", jets_.discr_particleNet_CvsL, "discr_particleNet_CvsL[nref]/F");
    t->Branch("discr_particleNet_QvsG", jets_.discr_particleNet_QvsG, "discr_particleNet_QvsG[nref]/F");
  }
  if (isMC_) {
    if (useHepMC_) {
      t->Branch("beamId1", &jets_.beamId1, "beamId1/I");
      t->Branch("beamId2", &jets_.beamId2, "beamId2/I");
    }

    t->Branch("pthat", &jets_.pthat, "pthat/F");

    // Only matched gen jets
    t->Branch("refpt", jets_.refpt, "refpt[nref]/F");
    t->Branch("refeta", jets_.refeta, "refeta[nref]/F");
    t->Branch("refy", jets_.refy, "refy[nref]/F");
    t->Branch("refphi", jets_.refphi, "refphi[nref]/F");
    t->Branch("refm", jets_.refm, "refm[nref]/F");
    t->Branch("refarea", jets_.refarea, "refarea[nref]/F");

    if (doGenTaus_) {
      t->Branch("reftau1", jets_.reftau1, "reftau1[nref]/F");
      t->Branch("reftau2", jets_.reftau2, "reftau2[nref]/F");
      t->Branch("reftau3", jets_.reftau3, "reftau3[nref]/F");
    }
    t->Branch("refdphijt", jets_.refdphijt, "refdphijt[nref]/F");
    t->Branch("refdrjt", jets_.refdrjt, "refdrjt[nref]/F");
    // matched parton
    //t->Branch("refparton_pt", jets_.refparton_pt, "refparton_pt[nref]/F");
    //t->Branch("refparton_flavor", jets_.refparton_flavor, "refparton_flavor[nref]/I");
    //t->Branch("refparton_flavorForB", jets_.refparton_flavorForB, "refparton_flavorForB[nref]/I");

    if (doGenSubJets_) {
      //t->Branch("refptG", jets_.refptG, "refptG[nref]/F");
      //t->Branch("refetaG", jets_.refetaG, "refetaG[nref]/F");
      //t->Branch("refphiG", jets_.refphiG, "refphiG[nref]/F");
      //t->Branch("refmG", jets_.refmG, "refmG[nref]/F");
      //t->Branch("refSubJetPt", &jets_.refSubJetPt);
      //t->Branch("refSubJetEta", &jets_.refSubJetEta);
      //t->Branch("refSubJetPhi", &jets_.refSubJetPhi);
      //t->Branch("refSubJetM", &jets_.refSubJetM);
      //t->Branch("refsym", jets_.refsym, "refsym[nref]/F");
      //t->Branch("refdroppedBranches", jets_.refdroppedBranches, "refdroppedBranches[nref]/I");
    }

    if (doJetConstituents_) {
      t->Branch("refConstituentsId", &jets_.refConstituentsId);
      t->Branch("refConstituentsE", &jets_.refConstituentsE);
      t->Branch("refConstituentsPt", &jets_.refConstituentsPt);
      t->Branch("refConstituentsEta", &jets_.refConstituentsEta);
      t->Branch("refConstituentsPhi", &jets_.refConstituentsPhi);
      t->Branch("refConstituentsM", &jets_.refConstituentsM);
      t->Branch("refSDConstituentsId", &jets_.refSDConstituentsId);
      t->Branch("refSDConstituentsE", &jets_.refSDConstituentsE);
      t->Branch("refSDConstituentsPt", &jets_.refSDConstituentsPt);
      t->Branch("refSDConstituentsEta", &jets_.refSDConstituentsEta);
      t->Branch("refSDConstituentsPhi", &jets_.refSDConstituentsPhi);
      t->Branch("refSDConstituentsM", &jets_.refSDConstituentsM);
    }

    t->Branch("genChargedSum", jets_.genChargedSum, "genChargedSum[nref]/F");
    t->Branch("genHardSum", jets_.genHardSum, "genHardSum[nref]/F");
    t->Branch("signalChargedSum", jets_.signalChargedSum, "signalChargedSum[nref]/F");
    t->Branch("signalHardSum", jets_.signalHardSum, "signalHardSum[nref]/F");

    if (doSubEvent_) {
      t->Branch("subid", jets_.subid, "subid[nref]/I");
    }

    if (fillGenJets_) {
      // For all gen jets, matched or unmatched
      t->Branch("ngen", &jets_.ngen, "ngen/I");
      t->Branch("genmatchindex", jets_.genmatchindex, "genmatchindex[ngen]/I");
      t->Branch("genpt", jets_.genpt, "genpt[ngen]/F");
      t->Branch("geneta", jets_.geneta, "geneta[ngen]/F");
      t->Branch("geny", jets_.geny, "geny[ngen]/F");
      if (doGenTaus_) {
        t->Branch("gentau1", jets_.gentau1, "gentau1[ngen]/F");
        t->Branch("gentau2", jets_.gentau2, "gentau2[ngen]/F");
        t->Branch("gentau3", jets_.gentau3, "gentau3[ngen]/F");
      }
      t->Branch("genphi", jets_.genphi, "genphi[ngen]/F");
      t->Branch("genm", jets_.genm, "genm[ngen]/F");
      t->Branch("gendphijt", jets_.gendphijt, "gendphijt[ngen]/F");
      t->Branch("gendrjt", jets_.gendrjt, "gendrjt[ngen]/F");

      t->Branch("gsjt1Pt", jets_.gsjt1Pt, "gsjt1Pt[ngen]/F");
      t->Branch("gsjt1Eta", jets_.gsjt1Eta, "gsjt1Eta[ngen]/F");
      t->Branch("gsjt1Phi", jets_.gsjt1Phi, "gsjt1Phi[ngen]/F");
      t->Branch("gsjt1E", jets_.gsjt1E, "gsjt1E[ngen]/F");
      t->Branch("gsjt1Y", jets_.gsjt1Y, "gsjt1Y[ngen]/F");
      t->Branch("gsjt1Pz", jets_.gsjt1Pz, "gsjt1Pz[ngen]/F");
      t->Branch("gsjt2Pt", jets_.gsjt2Pt, "gsjt2Pt[ngen]/F");
      t->Branch("gsjt2Eta", jets_.gsjt2Eta, "gsjt2Eta[ngen]/F");
      t->Branch("gsjt2Phi", jets_.gsjt2Phi, "gsjt2Phi[ngen]/F");
      t->Branch("gsjt2E", jets_.gsjt2E, "gsjt2E[ngen]/F");
      t->Branch("gsjt2Y", jets_.gsjt2Y, "gsjt2Y[ngen]/F");
      t->Branch("gsjt2Pz", jets_.gsjt2Pz, "gsjt2Pz[ngen]/F");



      //for reWTA reclustering
      if (doWTARecluster_) {
        t->Branch("WTAgeneta", jets_.WTAgeneta, "WTAgeneta[ngen]/F");
        t->Branch("WTAgenphi", jets_.WTAgenphi, "WTAgenphi[ngen]/F");
      }

      if (doGenSubJets_) {
        //t->Branch("genptG", jets_.genptG, "genptG[ngen]/F");
        //t->Branch("genetaG", jets_.genetaG, "genetaG[ngen]/F");
        //t->Branch("genphiG", jets_.genphiG, "genphiG[ngen]/F");
        //t->Branch("genmG", jets_.genmG, "genmG[ngen]/F");
        //t->Branch("genSubJetPt", &jets_.genSubJetPt);
        //t->Branch("genSubJetEta", &jets_.genSubJetEta);
        //t->Branch("genSubJetPhi", &jets_.genSubJetPhi);
        //t->Branch("genSubJetM", &jets_.genSubJetM);
        //t->Branch("gensym", jets_.gensym, "gensym[ngen]/F");
        //t->Branch("gendroppedBranches", jets_.gendroppedBranches, "gendroppedBranches[ngen]/I");
      }

      if (doJetConstituents_) {
        t->Branch("genConstituentsId", &jets_.genConstituentsId);
        t->Branch("genConstituentsE", &jets_.genConstituentsE);
        t->Branch("genConstituentsPt", &jets_.genConstituentsPt);
        t->Branch("genConstituentsEta", &jets_.genConstituentsEta);
        t->Branch("genConstituentsPhi", &jets_.genConstituentsPhi);
        t->Branch("genConstituentsM", &jets_.genConstituentsM);
        t->Branch("genSDConstituentsId", &jets_.genSDConstituentsId);
        t->Branch("genSDConstituentsE", &jets_.genSDConstituentsE);
        t->Branch("genSDConstituentsPt", &jets_.genSDConstituentsPt);
        t->Branch("genSDConstituentsEta", &jets_.genSDConstituentsEta);
        t->Branch("genSDConstituentsPhi", &jets_.genSDConstituentsPhi);
        t->Branch("genSDConstituentsM", &jets_.genSDConstituentsM);
      }

      if (doSubEvent_) {
        t->Branch("gensubid", jets_.gensubid, "gensubid[ngen]/I");
      }

      if (doJetTrueFlavour_) { 
	/*
        t->Branch("jtHadFlav", jets_.jtHadFlav, "jtHadFlav[nref]/I");
        t->Branch("jtParFlav", jets_.jtParFlav, "jtParFlav[nref]/I");
        t->Branch("jtNbHad", jets_.jtNbHad, "jtNbHad[nref]/I");
        t->Branch("jtNcHad", jets_.jtNcHad, "jtNcHad[nref]/I");
        t->Branch("jtNbPar", jets_.jtNbPar, "jtNbPar[nref]/I");
        t->Branch("jtNcPar", jets_.jtNcPar, "jtNcPar[nref]/I");
	*/
      }
    }
  }

  if (doLegacyBtagging_) {
    /* clear arrays */
    memset(jets_.discr_csvV2, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_muByIp3, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_muByPt, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_prob, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_probb, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_tcHighEff, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_tcHighPur, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_ssvHighEff, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_ssvHighPur, 0, MAXJETS * sizeof(float));
  }
  if (doCandidateBtagging_) {
    memset(jets_.discr_deepCSV, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_pfJP, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_deepFlavour_b, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_deepFlavour_bb, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_deepFlavour_lepb, 0, MAXJETS * sizeof(float));
    memset(jets_.discr_deepFlavour_c, 0, MAXJETS * sizeof(float));
  }
}

void HiInclusiveJetAnalyzer::analyze(const Event& iEvent, const EventSetup& iSetup) {
  int event = iEvent.id().event();
  int run = iEvent.id().run();
  int lumi = iEvent.id().luminosityBlock();

  jets_.run = run;
  jets_.evt = event;
  jets_.lumi = lumi;

  LogDebug("HiInclusiveJetAnalyzer") << "START event: " << event << " in run " << run << endl;

  // loop the events
  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jetTag_, jets);

  edm::Handle<pat::JetCollection> matchedjets;
  iEvent.getByToken(matchTag_, matchedjets);

  edm::Handle<pat::JetCollection> mujets;
  if (doMujets_) iEvent.getByToken(mujetTag_, mujets);

  edm::Handle<reco::JetTagCollection> jetTagInfos;
  iEvent.getByToken(jetTagInfos_, jetTagInfos);


  if (doGenSym_) {
    iEvent.getByToken(tokenGenSym_, genSymVM_);
    iEvent.getByToken(tokenGenDroppedBranches_, genDroppedBranchesVM_);
  }


  //edm::Handle<double> rho;
  //iEvent.getByToken(rho_, rho);
  
  if (doSubJetsNew_) {
    iEvent.getByToken(groomedJetsToken_, groomedJets);
    if (isMC_) {
      iEvent.getByToken(groomedGenJetsToken_, groomedGenJets);
    }
  }

  edm::Handle<edm::View<reco::PFCandidate>> droppedTracks; // pointer to container of pointers
  edm::Handle<reco::TrackToGenParticleMap> trackToGenParticleMap;
  if (doTracks_) {
    if (isMC_) {
      iEvent.getByToken(trackToGenParticleMapToken_, trackToGenParticleMap);
      // std::cout << "before getByToken" << std::endl;
      iEvent.getByToken(droppedTracksToken_, droppedTracks);
      // std::cout << "after getByToken, ndroppedTracks=" << droppedTracks->size() << std::endl;
    }
  }

  edm::Handle<edm::View<pat::PackedCandidate>> pfCandidates;
  iEvent.getByToken(pfCandidateLabel_, pfCandidates);
  edm::Handle<std::vector<pat::PackedGenParticle>> genParticles;
  //edm::Handle<reco::JetFlavourInfoMatchingCollection> jetFlavourInfos;
  if (isMC_) {
    iEvent.getByToken(genParticleSrc_, genParticles);
    //iEvent.getByToken(jetFlavourInfosToken_, jetFlavourInfos );
  }

  iEvent.getByToken(primaryVerticesToken_, primaryVertices);

  // FILL JRA TREE
  jets_.nref = 0;
  jets_.nvtx = primaryVertices->size();
  //jets_.rho = (float) *rho;
  // std::cout << *rho << ", " << jets_.rho << std::endl;
  
  if (doTracks_) jets_.ntrk = 0;
  if (doSvtx_) {
    jets_.nsvtx = 0;
    jets_.ntrkInSvtxNotInJet = 0;
  }
  int nsvtxCounterForTracks = 0;

  if (doJetConstituents_) {
    jets_.jtConstituentsId.clear();
    jets_.jtConstituentsE.clear();
    jets_.jtConstituentsPt.clear();
    jets_.jtConstituentsEta.clear();
    jets_.jtConstituentsPhi.clear();
    jets_.jtConstituentsM.clear();
    jets_.jtSDConstituentsE.clear();
    jets_.jtSDConstituentsPt.clear();
    jets_.jtSDConstituentsEta.clear();
    jets_.jtSDConstituentsPhi.clear();
    jets_.jtSDConstituentsM.clear();

    jets_.refConstituentsId.clear();
    jets_.refConstituentsE.clear();
    jets_.refConstituentsPt.clear();
    jets_.refConstituentsEta.clear();
    jets_.refConstituentsPhi.clear();
    jets_.refConstituentsM.clear();
    jets_.refSDConstituentsE.clear();
    jets_.refSDConstituentsPt.clear();
    jets_.refSDConstituentsEta.clear();
    jets_.refSDConstituentsPhi.clear();
    jets_.refSDConstituentsM.clear();

    jets_.genConstituentsId.clear();
    jets_.genConstituentsE.clear();
    jets_.genConstituentsPt.clear();
    jets_.genConstituentsEta.clear();
    jets_.genConstituentsPhi.clear();
    jets_.genConstituentsM.clear();
    jets_.genSDConstituentsE.clear();
    jets_.genSDConstituentsPt.clear();
    jets_.genSDConstituentsEta.clear();
    jets_.genSDConstituentsPhi.clear();
    jets_.genSDConstituentsM.clear();
  }

  for (unsigned int j = 0; j < jets->size(); ++j) {
    const pat::Jet& jet = (*jets)[j];
    // std::cout << "new jet with pt = " << jet.pt() << std::endl;
    auto pt = useRawPt_ ? jet.correctedJet("Uncorrected").pt() : jet.pt();
    if (pt < jetPtMin_)
      continue;
    if (std::abs(jet.eta()) > jetAbsEtaMax_)
      continue;

    if (doMujets_) {
      bool isMujet = false;
      double eps=1e-4;
      for (auto mujet : *mujets) {
        if (abs(jet.eta()-mujet.eta())>eps) continue;
        if (abs(jet.phi()-mujet.phi())>eps) continue;
        if (abs(jet.pt()-mujet.pt())>eps) continue;
        isMujet = true;
        // std::cout << "jtpt=" << jet.pt() << ", mujetpt=" << mujet.pt() << std::endl;
        break;
      }    

      if (!isMujet) continue;
    }

    if (doCandidateBtagging_){
      jets_.discr_deepCSV[jets_.nref] = jet.bDiscriminator(deepCSVJetTags_);
      // jets_.discr_pfJP[jets_.nref] = jet.bDiscriminator(pfJPJetTags_);
      jets_.discr_pfJP[jets_.nref] = (*jetTagInfos)[j].second;
      jets_.discr_deepFlavour_b[jets_.nref] = jet.bDiscriminator(deepFlavourJetTags_ + ":probb");
      jets_.discr_deepFlavour_bb[jets_.nref] = jet.bDiscriminator(deepFlavourJetTags_ + ":probbb");
      jets_.discr_deepFlavour_lepb[jets_.nref] = jet.bDiscriminator(deepFlavourJetTags_ + ":problepb");
      jets_.discr_deepFlavour_c[jets_.nref] = jet.bDiscriminator(deepFlavourJetTags_ + ":probbb");
      jets_.discr_particleNet_bb[jets_.nref] = jet.bDiscriminator(deepFlavourJetTags_ + ":probc");
      jets_.discr_particleNet_pu[jets_.nref] = jet.bDiscriminator(particleNetJetTags_ + ":probpu");
      jets_.discr_particleNet_cc[jets_.nref] = jet.bDiscriminator(particleNetJetTags_ + ":probcc");
      jets_.discr_particleNet_undef[jets_.nref] = jet.bDiscriminator(particleNetJetTags_ + ":probundef");
      jets_.discr_particleNet_c[jets_.nref] = jet.bDiscriminator(particleNetJetTags_ + ":probc");
      jets_.discr_particleNet_b[jets_.nref] = jet.bDiscriminator(particleNetJetTags_ + ":probb");
      jets_.discr_particleNet_uds[jets_.nref] = jet.bDiscriminator(particleNetJetTags_ + ":probuds");
      jets_.discr_particleNet_g[jets_.nref] = jet.bDiscriminator(particleNetJetTags_ + ":probg");
      jets_.discr_particleNet_BvsAll[jets_.nref] = jet.bDiscriminator(particleNetDiscriminatorsJetTags_ + ":BvsAll");
      jets_.discr_particleNet_CvsB[jets_.nref] = jet.bDiscriminator(particleNetDiscriminatorsJetTags_ + ":CvsB");
      jets_.discr_particleNet_CvsL[jets_.nref] = jet.bDiscriminator(particleNetDiscriminatorsJetTags_ + ":CvsL");
      jets_.discr_particleNet_QvsG[jets_.nref] = jet.bDiscriminator(particleNetDiscriminatorsJetTags_ + ":QvsG");

      // for (auto discrPair : jet.getPairDiscri()) {
      //   TString label = discrPair.first;
      //   // if (!label.Contains("Deep") && !label.Contains("deep") && !label.Contains("Net")) continue;
      //   if (!label.Contains("Net")) continue;
      //   // if (!label.Contains("pfJetProbabilityBJetTags")) continue;
      //   std::cout << label << " : " << discrPair.second << std::endl; 
      // }
      // std::cout << "end of b tagging" << std::endl;
    }
    if (doLegacyBtagging_) {
      jets_.discr_ssvHighEff[jets_.nref] = jet.bDiscriminator(simpleSVHighEffBJetTags_);
      jets_.discr_ssvHighPur[jets_.nref] = jet.bDiscriminator(simpleSVHighPurBJetTags_);
      jets_.discr_csvV2[jets_.nref] = jet.bDiscriminator(combinedSVV2BJetTags_);
      jets_.discr_prob[jets_.nref] = jet.bDiscriminator(jetPBJetTags_);
      jets_.discr_probb[jets_.nref] = jet.bDiscriminator(jetBPBJetTags_);
      jets_.discr_tcHighEff[jets_.nref] = jet.bDiscriminator(trackCHEBJetTags_);
      jets_.discr_tcHighPur[jets_.nref] = jet.bDiscriminator(trackCHPBJetTags_);

      const edm::View<pat::PackedCandidate>* pfCandidateColl = &(*pfCandidates);
      int pfMuonIndex = getPFJetMuon(jet, pfCandidateColl);

      if (pfMuonIndex >= 0) {
        const pat::PackedCandidate muon = pfCandidates->at(pfMuonIndex);
        jets_.mupt[jets_.nref] = muon.pt();
        jets_.mueta[jets_.nref] = muon.eta();
        jets_.muphi[jets_.nref] = muon.phi();
        jets_.mue[jets_.nref] = muon.energy();
        jets_.mudr[jets_.nref] = reco::deltaR(jet, muon);
        jets_.muptrel[jets_.nref] = getPtRel(muon, jet);
        jets_.muchg[jets_.nref] = muon.charge();
      } else {
        jets_.mupt[jets_.nref] = 0.0;
        jets_.mueta[jets_.nref] = 0.0;
        jets_.muphi[jets_.nref] = 0.0;
        jets_.mue[jets_.nref] = 0.0;
        jets_.mudr[jets_.nref] = 9.9;
        jets_.muptrel[jets_.nref] = 0.0;
        jets_.muchg[jets_.nref] = 0;
      }
    }

    // TEST: count tracks in SVs but not in jets
    if (false) {
      const reco::CandSecondaryVertexTagInfo *svTagInfo = jet.tagInfoCandSecondaryVertex(svTagInfoLabel_.c_str());

      int tracksNotInJet = 0;

      int nsv = svTagInfo->nVertices();
      for (int isv = 0; isv < nsv; isv++) {
        const std::vector<reco::CandidatePtr> svTracks = svTagInfo->vertexTracks(isv);
        for (auto svTrk : svTracks) {
          // look for track in this jet's tracks 
          const std::vector<reco::CandidatePtr> jetTracks = jet.getJetConstituents();
          auto itJetTrack = std::find(jetTracks.begin(), jetTracks.end(), svTrk);
          if (itJetTrack == jetTracks.end()) {
            tracksNotInJet++;

            // look for track in the other jets in the event
            bool trkInOtherJet = false;
            for (unsigned int ji = 0; ji < jets->size(); ++ji) {
              if (ji == j) continue; // skip the same jet
              const pat::Jet& tempJet = (*jets)[ji];
              const std::vector<reco::CandidatePtr> tempJetTracks = tempJet.getJetConstituents();
              auto itTempJetTrack = std::find(tempJetTracks.begin(), tempJetTracks.end(), svTrk);
              if (itTempJetTrack == tempJetTracks.end()) {
                continue;
              }
              trkInOtherJet = true;
              break;
            } // other jet loop

            // check if track is in ipTagInfo data
            const reco::CandIPTagInfo *ipTagInfo = jet.tagInfoCandIP(ipTagInfoLabel_.c_str());
            const std::vector<reco::CandidatePtr> ipTracks = ipTagInfo->selectedTracks();
            auto itIpTrk = std::find(ipTracks.begin(), ipTracks.end(), svTrk);
            bool trkInIPTagInfo = !(itIpTrk == ipTracks.end());

            // get status of track 
            int status = -1;
            if (isMC_ && trackToGenParticleMap->find(svTrk) != trackToGenParticleMap->end()) { 
              edm::Ptr<pat::PackedGenParticle> matchGenParticle = trackToGenParticleMap->at(svTrk);
              status = matchGenParticle->status();
            }
            std::cout << "!!! SV track not in assigned jet !!!" 
                      << "\t track found in other jet: " << trkInOtherJet
                      << "\t track in ipTagInfo: " << trkInIPTagInfo
                      << "\t track status: " << status 
                      << std::endl;
          } // end if track not found in jet
        } // svTrk loop
      } // sv loop
      if (tracksNotInJet > 0)
        std::cout << "tracks not in assigned jet: " << tracksNotInJet << std::endl;
    }

    // std::cout << "New jet with nref " << jets_.nref << std::endl;
    // std::cout << svTagInfos_ << std::endl;
    // std::cout << "start of svtx" << std::endl;
    if (doSvtx_ && jet.hasTagInfo(svTagInfoLabel_.c_str())) {
      const reco::CandSecondaryVertexTagInfo *svTagInfo = jet.tagInfoCandSecondaryVertex(svTagInfoLabel_.c_str());
      int nsv = svTagInfo->nVertices();
      jets_.jtNsvtx[jets_.nref] = 0;
      for (int isv = 0; isv < nsv; isv++) {
        int ijetSvtx = jets_.nsvtx + isv;
        jets_.svtxNtrk[ijetSvtx] = svTagInfo->nVertexTracks(isv);

        Measurement1D dl3d = svTagInfo->flightDistance(isv);
        jets_.svtxdl[ijetSvtx] = dl3d.value();
        jets_.svtxdls[ijetSvtx] = dl3d.significance();

        Measurement1D dl2d = svTagInfo->flightDistance(isv, 2);
        jets_.svtxdl2d[ijetSvtx] = dl2d.value();
        jets_.svtxdls2d[ijetSvtx] = dl2d.significance();

        const VertexCompositePtrCandidate svtx = svTagInfo->secondaryVertex(isv);
        double svtxM = svtx.p4().mass();
        double svtxPt = svtx.p4().pt();
        double normalizedChi2 = svtx.vertexNormalizedChi2();

        //mCorr=srqt(m^2+p^2sin^2(th)) + p*sin(th)
        double sinth = svtx.p4().Vect().Unit().Cross((svTagInfo->flightDirection(isv)).unit()).Mag2();
        sinth = sqrt(sinth);
        double underRoot = std::pow(svtxM, 2) + (std::pow(svtxPt, 2) * std::pow(sinth, 2));
        double svtxMcorr = std::sqrt(underRoot) + (svtxPt * sinth);

        jets_.svtxnormchi2[ijetSvtx] = normalizedChi2;
        jets_.svtxm[ijetSvtx] = svtxM;
        jets_.svtxmcorr[ijetSvtx] = svtxMcorr;
        jets_.svtxpt[ijetSvtx] = svtxPt;

        jets_.svtxJetId[ijetSvtx] = jets_.nref;

        const std::vector<reco::CandidatePtr> svTracks = svTagInfo->vertexTracks(isv);
        // std::cout << "nsvtracks = " << svTracks.size() << std::endl;
        for (auto svTrk : svTracks) {
          // look for track in this jet's tracks 
          const std::vector<reco::CandidatePtr> jetTracks = jet.getJetConstituents();
          auto itJetTrack = std::find(jetTracks.begin(), jetTracks.end(), svTrk);
          if (itJetTrack == jetTracks.end()) {
            jets_.trkInSvtxNotInJetSvId[jets_.ntrkInSvtxNotInJet] = ijetSvtx;
            jets_.trkInSvtxNotInJetPt[jets_.ntrkInSvtxNotInJet] = svTrk->pt();
            jets_.trkInSvtxNotInJetEta[jets_.ntrkInSvtxNotInJet] = svTrk->eta();
            jets_.trkInSvtxNotInJetPhi[jets_.ntrkInSvtxNotInJet] = svTrk->phi();

            // look for track in the other jets in the event
            jets_.trkInSvtxNotInJetOtherJetId[jets_.ntrkInSvtxNotInJet] = -1;
            for (unsigned int ji = 0; ji < jets->size(); ++ji) {
              if (ji == j) continue; // skip the same jet
              const pat::Jet& tempJet = (*jets)[ji];
              const std::vector<reco::CandidatePtr> tempJetTracks = tempJet.getJetConstituents();
              auto itTempJetTrack = std::find(tempJetTracks.begin(), tempJetTracks.end(), svTrk);
              if (itTempJetTrack == tempJetTracks.end()) {
                continue;
              }
              jets_.trkInSvtxNotInJetOtherJetId[jets_.ntrkInSvtxNotInJet] = ji;
              break;
            } // other jet loop

            // get status of track 
            int status = -1;
            if (isMC_ && trackToGenParticleMap->find(svTrk) != trackToGenParticleMap->end()) { 
              edm::Ptr<pat::PackedGenParticle> matchGenParticle = trackToGenParticleMap->at(svTrk);
              status = matchGenParticle->status();
            }
            jets_.trkInSvtxNotInJetMatchSta[jets_.ntrkInSvtxNotInJet] = status;

            // std::cout << "jets_.ntrkInSvtxNotInJet = " << jets_.ntrkInSvtxNotInJet << std::endl;
            jets_.ntrkInSvtxNotInJet++;
          }
        } // sv track loop
      } // sv loop
      jets_.jtNsvtx[jets_.nref] = nsv;
      jets_.nsvtx += nsv;
    } // endif doSvtx_
    // std::cout << "end of svtx" << std::endl;
    // std::cout << "Jet has tag infos: " << std::endl;
    // for (auto label : jet.tagInfoLabels()) {
    //   std::cout << label << std::endl;
    // }

    if (doTracks_ && jet.hasTagInfo(ipTagInfoLabel_.c_str())) {
      jets_.jtNtrk[jets_.nref] = 0;
      jets_.jtptCh[jets_.nref] = 0.;
      const reco::CandIPTagInfo *ipTagInfo = jet.tagInfoCandIP(ipTagInfoLabel_.c_str());
      const std::vector<reco::btag::TrackIPData> ipData = ipTagInfo->impactParameterData();
      const std::vector<reco::CandidatePtr> ipTracks = ipTagInfo->selectedTracks();

      reco::Candidate::PolarLorentzVector chJet(0., 0., 0., 0.);

      for (const reco::CandidatePtr constit : jet.getJetConstituents()) {
        // std::cout << "new jet constit with pt " << constit->pt() << std::endl;
        if (constit->charge() == 0) continue;
        if (constit->pt() < trkPtCut_) continue;        

        auto itIPTrack = std::find(ipTracks.begin(), ipTracks.end(), constit);
        if (itIPTrack == ipTracks.end()) continue;

        

        // Check if the track was dropped from the aggregation
        if (isMC_) {
          bool drop = false;
          double eps = 1e-4;
          // std::cout << "before droppedTracks" << std::endl;
          for (size_t idropped=0; idropped<droppedTracks->size(); idropped++) {
            reco::PFCandidate droppedTrack = (*droppedTracks)[idropped];
            if (std::abs(constit->eta()-droppedTrack.eta())>eps) continue;
            if (std::abs(constit->phi()-droppedTrack.phi())>eps) continue;
            if (std::abs(constit->pt()-droppedTrack.pt())>eps) continue;
            drop = true;
          }
          if (drop) continue;
          // std::cout << "after droppedTracks" << std::endl;
        }

        int ijetTrack = jets_.ntrk + jets_.jtNtrk[jets_.nref];

        reco::Candidate::PolarLorentzVector constitV(0., 0., 0., 0.);
        constitV.SetPt(constit->pt());
        constitV.SetEta(constit->eta());
        constitV.SetPhi(constit->phi());
        constitV.SetM(constit->mass());
        chJet += constitV;

        int itrk = itIPTrack - ipTracks.begin();
        const reco::btag::TrackIPData trkIPData = ipData[itrk];

        jets_.trkJetId[ijetTrack] = jets_.nref;  

        jets_.trkPt[ijetTrack] = constit->pt();
        jets_.trkEta[ijetTrack] = constit->eta();
        jets_.trkPhi[ijetTrack] = constit->phi();

        jets_.trkIp3d[ijetTrack] = trkIPData.ip3d.value();
        jets_.trkIp3dSig[ijetTrack] = trkIPData.ip3d.significance();

        jets_.trkIp2d[ijetTrack] = trkIPData.ip2d.value();
        jets_.trkIp2dSig[ijetTrack] = trkIPData.ip2d.significance();

        jets_.trkIpProb3d[ijetTrack] = ipTagInfo->probabilities(0)[itrk];
        jets_.trkIpProb2d[ijetTrack] = ipTagInfo->probabilities(1)[itrk];

        jets_.trkDistToAxis[ijetTrack] = trkIPData.distanceToJetAxis.value();
        jets_.trkDistToAxisSig[ijetTrack] = trkIPData.distanceToJetAxis.significance();

        jets_.trkSvtxId[ijetTrack] = -1;
        if (doSvtx_ && jet.hasTagInfo(svTagInfoLabel_.c_str())) {
          const reco::CandSecondaryVertexTagInfo *svTagInfo = jet.tagInfoCandSecondaryVertex(svTagInfoLabel_.c_str());
          int nsv = svTagInfo->nVertices();
          for (int isv = 0; isv < nsv; isv++) {
            const std::vector<reco::CandidatePtr> svTracks = svTagInfo->vertexTracks(isv);
            auto itSVTrack = std::find(svTracks.begin(), svTracks.end(), constit);
            if (itSVTrack == svTracks.end()) continue;
            jets_.trkSvtxId[ijetTrack] = nsvtxCounterForTracks + isv;
          } // end sv loop for tracks
        } // end doSvtx_

        Int_t status = -1; // default, no match
        if (isMC_ && trackToGenParticleMap->find(constit) != trackToGenParticleMap->end()) { 
          edm::Ptr<pat::PackedGenParticle> matchGenParticle = trackToGenParticleMap->at(constit);
          status = matchGenParticle->status();
        }
        jets_.trkMatchSta[ijetTrack] = status;
        jets_.trkPdgId[ijetTrack] = constit->pdgId();

        const reco::Track *constitTrack = constit->bestTrack();
        if (constitTrack) {
          // std::cout << "track exists " << std::endl;
          // std::cout << "testTrack dz " << testTrack->dz() << std::endl;
          jets_.trkDz[ijetTrack] = constitTrack->dz(primaryVertices->at(0).position());
        } else {
          jets_.trkDz[ijetTrack] = -100000.;
        }

        jets_.jtNtrk[jets_.nref]++;
      } // jet constituent loop

      jets_.jtptCh[jets_.nref] = chJet.pt();
      nsvtxCounterForTracks += jets_.jtNsvtx[jets_.nref];
      jets_.ntrk += jets_.jtNtrk[jets_.nref];
    } // endif doTracks_

    if (doHiJetID_) {
      // Jet ID variables

      jets_.muMax[jets_.nref] = 0;
      jets_.muSum[jets_.nref] = 0;
      jets_.muN[jets_.nref] = 0;

      jets_.eMax[jets_.nref] = 0;
      jets_.eSum[jets_.nref] = 0;
      jets_.eN[jets_.nref] = 0;

      jets_.neutralMax[jets_.nref] = 0;
      jets_.neutralSum[jets_.nref] = 0;
      jets_.neutralN[jets_.nref] = 0;

      jets_.photonMax[jets_.nref] = 0;
      jets_.photonSum[jets_.nref] = 0;
      jets_.photonN[jets_.nref] = 0;
      jets_.photonHardSum[jets_.nref] = 0;
      jets_.photonHardN[jets_.nref] = 0;

      jets_.chargedMax[jets_.nref] = 0;
      jets_.chargedSum[jets_.nref] = 0;
      jets_.chargedN[jets_.nref] = 0;
      jets_.chargedHardSum[jets_.nref] = 0;
      jets_.chargedHardN[jets_.nref] = 0;

      jets_.trackMax[jets_.nref] = 0;
      jets_.trackSum[jets_.nref] = 0;
      jets_.trackN[jets_.nref] = 0;
      jets_.trackHardSum[jets_.nref] = 0;
      jets_.trackHardN[jets_.nref] = 0;

      jets_.genChargedSum[jets_.nref] = 0;
      jets_.genHardSum[jets_.nref] = 0;

      jets_.signalChargedSum[jets_.nref] = 0;
      jets_.signalHardSum[jets_.nref] = 0;

      jets_.subid[jets_.nref] = -1;

      for (unsigned int icand = 0; icand < pfCandidates->size(); ++icand) {
        const pat::PackedCandidate& t = (*pfCandidates)[icand];

        if (!t.hasTrackDetails())
          continue;

        reco::Track const& track = t.pseudoTrack();

        if (useQuality_) {
          bool goodtrack = track.quality(reco::TrackBase::qualityByName(trackQuality_));
          if (!goodtrack)
            continue;
        }

        double dr = deltaR(jet, track);
        if (dr < rParam) {
          double ptcand = track.pt();
          jets_.trackSum[jets_.nref] += ptcand;
          jets_.trackN[jets_.nref] += 1;

          if (ptcand > hardPtMin_) {
            jets_.trackHardSum[jets_.nref] += ptcand;
            jets_.trackHardN[jets_.nref] += 1;
          }
          if (ptcand > jets_.trackMax[jets_.nref])
            jets_.trackMax[jets_.nref] = ptcand;
        }
      }

      for (unsigned int icand = 0; icand < pfCandidates->size(); ++icand) {
        const pat::PackedCandidate& track = (*pfCandidates)[icand];
        double dr = deltaR(jet, track);
        if (dr < rParam) {
          double ptcand = track.pt();
          int pfid = track.pdgId();

          switch (pfid) {
            case 1:
              jets_.chargedSum[jets_.nref] += ptcand;
              jets_.chargedN[jets_.nref] += 1;
              if (ptcand > hardPtMin_) {
                jets_.chargedHardSum[jets_.nref] += ptcand;
                jets_.chargedHardN[jets_.nref] += 1;
              }
              if (ptcand > jets_.chargedMax[jets_.nref])
                jets_.chargedMax[jets_.nref] = ptcand;
              break;

            case 2:
              jets_.eSum[jets_.nref] += ptcand;
              jets_.eN[jets_.nref] += 1;
              if (ptcand > jets_.eMax[jets_.nref])
                jets_.eMax[jets_.nref] = ptcand;
              break;

            case 3:
              jets_.muSum[jets_.nref] += ptcand;
              jets_.muN[jets_.nref] += 1;
              if (ptcand > jets_.muMax[jets_.nref])
                jets_.muMax[jets_.nref] = ptcand;
              break;

            case 4:
              jets_.photonSum[jets_.nref] += ptcand;
              jets_.photonN[jets_.nref] += 1;
              if (ptcand > hardPtMin_) {
                jets_.photonHardSum[jets_.nref] += ptcand;
                jets_.photonHardN[jets_.nref] += 1;
              }
              if (ptcand > jets_.photonMax[jets_.nref])
                jets_.photonMax[jets_.nref] = ptcand;
              break;

            case 5:
              jets_.neutralSum[jets_.nref] += ptcand;
              jets_.neutralN[jets_.nref] += 1;
              if (ptcand > jets_.neutralMax[jets_.nref])
                jets_.neutralMax[jets_.nref] = ptcand;
              break;

            default:
              break;
          }
        }
      }
    }

    if (doMatch_) {
      // Alternative reconstruction matching (PF for calo, calo for PF)

      double drMin = 100;
      for (unsigned int imatch = 0; imatch < matchedjets->size(); ++imatch) {
        const pat::Jet& mjet = (*matchedjets)[imatch];

        double dr = deltaR(jet, mjet);
        if (dr < drMin) {
          jets_.matchedPt[jets_.nref] = mjet.pt();

          jets_.matchedRawPt[jets_.nref] = mjet.correctedJet("Uncorrected").pt();
          jets_.matchedPu[jets_.nref] = mjet.pileup();
          if (isMC_) {
            jets_.matchedHadronFlavor[jets_.nref] = mjet.hadronFlavour();
            jets_.matchedPartonFlavor[jets_.nref] = mjet.partonFlavour();
          }

          jets_.matchedR[jets_.nref] = dr;
          drMin = dr;
        }
      }
    }

    jets_.rawpt[jets_.nref] = jet.correctedJet("Uncorrected").pt();
    jets_.jtpt[jets_.nref] = jet.pt();
    // std::cout << "uncorrected = " << jets_.rawpt[jets_.nref] << ", corrected = " << jets_.jtpt[jets_.nref] << std::endl;
    jets_.jteta[jets_.nref] = jet.eta();
    jets_.jtphi[jets_.nref] = jet.phi();
    jets_.jty[jets_.nref] = jet.eta();
    jets_.jtpu[jets_.nref] = jet.pileup();
    jets_.jtm[jets_.nref] = jet.mass();
    jets_.jtarea[jets_.nref] = jet.jetArea();

    //recluster the jet constituents in reWTA scheme-------------------------
    if (doWTARecluster_) {
      std::vector<fastjet::PseudoJet> candidates;
      auto daughters = jet.getJetConstituents();
      for (auto it = daughters.begin(); it != daughters.end(); ++it) {
        candidates.push_back(fastjet::PseudoJet((**it).px(), (**it).py(), (**it).pz(), (**it).energy()));
      }
      auto cs = new fastjet::ClusterSequence(candidates, WTAjtDef);
      std::vector<fastjet::PseudoJet> wtajt = fastjet::sorted_by_pt(cs->inclusive_jets(0));

      jets_.WTAeta[jets_.nref] = (!wtajt.empty()) ? wtajt[0].eta() : -999;
      jets_.WTAphi[jets_.nref] = (!wtajt.empty()) ? wtajt[0].phi_std() : -999;
      delete cs;
    }
    //------------------------------------------------------------------

    jets_.jttau1[jets_.nref] = -999.;
    jets_.jttau2[jets_.nref] = -999.;
    jets_.jttau3[jets_.nref] = -999.;

    jets_.jtsym[jets_.nref] = -999.;
    jets_.jtdroppedBranches[jets_.nref] = -999;

    if (doSubJets_)
      analyzeSubjets(jet);

    if (doSubJetsNew_) {
      int iGroomedJet = getGroomedJetIndex(jet, *groomedJets);
      if (iGroomedJet > -1) {
        const reco::Jet& groomedJet = (*groomedJets)[iGroomedJet];
        // std::cout << "groomed jet has " << groomedJet.numberOfDaughters() << " daughters" << std::endl;
        if (groomedJet.numberOfDaughters() > 0) {
          const Candidate & sjt1 = *groomedJet.daughter(0);       
          jets_.sjt1E[jets_.nref] = sjt1.energy();
          jets_.sjt1Y[jets_.nref] = sjt1.y();
          jets_.sjt1Pz[jets_.nref] = sjt1.pz();
          jets_.sjt1Pt[jets_.nref] = sjt1.pt();
          jets_.sjt1Eta[jets_.nref] = sjt1.eta();
          jets_.sjt1Phi[jets_.nref] = sjt1.phi();

          if (groomedJet.jetArea() > 0.5) jets_.sjt1HasHF[jets_.nref] = 1;
          else jets_.sjt1HasHF[jets_.nref] = 0;

          // std::cout << "new sjt" << std::endl;
          // std::cout << "\tsjt1Y = " << sjt1.y() << std::endl;
          // std::cout << "\tsjt1E = " << sjt1.energy() << std::endl;
          // std::cout << "\tsjt1Pz = " << sjt1.pz() << std::endl;
          // std::cout << "\tsjt1y calc = " << 0.5*sjt1.pz() << std::endl;
          
          if (groomedJet.numberOfDaughters() > 1) {
            const Candidate & sjt2 = *groomedJet.daughter(1);
            jets_.sjt2E[jets_.nref] = sjt2.energy();
            jets_.sjt2Y[jets_.nref] = sjt2.y();
            jets_.sjt2Pz[jets_.nref] = sjt2.pz();
            jets_.sjt2Pt[jets_.nref] = sjt2.pt();
            jets_.sjt2Eta[jets_.nref] = sjt2.eta();
            jets_.sjt2Phi[jets_.nref] = sjt2.phi();
          } else{
            jets_.sjt2Pt[jets_.nref] = -1;
            jets_.sjt2Eta[jets_.nref] = -999;
            jets_.sjt2Phi[jets_.nref] = -999;
            jets_.sjt2E[jets_.nref] = -1;
            jets_.sjt2Y[jets_.nref] = -999;
            jets_.sjt2Pz[jets_.nref] = -999;
          }
        } else {
          jets_.sjt1HasHF[jets_.nref] = -1;

          jets_.sjt1E[jets_.nref] = -1;
          jets_.sjt1Y[jets_.nref] = -999;
          jets_.sjt1Pz[jets_.nref] = -999;
          jets_.sjt1Pt[jets_.nref] = -1;
          jets_.sjt1Eta[jets_.nref] = -999;
          jets_.sjt1Phi[jets_.nref] = -999;
        }     
      } // end if groomedJet match exists
    
      if (isMC_) {
        const reco::GenJet *genJet = jet.genJet();
        // std::cout << " jet " 
        //             << " pt " << jet.pt()
        //             << " eta " << jet.eta()
        //             << " phi " << jet.phi()
        //             << std::endl;
        if (genJet) {
          // std::cout << " gen jet " 
          //           << " pt " << (*genJet).pt()
          //           << " eta " << (*genJet).eta()
          //           << " phi " << (*genJet).phi()
          //           << std::endl;
          int iGroomedGenJet = getGroomedJetIndex(*genJet, *groomedGenJets);
          // int iGroomedGenJet = -1;
          // std::cout << "groomed jets type: " << typeid(*groomedGenJets).name() << std::endl;
          if (iGroomedGenJet > -1) {
            const reco::Jet& groomedGenJet = (*groomedGenJets)[iGroomedGenJet];
            // std::cout << "groomed jet has " << groomedGenJet.numberOfDaughters() << " daughters" << std::endl;
            if (groomedGenJet.numberOfDaughters() > 0) {
              const Candidate & rsjt1 = *groomedGenJet.daughter(0);       
              jets_.rsjt1E[jets_.nref] = rsjt1.energy();
              jets_.rsjt1Y[jets_.nref] = rsjt1.y();
              jets_.rsjt1Pz[jets_.nref] = rsjt1.pz();
              jets_.rsjt1Pt[jets_.nref] = rsjt1.pt();
              jets_.rsjt1Eta[jets_.nref] = rsjt1.eta();
              jets_.rsjt1Phi[jets_.nref] = rsjt1.phi();

              
              if (groomedGenJet.numberOfDaughters() > 1) {
                const Candidate & rsjt2 = *groomedGenJet.daughter(1);
                jets_.rsjt2E[jets_.nref] = rsjt2.energy();
                jets_.rsjt2Y[jets_.nref] = rsjt2.y();
                jets_.rsjt2Pz[jets_.nref] = rsjt2.pz();
                jets_.rsjt2Pt[jets_.nref] = rsjt2.pt();
                jets_.rsjt2Eta[jets_.nref] = rsjt2.eta();
                jets_.rsjt2Phi[jets_.nref] = rsjt2.phi();
              } else{
                jets_.rsjt2Pt[jets_.nref] = -1;
                jets_.rsjt2Eta[jets_.nref] = -999;
                jets_.rsjt2Phi[jets_.nref] = -999;
                jets_.rsjt2E[jets_.nref] = -1;
                jets_.rsjt2Y[jets_.nref] = -999;
                jets_.rsjt2Pz[jets_.nref] = -999;
              }
            } else {
              jets_.rsjt1HasHF[jets_.nref] = -1;

              jets_.rsjt1E[jets_.nref] = -1;
              jets_.rsjt1Y[jets_.nref] = -999;
              jets_.rsjt1Pz[jets_.nref] = -999;
              jets_.rsjt1Pt[jets_.nref] = -1;
              jets_.rsjt1Eta[jets_.nref] = -999;
              jets_.rsjt1Phi[jets_.nref] = -999;
            }   
          } // end if groomedGenJet match exists
          // get charged pt of gen jet
          jets_.refptCh[jets_.nref] = 0.;
          jets_.refNtrk[jets_.nref] = 0;

          reco::Candidate::PolarLorentzVector chGenJet(0., 0., 0., 0.);

          for (auto genConstit : genJet->getJetConstituents()) {
            if (genConstit->pt() < trkPtCut_) continue;
            if (genConstit->charge() == 0) continue;

            bool isNeutrino = (genConstit->pdgId() == 12); // nue
            isNeutrino &= (genConstit->pdgId() == 14); // numu
            isNeutrino &= (genConstit->pdgId() == 16); // nutau
            isNeutrino &= (genConstit->pdgId() == 18); // nutau'
            if (isNeutrino) continue;

            reco::Candidate::PolarLorentzVector constitV(0., 0., 0., 0.);
            constitV.SetPt(genConstit->pt());
            constitV.SetEta(genConstit->eta());
            constitV.SetPhi(genConstit->phi());
            constitV.SetM(genConstit->mass());
            chGenJet += constitV;
            
            jets_.refNtrk[jets_.nref] += 1;
          }

          jets_.refptCh[jets_.nref] = chGenJet.pt();
        } // end if matched gen jet exists
      } // end if isMC_
    } // end if doSubJetsNew_

    if (jet.hasUserFloat(jetName_ + "Njettiness:tau1"))
      jets_.jttau1[jets_.nref] = jet.userFloat(jetName_ + "Njettiness:tau1");
    if (jet.hasUserFloat(jetName_ + "Njettiness:tau2"))
      jets_.jttau2[jets_.nref] = jet.userFloat(jetName_ + "Njettiness:tau2");
    if (jet.hasUserFloat(jetName_ + "Njettiness:tau3"))
      jets_.jttau3[jets_.nref] = jet.userFloat(jetName_ + "Njettiness:tau3");

    if (jet.hasUserFloat(jetName_ + "Jets:sym"))
      jets_.jtsym[jets_.nref] = jet.userFloat(jetName_ + "Jets:sym");
    if (jet.hasUserInt(jetName_ + "Jets:droppedBranches"))
      jets_.jtdroppedBranches[jets_.nref] = jet.userInt(jetName_ + "Jets:droppedBranches");

    if (jet.isPFJet()) {
      jets_.jtPfCHF[jets_.nref] = jet.chargedHadronEnergyFraction();
      jets_.jtPfNHF[jets_.nref] = jet.neutralHadronEnergyFraction();
      jets_.jtPfCEF[jets_.nref] = jet.chargedEmEnergyFraction();
      jets_.jtPfNEF[jets_.nref] = jet.neutralEmEnergyFraction();
      jets_.jtPfMUF[jets_.nref] = jet.muonEnergyFraction();

      jets_.jtPfCHM[jets_.nref] = jet.chargedHadronMultiplicity();
      jets_.jtPfNHM[jets_.nref] = jet.neutralHadronMultiplicity();
      jets_.jtPfCEM[jets_.nref] = jet.electronMultiplicity();
      jets_.jtPfNEM[jets_.nref] = jet.photonMultiplicity();
      jets_.jtPfMUM[jets_.nref] = jet.muonMultiplicity();
    } else {
      jets_.jtPfCHF[jets_.nref] = 0;
      jets_.jtPfNHF[jets_.nref] = 0;
      jets_.jtPfCEF[jets_.nref] = 0;
      jets_.jtPfNEF[jets_.nref] = 0;
      jets_.jtPfMUF[jets_.nref] = 0;

      jets_.jtPfCHM[jets_.nref] = 0;
      jets_.jtPfNHM[jets_.nref] = 0;
      jets_.jtPfCEM[jets_.nref] = 0;
      jets_.jtPfNEM[jets_.nref] = 0;
      jets_.jtPfMUM[jets_.nref] = 0;
    }

    //    if(isMC_){

    //      for(UInt_t i = 0; i < genParticles->size(); ++i){
    // const reco::GenParticle& p = (*genParticles)[i];
    // if ( p.status()!=1 || p.charge()==0) continue;
    // double dr = deltaR(jet,p);
    // if(dr < rParam){
    //   double ppt = p.pt();
    //   jets_.genChargedSum[jets_.nref] += ppt;
    //   if(ppt > hardPtMin_) jets_.genHardSum[jets_.nref] += ppt;
    //   if(p.collisionId() == 0){
    //     jets_.signalChargedSum[jets_.nref] += ppt;
    //     if(ppt > hardPtMin_) jets_.signalHardSum[jets_.nref] += ppt;
    //   }
    // }
    //      }
    //    }

    if (isMC_) {
      const reco::GenJet* genjet = jet.genJet();

      if (genjet) {
        jets_.refpt[jets_.nref] = genjet->pt();
        jets_.refeta[jets_.nref] = genjet->eta();
        jets_.refphi[jets_.nref] = genjet->phi();
        jets_.refm[jets_.nref] = genjet->mass();
        jets_.refarea[jets_.nref] = genjet->jetArea();
        jets_.refy[jets_.nref] = genjet->eta();
        jets_.refdphijt[jets_.nref] = reco::deltaPhi(jet.phi(), genjet->phi());
        jets_.refdrjt[jets_.nref] = reco::deltaR(jet.eta(), jet.phi(), genjet->eta(), genjet->phi());

        if (doSubEvent_) {
          const GenParticle* gencon = genjet->getGenConstituent(0);
          jets_.subid[jets_.nref] = gencon->collisionId();
        }
	/*
        if (doGenSubJets_){	  
          analyzeRefSubjets(*genjet);
	}
	*/
      } else {
        jets_.refpt[jets_.nref] = -999.;
        jets_.refeta[jets_.nref] = -999.;
        jets_.refphi[jets_.nref] = -999.;
        jets_.refm[jets_.nref] = -999.;
        jets_.refarea[jets_.nref] = -999.;
        jets_.refy[jets_.nref] = -999.;
        jets_.refdphijt[jets_.nref] = -999.;
        jets_.refdrjt[jets_.nref] = -999.;

        if (doJetConstituents_) {
          jets_.refConstituentsId.emplace_back(1, -999);
          jets_.refConstituentsE.emplace_back(1, -999);
          jets_.refConstituentsPt.emplace_back(1, -999);
          jets_.refConstituentsEta.emplace_back(1, -999);
          jets_.refConstituentsPhi.emplace_back(1, -999);
          jets_.refConstituentsM.emplace_back(1, -999);

          jets_.refSDConstituentsId.emplace_back(1, -999);
          jets_.refSDConstituentsE.emplace_back(1, -999);
          jets_.refSDConstituentsPt.emplace_back(1, -999);
          jets_.refSDConstituentsEta.emplace_back(1, -999);
          jets_.refSDConstituentsPhi.emplace_back(1, -999);
          jets_.refSDConstituentsM.emplace_back(1, -999);
        }

        if (doGenSubJets_) {
          jets_.refptG[jets_.nref] = -999.;
          jets_.refetaG[jets_.nref] = -999.;
          jets_.refphiG[jets_.nref] = -999.;
          jets_.refmG[jets_.nref] = -999.;
          jets_.refsym[jets_.nref] = -999.;
          //jets_.refdroppedBranches[jets_.nref] = -999;

          jets_.refSubJetPt.emplace_back(1, -999);
          jets_.refSubJetEta.emplace_back(1, -999);
          jets_.refSubJetPhi.emplace_back(1, -999);
          jets_.refSubJetM.emplace_back(1, -999);
        }
      }
      jets_.reftau1[jets_.nref] = -999.;
      jets_.reftau2[jets_.nref] = -999.;
      jets_.reftau3[jets_.nref] = -999.;
      /*
      // Get JER SFs
      JME::JetResolution resolution; // det-level jet is not matched to a truth-level jet
      JME::JetResolutionScaleFactor resolution_sf; // det-level jet is matched to a truth-level jet

      // std::cout << "right before creating the jer objects " << std::endl;
      resolution = JME::JetResolution(edm::FileInPath("HeavyIonsAnalysis/JetAnalysis/data/Summer19UL17_JRV3_MC_PtResolution_AK4PFchs.txt").fullPath());
      resolution_sf = JME::JetResolutionScaleFactor(edm::FileInPath("HeavyIonsAnalysis/JetAnalysis/data/Summer19UL17_JRV3_MC_SF_AK4PFchs.txt").fullPath());

      // std::cout << "created resolution objects" << std::endl;
      JME::JetParameters parameters_1;
      parameters_1.setJetPt(jets_.jtpt[jets_.nref]);
      parameters_1.setJetEta(jets_.jteta[jets_.nref]);
      parameters_1.setRho(0.);

      double sf;
      double sim_resolution = resolution.getResolution(parameters_1);

      sf = resolution_sf.getScaleFactor(parameters_1, Variation::NOMINAL);
      // std::cout << "read the sf" << std::endl;
      if (jets_.refpt[jets_.nref] > 0 ) sf = 1+(sf-1)*(jets_.jtpt[jets_.nref]-jets_.refpt[jets_.nref])/(jets_.jtpt[jets_.nref]); // det-level jet is matched to truth-level jet
      else sf = 1+gRandom->Gaus(0,sim_resolution)*std::sqrt(sf*sf-1); // “stochastic” smearing, which is what you will probably use for HIN
      jets_.jer_sf_nom[jets_.nref] = sf;

      sf = resolution_sf.getScaleFactor(parameters_1, Variation::UP);
      if (jets_.refpt[jets_.nref] > 0 ) sf = 1+(sf-1)*(jets_.jtpt[jets_.nref]-jets_.refpt[jets_.nref])/(jets_.jtpt[jets_.nref]); // det-level jet is matched to truth-level jet
      else sf = 1+gRandom->Gaus(0,sim_resolution)*std::sqrt(sf*sf-1); // “stochastic” smearing, which is what you will probably use for HIN
      jets_.jer_sf_up[jets_.nref] = sf;

      sf = resolution_sf.getScaleFactor(parameters_1, Variation::DOWN);
      if (jets_.refpt[jets_.nref] > 0 ) sf = 1+(sf-1)*(jets_.jtpt[jets_.nref]-jets_.refpt[jets_.nref])/(jets_.jtpt[jets_.nref]); // det-level jet is matched to truth-level jet
      else sf = 1+gRandom->Gaus(0,sim_resolution)*std::sqrt(sf*sf-1); // “stochastic” smearing, which is what you will probably use for HIN
      jets_.jer_sf_down[jets_.nref] = sf;

      // Get JEC uncertainty
      JetCorrectionUncertainty *jecUnc;
      jecUnc = new JetCorrectionUncertainty(edm::FileInPath("HeavyIonsAnalysis/JetAnalysis/data/Summer19UL17_V5_MC_Uncertainty_AK4PFchs.txt").fullPath());
      // std::cout << "created the jec unc object" << std::endl;
      jecUnc->setJetEta(jets_.jteta[jets_.nref]);
      jecUnc->setJetPt(jets_.jtpt[jets_.nref]);
      double unc = jecUnc->getUncertainty(true); //this returns the uncertainty on the correction, it should be of order 1-2%
      // Vary correction with newCorrection=1+factor*unc; jtpt=jtpt*newCorrection; where factor=0(nom) or +-1(up,down)
      jets_.jec_unc[jets_.nref] = unc;
      // std::cout << "read the jec unc" << std::endl;
      */
      jets_.refparton_flavorForB[jets_.nref] = jet.partonFlavour();

      //      if(jet.genParton()){
      // // matched partons
      // const reco::GenParticle & parton = *jet.genParton();

      // jets_.refparton_pt[jets_.nref] = parton.pt();
      // jets_.refparton_flavor[jets_.nref] = parton.pdgId();

      //      } else {
      jets_.refparton_pt[jets_.nref] = -999;
      jets_.refparton_flavor[jets_.nref] = -999;
      //      }

      if (doJetTrueFlavour_) {
        jets_.jtHadFlav[jets_.nref] = jet.hadronFlavour();
        jets_.jtParFlav[jets_.nref] = jet.partonFlavour();
      } else { 
        jets_.jtHadFlav[jets_.nref] = -1;
        jets_.jtParFlav[jets_.nref] = -1;
      }
      /*
      // bool foundMatch = false;
      for (const JetFlavourInfoMatching& jetFlavourInfoMatching : *jetFlavourInfos) {
        if (deltaR(jet.p4(), jetFlavourInfoMatching.first->p4()) < 1e-6) {
          JetFlavourInfo jetInfo = jetFlavourInfoMatching.second;
          const GenParticleRefVector &bHadronsInJet = jetInfo.getbHadrons();
          const GenParticleRefVector &cHadronsInJet = jetInfo.getcHadrons();

          jets_.jtNbHad[jets_.nref] = bHadronsInJet.size();
          jets_.jtNcHad[jets_.nref] = cHadronsInJet.size();

          const GenParticleRefVector &partonsInJet = jetInfo.getPartons();

          int nb=0;
          int nc=0;
          // bool hasBfromGSP = false;
          // bool hasCfromGSP = false;

          for (GenParticleRefVector::const_iterator it = partonsInJet.begin(); it != partonsInJet.end(); ++it) {
            int parFlav = (*it)->pdgId();
            // const Candidate* c = (*it).get();

            if(abs(parFlav)==5){
              nb++;
              // if(isFromGSP(c)) hasBfromGSP = true;
            }
            else if(abs(parFlav)==4){
              nc++;
              // if(isFromGSP(c)) hasCfromGSP = true;
            }
          }

          jets_.jtNbPar[jets_.nref] = nb;
          jets_.jtNcPar[jets_.nref] = nc;
          // jets_.jtHasGSPB[jets_.nref] = hasBfromGSP;
          // jets_.jtHasGSPC[jets_.nref] = hasCfromGSP;

          // foundMatch = true;
          break;
        }
      } // end loop over flavour info
      */
    } // endif isMC 
    jets_.nref++;
  } // jet loop

  if (isMC_) {
    if (useHepMC_) {
      edm::Handle<HepMCProduct> hepMCProduct;
      iEvent.getByToken(eventInfoTag_, hepMCProduct);
      const HepMC::GenEvent* MCEvt = hepMCProduct->GetEvent();

      std::pair<HepMC::GenParticle*, HepMC::GenParticle*> beamParticles = MCEvt->beam_particles();
      jets_.beamId1 = (beamParticles.first != 0) ? beamParticles.first->pdg_id() : 0;
      jets_.beamId2 = (beamParticles.second != 0) ? beamParticles.second->pdg_id() : 0;
    }

    edm::Handle<GenEventInfoProduct> hEventInfo;
    iEvent.getByToken(eventGenInfoTag_, hEventInfo);

    // binning values and qscale appear to be equivalent, but binning values not always present
    jets_.pthat = hEventInfo->qScale();

    edm::Handle<edm::View<reco::GenJet>> genjets;
    iEvent.getByToken(genjetTag_, genjets);

    //get gen-level n-jettiness
    edm::Handle<edm::ValueMap<float>> genTau1s;
    edm::Handle<edm::ValueMap<float>> genTau2s;
    edm::Handle<edm::ValueMap<float>> genTau3s;
    if (doGenTaus_) {
      iEvent.getByToken(tokenGenTau1_, genTau1s);
      iEvent.getByToken(tokenGenTau2_, genTau2s);
      iEvent.getByToken(tokenGenTau3_, genTau3s);
    }

    jets_.ngen = 0;

    for (unsigned int igen = 0; igen < genjets->size(); ++igen) {
      const reco::GenJet& genjet = (*genjets)[igen];
      float genjet_pt = genjet.pt();
      /*
      float tau1 = -999.;
      float tau2 = -999.;
      float tau3 = -999.;
      Ptr<reco::GenJet> genJetPtr = genjets->ptrAt(igen);
      if (doGenTaus_) {
        tau1 = (*genTau1s)[genJetPtr];
        tau2 = (*genTau2s)[genJetPtr];
        tau3 = (*genTau3s)[genJetPtr];
      }

      // find matching patJet if there is one
      jets_.gendrjt[jets_.ngen] = -1.0;
      jets_.genmatchindex[jets_.ngen] = -1;

      for (int ijet = 0; ijet < jets_.nref; ++ijet) {
        // poor man's matching, someone fix please

        double deltaPt = fabs(genjet.pt() - jets_.refpt[ijet]);  //Note: precision of this ~ .0001, so cut .01
        double deltaEta = fabs(
            genjet.eta() -
            jets_.refeta
                [ijet]);  //Note: precision of this is  ~.0000001, but keep it low, .0001 is well below cone size and typical pointing resolution
        double deltaPhi = fabs(reco::deltaPhi(
            genjet.phi(),
            jets_.refphi
                [ijet]));  //Note: precision of this is  ~.0000001, but keep it low, .0001 is well below cone size and typical pointing resolution

        if (deltaPt < 0.01 && deltaEta < .0001 && deltaPhi < .0001) {
          if (genjet_pt > genPtMin_) {
            jets_.genmatchindex[jets_.ngen] = (int)ijet;
            jets_.gendphijt[jets_.ngen] = reco::deltaPhi(jets_.refphi[ijet], genjet.phi());
            jets_.gendrjt[jets_.ngen] =
                sqrt(pow(jets_.gendphijt[jets_.ngen], 2) + pow(fabs(genjet.eta() - jets_.refeta[ijet]), 2));
          }
          if (doGenTaus_) {
            jets_.reftau1[ijet] = tau1;
            jets_.reftau2[ijet] = tau2;
            jets_.reftau3[ijet] = tau3;
          }
          break;
        }
      }

      //reWTA reclustering----------------------------------
      if (doWTARecluster_) {
        std::vector<fastjet::PseudoJet> candidates;
        auto daughters = genjet.getJetConstituents();
        for (auto it = daughters.begin(); it != daughters.end(); ++it) {
          candidates.push_back(fastjet::PseudoJet((**it).px(), (**it).py(), (**it).pz(), (**it).energy()));
        }
        auto cs = new fastjet::ClusterSequence(candidates, WTAjtDef);
        std::vector<fastjet::PseudoJet> wtajt = fastjet::sorted_by_pt(cs->inclusive_jets(0));

        jets_.WTAgeneta[jets_.ngen] = (!wtajt.empty()) ? wtajt[0].eta() : -999;
        jets_.WTAgenphi[jets_.ngen] = (!wtajt.empty()) ? wtajt[0].phi_std() : -999;
        delete cs;
      }
      //-------------------------------------------------
      */
      // threshold to reduce size of output in minbias PbPb
      if (genjet_pt > genPtMin_) {
        jets_.genpt[jets_.ngen] = genjet_pt;
        jets_.geneta[jets_.ngen] = genjet.eta();
        jets_.genphi[jets_.ngen] = genjet.phi();
        jets_.genm[jets_.ngen] = genjet.mass();
        jets_.geny[jets_.ngen] = genjet.eta();
	/*
        if (doGenTaus_) {
          jets_.gentau1[jets_.ngen] = tau1;
          jets_.gentau2[jets_.ngen] = tau2;
          jets_.gentau3[jets_.ngen] = tau3;
        }
	*/
        if (doGenSubJets_){
          analyzeGenSubjets(genjet);
	}
        if (doSubEvent_) {
          const GenParticle* gencon = genjet.getGenConstituent(0);
          jets_.gensubid[jets_.ngen] = gencon->collisionId();
        }
        jets_.ngen++;
      }
    }
  }

  t->Fill();

  //memset(&jets_,0,sizeof jets_);
  jets_ = {0};
}

int HiInclusiveJetAnalyzer::getPFJetMuon(const pat::Jet& pfJet,
                                         const edm::View<pat::PackedCandidate>* pfCandidateColl) {
  int pfMuonIndex = -1;
  float ptMax = 0.;

  for (unsigned icand = 0; icand < pfCandidateColl->size(); icand++) {
    const pat::PackedCandidate& pfCandidate = pfCandidateColl->at(icand);
    int id = pfCandidate.pdgId();
    if (abs(id) != 3)
      continue;

    if (reco::deltaR(pfJet, pfCandidate) > 0.5)
      continue;

    double pt = pfCandidate.pt();
    if (pt > ptMax) {
      ptMax = pt;
      pfMuonIndex = (int)icand;
    }
  }

  return pfMuonIndex;
}

double HiInclusiveJetAnalyzer::getPtRel(const pat::PackedCandidate& lep, const pat::Jet& jet)

{
  float lj_x = jet.p4().px();
  float lj_y = jet.p4().py();
  float lj_z = jet.p4().pz();

  // absolute values squared
  float lj2 = lj_x * lj_x + lj_y * lj_y + lj_z * lj_z;
  float lep2 = lep.px() * lep.px() + lep.py() * lep.py() + lep.pz() * lep.pz();

  // projection vec(mu) to lepjet axis
  float lepXlj = lep.px() * lj_x + lep.py() * lj_y + lep.pz() * lj_z;

  // absolute value squared and normalized
  float pLrel2 = lepXlj * lepXlj / lj2;

  // lep2 = pTrel2 + pLrel2
  float pTrel2 = lep2 - pLrel2;

  return (pTrel2 > 0) ? std::sqrt(pTrel2) : 0.0;
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::analyzeSubjets(const reco::Jet& jet) {
  std::vector<float> sjpt;
  std::vector<float> sjeta;
  std::vector<float> sjphi;
  std::vector<float> sjm;
  if (jet.numberOfDaughters() > 0) {
    for (unsigned k = 0; k < jet.numberOfDaughters(); ++k) {
      const reco::Candidate& dp = *jet.daughter(k);
      sjpt.push_back(dp.pt());
      sjeta.push_back(dp.eta());
      sjphi.push_back(dp.phi());
      sjm.push_back(dp.mass());
    }
  } else {
    sjpt.push_back(-999.);
    sjeta.push_back(-999.);
    sjphi.push_back(-999.);
    sjm.push_back(-999.);
  }
  jets_.jtSubJetPt.push_back(sjpt);
  jets_.jtSubJetEta.push_back(sjeta);
  jets_.jtSubJetPhi.push_back(sjphi);
  jets_.jtSubJetM.push_back(sjm);
}

//--------------------------------------------------------------------------------------------------
template <typename jetType>
int HiInclusiveJetAnalyzer::getGroomedJetIndex(const jetType& jet, const edm::View<reco::Jet> groomedJetsV) const {
  //Find closest soft-dropped gen jet
  double drMin = 100;
  int imatch = -1;
  for (unsigned int i = 0; i < groomedJetsV.size(); ++i) {
    const reco::Jet& mjet = groomedJetsV[i];

    double dr = deltaR(jet, mjet);
    if (dr < drMin) {
      imatch = i;
      drMin = dr;
    }
  }
  return imatch;
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::analyzeRefSubjets(const reco::GenJet& jet) {
  //Find closest soft-dropped gen jet
  int imatch = getGroomedJetIndex(jet, *groomedGenJets);
  double dr = 999.;
  if (imatch > -1) {
    const reco::Jet& mjet = (*groomedGenJets)[imatch];
    dr = deltaR(jet, mjet);
  }

  jets_.refptG[jets_.nref] = -999.;
  jets_.refetaG[jets_.nref] = -999.;
  jets_.refphiG[jets_.nref] = -999.;
  jets_.refmG[jets_.nref] = -999.;
  jets_.refsym[jets_.nref] = -999.;
  jets_.refdroppedBranches[jets_.nref] = -999;

  std::vector<float> sjpt;
  std::vector<float> sjeta;
  std::vector<float> sjphi;
  std::vector<float> sjm;
  if (imatch > -1 && dr < 0.4) {
    const reco::Jet& mjet = (*groomedGenJets)[imatch];
    jets_.refptG[jets_.nref] = mjet.pt();
    jets_.refetaG[jets_.nref] = mjet.eta();
    jets_.refphiG[jets_.nref] = mjet.phi();
    jets_.refmG[jets_.nref] = mjet.mass();

    if (mjet.numberOfDaughters() > 0) {
      for (unsigned k = 0; k < mjet.numberOfDaughters(); ++k) {
        const reco::Candidate& dp = *mjet.daughter(k);
        sjpt.push_back(dp.pt());
        sjeta.push_back(dp.eta());
        sjphi.push_back(dp.phi());
        sjm.push_back(dp.mass());
      }
    }

    if (doGenSym_) {
      /*
      Ptr<reco::Jet> genJetPtr = gensubjets_->ptrAt(imatch);
      float gensym = (*genSymVM_)[genJetPtr];
      jets_.refsym[jets_.nref] = gensym;
      int db = (*genDroppedBranchesVM_)[genJetPtr];
      jets_.refdroppedBranches[jets_.nref] = db;
      */
    }
  } else {
    jets_.refptG[jets_.nref] = -999.;
    jets_.refetaG[jets_.nref] = -999.;
    jets_.refphiG[jets_.nref] = -999.;
    jets_.refmG[jets_.nref] = -999.;

    sjpt.push_back(-999.);
    sjeta.push_back(-999.);
    sjphi.push_back(-999.);
    sjm.push_back(-999.);
  }

  jets_.refSubJetPt.push_back(sjpt);
  jets_.refSubJetEta.push_back(sjeta);
  jets_.refSubJetPhi.push_back(sjphi);
  jets_.refSubJetM.push_back(sjm);
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::analyzeGenSubjets(const reco::GenJet& jet) {

  //Find closest soft-dropped gen jet
  int imatch = getGroomedJetIndex(jet, *groomedGenJets);
  double dr = 999.;
  if (imatch > -1) {
    const reco::Jet& mjet = (*groomedGenJets)[imatch];
    dr = deltaR(jet, mjet);
  }
  
  jets_.gsjt1E[jets_.ngen] = -1;
  jets_.gsjt1Y[jets_.ngen] = -999;
  jets_.gsjt1Pz[jets_.ngen] = -999;
  jets_.gsjt1Pt[jets_.ngen] = -1;
  jets_.gsjt1Eta[jets_.ngen] = -999;
  jets_.gsjt1Phi[jets_.ngen] = -999;

  jets_.gsjt2Pt[jets_.ngen] = -1;
  jets_.gsjt2Eta[jets_.ngen] = -999;
  jets_.gsjt2Phi[jets_.ngen] = -999;
  jets_.gsjt2E[jets_.ngen] = -1;
  jets_.gsjt2Y[jets_.ngen] = -999;
  jets_.gsjt2Pz[jets_.ngen] = -999;
  


  if (imatch > -1 && dr < 0.4) {
    const reco::Jet& mjet = (*groomedGenJets)[imatch];
    
    if (mjet.numberOfDaughters() > 0) {
      const Candidate & gsjt1 = *mjet.daughter(0);       
      jets_.gsjt1E[jets_.ngen] = gsjt1.energy();
      jets_.gsjt1Y[jets_.ngen] = gsjt1.y();
      jets_.gsjt1Pz[jets_.ngen] = gsjt1.pz();
      jets_.gsjt1Pt[jets_.ngen] = gsjt1.pt();
      jets_.gsjt1Eta[jets_.ngen] = gsjt1.eta();
      jets_.gsjt1Phi[jets_.ngen] = gsjt1.phi();
      
      
      if (mjet.numberOfDaughters() > 1) {
	const Candidate & gsjt2 = *mjet.daughter(1);
	jets_.gsjt2E[jets_.ngen] = gsjt2.energy();
	jets_.gsjt2Y[jets_.ngen] = gsjt2.y();
	jets_.gsjt2Pz[jets_.ngen] = gsjt2.pz();
	jets_.gsjt2Pt[jets_.ngen] = gsjt2.pt();
	jets_.gsjt2Eta[jets_.ngen] = gsjt2.eta();
	jets_.gsjt2Phi[jets_.ngen] = gsjt2.phi();
      }
    }   
  } // end if groomedGenJet match exists
 
}

int HiInclusiveJetAnalyzer::trkGenPartMatch(reco::Jet::Constituent constituent, std::vector<pat::PackedGenParticle> genParticlesV, double ptCut) const
{
  // return status of matched particle
  int status = 1;
  
  double dRmin = 100.;
  double dR = dRmin;

  double trkPt = constituent->pt();
  double trkEta = constituent->eta();
  double trkPhi = constituent->phi();

  for (pat::PackedGenParticle genParticle : genParticlesV) {
    // [DEBUG]
    bool chargedOnly_ = true;

    double partPt = genParticle.pt();
    double relPt = trkPt / partPt;
    //cout << "genParticle status: " << genParticle.status() << endl;
    if (partPt < 0.5 * ptCut) continue;
	  if(chargedOnly_ && genParticle.charge() == 0) continue;
	
    double partEta = genParticle.eta();
    double partPhi = genParticle.phi();

    double dEta = trkEta - partEta;
    double dPhi = std::acos(std::cos(trkPhi - partPhi));

    dR = std::sqrt((dEta * dEta) + (dPhi * dPhi));

    // edit for conservative matching:
    if (dR > 0.02) continue;
    if ((relPt < 0.8) || (relPt > 1.2)) continue;

    if (dR < dRmin) {
      dRmin = dR;
      status = genParticle.status();
    }
  }
  //cout << "dRmin: " << dRmin << endl;
  return status;  
}

DEFINE_FWK_MODULE(HiInclusiveJetAnalyzer);
