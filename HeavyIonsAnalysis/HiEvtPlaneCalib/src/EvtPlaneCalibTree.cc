#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Math/Vector3D.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/CastorReco/interface/CastorTower.h"

#include "DataFormats/HeavyIonEvent/interface/Centrality.h"

#include "DataFormats/HeavyIonEvent/interface/EvtPlane.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CondFormats/DataRecord/interface/HeavyIonRPRcd.h"
#include "CondFormats/DataRecord/interface/HeavyIonRcd.h"
#include "CondFormats/HIObjects/interface/CentralityTable.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "CondFormats/HIObjects/interface/RPFlatParams.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefVector.h"

#include "RecoHI/HiEvtPlaneAlgos/interface/HiEvtPlaneFlatten.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TTree.h"
#include "TH1I.h"
#include "TF1.h"
#include "TList.h"
#include "TString.h"
#include "TMath.h"
#include <time.h>
#include <cstdlib>
#include <vector>

#include "RecoHI/HiEvtPlaneAlgos/interface/HiEvtPlaneList.h"
#include "RecoHI/HiEvtPlaneAlgos/interface/LoadEPDB.h"
#include "CondFormats/HIObjects/interface/CentralityTable.h"

using namespace std;
using namespace hi;
using namespace reco;
int bToCent(int b){
  int bounds[200]={
    0,1,2,3,4,5,6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,
    31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,
    51,52,53,54,55,56,57,58,59,60,
    61,62,63,64,66,70,73,77,81,85,
    89,93,97,102,106,111,116,122,127,133,
    138,145,151,157,164,171,178,185,193,200,
    208,216,225,233,242,252,261,271,281,291,
    301,312,323,334,346,357,370,382,394,407,
    421,434,448,462,476,490,505,521,536,552,
    568,584,601,618,635,652,670,688,707,725,
    744,764,783,803,824,844,865,886,907,928,
    950,973,995,1017,1040,1064,1087,1111,1135,1159,
    1183,1208,1233,1258,1283,1309,1335,1361,1388,1415,
    1443,1471,1500,1528,1558,1587,1617,1647,1677,1708,
    1739,1771,1803,1835,1868,1901,1935,1969,2004,2039,
    2075,2111,2147,2185,2223,2261,2300,2340,2381,2423,
    2465,2510,2556,2604,2656,2712,2778,2857,2971};
  for(int i=199; i>0; i--) {
    if(b<bounds[i]) continue;
    return 200-i;
  }
  return 200;
}

static const int ntrkbins = 15;
static const  double trkbins[]={0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80, 100};

static const int ncentbins = 13;
static const  double centbins[]={0, 5, 10, 15, 20, 25, 30, 35, 40, 50, 60, 70, 80, 100};

static const int nptbins = 28;
static const double ptbins[]={0.3, 0.4, 0.5,  0.6,  0.8,  1.0,  1.25,  1.50,  2.0,
			      2.5,  3.0,  3.5,  4.0,  5.0,  6.0,  7.0, 8.0, 10., 12.0, 14.0, 16.0,  20.0, 26.0, 35.0, 45.0, 60.0, 80.0, 100., 200.};
static const int MaxNumFlatBins = 200;

static const int netabins = 12;
static const double etabins[]={-2.4, -2.0, -1.6, -1.2, -0.8, -0.4, 0, 0.4, 0.8, 1.2, 1.6, 2.0, 2.4};

class EvtPlaneCalibTree : public edm::EDAnalyzer {
public:
  explicit EvtPlaneCalibTree(const edm::ParameterSet&);
  ~EvtPlaneCalibTree();
  
private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  edm::Service<TFileService> fs;
  
  std::string centralityVariable_;
  std::string centralityLabel_;
  std::string centralityMC_;
  
  edm::InputTag centralityBinTag_;
  edm::EDGetTokenT<int> centralityBinToken;
  edm::Handle<int> cbin_;
  
  edm::InputTag bTag_;
  edm::EDGetTokenT<double> bToken;
  edm::Handle<double> b_;

  edm::InputTag centralityTag_;
  edm::EDGetTokenT<reco::Centrality> centralityToken;
  edm::Handle<reco::Centrality> centrality_;
  
  edm::InputTag vertexTag_;
  edm::EDGetTokenT<std::vector<reco::Vertex>> vertexToken;
  edm::Handle<VertexCollection> vertexCollection_;
  edm::Handle<std::vector<reco::Vertex>> vertex_;
  
  edm::InputTag caloTag_;
  edm::EDGetTokenT<CaloTowerCollection> caloToken;
  edm::Handle<CaloTowerCollection> caloCollection_;
  
  //edm::InputTag castorTag_;
  //edm::EDGetTokenT<std::vector<reco::CastorTower>> castorToken;
  //edm::Handle<std::vector<reco::CastorTower>> castorCollection_;
  
  edm::InputTag trackTag_;
  edm::EDGetTokenT<reco::TrackCollection> trackToken;
  edm::Handle<reco::TrackCollection> trackCollection_;
  
  edm::InputTag inputPlanesTag_;
  edm::EDGetTokenT<reco::EvtPlaneCollection> inputPlanesToken;
  edm::Handle<reco::EvtPlaneCollection> inputPlanes_;
	
  enum    TrackCut {trackUndefine = 0, ppReco = 1, HIReco, Pixel, GenMC};
  TrackCut sTrackQuality;
  double  dzdzerror_;
  double  d0d0error_;
  double  pterror_;
  double dzdzerror_pix_;
  double chi2_;
  double minvz_;
  double maxvz_;
  bool TrackQuality_ppReco(const reco::TrackCollection::const_iterator&, const reco::VertexCollection&);
  bool TrackQuality_HIReco(const reco::TrackCollection::const_iterator&, const reco::VertexCollection&);
  bool TrackQuality_Pixel(const reco::TrackCollection::const_iterator&, const reco::VertexCollection&);
  bool TrackQuality_GenMC(const reco::TrackCollection::const_iterator&, const reco::VertexCollection&);
 
  bool foundCentTag;
  bool useECAL_;
  bool useHCAL_;
  bool useNtrk_;
  bool loadDB_;
  double minet_;
  double maxet_;
  double effm_;
  double minpt_;
  double maxpt_;
   int flatnvtxbins_;
  double flatminvtx_;
  double flatmaxvtx_;
  double flatdelvtx_;
  bool genMC_;
 //double dzerr_;
  bool FirstEvent;
  int FlatOrder_;
  uint runno_; 
  float centval;
  int bin;
  int cbin;
  int trkbin;
  uint ntrkval;
  float vtx;
  double caloCentRef_;
  double caloCentRefWidth_;
  int caloCentRefMinBin_;
  int caloCentRefMaxBin_;
  int NumFlatBins_;
  int CentBinCompression_;
  
  int HFEtScale_;
  bool bypassCentrality_;
  int obins;
  TTree * tree;
  TH1D * fparams;
  TH1I * iparams;
  TH1D * hNtrkoff;
  TH1D * htrkbins;
  TH1D * hcent;
  TH1D * hb;
  TH1D * hcentbins;
  TH1D * hflatbins;
  TH1D * hptspec;
  TH2F * hpt;
  TH2F * hpt2;
  TH2F * hptcnt;
  TH1D * HFpEt;
  TH1D * HFmEt;
  struct offstruct{
    TH2D * wqxtrk[7][ntrkbins];
    TH2D * wqytrk[7][ntrkbins];
    TH2D * wqcnt[ntrkbins];
  } offstruct[12];

  //TH2D * hcastor;
  float ws[NumEPNames];
  float wc[NumEPNames];
  float ws_no_w[NumEPNames];
  float wc_no_w[NumEPNames];
  float s[NumEPNames];
  float c[NumEPNames];
  float pt[NumEPNames];
  float pt2[NumEPNames];
  int cnt[NumEPNames];
  double nCentBins_;
  bool genFlatPsi_;
  bool useOffsetPsi_;
  bool storeNames_;
  int evcnt;
  int evtrkcnt;
  //===================================
  
  // int getNoff(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  //   int N = 0;
  //   using namespace edm;
  //   using namespace reco;
  //   iEvent.getByToken(vertexToken,vertex_);
  //   VertexCollection recoVertices = *vertex_;
  //   int primaryvtx = 0;
  //   double hold_d0d0error_ = d0d0error_;
  //   double hold_dzdzerror_ = dzdzerror_;
  //   double hold_pterror_ = pterror_;
  //   double hold_dzdzerror_pix_ = dzdzerror_pix_;
  //   double hold_chi2_ = chi2_;
  //   d0d0error_ = 3.;
  //   dzdzerror_ = 3.;
  //   pterror_ = 0.1;
  //   if(sTrackQuality == ppReco) {
  //     if ( recoVertices.size() > 100 ) return -1;
  //     sort(recoVertices.begin(), recoVertices.end(), [](const reco::Vertex &a, const reco::Vertex &b){
  // 	  if ( a.tracksSize() == b.tracksSize() ) return a.chi2() < b.chi2();
  // 	  return a.tracksSize() > b.tracksSize();
  // 	});
  //   } else {
  //     sort(recoVertices.begin(), recoVertices.end(), [](const reco::Vertex &a, const reco::Vertex &b){
  // 	  return a.tracksSize() > b.tracksSize();
  // 	});    
  //   }
    
  //   vtx = recoVertices[primaryvtx].z();
  //   if (fabs(vtx) < minvz_ || fabs(vtx) > maxvz_) {
  //     return -1;
  //   }        
    
  //   iEvent.getByLabel(trackTag_,trackCollection_);
    
  //   for(TrackCollection::const_iterator itTrack = trackCollection_->begin(); itTrack != trackCollection_->end(); ++itTrack) { 
  //     if ( sTrackQuality == HIReco and not TrackQuality_HIReco(itTrack, recoVertices) ) continue;
  //     else if ( sTrackQuality == ppReco and not TrackQuality_ppReco(itTrack, recoVertices) ) continue;
  //     else if ( sTrackQuality == Pixel  and not TrackQuality_Pixel (itTrack, recoVertices) ) continue;
  //     else if ( sTrackQuality == GenMC  and not TrackQuality_GenMC (itTrack, recoVertices) ) continue;
      
  //     if( itTrack->pt() < 0.4 ) continue;
  //     ++N;
  //   }
  //   d0d0error_ = hold_d0d0error_;
  //   dzdzerror_ = hold_dzdzerror_;
  //   pterror_ = hold_pterror_;
  //   dzdzerror_pix_ = hold_dzdzerror_pix_;
  //   chi2_ = hold_chi2_;
  //   return N;
  // }
  
  
private:
  
};
EvtPlaneCalibTree::EvtPlaneCalibTree(const edm::ParameterSet& iConfig) {
  foundCentTag = kTRUE;
  nCentBins_ = 200.;
  
  centralityVariable_ = iConfig.getParameter<std::string>("centralityVariable");
  if(iConfig.exists("nonDefaultGlauberModel")){
    centralityMC_ = iConfig.getParameter<std::string>("nonDefaultGlauberModel");
  }
  centralityLabel_ = centralityVariable_+centralityMC_;
  
  centralityBinTag_ = iConfig.getParameter<edm::InputTag>("centralityBinTag_");
  centralityBinToken = consumes<int>(centralityBinTag_);
 
 
  caloTag_ = iConfig.getParameter<edm::InputTag>("caloTag_");
  caloToken = consumes<CaloTowerCollection>(caloTag_);

  bTag_ = iConfig.getParameter<edm::InputTag>("bTag_");
  bToken = consumes<double>(bTag_);
 
  centralityTag_ = iConfig.getParameter<edm::InputTag>("centralityTag_");
  centralityToken = consumes<reco::Centrality>(centralityTag_);
  
  vertexTag_  = iConfig.getParameter<edm::InputTag>("vertexTag_");
  vertexToken = consumes<std::vector<reco::Vertex>>(vertexTag_);
  
  trackTag_ = iConfig.getParameter<edm::InputTag>("trackTag");
  trackToken = consumes<reco::TrackCollection>(trackTag_);
  if ( trackTag_.label() == "hiGeneralTracks" ) {
    sTrackQuality = HIReco;
    cout<<"hiGeneralTracks"<<endl;
  } else if ( trackTag_.label() == "generalTracks" ) {
    sTrackQuality = ppReco;
    cout<<"generalTracks"<<endl;
  } else if ( trackTag_.label() == "hiGeneralAndPixelTracks" ) {
    sTrackQuality = Pixel;
    cout<<"hiGeneralAndPixelTracks"<<endl;
  } else {
    sTrackQuality = trackUndefine;
  }
  
  inputPlanesTag_ = iConfig.getParameter<edm::InputTag>("inputPlanesTag_");
  inputPlanesToken = consumes<reco::EvtPlaneCollection>(inputPlanesTag_);
  
  genFlatPsi_ = iConfig.getUntrackedParameter<bool>("genFlatPsi_",true);
  useNtrk_ = iConfig.getUntrackedParameter<bool>("useNtrk",false);
  FlatOrder_ = iConfig.getUntrackedParameter<int>("FlatOrder_", 9);
  NumFlatBins_ = iConfig.getUntrackedParameter<int>("NumFlatBins_",40);
  CentBinCompression_ = iConfig.getUntrackedParameter<int>("CentBinCompression_",5);

  useNtrk_ = iConfig.getUntrackedParameter<bool>("useNtrk",false);
  if(useNtrk_) {
    NumFlatBins_ = ntrkbins;
    CentBinCompression_ = 1;
  }
  caloCentRef_ = iConfig.getUntrackedParameter<double>("caloCentRef_",80.);
  caloCentRefWidth_ = iConfig.getUntrackedParameter<double>("caloCentRefWidth_",5.);
  if(NumFlatBins_ > MaxNumFlatBins) {
    cout<<"NumFlatBins set to max of "<<MaxNumFlatBins<<endl;
  }
  useOffsetPsi_ = iConfig.getUntrackedParameter<bool>("useOffsetPsi_",true);
  minet_ = iConfig.getUntrackedParameter<double>("minet_",-1.);
  maxet_ = iConfig.getUntrackedParameter<double>("maxet_",-1.);
  effm_ = iConfig.getUntrackedParameter<double>("effm_",0.0);
  minpt_ = iConfig.getUntrackedParameter<double>("minpt_",0.3);
  maxpt_ = iConfig.getUntrackedParameter<double>("maxpt_",3.0);
  flatnvtxbins_ = iConfig.getParameter<int>("flatnvtxbins_") ;
  genMC_ = iConfig.getUntrackedParameter<bool>("genMC_",false);
  flatminvtx_ = iConfig.getParameter<double>("flatminvtx_") ;
  flatdelvtx_ = iConfig.getParameter<double>("flatdelvtx_") ;
  chi2_  = iConfig.getUntrackedParameter<double>("chi2_",200.);
  dzdzerror_ = iConfig.getUntrackedParameter<double>("dzdzerror_", 3.);
  d0d0error_ = iConfig.getUntrackedParameter<double>("d0d0error_", 3.);
  pterror_ = iConfig.getUntrackedParameter<double>("pterror_", 0.1);
  dzdzerror_pix_ = iConfig.getUntrackedParameter<double>("dzdzerror_pix_") ;
  minvz_ = iConfig.getUntrackedParameter<double>("minvz_", -15.);
  maxvz_ = iConfig.getUntrackedParameter<double>("maxvz_", 15.);
 
  storeNames_ = 1;
  
  FirstEvent = kTRUE;
  hcent = fs->make<TH1D>("cent","cent",101,0,100);
  hb = fs->make<TH1D>("b","b",12000,0,12000);
  HFpEt = fs->make<TH1D>("HFpEt","HFpEt",800,0,40);
  HFmEt = fs->make<TH1D>("HFmEt","HFmEt",800,0,40);

  hcentbins = fs->make<TH1D>("centbins","centbins",ncentbins,centbins);
  hflatbins = fs->make<TH1D>("flatbins","flatbins",100,0,100);
  htrkbins = fs->make<TH1D>("trkbins","trkbins",ntrkbins,trkbins);
  hptspec = fs->make<TH1D>("pt","pt",nptbins,ptbins);
  TString epnamesF = EPNames[0].data();
  epnamesF = epnamesF+"/F";
  for(int i = 0; i<NumEPNames; i++) if(i>0) epnamesF = epnamesF + ":" + EPNames[i].data() + "/F";  
  
  TString epnamesI = EPNames[0].data();
  epnamesI = epnamesI+"/I";
  for(int i = 0; i<NumEPNames; i++) if(i>0) epnamesI = epnamesI + ":" + EPNames[i].data() + "/I";  
  flatmaxvtx_ = flatminvtx_ + flatnvtxbins_*flatdelvtx_;
  
  cout<<"=========================="<<endl;
  if(genMC_) std::cout<<"Generator MC Replay"<<std::endl;
  cout<<"EvtPlaneProducer:         "<<endl;
  cout<<"  NumFlatBins_:           "<<NumFlatBins_<<endl;
  cout<<"  CentBinCompression_:    "<<CentBinCompression_<<endl;
  cout<<"  minet_:                 "<<minet_<<endl;
  cout<<"  maxet_:                 "<<maxet_<<endl;
  cout<<"  minpt_:                 "<<minpt_<<endl;
  cout<<"  maxpt_:                 "<<maxpt_<<endl; 
  cout<<"  flatnvtxbins_:          "<<flatnvtxbins_<<endl;
  cout<<"  flatminvtx_:            "<<flatminvtx_<<endl;
  cout<<"  flatmaxvtx_:            "<<flatmaxvtx_<<endl;
  cout<<"  flatdelvtx_:            "<<flatdelvtx_<<endl;
  cout<<"  dzdzerror_:             "<<dzdzerror_<<endl;
  cout<<"  d0d0error_:             "<<d0d0error_<<endl;
  cout<<"  pterror_:               "<<pterror_<<endl;
  cout<<"  dzdzerror_pix_          "<<dzdzerror_pix_<<endl;
  cout<<"  chi2_                   "<<chi2_<<endl;
  cout<<"  NumEPNames:             "<<NumEPNames<<endl;
  if(useNtrk_) cout<<"  useNtrk true"<<endl;
  cout<<"=========================="<<endl;
  
  TDirectory * save = gDirectory;
  TFileDirectory conddir = fs->mkdir("Conditions");
  conddir.make<TH1I>(centralityBinTag_.label().data(),centralityBinTag_.label().data(),1,0,1);
  conddir.make<TH1I>(centralityTag_.label().data(), centralityTag_.label().data(),1,0,1);
  conddir.make<TH1I>(vertexTag_.label().data(), vertexTag_.label().data(),1,0,1);
  conddir.make<TH1I>(trackTag_.label().data(), trackTag_.label().data(),1,0,1);
  conddir.make<TH1I>(inputPlanesTag_.label().data(), inputPlanesTag_.label().data(),1,0,1);
  string note_NumFlatBins = Form("NumFlatBins_%d",NumFlatBins_);
  conddir.make<TH1I>(note_NumFlatBins.data(), note_NumFlatBins.data(),1,0,1);
  string note_flatnvtxbins = Form("flatnvtxbins_%d",flatnvtxbins_);
  conddir.make<TH1I>(note_flatnvtxbins.data(), note_flatnvtxbins.data(),1,0,1);
  string note_flatminvtx = Form("flatminvtx_%07.2f",flatminvtx_);
  conddir.make<TH1I>(note_flatminvtx.data(), note_flatminvtx.data(),1,0,1);
  string note_flatdelvtx = Form("flatdelvtx_%07.2f",flatdelvtx_);
  conddir.make<TH1I>(note_flatdelvtx.data(), note_flatdelvtx.data(),1,0,1);
  string note_dzdzerror = Form("dzdzerror_%07.2f",dzdzerror_);
  conddir.make<TH1I>(note_dzdzerror.data(), note_dzdzerror.data(),1,0,1);
  string note_d0d0error = Form("d0d0error_%07.2f",d0d0error_);
  conddir.make<TH1I>(note_d0d0error.data(), note_d0d0error.data(),1,0,1);
  string note_dzdzerror_pix = Form("dzdzerror_pix_%07.2f",dzdzerror_pix_);
  conddir.make<TH1I>(note_dzdzerror_pix.data(), note_dzdzerror_pix.data(),1,0,1);
  string note_chi2 = Form("chi2_%07.2f",chi2_);
  conddir.make<TH1I>(note_chi2.data(), note_chi2.data(),1,0,1);
  string note_vtx_range = Form("vtx_%5.1f_%5.1f",minvz_,maxvz_);
  conddir.make<TH1I>(note_vtx_range.data(), note_vtx_range.data(),1,0,1);
  if(useNtrk_) {
    conddir.make<TH1I>("useNtrk_Set_True", "useNtrk_Set_True",1,0,1);
  } else {
    conddir.make<TH1I>("useNtrk_Set_False", "useNtrk_Set_False",1,0,1);
  }
  
  save->cd();
  
  fparams = fs->make<TH1D>("fparams","fparams",10,0,10);
  iparams = fs->make<TH1I>("iparams","iparams",10,0,10);
  hNtrkoff = fs->make<TH1D>("Ntrkoff","Ntrkoff",5001,0,5000);
  hpt    = fs->make<TH2F>("hpt",   "hpt",   NumFlatBins_,0,NumFlatBins_,flatnvtxbins_,flatminvtx_,flatmaxvtx_);
  hpt2   = fs->make<TH2F>("hpt2",  "hpt2",  NumFlatBins_,0,NumFlatBins_,flatnvtxbins_,flatminvtx_,flatmaxvtx_);
  hptcnt = fs->make<TH2F>("hptcnt","hptcnt",NumFlatBins_,0,NumFlatBins_,flatnvtxbins_,flatminvtx_,flatmaxvtx_);
  int nbins = ntrkbins;
  if( !useNtrk_) nbins = ncentbins;
  TFileDirectory subdir;
  for(int i = 0; i<nbins; i++) {
    if(useNtrk_) {
      subdir = fs->mkdir(Form("%d_%d",(int)trkbins[i],(int)trkbins[i+1]));
    } else {
      subdir = fs->mkdir(Form("%d_%d",(int)centbins[i],(int)centbins[i+1]));
    }
    for(int k = 0; k<flatnvtxbins_; k++) {
      offstruct[k].wqcnt[i]   = subdir.make<TH2D>(Form("wqcnt_%d_%d"  ,k,i),Form("wqcnt_%d_%d"  ,k,i),nptbins,ptbins, netabins, etabins);
      offstruct[k].wqcnt[i]->SetOption("colz");
      offstruct[k].wqcnt[i]->SetXTitle("p_{T} (GeV/c)");
      offstruct[k].wqcnt[i]->SetYTitle("#eta");
      for(int j = 0; j<7; j++) {
	offstruct[k].wqxtrk[j][i] = subdir.make<TH2D>(Form("wqxtrk%d_%d_%d",j+1,k,i),Form("wqxtrk%d_%d_%d",j+1,k,i),nptbins,ptbins, netabins, etabins);
	offstruct[k].wqytrk[j][i] = subdir.make<TH2D>(Form("wqytrk%d_%d_%d",j+1,k,i),Form("wqytrk%d_%d_%d",j+1,k,i),nptbins,ptbins, netabins, etabins);
	offstruct[k].wqxtrk[j][i]->SetOption("colz");
	offstruct[k].wqytrk[j][i]->SetOption("colz");
	offstruct[k].wqxtrk[j][i]->SetXTitle("p_{T} (GeV/c)");
	offstruct[k].wqxtrk[j][i]->SetYTitle("#eta");
	offstruct[k].wqytrk[j][i]->SetXTitle("p_{T} (GeV/c)");
	offstruct[k].wqytrk[j][i]->SetYTitle("#eta");
      }
    }
  }
  fparams->SetBinContent(1,minet_);
  fparams->SetBinContent(2,maxet_);
  fparams->SetBinContent(3,minpt_);
  fparams->SetBinContent(4,maxpt_);
  fparams->SetBinContent(5,flatminvtx_);
  fparams->SetBinContent(6,flatmaxvtx_);
  fparams->SetBinContent(7,flatdelvtx_);
  fparams->SetBinContent(8,caloCentRef_);
  fparams->SetBinContent(9,caloCentRefWidth_);
  iparams->SetBinContent(1,FlatOrder_);
  iparams->SetBinContent(2,NumFlatBins_);
  iparams->SetBinContent(3,CentBinCompression_);
  iparams->SetBinContent(4,flatnvtxbins_);
  tree = fs->make<TTree>("tree","EP tree");
  tree->Branch("Cent",    &centval,    "cent/F");
  tree->Branch("Vtx",     &vtx,        "vtx/F");
  tree->Branch("Run",     &runno_,     "run/i");
  tree->Branch("bin",     &bin,        "bin/I");
  tree->Branch("cbin",     &cbin,        "cbin/I");
  tree->Branch("trkbin",  &trkbin,     "trkbin/I");
  tree->Branch("NtrkOff",&ntrkval,     "ntrkoff/i");
  tree->Branch("ws", &ws, epnamesF.Data());
  tree->Branch("wc", &wc, epnamesF.Data());
  tree->Branch("ws_no_w", &ws_no_w, epnamesF.Data());
  tree->Branch("wc_no_w", &wc_no_w, epnamesF.Data());
  tree->Branch("pt", &pt, epnamesF.Data());
  tree->Branch("pt2", &pt2, epnamesF.Data());
  tree->Branch("cnt",&cnt,epnamesI.Data());
  runno_ = 0;
  evcnt = 0;
  evtrkcnt = 0;
}


EvtPlaneCalibTree::~EvtPlaneCalibTree()
{
  
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
EvtPlaneCalibTree::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  centval = -1;
  vtx = -100;
  //  ntrkval = getNoff(iEvent,iSetup);
  //if(ntrkval==0) return;
  ntrkval = -1;
  hNtrkoff->Fill(ntrkval);

  using namespace edm;
  using namespace std;
  using namespace reco;
  
  Bool_t newrun = kFALSE;
  if((int) runno_ != (int) iEvent.id().run()) newrun = kTRUE;
  runno_ = iEvent.id().run();
  //////////////////
  if(FirstEvent || newrun) {
    FirstEvent = kFALSE;
    newrun = kFALSE;
    //
    //Get Size of Centrality Table
    if(!useNtrk_ && centralityVariable_ != "MC") {
      edm::ESHandle<CentralityTable> centDB_;
      iSetup.get<HeavyIonRcd>().get(centralityLabel_,centDB_);
      nCentBins_ = (int) centDB_->m_table.size();
      //std::cout<<"nCentBins: "<<nCentBins_<<std::endl;
    }
  } //First event
  
  //
  //Get Centrality
  //
  if(!useNtrk_) {
    edm::Handle<reco::Centrality> centrality_;
    iEvent.getByToken(centralityToken, centrality_);
    iEvent.getByToken(centralityBinToken, cbin_);
    cbin = *cbin_;     
    double cscale = 100./nCentBins_;
    centval = cscale*cbin;

    hcent->Fill(centval);
    hcentbins->Fill(cbin);
    bin = cbin/CentBinCompression_; 
    ntrkval = centrality_->Ntracks();
    hNtrkoff->Fill(ntrkval);
    trkbin = hcentbins->FindBin(centval)-1;
    //std::cout<<cbin<<"\t"<<centval<<"\t"<<bin<<"\t"<<trkbin<<std::endl;
  } else {
    edm::Handle<reco::Centrality> centrality_;
    iEvent.getByToken(centralityToken, centrality_);
    ntrkval = centrality_->Ntracks();
    hNtrkoff->Fill(ntrkval);
    cbin = bToCent(ntrkval);
    double cscale = 0.5;
    centval = cscale*cbin;
    hcent->Fill(centval);
    hcentbins->Fill(cbin);
    bin = htrkbins->FindBin(centval)-1;
    trkbin = bin;
    htrkbins->Fill(centval);
    //std::cout<<ntrkval<<"\t"<<cbin<<"\t"<<centval<<"\t"<<trkbin<<"\t"<<bin<<std::endl;
    if(bin<0 || bin>=ntrkbins) return;
  }
  double b;
  if(genMC_) {
    iEvent.getByToken(bToken, b_);
    b = *b_;     
    hb->Fill(500.*b);

  } 
  //
  //Get Vertex
  //

  iEvent.getByToken(vertexToken, vertexCollection_);
  VertexCollection recoVertices = *vertexCollection_;
  if ( recoVertices.size() > 100 ) return;
  sort(recoVertices.begin(), recoVertices.end(), [](const reco::Vertex &a, const reco::Vertex &b){
      if ( a.tracksSize() == b.tracksSize() ) return a.chi2() < b.chi2();
      return a.tracksSize() > b.tracksSize();
    });

  int primaryvtx = 0;
   
  double vz = recoVertices[primaryvtx].z();
  vtx = vz;

  //CaloCheck
    iEvent.getByToken(caloToken,caloCollection_);
    double tower_eta,  tower_energyet, tower_energyet_e, tower_energyet_h;
    if(caloCollection_.isValid()){
      for (CaloTowerCollection::const_iterator j = caloCollection_->begin();j !=caloCollection_->end(); j++) {
	tower_eta        = j->eta();
	//tower_phi        = j->phi();
	tower_energyet_e   = j->emEt();
	tower_energyet_h   = j->hadEt();
	tower_energyet     = tower_energyet_e + tower_energyet_h;
	for(int i = 0; i<NumEPNames; i++) {
	  if(tower_eta > -5 && tower_eta < -3) HFmEt->Fill(tower_energyet);
	  if(tower_eta >  3 && tower_eta < 5) HFpEt->Fill(tower_energyet);
	}
      }
    }

  if(vtx>flatminvtx_ && vtx<flatmaxvtx_) {    
    //
    //Get Event Planes
    //
    iEvent.getByToken(inputPlanesToken,inputPlanes_);
    
    if(!inputPlanes_.isValid()){
      cout << "Error! Can't get hiEvtPlane product!" << endl;
      return ;
    }
    
    int i = 0;
    for (EvtPlaneCollection::const_iterator ep = inputPlanes_->begin();ep !=inputPlanes_->end(); ep++) {
      if(i>=NumEPNames || i!= ep->indx()) {
	cout<<"DATA INCONSISTENCY.  Too many or wrong EPs found in collection. Expect a crash!"<<endl;
      }

      ws[i]=0;
      wc[i]=0;
      ws_no_w[i] = 0;
      wc_no_w[i] = 0;
      cnt[i]=0;
      pt[i]=0;
      pt2[i]=0;

      double c = ep->sumCos(0);
      double s = ep->sumSin(0);
      double c_no_w = ep->sumCos(3);
      double s_no_w = ep->sumSin(3);
      uint m = ep->mult();
      float fpt = ep->sumPtOrEt();
      float fpt2 = ep->sumPtOrEt2();
      //std::cout<<"EvtPlaneCalibTree: "<<s<<"\t"<<c<<"\t"<<m<<"\tang: "<<ep->angle(0)<<std::endl;
      if(ep->angle(0)>-5) {      
	ws[i]=s;
	wc[i]=c;
	ws_no_w[i] = s_no_w;
	wc_no_w[i] = c_no_w;
	cnt[i]=m;
	pt[i]=fpt;
	pt2[i]=fpt2;
      }
      ++i;
    }    
    tree->Fill();   
    
    //Fill offset hists
    iEvent.getByToken(trackToken,trackCollection_);
    evtrkcnt = 0;
    for(reco::TrackCollection::const_iterator itTrack = trackCollection_->begin();
	itTrack != trackCollection_->end();                      
	++itTrack) {    
      if ( sTrackQuality == HIReco and not TrackQuality_HIReco(itTrack, recoVertices) ) continue;
      else if ( sTrackQuality == ppReco and not TrackQuality_ppReco(itTrack, recoVertices) ) continue;
      else if ( sTrackQuality == Pixel  and not TrackQuality_Pixel (itTrack, recoVertices) ) continue;
      
      double pt = itTrack->pt();
      double eta = itTrack->eta();
      double phi = itTrack->phi();
      if(fabs(eta)<0.8 && fabs(vtx)<15) {
	hptspec->Fill(pt); 
	if(pt>=0.3 && pt<3.0) ++evtrkcnt;
      }
      int k = (vtx-flatminvtx_)/flatdelvtx_;
      if(k>=0&&k<flatnvtxbins_) {
 	for(int j = 0; j<7; j++) {
	  offstruct[k].wqxtrk[j][trkbin]->Fill(pt, eta, cos( (1.+j) * phi ));
	  offstruct[k].wqytrk[j][trkbin]->Fill(pt, eta, sin( (1.+j) * phi ));
	}
	offstruct[k].wqcnt[trkbin]->Fill(pt,eta);
      }
      //cout<<evcnt<<"\t"<<evtrkcnt<<endl;
      ++evcnt;
    } 
  }
}

// ------------ method called once each job just before starting event loop  ------------
void 
EvtPlaneCalibTree::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EvtPlaneCalibTree::endJob() {
  cout<<"No. Events: "<<evcnt<<endl;
  cout<<"No. Tracks 0.3 to 3.0: "<<evtrkcnt<<endl;
}
///
bool
EvtPlaneCalibTree::TrackQuality_ppReco(const reco::TrackCollection::const_iterator& itTrack, const reco::VertexCollection& recoVertices)
{
        if ( itTrack->charge() == 0 ) return false;
        if ( !itTrack->quality(reco::TrackBase::highPurity) ) return false;
        if ( itTrack->ptError()/itTrack->pt() > pterror_ ) return false;
	int primaryvtx = 0;
	math::XYZPoint v1( recoVertices[primaryvtx].position().x(), recoVertices[primaryvtx].position().y(), recoVertices[primaryvtx].position().z() );
	double vxError = recoVertices[primaryvtx].xError();
	double vyError = recoVertices[primaryvtx].yError();
	double vzError = recoVertices[primaryvtx].zError();
        double d0 = -1.* itTrack->dxy(v1);
        double derror=sqrt(itTrack->dxyError()*itTrack->dxyError()+vxError*vyError);
        if ( fabs( d0/derror ) > d0d0error_ ) {
                return false;
        }
        double dz=itTrack->dz(v1);
        double dzerror=sqrt(itTrack->dzError()*itTrack->dzError()+vzError*vzError);
        if ( fabs( dz/dzerror ) > dzdzerror_ ) {
                return false;
        }
        return true;
}

///
bool
EvtPlaneCalibTree::TrackQuality_HIReco(const reco::TrackCollection::const_iterator& itTrack, const reco::VertexCollection& recoVertices)
{
	if ( itTrack->charge() == 0 ) return false;
	if ( !itTrack->quality(reco::TrackBase::highPurity) ) return false;
	if ( itTrack->numberOfValidHits() < 11 ) return false;
	if ( itTrack->normalizedChi2() / itTrack->hitPattern().trackerLayersWithMeasurement() > 0.15 ) {
		return false;
	}
	if ( itTrack->ptError()/itTrack->pt() > pterror_ ) {
		return false;
	}
	if (
		itTrack->originalAlgo() != 4 and
		itTrack->originalAlgo() != 5 and
		itTrack->originalAlgo() != 6 and
		itTrack->originalAlgo() != 7
	) {
		return false;
	}

	int primaryvtx = 0;
	math::XYZPoint v1( recoVertices[primaryvtx].position().x(), recoVertices[primaryvtx].position().y(), recoVertices[primaryvtx].position().z() );
	double vxError = recoVertices[primaryvtx].xError();
	double vyError = recoVertices[primaryvtx].yError();
	double vzError = recoVertices[primaryvtx].zError();
	double d0 = -1.* itTrack->dxy(v1);
	double derror=sqrt(itTrack->dxyError()*itTrack->dxyError()+vxError*vyError);
	if ( fabs( d0/derror ) > d0d0error_ ) {
		return false;
	}

	double dz=itTrack->dz(v1);
	double dzerror=sqrt(itTrack->dzError()*itTrack->dzError()+vzError*vzError);
	if ( fabs( dz/dzerror ) > dzdzerror_ ) {
		return false;
	}
	return true;
}

///
bool
EvtPlaneCalibTree::TrackQuality_Pixel(const reco::TrackCollection::const_iterator& itTrack, const reco::VertexCollection& recoVertices)
{
  if ( itTrack->charge() == 0 ) return false;
  if ( !itTrack->quality(reco::TrackBase::highPurity) ) return false;
  bool bPix = false;
  int nHits = itTrack->numberOfValidHits();
  
  int primaryvtx = 0;
  math::XYZPoint v1( recoVertices[primaryvtx].position().x(), recoVertices[primaryvtx].position().y(), recoVertices[primaryvtx].position().z() );
  double vxError = recoVertices[primaryvtx].xError();
  double vyError = recoVertices[primaryvtx].yError();
  double vzError = recoVertices[primaryvtx].zError();
  double d0 = -1.* itTrack->dxy(v1);
  
  double dz=itTrack->dz(v1);
  double dzerror=sqrt(itTrack->dzError()*itTrack->dzError()+vzError*vzError);
  if ( itTrack->pt() < 2.4 and (nHits==3 or nHits==4 or nHits==5 or nHits==6) ) bPix = true;
  if ( not bPix ) {
    if ( nHits < 11 ) return false;
    if ( itTrack->normalizedChi2() / itTrack->hitPattern().trackerLayersWithMeasurement() > 0.15 ) {
      return false;
    }
    if ( itTrack->ptError()/itTrack->pt() > pterror_ ) {
      return false;
    }
    if (
	itTrack->pt() > 2.4 and
	itTrack->originalAlgo() != 4 and
	itTrack->originalAlgo() != 5 and
	itTrack->originalAlgo() != 6 and
	itTrack->originalAlgo() != 7
	) {
      return false;
    }
    
    double derror=sqrt(itTrack->dxyError()*itTrack->dxyError()+vxError*vyError);
    if ( fabs( d0/derror ) > d0d0error_ ) {
      return false;
    }
    
    if ( fabs( dz/dzerror ) > dzdzerror_ ) {
      return false;
    }
  } else {
    if ( itTrack->normalizedChi2() / itTrack->hitPattern().trackerLayersWithMeasurement() > chi2_ ) return false;
    if ( fabs( dz/dzerror ) > dzdzerror_pix_ ) {
      return false;
    }
  }
  return true;
}
///
bool
EvtPlaneCalibTree::TrackQuality_GenMC(const reco::TrackCollection::const_iterator& itTrack, const reco::VertexCollection& recoVertices)
{
	if ( itTrack->charge() == 0 ) return false;
	if( fabs(itTrack->eta())>2.4  )return false;
	return true;
}


//define this as a plug-in
DEFINE_FWK_MODULE(EvtPlaneCalibTree);
