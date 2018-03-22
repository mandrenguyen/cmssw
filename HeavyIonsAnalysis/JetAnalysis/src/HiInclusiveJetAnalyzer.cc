/*
  Based on the jet response analyzer
  Modified by Matt Nguyen, November 2010

*/

#include "HeavyIonsAnalysis/JetAnalysis/interface/HiInclusiveJetAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include <Math/DistFunc.h>
#include "TMath.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "DataFormats/Common/interface/View.h"

#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "SimDataFormats/HiGenData/interface/GenHIEvent.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMapRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMap.h"
#include "L1Trigger/GlobalTrigger/plugins/L1GlobalTrigger.h"

#include "RecoBTag/SecondaryVertex/interface/TrackKinematics.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "fastjet/contrib/Njettiness.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/SoftDrop.hh"

using namespace std;
using namespace edm;
using namespace reco;

//template class std::vector<std::vector<std::vector<float> > >;

// class ExtraInfo : public fastjet::PseudoJet::UserInfoBase {
// public:
//   ExtraInfo(int id) : _index(id){}
//   int part_id() const { return _index; }
// protected:
//   int _index;
// };
/*
#ifdef __CINT__
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<vector<float> > >+;
#pragma link C++ class vector<vector<int> >+;
#endif
*/
HiInclusiveJetAnalyzer::HiInclusiveJetAnalyzer(const edm::ParameterSet& iConfig) :
	trackSelector(iConfig.getParameter<edm::ParameterSet>("trackSelection")),
        trackPseudoSelector(iConfig.getParameter<edm::ParameterSet>("trackSelection")),
        pseudoVertexV0Filter(iConfig.getParameter<edm::ParameterSet>("trackPairV0Filter")),
        trackPairV0Filter(iConfig.getParameter<edm::ParameterSet>("trackPairV0Filter")),
	geo(0)
{

  doMatch_ = iConfig.getUntrackedParameter<bool>("matchJets",false);
  jetTagLabel_ = iConfig.getParameter<InputTag>("jetTag");
  jetTag_ = consumes<reco::JetView> (jetTagLabel_);
  jetTagPat_ = consumes<pat::JetCollection> (jetTagLabel_);
  matchTag_ = consumes<reco::JetView> (iConfig.getUntrackedParameter<InputTag>("matchTag"));
  matchTagPat_ = consumes<pat::JetCollection> (iConfig.getUntrackedParameter<InputTag>("matchTag"));
  
  // vtxTag_ = iConfig.getUntrackedParameter<edm::InputTag>("vtxTag",edm::InputTag("hiSelectedVertex"));
  vtxTag_ = consumes<vector<reco::Vertex> >        (iConfig.getUntrackedParameter<edm::InputTag>("vtxTag",edm::InputTag("offlinePrimaryVertices")));  
  // iConfig.getUntrackedParameter<edm::InputTag>("vtxTag",edm::InputTag("hiSelectedVertex"));
  trackTag_ = consumes<reco::TrackCollection> (iConfig.getParameter<InputTag>("trackTag"));
  useQuality_ = iConfig.getUntrackedParameter<bool>("useQuality",1);
  trackQuality_ = iConfig.getUntrackedParameter<string>("trackQuality","highPurity");

  jetName_ = iConfig.getUntrackedParameter<string>("jetName");
  doGenTaus_ = iConfig.getUntrackedParameter<bool>("doGenTaus",0);
  doSubJets_ = iConfig.getUntrackedParameter<bool>("doSubJets",0);
  doJetConstituents_ = iConfig.getUntrackedParameter<bool>("doJetConstituents", false);
  doNewJetVars_ = iConfig.getUntrackedParameter<bool>("doNewJetVars", false);
  doGenSubJets_ = iConfig.getUntrackedParameter<bool>("doGenSubJets", false);
  subjetGenTag_ = consumes<reco::JetView> (iConfig.getUntrackedParameter<InputTag>("subjetGenTag"));

  // useGenTaus = true;
  // if (iConfig.exists("genTau1"))
  //   tokenGenTau1_          = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("genTau1"));
  // else useGenTaus = false;
  // if (iConfig.exists("genTau2"))
  //   tokenGenTau2_          = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("genTau2"));
  // else useGenTaus = false;
  // if (iConfig.exists("genTau3"))
  //   tokenGenTau3_          = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("genTau3"));
  // else useGenTaus = false;
 
  isPythia6_ = iConfig.getUntrackedParameter<bool>("isPythia6",true);  
  isMC_ = iConfig.getUntrackedParameter<bool>("isMC",false);
  useHepMC_ = iConfig.getUntrackedParameter<bool> ("useHepMC",false);
  fillGenJets_ = iConfig.getUntrackedParameter<bool>("fillGenJets",false);

  doExtendedFlavorTagging_ = iConfig.getUntrackedParameter<bool>("doExtendedFlavorTagging",false);
  doTrigger_ = iConfig.getUntrackedParameter<bool>("doTrigger",false);
  doHiJetID_ = iConfig.getUntrackedParameter<bool>("doHiJetID",false);
  if(doHiJetID_) jetIDweightFile_ = iConfig.getUntrackedParameter<string>("jetIDWeight","weights.xml");
  doStandardJetID_ = iConfig.getUntrackedParameter<bool>("doStandardJetID",false);

  rParam = iConfig.getParameter<double>("rParam");
  hardPtMin_ = iConfig.getUntrackedParameter<double>("hardPtMin",4);
  jetPtMin_ = iConfig.getParameter<double>("jetPtMin");

  if(isMC_){
    genjetTag_ = consumes<vector<reco::GenJet> > (iConfig.getParameter<InputTag>("genjetTag"));
    //genjetTag_ = consumes<edm::View<reco::Jet>>(iConfig.getParameter<InputTag>("genjetTag"));
    if(useHepMC_) eventInfoTag_ = consumes<HepMCProduct> (iConfig.getParameter<InputTag>("eventInfoTag"));
    //eventGenInfoTag_ = consumes<GenEventInfoProduct> (iConfig.getParameter<InputTag>("eventInfoTag"));
  }
  verbose_ = iConfig.getUntrackedParameter<bool>("verbose",false);

  useVtx_ = iConfig.getUntrackedParameter<bool>("useVtx",true);
  useJEC_ = iConfig.getUntrackedParameter<bool>("useJEC",true);
  usePat_ = iConfig.getUntrackedParameter<bool>("usePAT",true);

  doLifeTimeTagging_ = iConfig.getUntrackedParameter<bool>("doLifeTimeTagging",false);
  doLifeTimeTaggingExtras_ = iConfig.getUntrackedParameter<bool>("doLifeTimeTaggingExtras",true);
  saveBfragments_  = iConfig.getUntrackedParameter<bool>("saveBfragments",false);
  skipCorrections_  = iConfig.getUntrackedParameter<bool>("skipCorrections",false);

  pfCandidateLabel_ = consumes<reco::PFCandidateCollection> (iConfig.getUntrackedParameter<edm::InputTag>("pfCandidateLabel",edm::InputTag("particleFlowTmp")));

  doTower = iConfig.getUntrackedParameter<bool>("doTower",false);
  if(doTower){
    TowerSrc_ = consumes<CaloTowerCollection>( iConfig.getUntrackedParameter<edm::InputTag>("towersSrc",edm::InputTag("towerMaker")));
  }
  
  // EBSrc_ = iConfig.getUntrackedParameter<edm::InputTag>("EBRecHitSrc",edm::InputTag("ecalRecHit","EcalRecHitsEB"));
  // EESrc_ = iConfig.getUntrackedParameter<edm::InputTag>("EERecHitSrc",edm::InputTag("ecalRecHit","EcalRecHitsEE"));
  // HcalRecHitHFSrc_ = iConfig.getUntrackedParameter<edm::InputTag>("hcalHFRecHitSrc",edm::InputTag("hfreco"));
  // HcalRecHitHBHESrc_ = iConfig.getUntrackedParameter<edm::InputTag>("hcalHBHERecHitSrc",edm::InputTag("hbhereco"));

  genParticleSrc_ = consumes<reco::GenParticleCollection> (iConfig.getUntrackedParameter<edm::InputTag>("genParticles",edm::InputTag("hiGenParticles")));
  if(doSubJets_ && doExtendedFlavorTagging_){
	  subjetFlavourInfosToken_ = mayConsume<reco::JetFlavourInfoMatchingCollection>( iConfig.exists("subjetFlavourInfos") ? iConfig.getParameter<edm::InputTag>("subjetFlavourInfos") : edm::InputTag() );
	  groomedJetsToken_ = mayConsume<edm::View<reco::Jet> >( iConfig.exists("groomedJets") ? iConfig.getParameter<edm::InputTag>("groomedJets") : edm::InputTag() );
	  useSubjets_ = ( iConfig.exists("subjetFlavourInfos") && iConfig.exists("groomedJets") );
  }

  if(doTrigger_){
    L1gtReadout_ = consumes< L1GlobalTriggerReadoutRecord > (iConfig.getParameter<edm::InputTag>("L1gtReadout"));
    hltResName_ = consumes< TriggerResults >(iConfig.getUntrackedParameter<string>("hltTrgResults","TriggerResults::HLT"));


    if (iConfig.exists("hltTrgNames"))
      hltTrgNames_ = iConfig.getUntrackedParameter<vector<string> >("hltTrgNames");

    if (iConfig.exists("hltProcNames"))
      hltProcNames_ = iConfig.getUntrackedParameter<vector<string> >("hltProcNames");
    else {
      hltProcNames_.push_back("FU");
      hltProcNames_.push_back("HLT");
    }
  }
  if(doLifeTimeTagging_){
    bTagJetName_ = iConfig.getUntrackedParameter<string>("bTagJetName");
    ImpactParameterTagInfos_ = consumes<vector<TrackIPTagInfo> > (iConfig.getUntrackedParameter<string>("ImpactParameterTagInfos",(bTagJetName_+"ImpactParameterTagInfos")));
    JetProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("JetProbabilityBJetTags",(bTagJetName_+"JetProbabilityBJetTags")));
    JetBProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("JetBProbabilityBJetTags",(bTagJetName_+"JetBProbabilityBJetTags")));
    SecondaryVertexTagInfos_ = consumes<vector<SecondaryVertexTagInfo> > (iConfig.getUntrackedParameter<string>("SecondaryVertexTagInfos",(bTagJetName_+"SecondaryVertexTagInfos")));
    SimpleSecondaryVertexHighEffBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("SimpleSecondaryVertexHighEffBJetTags",(bTagJetName_+"SimpleSecondaryVertexHighEffBJetTags")));
    SimpleSecondaryVertexHighPurBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("SimpleSecondaryVertexHighPurBJetTags",(bTagJetName_+"SimpleSecondaryVertexHighPurBJetTags")));
    CombinedSecondaryVertexBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("CombinedSecondaryVertexBJetTags",(bTagJetName_+"CombinedSecondaryVertexBJetTags")));
    CombinedSecondaryVertexV2BJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("CombinedSecondaryVertexV2BJetTags",(bTagJetName_+"CombinedSecondaryVertexV2BJetTags")));

    if(doSubJets_){
	    CombinedSubjetSecondaryVertexBJetTags_ = mayConsume<JetTagCollection> (iConfig.getUntrackedParameter<string>("CombinedSubjetSecondaryVertexV2BJetTags",(bTagJetName_+"CombinedSubjetSecondaryVertexV2BJetTags")));
	    SubjetJetProbabilityBJetTags_ = mayConsume<JetTagCollection> (iConfig.getUntrackedParameter<string>("SubjetJetProbabilityBJetTags",(bTagJetName_+"SubjetJetProbabilityBJetTags")));
	    svSubjetTagInfos_ = mayConsume<vector<SecondaryVertexTagInfo> >(iConfig.getUntrackedParameter<string>("SecondaryVertexTagInfos",(bTagJetName_+"SubjetSecondaryVertexTagInfos")));
	    svImpactParameterTagInfos_ = mayConsume<vector<TrackIPTagInfo> >(iConfig.getUntrackedParameter<string>("ImpactParameterTagInfos",(bTagJetName_+"SubjetImpactParameterTagInfos")));
    } 
  }

  //  cout<<" jet collection : "<<jetTag_<<endl;
  doSubEvent_ = 0;

  if(isMC_){
    //     cout<<" genjet collection : "<<genjetTag_<<endl;
    genPtMin_ = iConfig.getUntrackedParameter<double>("genPtMin",10);
    doSubEvent_ = iConfig.getUntrackedParameter<bool>("doSubEvent",0);
  }

  fastjet::contrib::OnePass_KT_Axes     onepass_kt_axes;
  fastjet::contrib::NormalizedMeasure normalizedMeasure(1.,rParam);
  routine_ = std::auto_ptr<fastjet::contrib::Njettiness> ( new fastjet::contrib::Njettiness( onepass_kt_axes, normalizedMeasure) );

  if(doHiJetID_){
	  string inputArrs[] = { "trackMax/jtpt", "trackHardSum/jtpt", "trackHardN/jtpt", "chargedN/jtpt", "chargedHardSum/jtpt", "chargedHardN/jtpt", "photonN/jtpt", "photonHardSum/jtpt", "photonHardN/jtpt", "neutralN/jtpt", "hcalSum/jtpt", "ecalSum/jtpt", "chargedMax/jtpt", "chargedSum/jtpt", "neutralMax/jtpt", "neutralSum/jtpt", "photonMax/jtpt", "photonSum/jtpt", "eSum/jtpt", "muSum/jtpt" };
	  varAddr = std::unique_ptr<float[]> (new float[sizeof(inputArrs)/sizeof(inputArrs[0])]);

	  reader = std::unique_ptr<TMVA::Reader> (new TMVA::Reader("V:Color:!Silent"));
	  for(unsigned int ivar=0; ivar< (sizeof(inputArrs)/sizeof(inputArrs[0])); ivar++){ reader->AddVariable(inputArrs[ivar].c_str(), &(varAddr.get()[ivar])); }
	  edm::FileInPath fp(jetIDweightFile_.data());
	  std::string transFileName = fp.fullPath(); 
	  reader->BookMVA("BDTG",transFileName.c_str());
   }
}



HiInclusiveJetAnalyzer::~HiInclusiveJetAnalyzer() { }



void
HiInclusiveJetAnalyzer::beginRun(const edm::Run& run,
				 const edm::EventSetup & es) {}

void
HiInclusiveJetAnalyzer::beginJob() {

  //string jetTagName = jetTag_.label()+"_tree";
  string jetTagTitle = jetTagLabel_.label()+" Jet Analysis Tree";
  t = fs1->make<TTree>("t",jetTagTitle.c_str());

  //  TTree* t= new TTree("t","Jet Response Analyzer");
  //t->Branch("run",&jets_.run,"run/I");
  t->Branch("evt",&jets_.evt,"evt/I");
  //t->Branch("lumi",&jets_.lumi,"lumi/I");
  t->Branch("b",&jets_.b,"b/F");
  if (useVtx_) {
    t->Branch("vx",&jets_.vx,"vx/F");
    t->Branch("vy",&jets_.vy,"vy/F");
    t->Branch("vz",&jets_.vz,"vz/F");
  }

  t->Branch("nref",&jets_.nref,"nref/I");
  t->Branch("rawpt",jets_.rawpt,"rawpt[nref]/F");
  if(!skipCorrections_) t->Branch("jtpt",jets_.jtpt,"jtpt[nref]/F");
  t->Branch("jteta",jets_.jteta,"jteta[nref]/F");
  t->Branch("jty",jets_.jty,"jty[nref]/F");
  t->Branch("jtphi",jets_.jtphi,"jtphi[nref]/F");
  t->Branch("jtpu",jets_.jtpu,"jtpu[nref]/F");
  t->Branch("jtm",jets_.jtm,"jtm[nref]/F");
  t->Branch("jtarea",jets_.jtarea,"jtarea[nref]/F");

  if(doNewJetVars_){
    t->Branch("jtnCands",jets_.jtnCands,"jtnCands[nref]/I");
    t->Branch("jtnChCands",jets_.jtnChCands,"jtnChCands[nref]/I");
    t->Branch("jtnNeCands",jets_.jtnNeCands,"jtnNeCands[nref]/I");
    t->Branch("jtchargedSumConst",jets_.jtchargedSumConst,"jtchargedSumConst[nref]/F");
    t->Branch("jtchargedNConst"  ,jets_.jtchargedNConst  ,"jtchargedNConst  [nref]/I");  
    t->Branch("jteSumConst"      ,jets_.jteSumConst      ,"jteSumConst      [nref]/F");  
    t->Branch("jteNConst"        ,jets_.jteNConst        ,"jteNConst        [nref]/I");  
    t->Branch("jtmuSumConst"     ,jets_.jtmuSumConst     ,"jtmuSumConst     [nref]/F");  
    t->Branch("jtmuNConst"       ,jets_.jtmuNConst       ,"jtmuNConst       [nref]/I");  
    t->Branch("jtphotonSumConst" ,jets_.jtphotonSumConst ,"jtphotonSumConst [nref]/F"); 
    t->Branch("jtphotonNConst"   ,jets_.jtphotonNConst   ,"jtphotonNConst   [nref]/I");  
    t->Branch("jtneutralSumConst",jets_.jtneutralSumConst,"jtneutralSumConst[nref]/F");
    t->Branch("jtneutralNConst"  ,jets_.jtneutralNConst  ,"jtneutralNConst  [nref]/I");  
    t->Branch("jthfhadSumConst"  ,jets_.jthfhadSumConst  ,"jthfhadSumConst[nref]/F");
    t->Branch("jthfhadNConst"    ,jets_.jthfhadNConst    ,"jthfhadNConst  [nref]/I");  
    t->Branch("jthfemSumConst"   ,jets_.jthfemSumConst   ,"jthfemSumConst[nref]/F");
    t->Branch("jthfemNConst"     ,jets_.jthfemNConst     ,"jthfemNConst  [nref]/I");  
    t->Branch("jtMByPt",jets_.jtMByPt,"jtMByPt[nref]/F");
    t->Branch("jtRMSCand",jets_.jtRMSCand,"jtRMSCand[nref]/F");
    t->Branch("jtAxis1",jets_.jtAxis1,"jtAxis1[nref]/F");
    t->Branch("jtAxis2",jets_.jtAxis2,"jtAxis2[nref]/F");
    t->Branch("jtSigma",jets_.jtSigma,"jtSigma[nref]/F");
    t->Branch("jtR",jets_.jtR,"jtR[nref]/F");
    t->Branch("jtpTD",jets_.jtpTD,"jtpTD[nref]/F");
    t->Branch("jtpull",jets_.jtpull,"jtpull[nref]/F");
    t->Branch("jtrm0p5",jets_.jtrm0p5,"jtrm0p5[nref]/F");
    t->Branch("jtrm1",jets_.jtrm1,"jtrm1[nref]/F");
    t->Branch("jtrm2",jets_.jtrm2,"jtrm2[nref]/F");
    t->Branch("jtrm3",jets_.jtrm3,"jtrm3[nref]/F");
    t->Branch("jtSDm",jets_.jtSDm, "jtSDm[nref]/F");
    t->Branch("jtSDpt",jets_.jtSDpt, "jtSDpt[nref]/F");
    t->Branch("jtSDeta",jets_.jtSDeta, "jtSDeta[nref]/F");
    t->Branch("jtSDphi",jets_.jtSDphi, "jtSDphi[nref]/F");
    t->Branch("jtSDptFrac",jets_.jtSDptFrac, "jtSDptFrac[nref]/F");
    t->Branch("jtSDrm0p5",jets_.jtSDrm0p5, "jtSDrm0p5[nref]/F");
    t->Branch("jtSDrm1",jets_.jtSDrm1, "jtSDrm1[nref]/F");
    t->Branch("jtSDrm2",jets_.jtSDrm2, "jtSDrm2[nref]/F");
    t->Branch("jtSDrm3",jets_.jtSDrm3, "jtSDrm3[nref]/F");
  
    t->Branch("jtTbeta20p2",jets_.jtTbeta20p2,"jtTbeta20p2[nref]/F"); 
    t->Branch("jtTbeta20p3",jets_.jtTbeta20p3,"jtTbeta20p3[nref]/F"); 
    t->Branch("jtTbeta20p4",jets_.jtTbeta20p4,"jtTbeta20p4[nref]/F"); 
    t->Branch("jtTbeta20p5",jets_.jtTbeta20p5,"jtTbeta20p5[nref]/F"); 

    t->Branch("jtTbeta30p2",jets_.jtTbeta30p2,"jtTbeta30p2[nref]/F"); 
    t->Branch("jtTbeta30p3",jets_.jtTbeta30p3,"jtTbeta30p3[nref]/F"); 
    t->Branch("jtTbeta30p4",jets_.jtTbeta30p4,"jtTbeta30p4[nref]/F"); 
    t->Branch("jtTbeta30p5",jets_.jtTbeta30p5,"jtTbeta30p5[nref]/F"); 
      
    t->Branch("jtCbeta20p2",jets_.jtCbeta20p2,"jtCbeta20p2[nref]/F"); 
    t->Branch("jtCbeta20p3",jets_.jtCbeta20p3,"jtCbeta20p3[nref]/F"); 
    t->Branch("jtCbeta20p4",jets_.jtCbeta20p4,"jtCbeta20p4[nref]/F"); 
    t->Branch("jtCbeta20p5",jets_.jtCbeta20p5,"jtCbeta20p5[nref]/F"); 
			 
    t->Branch("jtZ11",jets_.jtZ11,"jtZ11[nref]/F");
    t->Branch("jtZ20",jets_.jtZ20,"jtZ20[nref]/F");
    t->Branch("jtZ22",jets_.jtZ22,"jtZ22[nref]/F");
    t->Branch("jtZ31",jets_.jtZ31,"jtZ31[nref]/F");
    t->Branch("jtZ33",jets_.jtZ33,"jtZ33[nref]/F");
    t->Branch("jtZ40",jets_.jtZ40,"jtZ40[nref]/F");
    t->Branch("jtZ42",jets_.jtZ42,"jtZ42[nref]/F");
    t->Branch("jtZ44",jets_.jtZ44,"jtZ44[nref]/F");
    t->Branch("jtZ51",jets_.jtZ51,"jtZ51[nref]/F");
    t->Branch("jtZ53",jets_.jtZ53,"jtZ53[nref]/F");
    t->Branch("jtZ55",jets_.jtZ55,"jtZ55[nref]/F");


    t->Branch("jtPhi1",jets_.jtPhi1,"jtPhi1[nref]/F");
    t->Branch("jtPhi2",jets_.jtPhi2,"jtPhi2[nref]/F");
    t->Branch("jtPhi3",jets_.jtPhi3,"jtPhi3[nref]/F");
    t->Branch("jtPhi4",jets_.jtPhi4,"jtPhi4[nref]/F");
    t->Branch("jtPhi5",jets_.jtPhi5,"jtPhi5[nref]/F");
    t->Branch("jtPhi6",jets_.jtPhi6,"jtPhi6[nref]/F");
    t->Branch("jtPhi7",jets_.jtPhi7,"jtPhi7[nref]/F");

    t->Branch("jtSkx",jets_.jtSkx,"jtSkx[nref]/F");
    t->Branch("jtSky",jets_.jtSky,"jtSky[nref]/F");


  }
  
  t->Branch("jtPfCHF",jets_.jtPfCHF,"jtPfCHF[nref]/F");
  t->Branch("jtPfNHF",jets_.jtPfNHF,"jtPfNHF[nref]/F");
  t->Branch("jtPfCEF",jets_.jtPfCEF,"jtPfCEF[nref]/F");
  t->Branch("jtPfNEF",jets_.jtPfNEF,"jtPfNEF[nref]/F");
  t->Branch("jtPfMUF",jets_.jtPfMUF,"jtPfMUF[nref]/F");

  t->Branch("jtPfCHM",jets_.jtPfCHM,"jtPfCHM[nref]/I");
  t->Branch("jtPfNHM",jets_.jtPfNHM,"jtPfNHM[nref]/I");
  t->Branch("jtPfCEM",jets_.jtPfCEM,"jtPfCEM[nref]/I");
  t->Branch("jtPfNEM",jets_.jtPfNEM,"jtPfNEM[nref]/I");
  t->Branch("jtPfMUM",jets_.jtPfMUM,"jtPfMUM[nref]/I");

  t->Branch("jttau1",jets_.jttau1,"jttau1[nref]/F");
  t->Branch("jttau2",jets_.jttau2,"jttau2[nref]/F");
  t->Branch("jttau3",jets_.jttau3,"jttau3[nref]/F");

  if(doExtendedFlavorTagging_){
  	t->Branch("jtHadronFlavor",jets_.jtHadronFlavor,"jtHadronFlavor[nref]/I");
	t->Branch("jtPartonFlavor",jets_.jtPartonFlavor,"jtPartonFlavor[nref]/I");
	t->Branch("jtbHadronDR",&jets_.jtbHadronDR);
	t->Branch("jtbHadronPt",&jets_.jtbHadronPt);
	t->Branch("jtbHadronEta",&jets_.jtbHadronEta);
	t->Branch("jtbHadronPhi",&jets_.jtbHadronPhi);
	t->Branch("jtbHadronPdg",&jets_.jtbHadronPdg);
	t->Branch("jtcHadronDR",&jets_.jtcHadronDR);
	t->Branch("jtcHadronPt",&jets_.jtcHadronPt);
	t->Branch("jtcHadronEta",&jets_.jtcHadronEta);
	t->Branch("jtcHadronPhi",&jets_.jtcHadronPhi);
	t->Branch("jtcHadronPdg",&jets_.jtcHadronPdg);
	t->Branch("jtPartonDR",&jets_.jtPartonDR);
	t->Branch("jtPartonPt",&jets_.jtPartonPt);
	t->Branch("jtPartonEta",&jets_.jtPartonEta);
	t->Branch("jtPartonPhi",&jets_.jtPartonPhi);
	t->Branch("jtPartonPdg",&jets_.jtPartonPdg);

  } 
 
  if(doSubJets_) {
    t->Branch("jtSubJetPt",&jets_.jtSubJetPt);
    t->Branch("jtSubJetEta",&jets_.jtSubJetEta);
    t->Branch("jtSubJetPhi",&jets_.jtSubJetPhi);
    t->Branch("jtSubJetM",&jets_.jtSubJetM);
    t->Branch("jtSubJetcsvV2",&jets_.jtSubJetcsvV2);
    t->Branch("jtSubJetJP",&jets_.jtSubJetJP);
    t->Branch("jtSubJetVtxType", &jets_.jtSubJetVtxType);

    /*
    t->Branch("jtSubJetSvtxm",&jets_.jtSubJetSvtxm);
    t->Branch("jtSubJetSvtxpt",&jets_.jtSubJetSvtxpt);
    t->Branch("jtSubJetSvtxeta",&jets_.jtSubJetSvtxeta);
    t->Branch("jtSubJetSvtxphi",&jets_.jtSubJetSvtxphi);
    t->Branch("jtSubJetSvtxNtrk",&jets_.jtSubJetSvtxNtrk);
    t->Branch("jtSubJetSvtxdl",&jets_.jtSubJetSvtxdl);
    t->Branch("jtSubJetSvtxdls",&jets_.jtSubJetSvtxdls);
    */
    if(doExtendedFlavorTagging_){
	t->Branch("jtSubJetHadronFlavor",&jets_.jtSubJetHadronFlavor);
	t->Branch("jtSubJetPartonFlavor",&jets_.jtSubJetPartonFlavor);
	/*
	t->Branch("jtSubJetHadronDR",&jets_.jtSubJetHadronDR);
	t->Branch("jtSubJetHadronPt",&jets_.jtSubJetHadronPt);
	t->Branch("jtSubJetHadronEta",&jets_.jtSubJetHadronEta);
	t->Branch("jtSubJetHadronPhi",&jets_.jtSubJetHadronPhi);
	t->Branch("jtSubJetHadronPdg",&jets_.jtSubJetHadronPdg);
	t->Branch("jtSubJetPartonDR",&jets_.jtSubJetPartonDR);
	t->Branch("jtSubJetPartonPt",&jets_.jtSubJetPartonPt);
	t->Branch("jtSubJetPartonEta",&jets_.jtSubJetPartonEta);
	t->Branch("jtSubJetPartonPhi",&jets_.jtSubJetPartonPhi);
	t->Branch("jtSubJetPartonPdg",&jets_.jtSubJetPartonPdg);
	*/
    }
  }

  if(doJetConstituents_){
    t->Branch("jtConstituentsId",&jets_.jtConstituentsId);
    t->Branch("jtConstituentsE",&jets_.jtConstituentsE);
    t->Branch("jtConstituentsPt",&jets_.jtConstituentsPt);
    t->Branch("jtConstituentsEta",&jets_.jtConstituentsEta);
    t->Branch("jtConstituentsPhi",&jets_.jtConstituentsPhi);
    t->Branch("jtConstituentsM",&jets_.jtConstituentsM);  
    t->Branch("jtSDConstituentsId",&jets_.jtSDConstituentsId);
    t->Branch("jtSDConstituentsE",&jets_.jtSDConstituentsE);
    t->Branch("jtSDConstituentsPt",&jets_.jtSDConstituentsPt);
    t->Branch("jtSDConstituentsEta",&jets_.jtSDConstituentsEta);
    t->Branch("jtSDConstituentsPhi",&jets_.jtSDConstituentsPhi);
    t->Branch("jtSDConstituentsM",&jets_.jtSDConstituentsM);  
  }
  // jet ID information, jet composition
  if(doHiJetID_){
    t->Branch("discr_jetID_cuts", jets_.discr_jetID_cuts,"discr_jetID_cuts[nref]/F");
    t->Branch("discr_jetID_bdt", jets_.discr_jetID_bdt,"discr_jetID_bdt[nref]/F");

    t->Branch("discr_fr01", jets_.discr_fr01,"discr_fr01[nref]/F");

    t->Branch("trackMax", jets_.trackMax,"trackMax[nref]/F");
    t->Branch("trackSum", jets_.trackSum,"trackSum[nref]/F");
    t->Branch("trackN", jets_.trackN,"trackN[nref]/I");
    t->Branch("trackHardSum", jets_.trackHardSum,"trackHardSum[nref]/F");
    t->Branch("trackHardN", jets_.trackHardN,"trackHardN[nref]/I");

    t->Branch("chargedMax", jets_.chargedMax,"chargedMax[nref]/F");
    t->Branch("chargedSum", jets_.chargedSum,"chargedSum[nref]/F");
    t->Branch("chargedN", jets_.chargedN,"chargedN[nref]/I");
    t->Branch("chargedHardSum", jets_.chargedHardSum,"chargedHardSum[nref]/F");
    t->Branch("chargedHardN", jets_.chargedHardN,"chargedHardN[nref]/I");

    t->Branch("photonMax", jets_.photonMax,"photonMax[nref]/F");
    t->Branch("photonSum", jets_.photonSum,"photonSum[nref]/F");
    t->Branch("photonN", jets_.photonN,"photonN[nref]/I");
    t->Branch("photonHardSum", jets_.photonHardSum,"photonHardSum[nref]/F");
    t->Branch("photonHardN", jets_.photonHardN,"photonHardN[nref]/I");

    t->Branch("neutralMax", jets_.neutralMax,"neutralMax[nref]/F");
    t->Branch("neutralSum", jets_.neutralSum,"neutralSum[nref]/F");
    t->Branch("neutralN", jets_.neutralN,"neutralN[nref]/I");

    t->Branch("hcalSum", jets_.hcalSum,"hcalSum[nref]/F");
    t->Branch("ecalSum", jets_.ecalSum,"ecalSum[nref]/F");

    t->Branch("eMax", jets_.eMax,"eMax[nref]/F");
    t->Branch("eSum", jets_.eSum,"eSum[nref]/F");
    t->Branch("eN", jets_.eN,"eN[nref]/I");

    t->Branch("muMax", jets_.muMax,"muMax[nref]/F");
    t->Branch("muSum", jets_.muSum,"muSum[nref]/F");
    t->Branch("muN", jets_.muN,"muN[nref]/I");
  }

  if(doStandardJetID_){
    t->Branch("fHPD",jets_.fHPD,"fHPD[nref]/F");
    t->Branch("fRBX",jets_.fRBX,"fRBX[nref]/F");
    t->Branch("n90",jets_.n90,"n90[nref]/I");
    t->Branch("fSubDet1",jets_.fSubDet1,"fSubDet1[nref]/F");
    t->Branch("fSubDet2",jets_.fSubDet2,"fSubDet2[nref]/F");
    t->Branch("fSubDet3",jets_.fSubDet3,"fSubDet3[nref]/F");
    t->Branch("fSubDet4",jets_.fSubDet4,"fSubDet4[nref]/F");
    t->Branch("restrictedEMF",jets_.restrictedEMF,"restrictedEMF[nref]/F");
    t->Branch("nHCAL",jets_.nHCAL,"nHCAL[nref]/I");
    t->Branch("nECAL",jets_.nECAL,"nECAL[nref]/I");
    t->Branch("apprHPD",jets_.apprHPD,"apprHPD[nref]/F");
    t->Branch("apprRBX",jets_.apprRBX,"apprRBX[nref]/F");

    //  t->Branch("hitsInN90",jets_.n90,"hitsInN90[nref]");
    t->Branch("n2RPC",jets_.n2RPC,"n2RPC[nref]/I");
    t->Branch("n3RPC",jets_.n3RPC,"n3RPC[nref]/I");
    t->Branch("nRPC",jets_.nRPC,"nRPC[nref]/I");

    t->Branch("fEB",jets_.fEB,"fEB[nref]/F");
    t->Branch("fEE",jets_.fEE,"fEE[nref]/F");
    t->Branch("fHB",jets_.fHB,"fHB[nref]/F");
    t->Branch("fHE",jets_.fHE,"fHE[nref]/F");
    t->Branch("fHO",jets_.fHO,"fHO[nref]/F");
    t->Branch("fLong",jets_.fLong,"fLong[nref]/F");
    t->Branch("fShort",jets_.fShort,"fShort[nref]/F");
    t->Branch("fLS",jets_.fLS,"fLS[nref]/F");
    t->Branch("fHFOOT",jets_.fHFOOT,"fHFOOT[nref]/F");
  }

  // Jet ID
  if(doMatch_){
    if(!skipCorrections_) t->Branch("matchedPt", jets_.matchedPt,"matchedPt[nref]/F");
    t->Branch("matchedRawPt", jets_.matchedRawPt,"matchedRawPt[nref]/F");
    t->Branch("matchedPu", jets_.matchedPu,"matchedPu[nref]/F");
    t->Branch("matchedR", jets_.matchedR,"matchedR[nref]/F");
  }

  // b-jet discriminators
  if (doLifeTimeTagging_) {

    t->Branch("discr_ssvHighEff",jets_.discr_ssvHighEff,"discr_ssvHighEff[nref]/F");
    t->Branch("discr_ssvHighPur",jets_.discr_ssvHighPur,"discr_ssvHighPur[nref]/F");

    t->Branch("discr_csvV1",jets_.discr_csvV1,"discr_csvV1[nref]/F");
    t->Branch("discr_csvV2",jets_.discr_csvV2,"discr_csvV2[nref]/F");
    t->Branch("discr_muByIp3",jets_.discr_muByIp3,"discr_muByIp3[nref]/F");
    t->Branch("discr_muByPt",jets_.discr_muByPt,"discr_muByPt[nref]/F");
    t->Branch("discr_prob",jets_.discr_prob,"discr_prob[nref]/F");
    t->Branch("discr_probb",jets_.discr_probb,"discr_probb[nref]/F");


    t->Branch("nsvtx",    jets_.nsvtx,    "nsvtx[nref]/I");
    t->Branch("svType", &jets_.svType);
    t->Branch("svtxntrk", &jets_.svtxntrk);
    t->Branch("svtxdl",   &jets_.svtxdl);
    t->Branch("svtxdls",  &jets_.svtxdls);
    t->Branch("svtxdl2d", &jets_.svtxdl2d);
    t->Branch("svtxdls2d", &jets_.svtxdls2d);
    t->Branch("svtxm",    &jets_.svtxm);
    t->Branch("svtxpt",   &jets_.svtxpt);
    t->Branch("svtxFlPhi",   &jets_.svtxFlPhi);
    t->Branch("svtxFlEta",   &jets_.svtxFlEta);
    t->Branch("svJetDeltaR", &jets_.svJetDeltaR);
    t->Branch("svtxmcorr", &jets_.svtxmcorr);

    t->Branch("nIPtrk",jets_.nIPtrk,"nIPtrk[nref]/I");
    t->Branch("nselIPtrk",jets_.nselIPtrk,"nselIPtrk[nref]/I");

    if (doLifeTimeTaggingExtras_) {
      t->Branch("nIP",&jets_.nIP,"nIP/I");
      t->Branch("ipJetIndex",jets_.ipJetIndex,"ipJetIndex[nIP]/I");
      t->Branch("ipPt",jets_.ipPt,"ipPt[nIP]/F");
      t->Branch("ipProb0",jets_.ipProb0,"ipProb0[nIP]/F");
      t->Branch("ipProb1",jets_.ipProb1,"ipProb1[nIP]/F");
      t->Branch("ip2d",jets_.ip2d,"ip2d[nIP]/F");
      t->Branch("ip2dSig",jets_.ip2dSig,"ip2dSig[nIP]/F");
      t->Branch("ip3d",jets_.ip3d,"ip3d[nIP]/F");
      t->Branch("ip3dSig",jets_.ip3dSig,"ip3dSig[nIP]/F");
      t->Branch("ipDist2Jet",jets_.ipDist2Jet,"ipDist2Jet[nIP]/F");
      t->Branch("ipDist2JetSig",jets_.ipDist2JetSig,"ipDist2JetSig[nIP]/F");
      t->Branch("ipClosest2Jet",jets_.ipClosest2Jet,"ipClosest2Jet[nIP]/F");

    }

  }


  if(isMC_){
    t->Branch("beamId1",&jets_.beamId1,"beamId1/I");
    t->Branch("beamId2",&jets_.beamId2,"beamId2/I");

    t->Branch("pthat",&jets_.pthat,"pthat/F");

    // Only matched gen jets
    t->Branch("refpt",jets_.refpt,"refpt[nref]/F");
    t->Branch("refeta",jets_.refeta,"refeta[nref]/F");
    t->Branch("refy",jets_.refy,"refy[nref]/F");
    t->Branch("refphi",jets_.refphi,"refphi[nref]/F");
    t->Branch("refm",jets_.refm,"refm[nref]/F");
    t->Branch("refarea",jets_.refarea,"refarea[nref]/F");

    if(doNewJetVars_){
      t->Branch("refnCands",jets_.refnCands,"refnCands[nref]/I");
      t->Branch("refnChCands",jets_.refnChCands,"refnChCands[nref]/I");
      t->Branch("refnNeCands",jets_.refnNeCands,"refnNeCands[nref]/I");
      t->Branch("refchargedSumConst",jets_.refchargedSumConst,"refchargedSumConst[nref]/F");
      t->Branch("refchargedNConst"  ,jets_.refchargedNConst  ,"refchargedNConst  [nref]/I");  
      t->Branch("refeSumConst"      ,jets_.refeSumConst      ,"refeSumConst      [nref]/F");  
      t->Branch("refeNConst"        ,jets_.refeNConst        ,"refeNConst        [nref]/I");  
      t->Branch("refmuSumConst"     ,jets_.refmuSumConst     ,"refmuSumConst     [nref]/F");  
      t->Branch("refmuNConst"       ,jets_.refmuNConst       ,"refmuNConst       [nref]/I");  
      t->Branch("refphotonSumConst" ,jets_.refphotonSumConst ,"refphotonSumConst [nref]/F"); 
      t->Branch("refphotonNConst"   ,jets_.refphotonNConst   ,"refphotonNConst   [nref]/I");  
      t->Branch("refneutralSumConst",jets_.refneutralSumConst,"refneutralSumConst[nref]/F");
      t->Branch("refneutralNConst"  ,jets_.refneutralNConst  ,"refneutralNConst  [nref]/I");  
      t->Branch("refhfhadSumConst"  ,jets_.refhfhadSumConst  ,"refhfhadSumConst  [nref]/F");
      t->Branch("refhfhadNConst"    ,jets_.refhfhadNConst    ,"refhfhadNConst    [nref]/I");  
      t->Branch("refhfemSumConst"   ,jets_.refhfemSumConst   ,"refhfemSumConst   [nref]/F");
      t->Branch("refhfemNConst"     ,jets_.refhfemNConst     ,"refhfemNConst     [nref]/I");  

      t->Branch("refMByPt",jets_.refMByPt,"refMByPt[nref]/F");
      t->Branch("refRMSCand",jets_.refRMSCand,"refRMSCand[nref]/F");
      t->Branch("refAxis1",jets_.refAxis1,"refAxis1[nref]/F");
      t->Branch("refAxis2",jets_.refAxis2,"refAxis2[nref]/F");
      t->Branch("refSigma",jets_.refSigma,"refSigma[nref]/F");
      t->Branch("refR",jets_.refR,"refR[nref]/F");
      t->Branch("refpTD",jets_.refpTD,"refpTD[nref]/F");
      t->Branch("refpull",jets_.refpull,"refpull[nref]/F");
      t->Branch("refrm0p5",jets_.refrm0p5,"refrm0p5[nref]/F");
      t->Branch("refrm1",jets_.refrm1,"refrm1[nref]/F");
      t->Branch("refrm2",jets_.refrm2,"refrm2[nref]/F");
      t->Branch("refrm3",jets_.refrm3,"refrm3[nref]/F");

      t->Branch("refSDm",jets_.refSDm, "refSDm[nref]/F");
      t->Branch("refSDpt",jets_.refSDpt, "refSDpt[nref]/F");
      t->Branch("refSDeta",jets_.refSDeta, "refSDeta[nref]/F");
      t->Branch("refSDphi",jets_.refSDphi, "refSDphi[nref]/F");
      t->Branch("refSDptFrac",jets_.refSDptFrac, "refSDptFrac[nref]/F");
      t->Branch("refSDrm0p5",jets_.refSDrm0p5, "refSDrm0p5[nref]/F");
      t->Branch("refSDrm1",jets_.refSDrm1, "refSDrm1[nref]/F");
      t->Branch("refSDrm2",jets_.refSDrm2, "refSDrm2[nref]/F");
      t->Branch("refSDrm3",jets_.refSDrm3, "refSDrm3[nref]/F");

      t->Branch("refTbeta20p2",jets_.refTbeta20p2,"refTbeta20p2[nref]/F"); 
      t->Branch("refTbeta20p3",jets_.refTbeta20p3,"refTbeta20p3[nref]/F"); 
      t->Branch("refTbeta20p4",jets_.refTbeta20p4,"refTbeta20p4[nref]/F"); 
      t->Branch("refTbeta20p5",jets_.refTbeta20p5,"refTbeta20p5[nref]/F"); 

      t->Branch("refTbeta30p2",jets_.refTbeta30p2,"refTbeta30p2[nref]/F"); 
      t->Branch("refTbeta30p3",jets_.refTbeta30p3,"refTbeta30p3[nref]/F"); 
      t->Branch("refTbeta30p4",jets_.refTbeta30p4,"refTbeta30p4[nref]/F"); 
      t->Branch("refTbeta30p5",jets_.refTbeta30p5,"refTbeta30p5[nref]/F"); 
      
      t->Branch("refCbeta20p2",jets_.refCbeta20p2,"refCbeta20p2[nref]/F"); 
      t->Branch("refCbeta20p3",jets_.refCbeta20p3,"refCbeta20p3[nref]/F"); 
      t->Branch("refCbeta20p4",jets_.refCbeta20p4,"refCbeta20p4[nref]/F"); 
      t->Branch("refCbeta20p5",jets_.refCbeta20p5,"refCbeta20p5[nref]/F"); 
			 
      t->Branch("refZ11",jets_.refZ11,"refZ11[nref]/F");
      t->Branch("refZ20",jets_.refZ20,"refZ20[nref]/F");
      t->Branch("refZ22",jets_.refZ22,"refZ22[nref]/F");
      t->Branch("refZ31",jets_.refZ31,"refZ31[nref]/F");
      t->Branch("refZ33",jets_.refZ33,"refZ33[nref]/F");
      t->Branch("refZ40",jets_.refZ40,"refZ40[nref]/F");
      t->Branch("refZ42",jets_.refZ42,"refZ42[nref]/F");
      t->Branch("refZ44",jets_.refZ44,"refZ44[nref]/F");
      t->Branch("refZ51",jets_.refZ51,"refZ51[nref]/F");
      t->Branch("refZ53",jets_.refZ53,"refZ53[nref]/F");
      t->Branch("refZ55",jets_.refZ55,"refZ55[nref]/F");


      t->Branch("refPhi1",jets_.refPhi1,"refPhi1[nref]/F");
      t->Branch("refPhi2",jets_.refPhi2,"refPhi2[nref]/F");
      t->Branch("refPhi3",jets_.refPhi3,"refPhi3[nref]/F");
      t->Branch("refPhi4",jets_.refPhi4,"refPhi4[nref]/F");
      t->Branch("refPhi5",jets_.refPhi5,"refPhi5[nref]/F");
      t->Branch("refPhi6",jets_.refPhi6,"refPhi6[nref]/F");
      t->Branch("refPhi7",jets_.refPhi7,"refPhi7[nref]/F");

      t->Branch("refSkx",jets_.refSkx,"refSkx[nref]/F");
      t->Branch("refSky",jets_.refSky,"refSky[nref]/F");
    }
    
    if(doGenTaus_) {
      t->Branch("reftau1",jets_.reftau1,"reftau1[nref]/F");
      t->Branch("reftau2",jets_.reftau2,"reftau2[nref]/F");
      t->Branch("reftau3",jets_.reftau3,"reftau3[nref]/F");
    }
    t->Branch("refdphijt",jets_.refdphijt,"refdphijt[nref]/F");
    t->Branch("refdrjt",jets_.refdrjt,"refdrjt[nref]/F");
    // matched parton
    t->Branch("refparton_pt",jets_.refparton_pt,"refparton_pt[nref]/F");
    t->Branch("refparton_flavor",jets_.refparton_flavor,"refparton_flavor[nref]/I");

    t->Branch("refparton_flavorProcess",jets_.refparton_flavorProcess,"refparton_flavorProcess[nref]/I");			

    if(doGenSubJets_) {
      t->Branch("refptG",jets_.refptG,"refptG[nref]/F");
      t->Branch("refetaG",jets_.refetaG,"refetaG[nref]/F");
      t->Branch("refphiG",jets_.refphiG,"refphiG[nref]/F");
      t->Branch("refmG",jets_.refmG,"refmG[nref]/F");
      t->Branch("refSubJetPt",&jets_.refSubJetPt);
      t->Branch("refSubJetEta",&jets_.refSubJetEta);
      t->Branch("refSubJetPhi",&jets_.refSubJetPhi);
      t->Branch("refSubJetM",&jets_.refSubJetM);
    }

    if(doJetConstituents_){
      t->Branch("refConstituentsId",&jets_.refConstituentsId);
      t->Branch("refConstituentsE",&jets_.refConstituentsE);
      t->Branch("refConstituentsPt",&jets_.refConstituentsPt);
      t->Branch("refConstituentsEta",&jets_.refConstituentsEta);
      t->Branch("refConstituentsPhi",&jets_.refConstituentsPhi);
      t->Branch("refConstituentsM",&jets_.refConstituentsM);  
      t->Branch("refSDConstituentsId",&jets_.refSDConstituentsId);
      t->Branch("refSDConstituentsE",&jets_.refSDConstituentsE);
      t->Branch("refSDConstituentsPt",&jets_.refSDConstituentsPt);
      t->Branch("refSDConstituentsEta",&jets_.refSDConstituentsEta);
      t->Branch("refSDConstituentsPhi",&jets_.refSDConstituentsPhi);
      t->Branch("refSDConstituentsM",&jets_.refSDConstituentsM);  
    }

    
    t->Branch("genChargedSum", jets_.genChargedSum,"genChargedSum[nref]/F");
    t->Branch("genHardSum", jets_.genHardSum,"genHardSum[nref]/F");
    t->Branch("signalChargedSum", jets_.signalChargedSum,"signalChargedSum[nref]/F");
    t->Branch("signalHardSum", jets_.signalHardSum,"signalHardSum[nref]/F");

    if(doSubEvent_){
      t->Branch("subid",jets_.subid,"subid[nref]/I");
    }

    if(fillGenJets_){
      // For all gen jets, matched or unmatched
      t->Branch("ngen",&jets_.ngen,"ngen/I");
      t->Branch("genmatchindex",jets_.genmatchindex,"genmatchindex[ngen]/I");
      t->Branch("genpt",jets_.genpt,"genpt[ngen]/F");
      t->Branch("geneta",jets_.geneta,"geneta[ngen]/F");
      t->Branch("geny",jets_.geny,"geny[ngen]/F");
      if(doGenTaus_) {
        t->Branch("gentau1",jets_.gentau1,"gentau1[ngen]/F");
        t->Branch("gentau2",jets_.gentau2,"gentau2[ngen]/F");
        t->Branch("gentau3",jets_.gentau3,"gentau3[ngen]/F");
      }
      t->Branch("genphi",jets_.genphi,"genphi[ngen]/F");
      t->Branch("genm",jets_.genm,"genm[ngen]/F");
      t->Branch("gendphijt",jets_.gendphijt,"gendphijt[ngen]/F");
      t->Branch("gendrjt",jets_.gendrjt,"gendrjt[ngen]/F");

      if(doNewJetVars_){
	t->Branch("gennCands",jets_.gennCands,"gennCands[ngen]/I");
	t->Branch("gennChCands",jets_.gennChCands,"gennChCands[ngen]/I");
	t->Branch("gennNeCands",jets_.gennNeCands,"gennNeCands[ngen]/I");
	t->Branch("genchargedNConst"  ,jets_.genchargedNConst  ,"genchargedNConst  [ngen]/I");  
	t->Branch("geneSumConst"      ,jets_.geneSumConst      ,"geneSumConst      [ngen]/F");  
	t->Branch("geneNConst"        ,jets_.geneNConst        ,"geneNConst        [ngen]/I");  
	t->Branch("genmuSumConst"     ,jets_.genmuSumConst     ,"genmuSumConst     [ngen]/F");  
	t->Branch("genmuNConst"       ,jets_.genmuNConst       ,"genmuNConst       [ngen]/I");  
	t->Branch("genphotonSumConst" ,jets_.genphotonSumConst ,"genphotonSumConst [ngen]/F"); 
	t->Branch("genphotonNConst"   ,jets_.genphotonNConst   ,"genphotonNConst   [ngen]/I");  
	t->Branch("genneutralSumConst",jets_.genneutralSumConst,"genneutralSumConst[ngen]/F");
	t->Branch("genneutralNConst"  ,jets_.genneutralNConst  ,"genneutralNConst  [ngen]/I");  
	t->Branch("genhfhadSumConst"  ,jets_.genhfhadSumConst  ,"genhfhadSumConst  [ngen]/F");
	t->Branch("genhfhadNConst"    ,jets_.genhfhadNConst    ,"genhfhadNConst    [ngen]/I");  
	t->Branch("genhfemSumConst"   ,jets_.genhfemSumConst   ,"genhfemSumConst   [ngen]/F");
	t->Branch("genhfemNConst"     ,jets_.genhfemNConst     ,"genhfemNConst     [ngen]/I");  

	t->Branch("genMByPt",jets_.genMByPt,"genMByPt[ngen]/F");
	t->Branch("genRMSCand",jets_.genRMSCand,"genRMSCand[ngen]/F");
	t->Branch("genAxis1",jets_.genAxis1,"genAxis1[ngen]/F");
	t->Branch("genAxis2",jets_.genAxis2,"genAxis2[ngen]/F");
	t->Branch("genSigma",jets_.genSigma,"genSigma[ngen]/F");
	t->Branch("genR",jets_.genR,"genR[ngen]/F");
	t->Branch("genpTD",jets_.genpTD,"genpTD[ngen]/F");
	t->Branch("genpull",jets_.genpull,"genpull[ngen]/F");
	t->Branch("genrm0p5",jets_.genrm0p5,"genrm0p5[ngen]/F");
	t->Branch("genrm1",jets_.genrm1,"genrm1[ngen]/F");
	t->Branch("genrm2",jets_.genrm2,"genrm2[ngen]/F");
	t->Branch("genrm3",jets_.genrm3,"genrm3[ngen]/F");

	t->Branch("genSDm",jets_.genSDm, "genSDm[ngen]/F");
	t->Branch("genSDpt",jets_.genSDpt, "genSDpt[ngen]/F");
	t->Branch("genSDeta",jets_.genSDeta, "genSDeta[ngen]/F");
	t->Branch("genSDphi",jets_.genSDphi, "genSDphi[ngen]/F");
	t->Branch("genSDptFrac",jets_.genSDptFrac, "genSDptFrac[ngen]/F");
	t->Branch("genSDrm0p5",jets_.genSDrm0p5, "genSDrm0p5[ngen]/F");
	t->Branch("genSDrm1",jets_.genSDrm1, "genSDrm1[ngen]/F");
	t->Branch("genSDrm2",jets_.genSDrm2, "genSDrm2[ngen]/F");
	t->Branch("genSDrm3",jets_.genSDrm3, "genSDrm3[ngen]/F");

	t->Branch("genTbeta20p2",jets_.genTbeta20p2,"genTbeta20p2[ngen]/F"); 
	t->Branch("genTbeta20p3",jets_.genTbeta20p3,"genTbeta20p3[ngen]/F"); 
	t->Branch("genTbeta20p4",jets_.genTbeta20p4,"genTbeta20p4[ngen]/F"); 
	t->Branch("genTbeta20p5",jets_.genTbeta20p5,"genTbeta20p5[ngen]/F"); 
      
	t->Branch("genTbeta30p2",jets_.genTbeta30p2,"genTbeta30p2[ngen]/F"); 
	t->Branch("genTbeta30p3",jets_.genTbeta30p3,"genTbeta30p3[ngen]/F"); 
	t->Branch("genTbeta30p4",jets_.genTbeta30p4,"genTbeta30p4[ngen]/F"); 
	t->Branch("genTbeta30p5",jets_.genTbeta30p5,"genTbeta30p5[ngen]/F"); 

	t->Branch("genCbeta20p2",jets_.genCbeta20p2,"genCbeta20p2[ngen]/F"); 
	t->Branch("genCbeta20p3",jets_.genCbeta20p3,"genCbeta20p3[ngen]/F"); 
	t->Branch("genCbeta20p4",jets_.genCbeta20p4,"genCbeta20p4[ngen]/F"); 
	t->Branch("genCbeta20p5",jets_.genCbeta20p5,"genCbeta20p5[ngen]/F"); 

	t->Branch("genZ11",jets_.genZ11,"genZ11[ngen]/F");
	t->Branch("genZ20",jets_.genZ20,"genZ20[ngen]/F");
	t->Branch("genZ22",jets_.genZ22,"genZ22[ngen]/F");
	t->Branch("genZ31",jets_.genZ31,"genZ31[ngen]/F");
	t->Branch("genZ33",jets_.genZ33,"genZ33[ngen]/F");
	t->Branch("genZ40",jets_.genZ40,"genZ40[ngen]/F");
	t->Branch("genZ42",jets_.genZ42,"genZ42[ngen]/F");
	t->Branch("genZ44",jets_.genZ44,"genZ44[ngen]/F");
	t->Branch("genZ51",jets_.genZ51,"genZ51[ngen]/F");
	t->Branch("genZ53",jets_.genZ53,"genZ53[ngen]/F");
	t->Branch("genZ55",jets_.genZ55,"genZ55[ngen]/F");


	t->Branch("genPhi1",jets_.genPhi1,"genPhi1[ngen]/F");
	t->Branch("genPhi2",jets_.genPhi2,"genPhi2[ngen]/F");
	t->Branch("genPhi3",jets_.genPhi3,"genPhi3[ngen]/F");
	t->Branch("genPhi4",jets_.genPhi4,"genPhi4[ngen]/F");
	t->Branch("genPhi5",jets_.genPhi5,"genPhi5[ngen]/F");
	t->Branch("genPhi6",jets_.genPhi6,"genPhi6[ngen]/F");
	t->Branch("genPhi7",jets_.genPhi7,"genPhi7[ngen]/F");

	t->Branch("genSkx",jets_.genSkx,"genSkx[ngen]/F");
	t->Branch("genSky",jets_.genSky,"genSky[ngen]/F");
      }
      
      if(doGenSubJets_) {
        t->Branch("genptG",jets_.genptG,"genptG[ngen]/F");
        t->Branch("genetaG",jets_.genetaG,"genetaG[ngen]/F");
        t->Branch("genphiG",jets_.genphiG,"genphiG[ngen]/F");
        t->Branch("genmG",jets_.genmG,"genmG[ngen]/F");
        t->Branch("genSubJetPt",&jets_.genSubJetPt);
        t->Branch("genSubJetEta",&jets_.genSubJetEta);
        t->Branch("genSubJetPhi",&jets_.genSubJetPhi);
        t->Branch("genSubJetM",&jets_.genSubJetM);
      }

      if(doJetConstituents_){
	t->Branch("genConstituentsId",&jets_.genConstituentsId);
	t->Branch("genConstituentsE",&jets_.genConstituentsE);
	t->Branch("genConstituentsPt",&jets_.genConstituentsPt);
	t->Branch("genConstituentsEta",&jets_.genConstituentsEta);
	t->Branch("genConstituentsPhi",&jets_.genConstituentsPhi);
	t->Branch("genConstituentsM",&jets_.genConstituentsM);  
	t->Branch("genSDConstituentsId",&jets_.genSDConstituentsId);
	t->Branch("genSDConstituentsE",&jets_.genSDConstituentsE);
	t->Branch("genSDConstituentsPt",&jets_.genSDConstituentsPt);
	t->Branch("genSDConstituentsEta",&jets_.genSDConstituentsEta);
	t->Branch("genSDConstituentsPhi",&jets_.genSDConstituentsPhi);
	t->Branch("genSDConstituentsM",&jets_.genSDConstituentsM);  
      }
      
      if(doSubEvent_){
	t->Branch("gensubid",jets_.gensubid,"gensubid[ngen]/I");
      }
    }

    if(saveBfragments_  ) {
      t->Branch("bMult",&jets_.bMult,"bMult/I");
      t->Branch("bJetIndex",jets_.bJetIndex,"bJetIndex[bMult]/I");
      t->Branch("bStatus",jets_.bStatus,"bStatus[bMult]/I");
      t->Branch("bVx",jets_.bVx,"bVx[bMult]/F");
      t->Branch("bVy",jets_.bVy,"bVy[bMult]/F");
      t->Branch("bVz",jets_.bVz,"bVz[bMult]/F");
      t->Branch("bPt",jets_.bPt,"bPt[bMult]/F");
      t->Branch("bEta",jets_.bEta,"bEta[bMult]/F");
      t->Branch("bPhi",jets_.bPhi,"bPhi[bMult]/F");
      t->Branch("bPdg",jets_.bPdg,"bPdg[bMult]/I");
      t->Branch("bChg",jets_.bChg,"bChg[bMult]/I");
    }

  }
  /*
    if(!isMC_){
    t->Branch("nL1TBit",&jets_.nL1TBit,"nL1TBit/I");
    t->Branch("l1TBit",jets_.l1TBit,"l1TBit[nL1TBit]/O");

    t->Branch("nL1ABit",&jets_.nL1ABit,"nL1ABit/I");
    t->Branch("l1ABit",jets_.l1ABit,"l1ABit[nL1ABit]/O");

    t->Branch("nHLTBit",&jets_.nHLTBit,"nHLTBit/I");
    t->Branch("hltBit",jets_.hltBit,"hltBit[nHLTBit]/O");

    }
  */
  TH1D::SetDefaultSumw2();

}


void
HiInclusiveJetAnalyzer::analyze(const Event& iEvent,
				const EventSetup& iSetup) {




  int event = iEvent.id().event();
  int run = iEvent.id().run();
  int lumi = iEvent.id().luminosityBlock();

  jets_.run = run;
  jets_.evt = event;
  jets_.lumi = lumi;

  LogDebug("HiInclusiveJetAnalyzer")<<"START event: "<<event<<" in run "<<run<<endl;

  int bin = -1;
  double hf = 0.;
  double b = 999.;

  if(doHiJetID_ && !geo){
    edm::ESHandle<CaloGeometry> pGeo;
    iSetup.get<CaloGeometryRecord>().get(pGeo);
    geo = pGeo.product();
  }

  // loop the events

  jets_.bin = bin;
  jets_.hf = hf;

  reco::Vertex::Point vtx(0,0,0);
  if (useVtx_) {
    edm::Handle<vector<reco::Vertex> >vertex;
    iEvent.getByToken(vtxTag_, vertex);

    if(vertex->size()>0) {
      jets_.vx=vertex->begin()->x();
      jets_.vy=vertex->begin()->y();
      jets_.vz=vertex->begin()->z();
      vtx = vertex->begin()->position();
    }
	//cout << " all vertices: "<< endl;
	for(unsigned ivtx=0; ivtx<vertex->size(); ivtx++){ 
	//	cout << " vtx x: " << vertex->at(ivtx).x() << " y: "<< vertex->at(ivtx).y() << " z: " << vertex->at(ivtx).z() << endl;
	}
  }

  edm::Handle<pat::JetCollection> patjets;
  if(usePat_)iEvent.getByToken(jetTagPat_, patjets);
  edm::Handle<pat::JetCollection> patmatchedjets;
  iEvent.getByToken(matchTagPat_, patmatchedjets);

  edm::Handle<reco::JetView> matchedjets;
  iEvent.getByToken(matchTag_, matchedjets);

  if(doGenSubJets_)
    iEvent.getByToken(subjetGenTag_, gensubjets_);
  
  edm::Handle<reco::JetView> jets;
  iEvent.getByToken(jetTag_, jets);

  edm::Handle<reco::PFCandidateCollection> pfCandidates;
  iEvent.getByToken(pfCandidateLabel_,pfCandidates);

  edm::Handle<reco::TrackCollection> tracks;
  iEvent.getByToken(trackTag_,tracks);

  // edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > ebHits;
  // edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > eeHits;

  //edm::Handle<HFRecHitCollection> hfHits;
  //edm::Handle<HBHERecHitCollection> hbheHits;


  edm::Handle<reco::GenParticleCollection> genparts;
  iEvent.getByToken(genParticleSrc_,genparts);

  //Get all the b-tagging handles

  //------------------------------------------------------
  // Jet Probability tagger
  //------------------------------------------------------
  Handle<vector<TrackIPTagInfo> > tagInfo;
  Handle<JetTagCollection> jetTags_JP;
  Handle<JetTagCollection> jetTags_JB;
  Handle<JetTagCollection> jetTags_subjet_JP;

  //------------------------------------------------------
  // Secondary vertex taggers
  //------------------------------------------------------
  Handle<vector<SecondaryVertexTagInfo> > tagInfoSVx;
  Handle<JetTagCollection> jetTags_SvtxHighEff;
  Handle<JetTagCollection> jetTags_SvtxHighPur;
  Handle<JetTagCollection> jetTags_CombinedSvtx;
  Handle<JetTagCollection> jetTags_CombinedSvtxV2;
  
  Handle<JetTagCollection> jetTags_subjet_combinedSvtx;
  Handle<vector<TrackIPTagInfo> > subjetTagInfo;
  Handle<vector<SecondaryVertexTagInfo> > subjetTagInfoSVx;




  if(doLifeTimeTagging_){
    iEvent.getByToken(ImpactParameterTagInfos_, tagInfo);
    iEvent.getByToken(JetProbabilityBJetTags_, jetTags_JP);
    iEvent.getByToken(JetBProbabilityBJetTags_, jetTags_JB);
    iEvent.getByToken(SecondaryVertexTagInfos_, tagInfoSVx);
    iEvent.getByToken(SimpleSecondaryVertexHighEffBJetTags_, jetTags_SvtxHighEff);
    iEvent.getByToken(SimpleSecondaryVertexHighPurBJetTags_, jetTags_SvtxHighPur);
    iEvent.getByToken(CombinedSecondaryVertexBJetTags_, jetTags_CombinedSvtx);
    iEvent.getByToken(CombinedSecondaryVertexV2BJetTags_, jetTags_CombinedSvtxV2);
    if(doSubJets_){   
	    iEvent.getByToken(CombinedSubjetSecondaryVertexBJetTags_, jetTags_subjet_combinedSvtx);
	    iEvent.getByToken(SubjetJetProbabilityBJetTags_, jetTags_subjet_JP);
	    iEvent.getByToken(svImpactParameterTagInfos_,subjetTagInfo);
	    iEvent.getByToken(svSubjetTagInfos_,subjetTagInfoSVx);
    }

  }

  // get tower information
  edm::Handle<CaloTowerCollection> towers;
  if(doTower){
    iEvent.getByToken(TowerSrc_,towers);
  }

  // FILL JRA TREE
  jets_.b = b;
  jets_.nref = 0;

  if(doTrigger_){
    fillL1Bits(iEvent);
    fillHLTBits(iEvent);
  }

  if( doJetConstituents_ ){
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



  for(unsigned int j = 0; j < jets->size(); ++j){
    const reco::Jet& jet = (*jets)[j];

    if(jet.pt() < jetPtMin_) continue;
    if (useJEC_ && usePat_){
      jets_.rawpt[jets_.nref]=(*patjets)[j].correctedJet("Uncorrected").pt();
    }

    math::XYZVector jetDir = jet.momentum().Unit();
    if(doLifeTimeTagging_){
      int ith_tagged =    this->TaggedJet(jet,jetTags_SvtxHighEff);
      if(ith_tagged >= 0){
	jets_.discr_ssvHighEff[jets_.nref] = (*jetTags_SvtxHighEff)[ith_tagged].second;
       
	// assumes there's only one SV collection attached (note there's a different method for candidate SVs)
	const reco::SecondaryVertexTagInfo * tagInfoSV = (*patjets)[j].tagInfoSecondaryVertex();	
	const reco::CandSecondaryVertexTagInfo *tagInfoCandSV = (*patjets)[j].tagInfoCandSecondaryVertex();	
	
	if(tagInfoSV) fillInfoSV(tagInfoSV, jetDir);
	else if(tagInfoCandSV) fillInfoCandSV(tagInfoCandSV, jetDir);
      }

      ith_tagged    = this->TaggedJet(jet,jetTags_SvtxHighPur);
      if(ith_tagged >= 0) jets_.discr_ssvHighPur[jets_.nref]  = (*jetTags_SvtxHighPur)[ith_tagged].second;

      ith_tagged          = this->TaggedJet(jet,jetTags_CombinedSvtx);
      if(ith_tagged >= 0) jets_.discr_csvV1[jets_.nref]  = (*jetTags_CombinedSvtx)[ith_tagged].second;

      ith_tagged          = this->TaggedJet(jet,jetTags_CombinedSvtxV2);
      if(ith_tagged >= 0) jets_.discr_csvV2[jets_.nref]  = (*jetTags_CombinedSvtxV2)[ith_tagged].second;

      if(ith_tagged >= 0){
	ith_tagged = this->TaggedJet(jet,jetTags_JP);
	jets_.discr_prob[jets_.nref]  = (*jetTags_JP)[ith_tagged].second;

	const TrackIPTagInfo * tagInfoIP= (*patjets)[j].tagInfoTrackIP();
	const CandIPTagInfo * tagInfoCandIP= (*patjets)[j].tagInfoCandIP();

	if(tagInfoIP) fillInfoIP(tagInfoIP);
	else if(tagInfoCandIP) fillInfoCandIP(tagInfoCandIP);
      }
      ith_tagged = this->TaggedJet(jet,jetTags_JB);
      if(ith_tagged >= 0) jets_.discr_probb[jets_.nref]  = (*jetTags_JB)[ith_tagged].second;

    }

    if(doHiJetID_){
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

      jets_.hcalSum[jets_.nref] = 0;
      jets_.ecalSum[jets_.nref] = 0;

      jets_.genChargedSum[jets_.nref] = 0;
      jets_.genHardSum[jets_.nref] = 0;

      jets_.signalChargedSum[jets_.nref] = 0;
      jets_.signalHardSum[jets_.nref] = 0;

      jets_.subid[jets_.nref] = -1;

      for(unsigned int icand = 0; icand < tracks->size(); ++icand){
	const reco::Track& track = (*tracks)[icand];
	if(useQuality_ ){
	  bool goodtrack = track.quality(reco::TrackBase::qualityByName(trackQuality_));
	  if(!goodtrack) continue;
	}

	double dr = deltaR(jet,track);
	if(dr < rParam){
	  double ptcand = track.pt();
	  jets_.trackSum[jets_.nref] += ptcand;
	  jets_.trackN[jets_.nref] += 1;

	  if(ptcand > hardPtMin_){
	    jets_.trackHardSum[jets_.nref] += ptcand;
	    jets_.trackHardN[jets_.nref] += 1;

	  }
	  if(ptcand > jets_.trackMax[jets_.nref]) jets_.trackMax[jets_.nref] = ptcand;
	}
      }

      for(unsigned int icand = 0; icand < pfCandidates->size(); ++icand){
        const reco::PFCandidate& track = (*pfCandidates)[icand];
        double dr = deltaR(jet,track);
        if(dr < rParam){
	  double ptcand = track.pt();
	  int pfid = track.particleId();

	  switch(pfid){
	  case 1:
	    jets_.chargedSum[jets_.nref] += ptcand;
	    jets_.chargedN[jets_.nref] += 1;
	    if(ptcand > hardPtMin_){
	      jets_.chargedHardSum[jets_.nref] += ptcand;
	      jets_.chargedHardN[jets_.nref] += 1;
	    }
	    if(ptcand > jets_.chargedMax[jets_.nref]) jets_.chargedMax[jets_.nref] = ptcand;
	    break;

	  case 2:
	    jets_.eSum[jets_.nref] += ptcand;
	    jets_.eN[jets_.nref] += 1;
	    if(ptcand > jets_.eMax[jets_.nref]) jets_.eMax[jets_.nref] = ptcand;
	    break;

	  case 3:
	    jets_.muSum[jets_.nref] += ptcand;
	    jets_.muN[jets_.nref] += 1;
	    if(ptcand > jets_.muMax[jets_.nref]) jets_.muMax[jets_.nref] = ptcand;
	    break;

	  case 4:
	    jets_.photonSum[jets_.nref] += ptcand;
	    jets_.photonN[jets_.nref] += 1;
	    if(ptcand > hardPtMin_){
	      jets_.photonHardSum[jets_.nref] += ptcand;
	      jets_.photonHardN[jets_.nref] += 1;
	    }
	    if(ptcand > jets_.photonMax[jets_.nref]) jets_.photonMax[jets_.nref] = ptcand;
	    break;

	  case 5:
	    jets_.neutralSum[jets_.nref] += ptcand;
	    jets_.neutralN[jets_.nref] += 1;
	    if(ptcand > jets_.neutralMax[jets_.nref]) jets_.neutralMax[jets_.nref] = ptcand;
	    break;

	  default:
	    break;

	  }
	}
      }

      // Calorimeter fractions
      // Jet ID for CaloJets
      if(doTower){
	// changing it to take things from towers
	for(unsigned int i = 0; i < towers->size(); ++i){
	  
	  const CaloTower & hit= (*towers)[i];
	  double dr = deltaR(jet.eta(), jet.phi(), hit.p4(vtx).Eta(), hit.p4(vtx).Phi());
	  if(dr < rParam){
	    jets_.ecalSum[jets_.nref] += hit.emEt(vtx);
	    jets_.hcalSum[jets_.nref] += hit.hadEt(vtx);
	  }
	}
      }
    }

    


    if(doMatch_){
      // Alternative reconstruction matching (PF for calo, calo for PF)

      double drMin = 100;
      for(unsigned int imatch = 0 ; imatch < matchedjets->size(); ++imatch){
	const reco::Jet& mjet = (*matchedjets)[imatch];

	double dr = deltaR(jet,mjet);
	if(dr < drMin){
	  jets_.matchedPt[jets_.nref] = mjet.pt();

	  if(usePat_){
	    const pat::Jet& mpatjet = (*patmatchedjets)[imatch];
	    jets_.matchedRawPt[jets_.nref] = mpatjet.correctedJet("Uncorrected").pt();
	    jets_.matchedPu[jets_.nref] = mpatjet.pileup();
	  }
	  jets_.matchedR[jets_.nref] = dr;
	  drMin = dr;
	}
      }

    }
    //     if(etrk.quality(reco::TrackBase::qualityByName(qualityString_))) pev_.trkQual[pev_.nTrk]=1;

    if(doHiJetID_){
      // JetID Selections for 5 TeV PbPb
      // Cuts by Yen-Jie Lee, BDT by Kurt Jung
      double rawpt = jets_.rawpt[jets_.nref];

      float jtpt = jet.pt();
      int ijet = jets_.nref;
      jets_.discr_jetID_cuts[jets_.nref] = jets_.neutralMax[ijet]/rawpt*0.085 + 
					   jets_.photonMax[ijet]/rawpt*0.337 + 
					   jets_.chargedMax[ijet]/rawpt*0.584 + 
					   jets_.neutralSum[ijet]/rawpt*-0.454 + 
					   jets_.photonSum[ijet]/rawpt*-0.127 + 
					   jets_.chargedSum[ijet]/rawpt*(-0.239) + 
					   jets_.jtpu[ijet]/rawpt*(-0.184) + 0.173;
      //begin bdt - TMVA requires you to load in all input vars and names into separate containers (eyeroll)
      float tempVarAddr[] = {jets_.trackMax[ijet]/jtpt, jets_.trackHardSum[ijet]/jtpt, jets_.trackHardN[ijet]/jtpt, jets_.chargedN[ijet]/jtpt, jets_.chargedHardSum[ijet]/jtpt, jets_.chargedHardN[ijet]/jtpt, jets_.photonN[ijet]/jtpt, jets_.photonHardSum[ijet]/jtpt, jets_.photonHardN[ijet]/jtpt, jets_.neutralN[ijet]/jtpt, jets_.hcalSum[ijet]/jtpt, jets_.ecalSum[ijet]/jtpt, jets_.chargedMax[ijet]/jtpt, jets_.chargedSum[ijet], jets_.neutralMax[ijet]/jtpt, jets_.neutralSum[ijet]/jtpt, jets_.photonMax[ijet]/jtpt, jets_.photonSum[ijet]/jtpt, jets_.eSum[ijet]/jtpt, jets_.muSum[ijet]/jtpt};
      for(unsigned int ivar=0; ivar<(sizeof(tempVarAddr)/sizeof(tempVarAddr[0])); ivar++){ varAddr[ivar] = tempVarAddr[ivar]; }
      jets_.discr_jetID_bdt[jets_.nref] = reader->EvaluateMVA("BDTG");

      /////////////////////////////////////////////////////////////////
      // Jet core pt^2 discriminant for fake jets
      // Edited by Yue Shi Lai <ylai@mit.edu>

      // Initial value is 0
      jets_.discr_fr01[jets_.nref] = 0;
      // Start with no directional adaption, i.e. the fake rejection
      // axis is the jet axis
      float pseudorapidity_adapt = jets_.jteta[jets_.nref];
      float azimuth_adapt = jets_.jtphi[jets_.nref];

      // Unadapted discriminant with adaption search
      for (size_t iteration = 0; iteration < 2; iteration++) {
	float pseudorapidity_adapt_new = pseudorapidity_adapt;
	float azimuth_adapt_new = azimuth_adapt;
	float max_weighted_perp = 0;
	float perp_square_sum = 0;

	for (size_t index_pf_candidate = 0;
	     index_pf_candidate < pfCandidates->size();
	     index_pf_candidate++) {
	  const reco::PFCandidate &p =
	    (*pfCandidates)[index_pf_candidate];

	  switch (p.particleId()) {
	    //case 1:	// Charged hadron
	    //case 3:	// Muon
	  case 4:	// Photon
	    {
	      const float dpseudorapidity =
		p.eta() - pseudorapidity_adapt;
	      const float dazimuth =
		reco::deltaPhi(p.phi(), azimuth_adapt);
	      // The Gaussian scale factor is 0.5 / (0.1 * 0.1)
	      // = 50
	      const float angular_weight =
		exp(-50.0F * (dpseudorapidity * dpseudorapidity +
			      dazimuth * dazimuth));
	      const float weighted_perp =
		angular_weight * p.pt() * p.pt();
	      const float weighted_perp_square =
		weighted_perp * p.pt();

	      perp_square_sum += weighted_perp_square;
	      if (weighted_perp >= max_weighted_perp) {
		pseudorapidity_adapt_new = p.eta();
		azimuth_adapt_new = p.phi();
		max_weighted_perp = weighted_perp;
	      }
	    }
	  default:
	    break;
	  }
	}
	// Update the fake rejection value
	jets_.discr_fr01[jets_.nref] = std::max(
						jets_.discr_fr01[jets_.nref], perp_square_sum);
	// Update the directional adaption
	pseudorapidity_adapt = pseudorapidity_adapt_new;
	azimuth_adapt = azimuth_adapt_new;
      }
    }
    jets_.jtpt[jets_.nref] = jet.pt();
    jets_.jteta[jets_.nref] = jet.eta();
    jets_.jtphi[jets_.nref] = jet.phi();
    jets_.jty[jets_.nref] = jet.eta();
    jets_.jtpu[jets_.nref] = jet.pileup();
    jets_.jtm[jets_.nref] = jet.mass();
    jets_.jtarea[jets_.nref] = jet.jetArea();

    //! fill in the new jet varibles
    if(doNewJetVars_)
      fillNewJetVarsRecoJet(jet);

    jets_.jttau1[jets_.nref] = -999.;
    jets_.jttau2[jets_.nref] = -999.;
    jets_.jttau3[jets_.nref] = -999.;

   edm::Handle<reco::JetFlavourInfoMatchingCollection> theSubjetFlavourInfos;
   edm::Handle<edm::View<reco::Jet> > groomedJets; 
   if(doExtendedFlavorTagging_){
     if(doSubJets_ && useSubjets_){
       iEvent.getByToken(subjetFlavourInfosToken_, theSubjetFlavourInfos);
       iEvent.getByToken(groomedJetsToken_, groomedJets);
     }   
   }
   if(doSubJets_) {
     //cout<<" jet pt "<<jet.pt()<<" eta "<<jet.eta() <<" phi "<<jet.phi()<<endl;
       // new method using only pat
     if((*patjets)[j].hasSubjets("SoftDrop")){  // currently hard-coded, but could be setup to do multiple subjet collections
       const pat::JetPtrCollection & subjets = (*patjets)[j].subjets("SoftDrop");       
       cout<<" new method "<<endl;
       analyzeSubjets(subjets);
     }
     else {
       cout<<" old method "<<endl;
       // old method of grabbing subjets from reco::Jets, get rid of it eventually
       analyzeSubjets(jet, jets_.nref, theSubjetFlavourInfos, groomedJets, jetTags_subjet_combinedSvtx,  jetTags_subjet_JP, subjetTagInfo, subjetTagInfoSVx);
     }
   }
    if(usePat_){
      if( (*patjets)[j].hasUserFloat(jetName_+"Njettiness:tau1") )
        jets_.jttau1[jets_.nref] = (*patjets)[j].userFloat(jetName_+"Njettiness:tau1");
      if( (*patjets)[j].hasUserFloat(jetName_+"Njettiness:tau2") )
        jets_.jttau2[jets_.nref] = (*patjets)[j].userFloat(jetName_+"Njettiness:tau2");
      if( (*patjets)[j].hasUserFloat(jetName_+"Njettiness:tau3") )
        jets_.jttau3[jets_.nref] = (*patjets)[j].userFloat(jetName_+"Njettiness:tau3");

      if( (*patjets)[j].isPFJet()) {
        jets_.jtPfCHF[jets_.nref] = (*patjets)[j].chargedHadronEnergyFraction();
        jets_.jtPfNHF[jets_.nref] = (*patjets)[j].neutralHadronEnergyFraction();
        jets_.jtPfCEF[jets_.nref] = (*patjets)[j].chargedEmEnergyFraction();
        jets_.jtPfNEF[jets_.nref] = (*patjets)[j].neutralEmEnergyFraction();
        jets_.jtPfMUF[jets_.nref] = (*patjets)[j].muonEnergyFraction();

        jets_.jtPfCHM[jets_.nref] = (*patjets)[j].chargedHadronMultiplicity();
        jets_.jtPfNHM[jets_.nref] = (*patjets)[j].neutralHadronMultiplicity();
        jets_.jtPfCEM[jets_.nref] = (*patjets)[j].electronMultiplicity();
        jets_.jtPfNEM[jets_.nref] = (*patjets)[j].photonMultiplicity();
        jets_.jtPfMUM[jets_.nref] = (*patjets)[j].muonMultiplicity();
      }
        
      if(doStandardJetID_){
	jets_.fHPD[jets_.nref] = (*patjets)[j].jetID().fHPD;
	jets_.fRBX[jets_.nref] = (*patjets)[j].jetID().fRBX;
	jets_.n90[jets_.nref] = (*patjets)[j].n90();

	jets_.fSubDet1[jets_.nref] = (*patjets)[j].jetID().fSubDetector1;
	jets_.fSubDet2[jets_.nref] = (*patjets)[j].jetID().fSubDetector2;
	jets_.fSubDet3[jets_.nref] = (*patjets)[j].jetID().fSubDetector3;
	jets_.fSubDet4[jets_.nref] = (*patjets)[j].jetID().fSubDetector4;
	jets_.restrictedEMF[jets_.nref] = (*patjets)[j].jetID().restrictedEMF;
	jets_.nHCAL[jets_.nref] = (*patjets)[j].jetID().nHCALTowers;
	jets_.nECAL[jets_.nref] = (*patjets)[j].jetID().nECALTowers;
	jets_.apprHPD[jets_.nref] = (*patjets)[j].jetID().approximatefHPD;
	jets_.apprRBX[jets_.nref] = (*patjets)[j].jetID().approximatefRBX;

	//       jets_.n90[jets_.nref] = (*patjets)[j].jetID().hitsInN90;
	jets_.n2RPC[jets_.nref] = (*patjets)[j].jetID().numberOfHits2RPC;
	jets_.n3RPC[jets_.nref] = (*patjets)[j].jetID().numberOfHits3RPC;
	jets_.nRPC[jets_.nref] = (*patjets)[j].jetID().numberOfHitsRPC;

	jets_.fEB[jets_.nref] = (*patjets)[j].jetID().fEB;
	jets_.fEE[jets_.nref] = (*patjets)[j].jetID().fEE;
	jets_.fHB[jets_.nref] = (*patjets)[j].jetID().fHB;
	jets_.fHE[jets_.nref] = (*patjets)[j].jetID().fHE;
	jets_.fHO[jets_.nref] = (*patjets)[j].jetID().fHO;
	jets_.fLong[jets_.nref] = (*patjets)[j].jetID().fLong;
	jets_.fShort[jets_.nref] = (*patjets)[j].jetID().fShort;
	jets_.fLS[jets_.nref] = (*patjets)[j].jetID().fLS;
	jets_.fHFOOT[jets_.nref] = (*patjets)[j].jetID().fHFOOT;
      }

    }

    if(isMC_){

      for(UInt_t i = 0; i < genparts->size(); ++i){
	const reco::GenParticle& p = (*genparts)[i];
	if ( p.status()!=1 || p.charge()==0) continue;
	double dr = deltaR(jet,p);
	if(dr < rParam){
	  double ppt = p.pt();
	  jets_.genChargedSum[jets_.nref] += ppt;
	  if(ppt > hardPtMin_) jets_.genHardSum[jets_.nref] += ppt;
	  if(p.collisionId() == 0){
	    jets_.signalChargedSum[jets_.nref] += ppt;
	    if(ppt > hardPtMin_) jets_.signalHardSum[jets_.nref] += ppt;
	  }
	}
      }
    }

    if(isMC_ && usePat_){


      const reco::GenJet * genjet = (*patjets)[j].genJet();

      if(genjet){
	jets_.refpt[jets_.nref] = genjet->pt();
	jets_.refeta[jets_.nref] = genjet->eta();
	jets_.refphi[jets_.nref] = genjet->phi();
        jets_.refm[jets_.nref] = genjet->mass();
        jets_.refarea[jets_.nref] = genjet->jetArea();
        jets_.refy[jets_.nref] = genjet->eta();
	jets_.refdphijt[jets_.nref] = reco::deltaPhi(jet.phi(), genjet->phi());
	jets_.refdrjt[jets_.nref] = reco::deltaR(jet.eta(),jet.phi(),genjet->eta(),genjet->phi());

	if(doSubEvent_){
	  const GenParticle* gencon = genjet->getGenConstituent(0);
	  jets_.subid[jets_.nref] = gencon->collisionId();
	}

	if(doNewJetVars_)
	  fillNewJetVarsRefJet(*genjet);
	
        if(doGenSubJets_ && !(*patjets)[j].hasSubjets("SoftDrop"))  // if the subjets are in PAT, this is now in the analyzeSubjets function
	  analyzeRefSubjets(*genjet);

      }else{
	jets_.refpt[jets_.nref] = -999.;
	jets_.refeta[jets_.nref] = -999.;
	jets_.refphi[jets_.nref] = -999.;
        jets_.refm[jets_.nref] = -999.;
        jets_.refarea[jets_.nref] = -999.;
	jets_.refy[jets_.nref] = -999.;
	jets_.refdphijt[jets_.nref] = -999.;
	jets_.refdrjt[jets_.nref] = -999.;

	if(doNewJetVars_){
	  jets_.refnCands[jets_.nref] = -999;
	  jets_.refnChCands[jets_.nref] = -999;
	  jets_.refnNeCands[jets_.nref] = -999;
	  jets_.refchargedSumConst[jets_.nref] = -999;
	  jets_.refchargedNConst  [jets_.nref] = -999;  
	  jets_.refeSumConst      [jets_.nref] = -999;  
	  jets_.refeNConst        [jets_.nref] = -999;  
	  jets_.refmuSumConst     [jets_.nref] = -999;  
	  jets_.refmuNConst       [jets_.nref] = -999;  
	  jets_.refphotonSumConst [jets_.nref] = -999; 
	  jets_.refphotonNConst   [jets_.nref] = -999;  
	  jets_.refneutralSumConst[jets_.nref] = -999;
	  jets_.refneutralNConst  [jets_.nref] = -999;  
	  jets_.refMByPt[jets_.nref] = -999.;
	  jets_.refRMSCand[jets_.nref] = -999.;
	  jets_.refAxis1[jets_.nref] = -999.;
	  jets_.refAxis2[jets_.nref] = -999.;
	  jets_.refSigma[jets_.nref] = -999.;
	  jets_.refrm3[jets_.nref] = -999.;
	  jets_.refrm2[jets_.nref] = -999.;
	  jets_.refrm1[jets_.nref] = -999.;
	  jets_.refrm0p5[jets_.nref] = -999.;
	  jets_.refR[jets_.nref] = -999.;
	  jets_.refpull[jets_.nref] = -999.;
	  jets_.refpTD[jets_.nref] = -999.;
	  jets_.refSDm[jets_.nref] = -999;
	  jets_.refSDpt[jets_.nref] = -999;
	  jets_.refSDeta[jets_.nref] = -999;
	  jets_.refSDphi[jets_.nref] = -999;
	  jets_.refSDptFrac[jets_.nref] = -999;
	  jets_.refSDrm0p5[jets_.nref] = -999;
	  jets_.refSDrm1[jets_.nref] = -999;
	  jets_.refSDrm2[jets_.nref] = -999;
	  jets_.refSDrm3[jets_.nref] = -999;

	  jets_.refTbeta20p2[jets_.nref] = -999; 
	  jets_.refTbeta20p3[jets_.nref] = -999; 
	  jets_.refTbeta20p4[jets_.nref] = -999; 
	  jets_.refTbeta20p5[jets_.nref] = -999; 

	  jets_.refTbeta30p2[jets_.nref] = -999; 
	  jets_.refTbeta30p3[jets_.nref] = -999; 
	  jets_.refTbeta30p4[jets_.nref] = -999; 
	  jets_.refTbeta30p5[jets_.nref] = -999; 
			       
	  jets_.refCbeta20p2[jets_.nref] = -999; 
	  jets_.refCbeta20p3[jets_.nref] = -999; 
	  jets_.refCbeta20p4[jets_.nref] = -999; 
	  jets_.refCbeta20p5[jets_.nref] = -999; 

	  jets_.refZ11[jets_.nref] = -999;
	  jets_.refZ20[jets_.nref] = -999;
	  jets_.refZ22[jets_.nref] = -999;
	  jets_.refZ31[jets_.nref] = -999;
	  jets_.refZ33[jets_.nref] = -999;
	  jets_.refZ40[jets_.nref] = -999;
	  jets_.refZ42[jets_.nref] = -999;
	  jets_.refZ44[jets_.nref] = -999;
	  jets_.refZ51[jets_.nref] = -999;
	  jets_.refZ53[jets_.nref] = -999;
	  jets_.refZ55[jets_.nref] = -999;


	  jets_.refPhi1[jets_.nref] = -999;
	  jets_.refPhi2[jets_.nref] = -999;
	  jets_.refPhi3[jets_.nref] = -999;
	  jets_.refPhi4[jets_.nref] = -999;
	  jets_.refPhi5[jets_.nref] = -999;
	  jets_.refPhi6[jets_.nref] = -999;
	  jets_.refPhi7[jets_.nref] = -999;

	  jets_.refSkx[jets_.nref] = -999;
	  jets_.refSky[jets_.nref] = -999;
	}
	
	if( doJetConstituents_ ){
	  std::vector<int>   refCId;	  
	  std::vector<float> refCE;
	  std::vector<float> refCPt;
	  std::vector<float> refCEta;
	  std::vector<float> refCPhi;
	  std::vector<float> refCM;
	  refCId.push_back(-999);	  
	  refCE.push_back(-999);
	  refCPt.push_back(-999);
	  refCEta.push_back(-999);
	  refCPhi.push_back(-999);
	  refCM.push_back(-999);

	  jets_.refConstituentsId.push_back(refCId);
	  jets_.refConstituentsE.push_back(refCE);
	  jets_.refConstituentsPt.push_back(refCPt);
	  jets_.refConstituentsEta.push_back(refCEta);
	  jets_.refConstituentsPhi.push_back(refCPhi);
	  jets_.refConstituentsM.push_back(refCM);
	  
	  std::vector<int> refSDId;
	  std::vector<float> refSDCE;
	  std::vector<float> refSDCPt;
	  std::vector<float> refSDCEta;
	  std::vector<float> refSDCPhi;
	  std::vector<float> refSDCM;
	  
	  refSDId.push_back(-999);
	  refSDCE.push_back(-999);
	  refSDCPt.push_back(-999);
	  refSDCEta.push_back(-999);
	  refSDCPhi.push_back(-999);
	  refSDCM.push_back(-999);
	  
	  jets_.refSDConstituentsId.push_back(refSDId);
	  jets_.refSDConstituentsE.push_back(refSDCE);
	  jets_.refSDConstituentsPt.push_back(refSDCPt);
	  jets_.refSDConstituentsEta.push_back(refSDCEta);
	  jets_.refSDConstituentsPhi.push_back(refSDCPhi);
	  jets_.refSDConstituentsM.push_back(refSDCM);
	}

	if(doGenSubJets_) {
	  jets_.refptG[jets_.nref]  = -999.;
	  jets_.refetaG[jets_.nref] = -999.;
          jets_.refphiG[jets_.nref] = -999.;
          jets_.refmG[jets_.nref]   = -999.;

          std::vector<float> sjpt;
          std::vector<float> sjeta;
          std::vector<float> sjphi;
          std::vector<float> sjm;
          sjpt.push_back(-999.);
          sjeta.push_back(-999.);
          sjphi.push_back(-999.);
          sjm.push_back(-999.);

          jets_.refSubJetPt.push_back(sjpt);
          jets_.refSubJetEta.push_back(sjeta);
          jets_.refSubJetPhi.push_back(sjphi);
          jets_.refSubJetM.push_back(sjm);

        }
      }
      jets_.reftau1[jets_.nref] = -999.;
      jets_.reftau2[jets_.nref] = -999.;
      jets_.reftau3[jets_.nref] = -999.;
      
      jets_.jtHadronFlavor[jets_.nref] = (*patjets)[j].hadronFlavour();
      jets_.jtPartonFlavor[jets_.nref] = (*patjets)[j].partonFlavour();
     

      if(doExtendedFlavorTagging_){
	vector<float> bdr, bpt, beta, bphi, bpdg, cdr, cpt, ceta, cphi, cpdg, pdr, ppt, peta, pphi, ppdg;

	const reco::JetFlavourInfo & jetFlavor = (*patjets)[j].jetFlavourInfo();
	
	const reco::GenParticleRefVector &bHadrons = jetFlavor.getbHadrons();	
	for(reco::GenParticleRefVector::const_iterator it = bHadrons.begin(); it != bHadrons.end(); ++it){
	  bdr.push_back(reco::deltaR(jet.eta(), jet.phi(), (*it)->eta(), (*it)->phi()));
	  bpt.push_back((*it)->pt());
	  beta.push_back((*it)->eta());
	  bphi.push_back((*it)->phi());
	  bpdg.push_back((*it)->pdgId());
	}		

	const reco::GenParticleRefVector &cHadrons = jetFlavor.getcHadrons();
	for(reco::GenParticleRefVector::const_iterator it = cHadrons.begin(); it != cHadrons.end(); ++it){
	  cdr.push_back(reco::deltaR(jet.eta(), jet.phi(), (*it)->eta(), (*it)->phi()));
	  cpt.push_back((*it)->pt());
	  ceta.push_back((*it)->eta());
	  cphi.push_back((*it)->phi());
	  cpdg.push_back((*it)->pdgId());
	}

	const reco::GenParticleRefVector & partons = jetFlavor.getPartons();
	for(reco::GenParticleRefVector::const_iterator it = partons.begin(); it != partons.end(); ++it){
	  pdr.push_back(reco::deltaR( jet.eta(), jet.phi(), (*it)->eta(), (*it)->phi() ));
	  ppt.push_back((*it)->pt());
	  peta.push_back((*it)->eta());
	  pphi.push_back((*it)->phi());
	  ppdg.push_back((*it)->pdgId());
	}

	jets_.jtbHadronDR.push_back(bdr);
	jets_.jtbHadronPt.push_back(bpt);
	jets_.jtbHadronEta.push_back(beta);
	jets_.jtbHadronPhi.push_back(bphi);
	jets_.jtbHadronPdg.push_back(bpdg);
	jets_.jtcHadronDR.push_back(cdr);
	jets_.jtcHadronPt.push_back(cpt);
	jets_.jtcHadronEta.push_back(ceta);
	jets_.jtcHadronPhi.push_back(cphi);
	jets_.jtcHadronPdg.push_back(cpdg);
	jets_.jtPartonDR.push_back(pdr);
	jets_.jtPartonPt.push_back(ppt);
	jets_.jtPartonEta.push_back(peta);
	jets_.jtPartonPhi.push_back(pphi);
	jets_.jtPartonPdg.push_back(ppdg);
	

      }

	//Matt's flavor production code
      if(isPythia6_){

	      if(jets_.jtHadronFlavor[jets_.nref]==4||jets_.jtHadronFlavor[jets_.nref]==5){

		      int partonMatchIndex = findMatchedParton(jet.eta(), jet.phi(), 0.3, genparts, jets_.jtHadronFlavor[jets_.nref]);
		      if(partonMatchIndex<0){
			//cout<< "jet " << jets_.nref << " declared flavor " << jets_.jtHadronFlavor[jets_.nref] << " couldn't find the parton "<<endl;
			      jets_.refparton_flavorProcess[jets_.nref] = 0;
		      }
		      else{
			      int flavorProcess =  getFlavorProcess(partonMatchIndex, genparts);
			      jets_.refparton_flavorProcess[jets_.nref] = flavorProcess;
		      }

	      } // end if abs(partonFlavor)==4||abs(partonFlavor)==5
	      else jets_.refparton_flavorProcess[jets_.nref] = 0;
      }


      // matched partons
      const reco::GenParticle & parton = *(*patjets)[j].genParton();

      if((*patjets)[j].genParton()){
	jets_.refparton_pt[jets_.nref] = parton.pt();
	jets_.refparton_flavor[jets_.nref] = parton.pdgId();

	if(saveBfragments_ && abs(jets_.jtPartonFlavor[jets_.nref])==5){

	  usedStringPts.clear();

	  // uncomment this if you want to know the ugly truth about parton matching -matt
	  //if(jet.pt() > 50 &&abs(parton.pdgId())!=5 && parton.pdgId()!=21)
	  // cout<<" Identified as a b, but doesn't match b or gluon, id = "<<parton.pdgId()<<endl;

	  jets_.bJetIndex[jets_.bMult] = jets_.nref;
	  jets_.bStatus[jets_.bMult] = parton.status();
	  jets_.bVx[jets_.bMult] = parton.vx();
	  jets_.bVy[jets_.bMult] = parton.vy();
	  jets_.bVz[jets_.bMult] = parton.vz();
	  jets_.bPt[jets_.bMult] = parton.pt();
	  jets_.bEta[jets_.bMult] = parton.eta();
	  jets_.bPhi[jets_.bMult] = parton.phi();
	  jets_.bPdg[jets_.bMult] = parton.pdgId();
	  jets_.bChg[jets_.bMult] = parton.charge();
	  jets_.bMult++;
	  saveDaughters(parton);
	}
      } else {
	jets_.refparton_pt[jets_.nref] = -999;
	jets_.refparton_flavor[jets_.nref] = -999;
      }
    }
    jets_.nref++;
  }

  if(isMC_){
  
    if(useHepMC_) {
      edm::Handle<HepMCProduct> hepMCProduct;
      iEvent.getByToken(eventInfoTag_,hepMCProduct);
      const HepMC::GenEvent* MCEvt = hepMCProduct->GetEvent();

      std::pair<HepMC::GenParticle*,HepMC::GenParticle*> beamParticles = MCEvt->beam_particles();
      if(beamParticles.first != 0)jets_.beamId1 = beamParticles.first->pdg_id();
      if(beamParticles.second != 0)jets_.beamId2 = beamParticles.second->pdg_id();
    }
    
    //edm::Handle<GenEventInfoProduct> hEventInfo;
    //iEvent.getByToken(eventGenInfoTag_,hEventInfo);
    //jets_.pthat = hEventInfo->binningValues()[0];

    // binning values and qscale appear to be equivalent, but binning values not always present
    //jets_.pthat = hEventInfo->qScale();

    edm::Handle<vector<reco::GenJet> >genjets;
    //edm::Handle<edm::View<reco::Jet>> genjets;
    iEvent.getByToken(genjetTag_, genjets);
    
    //get gen-level n-jettiness
    // edm::Handle<edm::ValueMap<float> > genTau1s;
    // edm::Handle<edm::ValueMap<float> > genTau2s;
    // edm::Handle<edm::ValueMap<float> > genTau3s;
    // if(useGenTaus) {
    //   Printf("get gen taus");
    //   iEvent.getByToken(tokenGenTau1_,genTau1s);
    //   iEvent.getByToken(tokenGenTau2_,genTau2s);
    //   iEvent.getByToken(tokenGenTau3_,genTau3s);
    // }
    
    jets_.ngen = 0;

    //int igen = 0;
    for(unsigned int igen = 0 ; igen < genjets->size(); ++igen){
      //for ( typename edm::View<reco::Jet>::const_iterator genjetIt = genjets->begin() ; genjetIt != genjets->end() ; ++genjetIt ) {
      const reco::GenJet & genjet = (*genjets)[igen];
      //edm::Ptr<reco::Jet> genjetPtr = genjets->ptrAt(genjetIt - genjets->begin());
      //const reco::GenJet genjet = (*dynamic_cast<const reco::GenJet*>(&(*genjetPtr)));
      float genjet_pt = genjet.pt();

      float tau1 =  -999.;
      float tau2 =  -999.;
      float tau3 =  -9999.;
      if(doGenTaus_) {
        tau1 =  getTau(1,genjet);
        tau2 =  getTau(2,genjet);
        tau3 =  getTau(3,genjet);
      }

      // find matching patJet if there is one
      jets_.gendrjt[jets_.ngen] = -1.0;
      jets_.genmatchindex[jets_.ngen] = -1;
      
      for(int ijet = 0 ; ijet < jets_.nref; ++ijet){
        // poor man's matching, someone fix please

	double deltaPt = fabs(genjet.pt()-jets_.refpt[ijet]); //Note: precision of this ~ .0001, so cut .01
	double deltaEta = fabs(genjet.eta()-jets_.refeta[ijet]); //Note: precision of this is  ~.0000001, but keep it low, .0001 is well below cone size and typical pointing resolution
	double deltaPhi = fabs(reco::deltaPhi(genjet.phi(), jets_.refphi[ijet])); //Note: precision of this is  ~.0000001, but keep it low, .0001 is well below cone size and typical pointing resolution

        if(deltaPt < 0.01 && deltaEta < .0001 && deltaPhi < .0001){
          if(genjet_pt>genPtMin_) {
            jets_.genmatchindex[jets_.ngen] = (int)ijet;
            jets_.gendphijt[jets_.ngen] = reco::deltaPhi(jets_.refphi[ijet],genjet.phi());
            jets_.gendrjt[jets_.ngen] = sqrt(pow(jets_.gendphijt[jets_.ngen],2)+pow(fabs(genjet.eta()-jets_.refeta[ijet]),2));
          }
          if(doGenTaus_) {
            jets_.reftau1[ijet] = tau1;
            jets_.reftau2[ijet] = tau2;
            jets_.reftau3[ijet] = tau3;
          }
          break;
        }
      }

      // threshold to reduce size of output in minbias PbPb
      if(genjet_pt>genPtMin_){
	jets_.genpt [jets_.ngen] = genjet_pt;
	jets_.geneta[jets_.ngen] = genjet.eta();
	jets_.genphi[jets_.ngen] = genjet.phi();
        jets_.genm  [jets_.ngen] = genjet.mass();
	jets_.geny  [jets_.ngen] = genjet.eta();

	if(doNewJetVars_)
	  fillNewJetVarsGenJet(genjet);      

        if(doGenTaus_) {
          jets_.gentau1[jets_.ngen] = tau1;
          jets_.gentau2[jets_.ngen] = tau2;
          jets_.gentau3[jets_.ngen] = tau3;
        }

        if(doGenSubJets_)
          analyzeGenSubjets(genjet);

	if(doSubEvent_){
	  const GenParticle* gencon = genjet.getGenConstituent(0);
	  jets_.gensubid[jets_.ngen] = gencon->collisionId();
	}
	jets_.ngen++;
      }
    }
  }


  t->Fill();

  memset(&jets_,0,sizeof jets_);
}


//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::fillL1Bits(const edm::Event &iEvent)
{
  edm::Handle< L1GlobalTriggerReadoutRecord >  L1GlobalTrigger;

  iEvent.getByToken(L1gtReadout_, L1GlobalTrigger);
  const TechnicalTriggerWord&  technicalTriggerWordBeforeMask = L1GlobalTrigger->technicalTriggerWord();

  for (int i=0; i<64;i++)
    {
      jets_.l1TBit[i] = technicalTriggerWordBeforeMask.at(i);
    }
  jets_.nL1TBit = 64;

  int ntrigs = L1GlobalTrigger->decisionWord().size();
  jets_.nL1ABit = ntrigs;

  for (int i=0; i != ntrigs; i++) {
    bool accept = L1GlobalTrigger->decisionWord()[i];
    //jets_.l1ABit[i] = (accept == true)? 1:0;
    if(accept== true){
      jets_.l1ABit[i] = 1;
    }
    else{
      jets_.l1ABit[i] = 0;
    }

  }
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::fillHLTBits(const edm::Event &iEvent)
{
  // Fill HLT trigger bits.
  Handle<TriggerResults> triggerResultsHLT;
  iEvent.getByToken(hltResName_, triggerResultsHLT);

  const TriggerResults *hltResults = triggerResultsHLT.product();
  const TriggerNames & triggerNames = iEvent.triggerNames(*hltResults);

  jets_.nHLTBit = hltTrgNames_.size();

  for(size_t i=0;i<hltTrgNames_.size();i++){

    for(size_t j=0;j<triggerNames.size();++j) {

      if(triggerNames.triggerName(j) == hltTrgNames_[i]){

	//cout <<"hltTrgNames_(i) "<<hltTrgNames_[i]<<endl;
	//cout <<"triggerName(j) "<<triggerNames.triggerName(j)<<endl;
	//cout<<" result "<<triggerResultsHLT->accept(j)<<endl;
	jets_.hltBit[i] = triggerResultsHLT->accept(j);
      }

    }
  }
}



// Recursive function, but this version gets called only the first time
void
HiInclusiveJetAnalyzer::saveDaughters(const reco::GenParticle &gen){

  for(unsigned i=0;i<gen.numberOfDaughters();i++){
    const reco::Candidate & daughter = *gen.daughter(i);
    double daughterPt = daughter.pt();
    if(daughterPt<1.) continue;
    double daughterEta = daughter.eta();
    if(fabs(daughterEta)>3.) continue;
    int daughterPdgId = daughter.pdgId();
    int daughterStatus = daughter.status();
    // Special case when b->b+string, both b and string contain all daughters, so only take the string
    if(gen.pdgId()==daughterPdgId && gen.status()==3 && daughterStatus==2) continue;

    // cheesy way of finding strings which were already used
    if(daughter.pdgId()==92){
      for(unsigned ist=0;ist<usedStringPts.size();ist++){
	if(fabs(daughter.pt() - usedStringPts[ist]) < 0.0001) return;
      }
      usedStringPts.push_back(daughter.pt());
    }
    jets_.bJetIndex[jets_.bMult] = jets_.nref;
    jets_.bStatus[jets_.bMult] = daughterStatus;
    jets_.bVx[jets_.bMult] = daughter.vx();
    jets_.bVy[jets_.bMult] = daughter.vy();
    jets_.bVz[jets_.bMult] = daughter.vz();
    jets_.bPt[jets_.bMult] = daughterPt;
    jets_.bEta[jets_.bMult] = daughterEta;
    jets_.bPhi[jets_.bMult] = daughter.phi();
    jets_.bPdg[jets_.bMult] = daughterPdgId;
    jets_.bChg[jets_.bMult] = daughter.charge();
    jets_.bMult++;
    saveDaughters(daughter);
  }
}

// This version called for all subsequent calls
void
HiInclusiveJetAnalyzer::saveDaughters(const reco::Candidate &gen){

  for(unsigned i=0;i<gen.numberOfDaughters();i++){
    const reco::Candidate & daughter = *gen.daughter(i);
    double daughterPt = daughter.pt();
    if(daughterPt<1.) continue;
    double daughterEta = daughter.eta();
    if(fabs(daughterEta)>3.) continue;
    int daughterPdgId = daughter.pdgId();
    int daughterStatus = daughter.status();
    // Special case when b->b+string, both b and string contain all daughters, so only take the string
    if(gen.pdgId()==daughterPdgId && gen.status()==3 && daughterStatus==2) continue;

    // cheesy way of finding strings which were already used
    if(daughter.pdgId()==92){
      for(unsigned ist=0;ist<usedStringPts.size();ist++){
	if(fabs(daughter.pt() - usedStringPts[ist]) < 0.0001) return;
      }
      usedStringPts.push_back(daughter.pt());
    }

    jets_.bJetIndex[jets_.bMult] = jets_.nref;
    jets_.bStatus[jets_.bMult] = daughterStatus;
    jets_.bVx[jets_.bMult] = daughter.vx();
    jets_.bVy[jets_.bMult] = daughter.vy();
    jets_.bVz[jets_.bMult] = daughter.vz();
    jets_.bPt[jets_.bMult] = daughterPt;
    jets_.bEta[jets_.bMult] = daughterEta;
    jets_.bPhi[jets_.bMult] = daughter.phi();
    jets_.bPdg[jets_.bMult] = daughterPdgId;
    jets_.bChg[jets_.bMult] = daughter.charge();
    jets_.bMult++;
    saveDaughters(daughter);
  }
}

double HiInclusiveJetAnalyzer::getEt(math::XYZPoint pos, double energy){
  double et = energy*sin(pos.theta());
  return et;
}

math::XYZPoint HiInclusiveJetAnalyzer::getPosition(const DetId &id, reco::Vertex::Point vtx){
  const GlobalPoint& pos=geo->getPosition(id);
  math::XYZPoint posV(pos.x() - vtx.x(),pos.y() - vtx.y(),pos.z() - vtx.z());
  return posV;
}

int HiInclusiveJetAnalyzer::TaggedJet(Jet calojet, Handle<JetTagCollection > jetTags ) {
  double small = 1.e-5;
  int result = -1;

  for (size_t t = 0; t < jetTags->size(); t++) {
    RefToBase<Jet> jet_p = (*jetTags)[t].first;
    if (jet_p.isNull()) {
      continue;
    }
    if (DeltaR<Candidate>()( calojet, *jet_p ) < small) {
      result = (int) t;
    }
  }
  return result;
}
// Matt Flavor Production Identification
// //-------------------------------------------------------------------------------------------------
int HiInclusiveJetAnalyzer::findMatchedParton(float eta, float phi, float maxDr, Handle<GenParticleCollection > genparts, int partonFlavor=0){

	float highestPartonPt =-1.;
	int matchIndex =-1;
	for( size_t i = 0; i < genparts->size(); ++ i ) {
		const GenParticle & genCand = (*genparts)[ i ];

		if(partonFlavor!=0){
//			cout << " candidate " << i << " pdg: "<< genCand.pdgId() << " status: "<< genCand.status() << " dr: "<< deltaR(eta,phi,genCand.eta(),genCand.phi()) << " flavorToMatch: "<< partonFlavor << endl;
			if(abs(genCand.pdgId())!=partonFlavor) continue;
			if(genCand.status()<20) continue;
		}
		double dr = deltaR(eta,phi,genCand.eta(),genCand.phi());
		if(dr>maxDr) continue;
		if(genCand.pt() > highestPartonPt){
			matchIndex = i;
			highestPartonPt = genCand.pt();
		}
	}
	return matchIndex;
}

int HiInclusiveJetAnalyzer::getFlavorProcess(int index, Handle<GenParticleCollection > genparts){

	const GenParticle & constParton = (*genparts)[ index ];
	GenParticle *matchedParton = const_cast<reco::GenParticle*>(&constParton);
	if(matchedParton->numberOfMothers()>1) cout<<" too many parents "<<endl;
	if(matchedParton->numberOfMothers()==0){ /*cout <<" found primary quark of pdg: " << matchedParton->pdgId() << endl;*/ return 1;}
	int momID = matchedParton->mother(0)->pdgId();
	int momIndex = findMatchedParton(matchedParton->mother(0)->eta(),matchedParton->mother(0)->phi(),0.001,genparts);
	if(momIndex==index){ cout << " WARNING! Particle is its own mother??" << endl; return -1; }
	int gmomIdx=-1;
	if(matchedParton->mother(0)->numberOfMothers()>0){
		gmomIdx = findMatchedParton(matchedParton->mother(0)->mother(0)->eta(),matchedParton->mother(0)->mother(0)->phi(),0.001,genparts);
		if(gmomIdx==index) { cout << "WARNING! Particle is its own grandmother" << endl; return -1; }
	}

	while(abs(matchedParton->mother(0)->pdgId())==5 && matchedParton->numberOfMothers()>0){
		if(matchedParton->mother(0)->status()==21) return 1;  // primary b-quark
		else {
			matchedParton = (reco::GenParticle*)(reco::LeafCandidate*)(matchedParton->mother(0));
			//return getFlavorProcess(momIndex,genparts);
		}
	}
	if(abs(momID)==5 && matchedParton->numberOfMothers()==0){ cout << "warning - found orphaned b-quark!" << endl; return -1; }

	if(!isPythia6_){
		if(matchedParton->status()>20 && matchedParton->status()<30) return 2;
		if(matchedParton->status()>30 && matchedParton->status()<40) return 3;
		if(matchedParton->status()>40 && matchedParton->status()<50) return 4;
		if(matchedParton->status()>50){
			if(abs(momID)==21) return 5;
			return 6;
		}
	}
	else{
		if(momIndex<2) return 4; // initial state GSP
		else if(momIndex<4) return 3; // sometimes GSP, sometimes associated to FEX
		else if(momIndex<6) return 2; // primaries
		else if(momIndex<8){
			if(momID==21) return 6;  // final state hard GSP
			else return 5; // final state soft GSP
		}
	}
	//else cout<<" should never get here "<<" momID "<<momID<<" momIndex "<<momIndex<<endl;


	return -1;

}

//--------------------------------------------------------------------------------------------------
float HiInclusiveJetAnalyzer::getTau(unsigned num, const reco::GenJet object) const
{
  std::vector<fastjet::PseudoJet> FJparticles;
  if(object.numberOfDaughters()>0) {
    for (unsigned k = 0; k < object.numberOfDaughters(); ++k)
      {
        const reco::Candidate & dp = *object.daughter(k);
        //const reco::CandidatePtr & dp = object.daughterPtr(k);
        if(dp.pt()>0.) FJparticles.push_back( fastjet::PseudoJet( dp.px(), dp.py(), dp.pz(), dp.energy() ) );
      }
  }
  return routine_->getTau(num, FJparticles);
}

// new style, everything from PAT
void HiInclusiveJetAnalyzer::analyzeSubjets( const pat::JetPtrCollection & subjets){

  std::vector<float> sjpt;
  std::vector<float> sjeta;
  std::vector<float> sjphi;
  std::vector<float> sjm;
  std::vector<int> sjhadronFlavor;
  std::vector<int> sjpartonFlavor;
  std::vector<float> sjcsvV2;
  std::vector<float> sjjptag;
  std::vector<int> sjVtxType;

  std::vector<float> sjrefpt;
  std::vector<float> sjrefeta;
  std::vector<float> sjrefphi;
  std::vector<float> sjrefm;

  
  for(size_t isub=0; isub<subjets.size(); ++isub){
    
    const pat::Jet & subjet = subjets[isub]; 
    
    //cout<<" subjet pt "<<subjet.pt()<<" eta "<<subjet.eta() <<" phi "<<subjet.phi()<<endl;
    sjpt.push_back(subjet.pt());
    sjeta.push_back(subjet.eta());
    sjphi.push_back(subjet.phi());
    sjm.push_back(subjet.mass());
    sjhadronFlavor.push_back(subjet.hadronFlavour());
    sjpartonFlavor.push_back(subjet.partonFlavour());

    // these are hard coded for now, till I figure out how to do it right
    sjcsvV2.push_back(subjet.bDiscriminator("akSoftDrop4PFCombinedSubjetSecondaryVertexV2BJetTags"));
    sjjptag.push_back(subjet.bDiscriminator("akSoftDrop4PFSubjetJetProbabilityBJetTags"));
    if(subjet.hasTagInfo("akSoftDrop4PFSubjetSecondaryVertex")){
      const reco::SecondaryVertexTagInfo * svTagInfo = subjet.tagInfoSecondaryVertex("akSoftDrop4PFSubjetSecondaryVertex");
      const reco::CandSecondaryVertexTagInfo *svCandTagInfo  = subjet.tagInfoCandSecondaryVertex("akSoftDrop4PFSubjetSecondaryVertex");
      //overloading vtxType with the (related) number of vertices.  Will come back and fill out more sv info later
      if(svTagInfo)sjVtxType.push_back(svTagInfo->nVertices());
      else if(svCandTagInfo)sjVtxType.push_back(svCandTagInfo->nVertices());

    }

    const reco::GenJet * gensubjet = subjet.genJet();
    
    if(gensubjet){
      sjrefpt.push_back(gensubjet->pt());
      sjrefeta.push_back(gensubjet->eta());
      sjrefphi.push_back(gensubjet->phi());	   
      sjrefm.push_back(gensubjet->mass());	   
    }
    else{
      sjrefpt.push_back(-999);
      sjrefeta.push_back(-999);
      sjrefphi.push_back(-999);	   
      sjrefm.push_back(-999);	   
    }
    
  }
  
  jets_.jtSubJetPt.push_back(sjpt);
  jets_.jtSubJetEta.push_back(sjeta);
  jets_.jtSubJetPhi.push_back(sjphi);
  jets_.jtSubJetM.push_back(sjm);
  jets_.jtSubJetVtxType.push_back(sjVtxType);
  jets_.jtSubJetcsvV2.push_back(sjcsvV2);
  jets_.jtSubJetJP.push_back(sjjptag);


  if(doExtendedFlavorTagging_){
    jets_.jtSubJetHadronFlavor.push_back(sjhadronFlavor);
    jets_.jtSubJetPartonFlavor.push_back(sjpartonFlavor); 
  }


  jets_.refSubJetPt.push_back(sjrefpt);
  jets_.refSubJetEta.push_back(sjrefeta);
  jets_.refSubJetPhi.push_back(sjrefphi);
  jets_.refSubJetM.push_back(sjrefm);
  
  
  cout<<" returning "<<endl;
}


//--------------------------------------------------------------------------------------------------
//old method from matching by hand
void HiInclusiveJetAnalyzer::analyzeSubjets(const reco::Jet jet, int idx, edm::Handle<reco::JetFlavourInfoMatchingCollection> theSubjetFlavourInfos, edm::Handle<edm::View<reco::Jet> > groomedJets, Handle<JetTagCollection> jetTags_CombinedSvtx,  Handle<JetTagCollection> jetTags_JP, Handle<vector<TrackIPTagInfo> > subjetIP, Handle<vector<SecondaryVertexTagInfo> > subjetSV) {

  std::vector<float> sjpt;
  std::vector<float> sjeta;
  std::vector<float> sjphi;
  std::vector<float> sjm;
  std::vector<int> hadronFlavor;
  std::vector<int> partonFlavor;
  std::vector<float> csvV2;
  std::vector<float> jptag;
  std::vector<int> sjVtxType;
  std::vector<std::vector<float>> subjetVtxMass;
  std::vector<std::vector<float>> subjetVtxPt;
  std::vector<std::vector<float>> subjetVtxEta;
  std::vector<std::vector<float>> subjetVtxPhi;
  std::vector<std::vector<float>> subjetVtxNtrk;
  std::vector<std::vector<float>> subjetVtxdl;
  std::vector<std::vector<float>> subjetVtxdls;
  std::vector<std::vector<float>> hadronDR;
  std::vector<std::vector<float>> hadronPt;
  std::vector<std::vector<float>> hadronEta;
  std::vector<std::vector<float>> hadronPhi;
  std::vector<std::vector<float>> hadronPdg;
  std::vector<std::vector<float>> partonDR;
  std::vector<std::vector<float>> partonPt;
  std::vector<std::vector<float>> partonEta;
  std::vector<std::vector<float>> partonPhi;
  std::vector<std::vector<float>> partonPdg;

  
  if(jet.numberOfDaughters()>0) {
    for (unsigned k = 0; k < jet.numberOfDaughters(); ++k) {
      const reco::Candidate & dp = *jet.daughter(k);
      sjpt.push_back(dp.pt());
      sjeta.push_back(dp.eta());
      sjphi.push_back(dp.phi());
      sjm.push_back(dp.mass());
      
      vector<float> svm, svpt, sveta, svphi, svntrk, svdl, svdls; 
      const reco::Jet *subjd = dynamic_cast<const reco::Jet*>(jet.daughter(k));
      int ith_tagged = TaggedJet(*subjd,jetTags_CombinedSvtx);
      if(ith_tagged >= 0) csvV2.push_back((*jetTags_CombinedSvtx)[ith_tagged].second);
      else csvV2.push_back(-1.);
      ith_tagged = TaggedJet(*subjd,jetTags_JP);
      if(ith_tagged >= 0) jptag.push_back((*jetTags_JP)[ith_tagged].second);
      else jptag.push_back(-1.);

      ith_tagged = TaggedJet(*subjd,jetTags_CombinedSvtx);
      if(ith_tagged>=0){
	const SecondaryVertexTagInfo &subjetInfo = (*subjetSV)[ith_tagged];
	for(unsigned isv=0; isv<subjetInfo.nVertices(); isv++){
	  //cout << "subjet " << ith_tagged << " x,y,z: " << subjetInfo.secondaryVertex(isv).x() << " " << subjetInfo.secondaryVertex(isv).y() << " " << subjetInfo.secondaryVertex(isv).z() << endl;
	  svm.push_back(subjetInfo.secondaryVertex(isv).p4().mass());
	  svpt.push_back(subjetInfo.secondaryVertex(isv).p4().pt());
	  sveta.push_back(subjetInfo.secondaryVertex(isv).p4().eta());
	  svphi.push_back(subjetInfo.secondaryVertex(isv).p4().phi());
	  svntrk.push_back(subjetInfo.nVertexTracks(isv));
	  svdl.push_back(subjetInfo.flightDistance(isv).value());
	  svdls.push_back(subjetInfo.flightDistance(isv).significance());
	}
	if((*jetTags_CombinedSvtx)[ith_tagged].second > 0.9 && subjetInfo.nVertices()>0){
	  //cout << "full vtx criteria met for subjet " << k << " (pt=" << subjd->pt() << ", csv="<< (*jetTags_CombinedSvtx)[ith_tagged].second << ") " << endl;
	  //cout << "full vtx reco tracks: "<< endl;
	  int itrack=0;
	  for(reco::Vertex::trackRef_iterator it = subjetInfo.secondaryVertex(0).tracks_begin(); it != subjetInfo.secondaryVertex(0).tracks_end(); it++, itrack++){
	    //cout << "sv track " << itrack << " pt: "<< (*it)->pt() << " eta: " << (*it)->eta() << " phi: "<< (*it)->phi() << endl; 
	  }	  
	  
	}
	//if((*jetTags_CombinedSvtx)[ith_tagged].second > 0.9 && subjetInfo.nVertices()==0) cout << "WARNING! No vertex found in a large CSV subjet!!" << endl;
	if(subjetInfo.nVertices()>0) sjVtxType.push_back(0);
	else{
	  const TrackIPTagInfo &ipInfo = (*subjetIP)[ith_tagged];		    	
	  GlobalPoint pv(ipInfo.primaryVertex()->position().x(),ipInfo.primaryVertex()->position().y(),ipInfo.primaryVertex()->position().z());
	  std::vector<std::size_t> indices = ipInfo.sortedIndexes(reco::btag::IP3DSig);
	  const std::vector<reco::btag::TrackIPData> &ipData = ipInfo.impactParameterData();
	  
	  const TrackIPTagInfo::input_container &tracks = ipInfo.selectedTracks();
	  std::vector<const Track *> pseudoVertexTracks;	
	  int nVtxTrks=0;		
	  
	  const Track * trackPairV0Test[2];
	  //	  cout << "pseudoVtx criteria met for subjet " << k << " (pt=" << subjd->pt() << ", csv=" << (*jetTags_CombinedSvtx)[ith_tagged].second <<") " << endl;
	  //	  if((*jetTags_CombinedSvtx)[ith_tagged].second > 0.9) cout << " track debug info for PSVtx: " << endl;
	  for(std::size_t i=0; i<indices.size(); i++) {
	    std::size_t idx = indices[i];
	    const reco::btag::TrackIPData &data = ipData[idx];
	    const Track * trackPtr = reco::btag::toTrack(tracks[idx]);
	    const Track &track = *trackPtr;		
	    
	    if (!trackSelector(track, data, *subjd, pv)) continue;
	    if( trackPseudoSelector(track, data, *subjd, pv)) {
	      pseudoVertexTracks.push_back(trackPtr);
	      //			  if((*jetTags_CombinedSvtx)[ith_tagged].second > 0.9) cout << " track " << i << " pt: "<< track.pt() << " eta: "<< track.eta() << " phi: " << track.phi() << " displacement: "<< data.ip3d.value() << " dist to jet axis: "<< data.distanceToJetAxis.value() << endl;
	      nVtxTrks++;
	    }
	    // check against all other tracks for V0 track pairs
	    trackPairV0Test[0] = reco::btag::toTrack(tracks[idx]);
	    bool ok = true;
	    for(std::size_t j=0; j<indices.size(); j++) {
	      if (i == j) continue;
	      
	      std::size_t pairIdx = indices[j];
	      const reco::btag::TrackIPData &pairTrackData = ipData[pairIdx];
	      const Track * pairTrackPtr = reco::btag::toTrack(tracks[pairIdx]);
	      const Track &pairTrack = *pairTrackPtr;
	      
	      if (!trackSelector(pairTrack, pairTrackData, *subjd, pv))
		continue;
	      
	      trackPairV0Test[1] = pairTrackPtr;
	      if (!trackPairV0Filter(trackPairV0Test, 2)) {
		ok = false;
		break;
	      }
	    }
	    if (!ok) continue;
	  }
	  if(nVtxTrks >= 2 && pseudoVertexV0Filter(pseudoVertexTracks)) sjVtxType.push_back(1);
	  else sjVtxType.push_back(2); 
	}
	subjetVtxMass.push_back(svm);
	subjetVtxPt.push_back(svpt);
	subjetVtxEta.push_back(sveta);
	subjetVtxPhi.push_back(svphi);
	subjetVtxNtrk.push_back(svntrk);
	subjetVtxdl.push_back(svdl);
	subjetVtxdls.push_back(svdls);
	
	if(doExtendedFlavorTagging_){	       
	  vector<float> hdr, hpt, heta, hphi, hpdg, pdr, ppt, peta, pphi, ppdg;
	  for ( reco::JetFlavourInfoMatchingCollection::const_iterator sj  = theSubjetFlavourInfos->begin(); sj != theSubjetFlavourInfos->end(); sj++ ) { 
	    if( sqrt(reco::deltaR2(subjd->eta(), subjd->phi(), (*sj).first.get()->eta(), (*sj).first.get()->phi() )) <0.01  ){
	      
	      reco::Jet *aSubjet = const_cast<reco::Jet*>((*sj).first.get());
	      
	      reco::JetFlavourInfo aInfo = (*sj).second;	
	      hadronFlavor.push_back(aInfo.getHadronFlavour());
	      partonFlavor.push_back(aInfo.getPartonFlavour());
	      const reco::GenParticleRefVector &bHadrons = aInfo.getbHadrons();
	      for(reco::GenParticleRefVector::const_iterator it = bHadrons.begin(); it != bHadrons.end(); ++it){
		hdr.push_back(reco::deltaR(aSubjet->eta(), aSubjet->phi(), (*it)->eta(), (*it)->phi()));
		hpt.push_back((*it)->pt());
		heta.push_back((*it)->eta());
		hphi.push_back((*it)->phi());
		hpdg.push_back((*it)->pdgId());
	      }
	      const reco::GenParticleRefVector &cHadrons = aInfo.getcHadrons();
	      for(reco::GenParticleRefVector::const_iterator it = cHadrons.begin(); it != cHadrons.end(); ++it){
		hdr.push_back(reco::deltaR(aSubjet->eta(), aSubjet->phi(), (*it)->eta(), (*it)->phi()));
		hpt.push_back((*it)->pt());
		heta.push_back((*it)->eta());
		hphi.push_back((*it)->phi());
		hpdg.push_back((*it)->pdgId());
	      }
	      const reco::GenParticleRefVector & partons = aInfo.getPartons();
	      for(reco::GenParticleRefVector::const_iterator it = partons.begin(); it != partons.end(); ++it){
		pdr.push_back(reco::deltaR( aSubjet->eta(), aSubjet->phi(), (*it)->eta(), (*it)->phi() ));
		ppt.push_back((*it)->pt());
		peta.push_back((*it)->eta());
		pphi.push_back((*it)->phi());
		ppdg.push_back((*it)->pdgId());
	      }
	      break;	
	    } // end if reco::deltaR2 subjd (*sj).first.get()
	  } // end for const_iterator sj  = theSubjetFlavourInfos->begin()
	  hadronDR.push_back(hdr);
	  hadronPt.push_back(hpt);
	  hadronEta.push_back(heta);
	  hadronPhi.push_back(hphi);
	  hadronPdg.push_back(hpdg);
	  partonDR.push_back(pdr);
	  partonPt.push_back(ppt);
	  partonEta.push_back(peta);
	  partonPhi.push_back(pphi);
	  partonPdg.push_back(ppdg);
	}	
	
      }
    }
  } else {
    sjpt.push_back(-999.);
    sjeta.push_back(-999.);
    sjphi.push_back(-999.);
    sjm.push_back(-999.);
    sjVtxType.push_back(-999.);
    csvV2.push_back(-999.);
    jptag.push_back(-999.);
    if(doExtendedFlavorTagging_){
      hadronFlavor.push_back(-999);
      partonFlavor.push_back(-999);
    }
  }

  jets_.jtSubJetPt.push_back(sjpt);
  jets_.jtSubJetEta.push_back(sjeta);
  jets_.jtSubJetPhi.push_back(sjphi);
  jets_.jtSubJetM.push_back(sjm);
  jets_.jtSubJetVtxType.push_back(sjVtxType);
  jets_.jtSubJetcsvV2.push_back(csvV2);
  jets_.jtSubJetJP.push_back(jptag);
  /*
  jets_.jtSubJetSvtxm.push_back(subjetVtxMass);
  jets_.jtSubJetSvtxpt.push_back(subjetVtxPt);
  jets_.jtSubJetSvtxeta.push_back(subjetVtxEta);
  jets_.jtSubJetSvtxphi.push_back(subjetVtxPhi);
  jets_.jtSubJetSvtxNtrk.push_back(subjetVtxNtrk);
  jets_.jtSubJetSvtxdl.push_back(subjetVtxdl);
  jets_.jtSubJetSvtxdls.push_back(subjetVtxdls);
  */
  if(doExtendedFlavorTagging_){
	  jets_.jtSubJetHadronFlavor.push_back(hadronFlavor);
	  jets_.jtSubJetPartonFlavor.push_back(partonFlavor); 
	  /*
	  jets_.jtSubJetHadronDR.push_back(hadronDR);
	  jets_.jtSubJetHadronPt.push_back(hadronPt);
	  jets_.jtSubJetHadronEta.push_back(hadronEta);
	  jets_.jtSubJetHadronPhi.push_back(hadronPhi);
	  jets_.jtSubJetHadronPdg.push_back(hadronPdg);
	  jets_.jtSubJetPartonDR.push_back(partonDR);
	  jets_.jtSubJetPartonPt.push_back(partonPt);
	  jets_.jtSubJetPartonEta.push_back(partonEta);
	  jets_.jtSubJetPartonPhi.push_back(partonPhi);
	  jets_.jtSubJetPartonPdg.push_back(partonPdg);
  */
  }	
}


//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::fillNewJetVarsRecoJet(const reco::Jet jet){

  int nCands = 0;
  int nChCands = 0;
  int nNeCands = 0;

  float jetMByPt = (jet.mass()>0 && jet.pt()!=0) ? jet.mass()/jet.pt() : -999;

  float tot_wt=0;
      
  float RMSCand = 0.0;
  float rmscand_n=0;
  float rmscand_d=0;
      
  float M11=0,M22=0,M12=0,M21=0;
  float Axis1 = 0.0;
  float Axis2 = 0.0;
  float Sigma = 0.0;
      
  float maxCandPt=-999;
  float sumCandPt=0;
  float sumCandPtSq=0;
  float R = 0.0;
  float pTD = 0.0;

  float rm0p5 = 0.0;
  float rm1 = 0.0;
  float rm2 = 0.0;
  float rm3 = 0.0;
      
  TVector2 t_Vect(0,0);
  TVector2 r(0,0);
  float pull = 0.0;

  float sd_m = 0.0;
  float sd_pt = 0.0;
  float sd_eta = 0.0;
  float sd_phi = 0.0;
  float sd_ptfrac = 0.0;
  float sd_rm0p5 = 0.0;
  float sd_rm1 = 0.0;
  float sd_rm2 = 0.0;
  float sd_rm3 = 0.0;

  float Tbeta20p2 = 0.0;
  float Tbeta20p3 = 0.0;
  float Tbeta20p4 = 0.0;
  float Tbeta20p5 = 0.0;

  float Tbeta30p2 = 0.0;
  float Tbeta30p3 = 0.0;
  float Tbeta30p4 = 0.0;
  float Tbeta30p5 = 0.0;

  //! Double ratio of the moments
  //!  Cbeta2 = (Tbeta3 * Tbeta0)/pow(Tbeta2,2);
  //! Tbeta0 = sumpT_{i};
  float Cbeta20p2 = 0.0;
  float Cbeta20p3 = 0.0;
  float Cbeta20p4 = 0.0;
  float Cbeta20p5 = 0.0;

  // //! Moments shifted to centriod
  float Mu00=0.0, Mu01=0.0, Mu10=0.0, Mu11=0.0;
  float Mu20=0.0, Mu02=0.0, Mu12=0.0, Mu21=0.0;
  float Mu30=0.0, Mu03=0.0;

  float Phi1 = 0.0;
  float Phi2 = 0.0;
  float Phi3 = 0.0;
  float Phi4 = 0.0;
  float Phi5 = 0.0;
  float Phi6 = 0.0;
  float Phi7 = 0.0;

  float Skx=0.0;
  float Sky=0.0;

  float Z11=0.0;
  float Z20=0.0;
  float Z22=0.0;
  float Z31=0.0;
  float Z33=0.0;
  float Z40=0.0;
  float Z42=0.0;
  float Z44=0.0;
  float Z51=0.0;
  float Z53=0.0;
  float Z55=0.0;

  TVector2 tmp_Z11(0,0);
  TVector2 tmp_Z20(0,0);
  TVector2 tmp_Z22(0,0);
  TVector2 tmp_Z31(0,0);
  TVector2 tmp_Z33(0,0);
  TVector2 tmp_Z40(0,0);
  TVector2 tmp_Z42(0,0);
  TVector2 tmp_Z44(0,0);
  TVector2 tmp_Z51(0,0);
  TVector2 tmp_Z53(0,0);
  TVector2 tmp_Z55(0,0);

  
  vector<fastjet::PseudoJet> jetconsts;
  std::vector<int>   jcid;
  std::vector<float> jcE;
  std::vector<float> jcpt;
  std::vector<float> jceta;
  std::vector<float> jcphi;
  std::vector<float> jcm;
  std::vector<int>   jcSDid;
  std::vector<float> jcSDE;
  std::vector<float> jcSDpt;
  std::vector<float> jcSDeta;
  std::vector<float> jcSDphi;
  std::vector<float> jcSDm;

  float chargedSumConst = 0;
  int   chargedNConst = 0;
  float eSumConst = 0;
  int   eNConst = 0;
  float muSumConst = 0;
  int   muNConst = 0;
  float photonSumConst = 0;
  int   photonNConst = 0;
  float neutralSumConst = 0;
  int   neutralNConst = 0;
  float hfhadSumConst = 0;
  int   hfhadNConst = 0;
  float hfemSumConst = 0;
  int   hfemNConst = 0;
  if(jet.numberOfDaughters()>0) {
    nCands = jet.numberOfDaughters();
    for (unsigned k = 0; k < jet.numberOfDaughters(); ++k) {
      const reco::Candidate & dp = *jet.daughter(k);
      if(dp.charge() == 0){
	nNeCands++;
      }else{
	nChCands++;
      }

      float ptcand = dp.pt();
      int pfid     = dp.pdgId();

      switch(std::abs(pfid)){
      case 211:  //PFCandidate::h charged hadron
      case 321:
      case 2212:
	chargedSumConst += ptcand;
	chargedNConst++;
	break;
      case 11:  //PFCandidate::e // electron
	eSumConst += ptcand;
	eNConst++;
	break;
      case 13: //PFCandidate::mu // muon
	muSumConst += ptcand;
	muNConst++;
	break;
      case 22: //PFCandidate:gamma // gamma
	photonSumConst += ptcand;
	photonNConst++;
	break;
      case 130:  //PFCandidate::h0 //Neutral hadron
      case 2112:
	neutralSumConst += ptcand;
	neutralNConst++;
	break;
      case 1:  //PFCandidate::h_HF //hadron in HF
	hfhadSumConst += ptcand; 
	hfhadNConst++;
      case 2:  //PFCandidate::egamma_HF //electromagnetic in HF
	hfemSumConst += ptcand; 
	hfemNConst++;
      default:
	break;
      }

      if( dp.pt() > maxCandPt )	maxCandPt=dp.pt();
      sumCandPt   += dp.pt();
      sumCandPtSq += pow(dp.pt(),2);
	
      float wt = pow(dp.pt(),2);
      tot_wt += wt;
	
      //! RMSCand
      float deta = dp.eta()- jet.eta();
      float dphi = deltaPhi(dp.phi(), jet.phi());
      float dr = deltaR(dp.eta(), dp.phi(), jet.eta(), jet.phi());
      rmscand_n += wt*dr*dr;
      rmscand_d += wt; 
	
      M11 += wt*deta*deta;
      M22 += wt*dphi*dphi;
      M12 += wt*deta*dphi;
      M21 += wt*deta*dphi;
	
      //! Pull
      TLorentzVector pfvec;
      pfvec.SetPtEtaPhiE(dp.pt(), dp.eta(), dp.phi(), dp.energy());
      fastjet::PseudoJet jc = fastjet::PseudoJet(dp.px(), dp.py(), dp.pz(), dp.energy());
      jc.set_user_info(new ExtraInfo(dp.pdgId()));
      jetconsts.push_back(jc);

      jcid.push_back(dp.pdgId());
      jcE.push_back(jc.e());
      jcpt.push_back(jc.pt());
      jcphi.push_back(jc.phi());
      jceta.push_back(jc.eta());
      jcm.push_back(jc.m());
      
      float pfy = pfvec.Rapidity();
      float dy = pfy - jet.y();
      r.Set( dy, dphi );
      float r_mag = r.Mod();
      t_Vect += ( dp.pt() /  jet.pt() ) * r_mag *r;

      rm0p5+=(dp.pt()*pow(dr,0.5))/jet.pt();
      rm1+=(dp.pt()*pow(dr,1))/jet.pt();
      rm2+=(dp.pt()*pow(dr,2))/jet.pt();
      rm3+=(dp.pt()*pow(dr,3))/jet.pt();      


      //! Added by Pawan ZM 
      tmp_Z11 += ( ( dp.pt() /  jet.pt() ) * r_mag * r );

      tmp_Z20 += ( ( dp.pt() /  jet.pt() ) * (2.*r_mag*r_mag - 1) * r );
      tmp_Z22 += ( ( dp.pt() /  jet.pt() ) * r_mag * r_mag * r);

      tmp_Z31 += ( ( dp.pt() /  jet.pt() ) * ( (3. * r_mag * r_mag * r_mag) - 2.*r_mag ) * r );
      tmp_Z33 += ( ( dp.pt() /  jet.pt() ) * r_mag * r_mag * r_mag * r );

      tmp_Z40 += ( ( dp.pt() /  jet.pt() ) * ( 6.*pow(r_mag,4) - 6.*pow(r_mag,2) + 1 ) * r );
      tmp_Z42 += ( ( dp.pt() /  jet.pt() ) * ( 4.*pow(r_mag,4) - 3.*pow(r_mag,2)) * r );
      tmp_Z44 += ( ( dp.pt() /  jet.pt() ) * pow(r_mag,4) * r);

      tmp_Z51 += ( ( dp.pt() /  jet.pt() ) * ( 10.*pow(r_mag,5) - 12.*pow(r_mag,3) + 3.*r_mag ) * r );
      tmp_Z53 += ( ( dp.pt() /  jet.pt() ) * ( 5.*pow(r_mag,5) - 4.*pow(r_mag,3)) * r );
      tmp_Z55 += ( ( dp.pt() /  jet.pt() ) * pow(r_mag,5) * r );


      //! For two-point moment
      for (unsigned l = k+1; l < jet.numberOfDaughters(); ++l) {
        const reco::Candidate & dl = *jet.daughter(l);
        float dr_kl = deltaR(dl.eta(), dl.phi(), dp.eta(), dp.phi());
	
        Tbeta20p2 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.2) );
        Tbeta20p3 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.3) );
        Tbeta20p4 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.4) );
        Tbeta20p5 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.5) );

	//! For three-point moment
	for (unsigned m = l+1; m < jet.numberOfDaughters(); ++m) {
	  const reco::Candidate & dm = *jet.daughter(m);
	  float dr_km = deltaR(dm.eta(), dm.phi(), dp.eta(), dp.phi());
	  float dr_lm = deltaR(dm.eta(), dm.phi(), dl.eta(), dl.phi());
	  
	  Tbeta30p2 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.2) );
	  Tbeta30p3 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.3) );
	  Tbeta30p4 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.4) );
	  Tbeta30p5 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.5) );
	}
      }

      //! Hu Moment Calculation 
      Mu00 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu01 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));
      Mu10 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu11 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));

      Mu20 += (pow(dp.eta()-jet.eta(),2)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu02 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),2)*(dp.pt()/jet.pt()));
      Mu12 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),2)*(dp.pt()/jet.pt()));
      Mu21 += (pow(dp.eta()-jet.eta(),2)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));

      Mu30 += (pow(dp.eta()-jet.eta(),3)*pow(dp.eta()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu03 += (pow(dp.eta()-jet.eta(),0)*pow(dp.eta()-jet.phi(),3)*(dp.pt()/jet.pt()));
    }
  }
  fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, 999);
  fastjet::ClusterSequence thisClustering_basic(jetconsts, jetDef);
  std::vector<fastjet::PseudoJet> out_jets_basic = thisClustering_basic.inclusive_jets(0);
  if(out_jets_basic.size() == 1){ 
    fastjet::contrib::RecursiveSymmetryCutBase::SymmetryMeasure  symmetry_measure = fastjet::contrib::RecursiveSymmetryCutBase::scalar_z;
    fastjet::contrib::SoftDrop sd(0, 0.1, symmetry_measure, rParam);
    fastjet::PseudoJet sd_jet = sd(out_jets_basic[0]);
    if(sd_jet != 0){
      sd_m = sd_jet.m();
      sd_pt = sd_jet.pt();
      sd_eta = sd_jet.eta();
      sd_phi = sd_jet.phi();
      sd_ptfrac = sd_jet.pt()/jet.pt();    
      std::vector<fastjet::PseudoJet> sd_jetconsts = sd_jet.constituents();
      if(sd_jetconsts.size()!=0){
	for (unsigned k = 0; k < sd_jetconsts.size(); ++k) {
	  const fastjet::PseudoJet dp = sd_jetconsts.at(k);
	  jcSDid.push_back(dp.user_info<ExtraInfo>().part_id());
	  jcSDE.push_back(dp.e());
	  jcSDpt.push_back(dp.pt());
	  jcSDphi.push_back(dp.phi());
	  jcSDeta.push_back(dp.eta());
	  jcSDm.push_back(dp.m());
	  float dr = deltaR(dp.eta(), dp.phi(), jet.eta(), jet.phi());
	  sd_rm0p5+=(dp.pt()*pow(dr,0.5))/sd_jet.pt();
	  sd_rm1+=(dp.pt()*pow(dr,1))/sd_jet.pt();
	  sd_rm2+=(dp.pt()*pow(dr,2))/sd_jet.pt();
	  sd_rm3+=(dp.pt()*pow(dr,3))/sd_jet.pt();
	}
      }
    }
  }

  RMSCand = (rmscand_d > 0 ) ? sqrt ( rmscand_n / rmscand_d ) : -999;
      
  M12 = -1.*M12;
  M21 = -1.*M21;
      
  //! eign values
  float trace = M11 + M22;
  float detrm = (M11*M22) - (M12*M21);
      
  float lam1 = trace/2. + sqrt( pow(trace,2)/4. - detrm );
  float lam2 = trace/2. - sqrt( pow(trace,2)/4. - detrm );
      
  Axis1 = (tot_wt > 0 && lam1 >= 0) ? sqrt( lam1 / tot_wt ) : -999;
  Axis2 = (tot_wt > 0 && lam2 >=0 ) ? sqrt( lam2 / tot_wt ) : -999;
  
  Sigma = (tot_wt > 0 ) ? sqrt( pow(Axis1,2) + pow(Axis2,2) ) : -999;
      
  R = (sumCandPt > 0 ) ? maxCandPt / sumCandPt : -999;
      
  pTD = (sumCandPt > 0 ) ? sqrt( sumCandPtSq ) / sumCandPt : -999;
  
  pull = t_Vect.Mod();



  Tbeta20p2 /= pow(jet.pt(),2); 
  Tbeta20p3 /= pow(jet.pt(),2); 
  Tbeta20p4 /= pow(jet.pt(),2); 
  Tbeta20p5 /= pow(jet.pt(),2); 

  Tbeta30p2 /= pow(jet.pt(),3); 
  Tbeta30p3 /= pow(jet.pt(),3); 
  Tbeta30p4 /= pow(jet.pt(),3); 
  Tbeta30p5 /= pow(jet.pt(),3); 

  Cbeta20p2 = ( Tbeta20p2 > 0 ) ? Tbeta30p2 / Tbeta20p2 : -999;
  Cbeta20p3 = ( Tbeta20p3 > 0 ) ? Tbeta30p3 / Tbeta20p3 : -999;
  Cbeta20p4 = ( Tbeta20p4 > 0 ) ? Tbeta30p4 / Tbeta20p4 : -999;
  Cbeta20p5 = ( Tbeta20p5 > 0 ) ? Tbeta30p5 / Tbeta20p5 : -999;

  //! Hu Moments
  float Eta02 = Mu20/pow(Mu00,(0+2+1));
  float Eta20 = Mu20/pow(Mu00,(2+0+1));
  float Eta11 = Mu11/pow(Mu00,(1+1+1));
  float Eta12 = Mu12/pow(Mu00,(1+2+1));
  float Eta21 = Mu21/pow(Mu00,(2+1+1));
  float Eta03 = Mu03/pow(Mu00,(0+3+1));
  float Eta30 = Mu30/pow(Mu00,(3+0+1));
  
  if( Mu00 > 0 ){
    Phi1 = (Eta20 + Eta02);
    Phi2 = pow((Eta20 - Eta02),2) + 4.*pow(Eta11,2);
    Phi3 = pow((Eta30 - 3.*Eta12),2) + pow((3.*Eta21 - Eta03),2);
    Phi4 = pow((Eta30 + Eta12),2) + pow((Eta21 + Eta03),2);
    
    Phi5 = (Eta30 - 3.*Eta12)*(Eta30 + Eta12)*(pow((Eta30 + Eta12),2) - 3.*pow((Eta21 + Eta03),2)) + (3.*Eta21 - Eta03)*(Eta21 + Eta03)*(3.*pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2));
    
    Phi6 = ((Eta20 - Eta02 )*(pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2))) + 4.*Eta11*(Eta30 + Eta12)*(Eta21 + Eta03);
    
    Phi7 = (3.*Eta21 - Eta03)*(Eta30 + Eta12)*(pow((Eta30 + Eta12),2) - 3.*pow((Eta21 + Eta03),2)) - (Eta30 - 3.*Eta12)*(Eta21 + Eta03)*(3.*pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2));
  }else{
    Phi1 = Phi2 = Phi3 = Phi4 = Phi5 = Phi6 = Phi7 = -999;
  }
  
  Skx = ( Mu20 > 0 ) ? Mu30/pow(Mu20,1.5) : -999;
  Sky = ( Mu02 > 0 ) ? Mu03/pow(Mu02,1.5) : -999;
  
  Z11 += tmp_Z11.Mod();
  Z20 += tmp_Z20.Mod();
  Z22 += tmp_Z22.Mod();
  Z31 += tmp_Z31.Mod();
  Z33 += tmp_Z33.Mod();
  Z40 += tmp_Z40.Mod();
  Z42 += tmp_Z42.Mod();
  Z44 += tmp_Z44.Mod();
  Z51 += tmp_Z51.Mod();
  Z53 += tmp_Z53.Mod();
  Z55 += tmp_Z55.Mod();


  
  jets_.jtnCands[jets_.nref] = nCands;
  jets_.jtnChCands[jets_.nref] = nChCands;
  jets_.jtnNeCands[jets_.nref] = nNeCands;
  
  jets_.jtchargedSumConst[jets_.nref] = chargedSumConst;
  jets_.jtchargedNConst  [jets_.nref] = chargedNConst;  
  jets_.jteSumConst      [jets_.nref] = eSumConst;      
  jets_.jteNConst        [jets_.nref] = eNConst;        
  jets_.jtmuSumConst     [jets_.nref] = muSumConst;     
  jets_.jtmuNConst       [jets_.nref] = muNConst;       
  jets_.jtphotonSumConst [jets_.nref] = photonSumConst; 
  jets_.jtphotonNConst   [jets_.nref] = photonNConst;   
  jets_.jtneutralSumConst[jets_.nref] = neutralSumConst;
  jets_.jtneutralNConst  [jets_.nref] = neutralNConst;  
  jets_.jthfhadSumConst  [jets_.nref] = hfhadSumConst;
  jets_.jthfhadNConst    [jets_.nref] = hfhadNConst;
  jets_.jthfemSumConst   [jets_.nref] = hfemSumConst;
  jets_.jthfemNConst     [jets_.nref] = hfemNConst;

  jets_.jtMByPt[jets_.nref] = jetMByPt;
  jets_.jtRMSCand[jets_.nref] = RMSCand;
  jets_.jtAxis1[jets_.nref] = Axis1;
  jets_.jtAxis2[jets_.nref] = Axis2;
  jets_.jtSigma[jets_.nref] = Sigma;
  jets_.jtrm3[jets_.nref] = rm3;
  jets_.jtrm2[jets_.nref] = rm2;
  jets_.jtrm1[jets_.nref] = rm1;
  jets_.jtrm0p5[jets_.nref] = rm0p5;
  jets_.jtR[jets_.nref] = R;
  jets_.jtpull[jets_.nref] = pull;
  jets_.jtpTD[jets_.nref] = pTD;
  jets_.jtSDm[jets_.nref] = sd_m;
  jets_.jtSDpt[jets_.nref] = sd_pt;
  jets_.jtSDeta[jets_.nref] = sd_eta;
  jets_.jtSDphi[jets_.nref] = sd_phi;
  jets_.jtSDptFrac[jets_.nref] = sd_ptfrac;
  jets_.jtSDrm0p5[jets_.nref] = sd_rm0p5;
  jets_.jtSDrm1[jets_.nref] = sd_rm1;
  jets_.jtSDrm2[jets_.nref] = sd_rm2;
  jets_.jtSDrm3[jets_.nref] = sd_rm3;

  jets_.jtConstituentsId.push_back(jcid);
  jets_.jtConstituentsE.push_back(jcE);
  jets_.jtConstituentsPt.push_back(jcpt);
  jets_.jtConstituentsEta.push_back(jceta);
  jets_.jtConstituentsPhi.push_back(jcphi);
  jets_.jtConstituentsM.push_back(jcm);
  jets_.jtSDConstituentsId.push_back(jcSDid);
  jets_.jtSDConstituentsE.push_back(jcSDE);
  jets_.jtSDConstituentsPt.push_back(jcSDpt);
  jets_.jtSDConstituentsEta.push_back(jcSDeta);
  jets_.jtSDConstituentsPhi.push_back(jcSDphi);
  jets_.jtSDConstituentsM.push_back(jcSDm);
  
  jets_.jtTbeta20p2[jets_.nref] = Tbeta20p2; 
  jets_.jtTbeta20p3[jets_.nref] = Tbeta20p3; 
  jets_.jtTbeta20p4[jets_.nref] = Tbeta20p4; 
  jets_.jtTbeta20p5[jets_.nref] = Tbeta20p5; 

  jets_.jtTbeta30p2[jets_.nref] = Tbeta30p2; 
  jets_.jtTbeta30p3[jets_.nref] = Tbeta30p3; 
  jets_.jtTbeta30p4[jets_.nref] = Tbeta30p4; 
  jets_.jtTbeta30p5[jets_.nref] = Tbeta30p5; 

  jets_.jtCbeta20p2[jets_.nref] = Cbeta20p2; 
  jets_.jtCbeta20p3[jets_.nref] = Cbeta20p3; 
  jets_.jtCbeta20p4[jets_.nref] = Cbeta20p4; 
  jets_.jtCbeta20p5[jets_.nref] = Cbeta20p5; 
			       

  jets_.jtZ11[jets_.nref] = Z11;
  jets_.jtZ20[jets_.nref] = Z20;
  jets_.jtZ22[jets_.nref] = Z22;
  jets_.jtZ31[jets_.nref] = Z31;
  jets_.jtZ33[jets_.nref] = Z33;
  jets_.jtZ40[jets_.nref] = Z40;
  jets_.jtZ42[jets_.nref] = Z42;
  jets_.jtZ44[jets_.nref] = Z44;
  jets_.jtZ51[jets_.nref] = Z51;
  jets_.jtZ53[jets_.nref] = Z53;
  jets_.jtZ55[jets_.nref] = Z55;


  jets_.jtPhi1[jets_.nref] = Phi1;
  jets_.jtPhi2[jets_.nref] = Phi2;
  jets_.jtPhi3[jets_.nref] = Phi3;
  jets_.jtPhi4[jets_.nref] = Phi4;
  jets_.jtPhi5[jets_.nref] = Phi5;
  jets_.jtPhi6[jets_.nref] = Phi6;
  jets_.jtPhi7[jets_.nref] = Phi7;

  jets_.jtSkx[jets_.nref] = Skx;
  jets_.jtSky[jets_.nref] = Sky;
}			       
			       
//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::fillNewJetVarsRefJet(const reco::GenJet jet){
			       
  int nCands = 0;	       
  int nChCands = 0;	       
  int nNeCands = 0;	       

  float jetMByPt = (jet.mass()>0 && jet.pt()!=0) ? jet.mass()/jet.pt() : -999;

  float tot_wt=0;
      
  float RMSCand = 0.0;
  float rmscand_n=0;
  float rmscand_d=0;
      
  float M11=0,M22=0,M12=0,M21=0;
  float Axis1 = 0.0;
  float Axis2 = 0.0;
  float Sigma = 0.0;
      
  float maxCandPt=-999;
  float sumCandPt=0;
  float sumCandPtSq=0;
  float R = 0.0;
  float pTD = 0.0;

  float rm0p5 = 0.0;
  float rm1 = 0.0;
  float rm2 = 0.0;
  float rm3 = 0.0;
      
  TVector2 t_Vect(0,0);
  TVector2 r(0,0);
  float pull = 0.0;

  float sd_m = 0.0;
  float sd_pt = 0.0;
  float sd_eta = 0.0;
  float sd_phi = 0.0;
  float sd_ptfrac = 0.0;
  float sd_rm0p5 = 0.0;
  float sd_rm1 = 0.0;
  float sd_rm2 = 0.0;
  float sd_rm3 = 0.0;

  float Tbeta20p2 = 0.0;
  float Tbeta20p3 = 0.0;
  float Tbeta20p4 = 0.0;
  float Tbeta20p5 = 0.0;

  float Tbeta30p2 = 0.0;
  float Tbeta30p3 = 0.0;
  float Tbeta30p4 = 0.0;
  float Tbeta30p5 = 0.0;

  float Cbeta20p2 = 0.0;
  float Cbeta20p3 = 0.0;
  float Cbeta20p4 = 0.0;
  float Cbeta20p5 = 0.0;

  // //! Moments shifted to centriod
  float Mu00=0.0, Mu01=0.0, Mu10=0.0, Mu11=0.0;
  float Mu20=0.0, Mu02=0.0, Mu12=0.0, Mu21=0.0;
  float Mu30=0.0, Mu03=0.0;

  float Phi1 = 0.0;
  float Phi2 = 0.0;
  float Phi3 = 0.0;
  float Phi4 = 0.0;
  float Phi5 = 0.0;
  float Phi6 = 0.0;
  float Phi7 = 0.0;

  float Skx=0.0;
  float Sky=0.0;

  float Z11=0.0;
  float Z20=0.0;
  float Z22=0.0;
  float Z31=0.0;
  float Z33=0.0;
  float Z40=0.0;
  float Z42=0.0;
  float Z44=0.0;
  float Z51=0.0;
  float Z53=0.0;
  float Z55=0.0;

  TVector2 tmp_Z11(0,0);
  TVector2 tmp_Z20(0,0);
  TVector2 tmp_Z22(0,0);
  TVector2 tmp_Z31(0,0);
  TVector2 tmp_Z33(0,0);
  TVector2 tmp_Z40(0,0);
  TVector2 tmp_Z42(0,0);
  TVector2 tmp_Z44(0,0);
  TVector2 tmp_Z51(0,0);
  TVector2 tmp_Z53(0,0);
  TVector2 tmp_Z55(0,0);


  float chargedSumConst = 0;
  int   chargedNConst = 0;
  float eSumConst = 0;
  int   eNConst = 0;
  float muSumConst = 0;
  int   muNConst = 0;
  float photonSumConst = 0;
  int   photonNConst = 0;
  float neutralSumConst = 0;
  int   neutralNConst = 0;
  float hfhadSumConst = 0;
  int   hfhadNConst = 0;
  float hfemSumConst = 0;
  int   hfemNConst = 0;


  vector<fastjet::PseudoJet> jetconsts;
  std::vector<int> jcid;
  std::vector<float> jcE;
  std::vector<float> jcpt;
  std::vector<float> jceta;
  std::vector<float> jcphi;
  std::vector<float> jcm;
  std::vector<int> jcSDid;
  std::vector<float> jcSDE;
  std::vector<float> jcSDpt;
  std::vector<float> jcSDeta;
  std::vector<float> jcSDphi;
  std::vector<float> jcSDm;

  if(jet.numberOfDaughters()>0) {
    nCands = jet.numberOfDaughters();
    for (unsigned k = 0; k < jet.numberOfDaughters(); ++k) {
      const reco::Candidate & dp = *jet.daughter(k);

      if(dp.charge() == 0){
	nNeCands++;
      }else{
	nChCands++;
      }

      float ptcand = dp.pt();
      int pfid     = dp.pdgId();
      
      switch(std::abs(pfid)){
      case 211:  //PFCandidate::h charged hadron
      case 321:
      case 2212:
	chargedSumConst += ptcand;
	chargedNConst++;
	break;
      case 11:  //PFCandidate::e // electron
	eSumConst += ptcand;
	eNConst++;
	break;
      case 13: //PFCandidate::mu // muon
	muSumConst += ptcand;
	muNConst++;
	break;
      case 22: //PFCandidate:gamma // gamma
	photonSumConst += ptcand;
	photonNConst++;
	break;
      case 130:  //PFCandidate::h0 //Neutral hadron
      case 2112:
	neutralSumConst += ptcand;
	neutralNConst++;
	break;
      case 1:  //PFCandidate::h_HF //hadron in HF
	hfhadSumConst += ptcand; 
	hfhadNConst++;
      case 2:  //PFCandidate::egamma_HF //electromagnetic in HF
	hfemSumConst += ptcand; 
	hfemNConst++;
      default:
	break;
      }


      if( dp.pt() > maxCandPt )maxCandPt=dp.pt();
      sumCandPt   += dp.pt();
      sumCandPtSq += pow(dp.pt(),2);
	
      float wt = pow(dp.pt(),2);
      tot_wt += wt;
	
      //! RMSCand
      float deta = dp.eta()- jet.eta();
      float dphi = deltaPhi(dp.phi(), jet.phi());
      float dr = deltaR(dp.eta(), dp.phi(), jet.eta(), jet.phi());
      rmscand_n += wt*dr*dr;
      rmscand_d += wt; 
	
      M11 += wt*deta*deta;
      M22 += wt*dphi*dphi;
      M12 += wt*deta*dphi;
      M21 += wt*deta*dphi;
	
      //! Pull
      TLorentzVector pfvec;
      pfvec.SetPtEtaPhiE(dp.pt(), dp.eta(), dp.phi(), dp.energy());
      fastjet::PseudoJet jc = fastjet::PseudoJet(dp.px(), dp.py(), dp.pz(), dp.energy());
      jc.set_user_info(new ExtraInfo(dp.pdgId()));
      jetconsts.push_back(jc);
      jcid.push_back(dp.pdgId());
      jcE.push_back(jc.e());
      jcpt.push_back(jc.pt());
      jcphi.push_back(jc.phi());
      jceta.push_back(jc.eta());
      jcm.push_back(jc.m());

      float pfy = pfvec.Rapidity();
      float dy = pfy - jet.y();
      r.Set( dy, dphi );
      float r_mag = r.Mod();
      t_Vect += ( dp.pt() /  jet.pt() ) * r_mag *r;

      rm0p5+=(dp.pt()*pow(dr,0.5))/jet.pt();
      rm1+=(dp.pt()*pow(dr,1))/jet.pt();
      rm2+=(dp.pt()*pow(dr,2))/jet.pt();
      rm3+=(dp.pt()*pow(dr,3))/jet.pt();      
      
      //! Added by Pawan ZM 
      tmp_Z11 += ( ( dp.pt() /  jet.pt() ) * r_mag * r);

      tmp_Z20 += ( ( dp.pt() /  jet.pt() ) * (2.*r_mag*r_mag - 1) * r);
      tmp_Z22 += ( ( dp.pt() /  jet.pt() ) * r_mag * r_mag * r);

      tmp_Z31 += ( ( dp.pt() /  jet.pt() ) * ( (3. * r_mag * r_mag * r_mag) - 2.*r_mag ) * r);
      tmp_Z33 += ( ( dp.pt() /  jet.pt() ) * r_mag * r_mag * r_mag * r );

      tmp_Z40 += (( dp.pt() /  jet.pt() ) * ( 6.*pow(r_mag,4) - 6.*pow(r_mag,2) + 1 ) * r);
      tmp_Z42 += (( dp.pt() /  jet.pt() ) * ( 4.*pow(r_mag,4) - 3.*pow(r_mag,2)) * r );
      tmp_Z44 += (( dp.pt() /  jet.pt() ) * pow(r_mag,4) * r);

      tmp_Z51 += (( dp.pt() /  jet.pt() ) * ( 10.*pow(r_mag,5) - 12.*pow(r_mag,3) + 3.*r_mag ) * r);
      tmp_Z53 += (( dp.pt() /  jet.pt() ) * ( 5.*pow(r_mag,5) - 4.*pow(r_mag,3)) * r );
      tmp_Z55 += (( dp.pt() /  jet.pt() ) * pow(r_mag,5) * r );


      //! For two-point moment
      for (unsigned l = k+1; l < jet.numberOfDaughters(); ++l) {
        const reco::Candidate & dl = *jet.daughter(l);
        float dr_kl = deltaR(dl.eta(), dl.phi(), dp.eta(), dp.phi());
	
        Tbeta20p2 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.2) );
        Tbeta20p3 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.3) );
        Tbeta20p4 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.4) );
        Tbeta20p5 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.5) );

	//! For three-point moment
	for (unsigned m = l+1; m < jet.numberOfDaughters(); ++m) {
	  const reco::Candidate & dm = *jet.daughter(m);
	  float dr_km = deltaR(dm.eta(), dm.phi(), dp.eta(), dp.phi());
	  float dr_lm = deltaR(dm.eta(), dm.phi(), dl.eta(), dl.phi());
	  
	  Tbeta30p2 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.2) );
	  Tbeta30p3 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.3) );
	  Tbeta30p4 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.4) );
	  Tbeta30p5 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.5) );
	}
      }

      //! Hu Moment Calculation 
      Mu00 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu01 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));
      Mu10 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu11 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));

      Mu20 += (pow(dp.eta()-jet.eta(),2)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu02 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),2)*(dp.pt()/jet.pt()));
      Mu12 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),2)*(dp.pt()/jet.pt()));
      Mu21 += (pow(dp.eta()-jet.eta(),2)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));

      Mu30 += (pow(dp.eta()-jet.eta(),3)*pow(dp.eta()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu03 += (pow(dp.eta()-jet.eta(),0)*pow(dp.eta()-jet.phi(),3)*(dp.pt()/jet.pt()));
    }
  }

  fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, 999);
  fastjet::ClusterSequence thisClustering_basic(jetconsts, jetDef);
  std::vector<fastjet::PseudoJet> out_jets_basic = thisClustering_basic.inclusive_jets(0);
  if(out_jets_basic.size() == 1){ 
    fastjet::contrib::RecursiveSymmetryCutBase::SymmetryMeasure  symmetry_measure = fastjet::contrib::RecursiveSymmetryCutBase::scalar_z;
    fastjet::contrib::SoftDrop sd(0, 0.1, symmetry_measure, rParam);
    fastjet::PseudoJet sd_jet = sd(out_jets_basic[0]);
    if(sd_jet != 0){
      sd_m = sd_jet.m();
      sd_pt = sd_jet.pt();
      sd_eta = sd_jet.eta();
      sd_phi = sd_jet.phi();
      sd_ptfrac = sd_jet.pt()/jet.pt();    
      std::vector<fastjet::PseudoJet> sd_jetconsts = sd_jet.constituents();
      if(sd_jetconsts.size()!=0){
	for (unsigned k = 0; k < sd_jetconsts.size(); ++k) {
	  const fastjet::PseudoJet dp = sd_jetconsts.at(k);
	  jcSDid.push_back(dp.user_info<ExtraInfo>().part_id());
	  jcSDE.push_back(dp.e());
	  jcSDpt.push_back(dp.pt());
	  jcSDphi.push_back(dp.phi());
	  jcSDeta.push_back(dp.eta());
	  jcSDm.push_back(dp.m());
	  float dr = deltaR(dp.eta(), dp.phi(), jet.eta(), jet.phi());
	  sd_rm0p5+=(dp.pt()*pow(dr,0.5))/sd_jet.pt();
	  sd_rm1+=(dp.pt()*pow(dr,1))/sd_jet.pt();
	  sd_rm2+=(dp.pt()*pow(dr,2))/sd_jet.pt();
	  sd_rm3+=(dp.pt()*pow(dr,3))/sd_jet.pt();
	}
      }
    }
  }
  
  RMSCand = (rmscand_d > 0 ) ? sqrt ( rmscand_n / rmscand_d ) : -999;
      
  M12 = -1.*M12;
  M21 = -1.*M21;
      
  //! eign values
  float trace = M11 + M22;
  float detrm = (M11*M22) - (M12*M21);
      
  float lam1 = trace/2. + sqrt( pow(trace,2)/4. - detrm );
  float lam2 = trace/2. - sqrt( pow(trace,2)/4. - detrm );
      
  Axis1 = (tot_wt > 0 && lam1>=0 ) ? sqrt( lam1 / tot_wt ) : -999;
  Axis2 = (tot_wt > 0 && lam2>=0 ) ? sqrt( lam2 / tot_wt ) : -999;
  
  Sigma = (tot_wt > 0 ) ? sqrt( pow(Axis1,2) + pow(Axis2,2) ) : -999;
      
  R = (sumCandPt > 0 ) ? maxCandPt / sumCandPt : -999;
      
  pTD = (sumCandPt > 0 ) ? sqrt( sumCandPtSq ) / sumCandPt : -999;

      
  pull = t_Vect.Mod();
    
  Tbeta20p2 /= pow(jet.pt(),2); 
  Tbeta20p3 /= pow(jet.pt(),2); 
  Tbeta20p4 /= pow(jet.pt(),2); 
  Tbeta20p5 /= pow(jet.pt(),2); 

  Tbeta30p2 /= pow(jet.pt(),3); 
  Tbeta30p3 /= pow(jet.pt(),3); 
  Tbeta30p4 /= pow(jet.pt(),3); 
  Tbeta30p5 /= pow(jet.pt(),3); 

  Cbeta20p2 = ( Tbeta20p2 > 0 ) ? Tbeta30p2 / Tbeta20p2 : -999;
  Cbeta20p3 = ( Tbeta20p3 > 0 ) ? Tbeta30p3 / Tbeta20p3 : -999;
  Cbeta20p4 = ( Tbeta20p4 > 0 ) ? Tbeta30p4 / Tbeta20p4 : -999;
  Cbeta20p5 = ( Tbeta20p5 > 0 ) ? Tbeta30p5 / Tbeta20p5 : -999;

  //! Hu Moments
  if( Mu00 > 0 ){
    float Eta02 = Mu20/pow(Mu00,(0+2+1));
    float Eta20 = Mu20/pow(Mu00,(2+0+1));
    float Eta11 = Mu11/pow(Mu00,(1+1+1));
    float Eta12 = Mu12/pow(Mu00,(1+2+1));
    float Eta21 = Mu21/pow(Mu00,(2+1+1));
    float Eta03 = Mu03/pow(Mu00,(0+3+1));
    float Eta30 = Mu30/pow(Mu00,(3+0+1));
    
    
    Phi1 = (Eta20 + Eta02);
    Phi2 = pow((Eta20 - Eta02),2) + 4.*pow(Eta11,2);
    Phi3 = pow((Eta30 - 3.*Eta12),2) + pow((3.*Eta21 - Eta03),2);
    Phi4 = pow((Eta30 + Eta12),2) + pow((Eta21 + Eta03),2);
    
    Phi5 = (Eta30 - 3.*Eta12)*(Eta30 + Eta12)*(pow((Eta30 + Eta12),2) - 3.*pow((Eta21 + Eta03),2)) + (3.*Eta21 - Eta03)*(Eta21 + Eta03)*(3.*pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2));
    
    Phi6 = ((Eta20 - Eta02 )*(pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2))) + 4.*Eta11*(Eta30 + Eta12)*(Eta21 + Eta03);
    
    Phi7 = (3.*Eta21 - Eta03)*(Eta30 + Eta12)*(pow((Eta30 + Eta12),2) - 3.*pow((Eta21 + Eta03),2)) - (Eta30 - 3.*Eta12)*(Eta21 + Eta03)*(3.*pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2));
  }else{
    Phi1 = Phi2 = Phi3 = Phi4 = Phi5 = Phi6 = Phi7 = -999;
  }

  Skx = ( Mu20 > 0 ) ? Mu30/pow(Mu20,1.5) : -999;
  Sky = ( Mu02 > 0 ) ? Mu03/pow(Mu02,1.5) : -999;

  
  Z11 += tmp_Z11.Mod();
  Z20 += tmp_Z20.Mod();
  Z22 += tmp_Z22.Mod();
  Z31 += tmp_Z31.Mod();
  Z33 += tmp_Z33.Mod();
  Z40 += tmp_Z40.Mod();
  Z42 += tmp_Z42.Mod();
  Z44 += tmp_Z44.Mod();
  Z51 += tmp_Z51.Mod();
  Z53 += tmp_Z53.Mod();
  Z55 += tmp_Z55.Mod();



  jets_.refnCands[jets_.nref] = nCands;
  jets_.refnChCands[jets_.nref] = nChCands;
  jets_.refnNeCands[jets_.nref] = nNeCands;
  jets_.refchargedSumConst[jets_.nref] = chargedSumConst;
  jets_.refchargedNConst  [jets_.nref] = chargedNConst;  
  jets_.refeSumConst      [jets_.nref] = eSumConst;      
  jets_.refeNConst        [jets_.nref] = eNConst;        
  jets_.refmuSumConst     [jets_.nref] = muSumConst;     
  jets_.refmuNConst       [jets_.nref] = muNConst;       
  jets_.refphotonSumConst [jets_.nref] = photonSumConst; 
  jets_.refphotonNConst   [jets_.nref] = photonNConst;   
  jets_.refneutralSumConst[jets_.nref] = neutralSumConst;
  jets_.refneutralNConst  [jets_.nref] = neutralNConst;  
  jets_.refhfhadSumConst  [jets_.nref] = hfhadSumConst;
  jets_.refhfhadNConst    [jets_.nref] = hfhadNConst;
  jets_.refhfemSumConst   [jets_.nref] = hfemSumConst;
  jets_.refhfemNConst     [jets_.nref] = hfemNConst;

  jets_.refMByPt[jets_.nref] = jetMByPt;
  jets_.refRMSCand[jets_.nref] = RMSCand;
  jets_.refAxis1[jets_.nref] = Axis1;
  jets_.refAxis2[jets_.nref] = Axis2;
  jets_.refSigma[jets_.nref] = Sigma;
  jets_.refrm3[jets_.nref] = rm3;
  jets_.refrm2[jets_.nref] = rm2;
  jets_.refrm1[jets_.nref] = rm1;
  jets_.refrm0p5[jets_.nref] = rm0p5;
  jets_.refR[jets_.nref] = R;
  jets_.refpull[jets_.nref] = pull;
  jets_.refpTD[jets_.nref] = pTD;
  jets_.refSDm[jets_.nref] = sd_m;
  jets_.refSDpt[jets_.nref] = sd_pt;
  jets_.refSDeta[jets_.nref] = sd_eta;
  jets_.refSDphi[jets_.nref] = sd_phi;
  jets_.refSDptFrac[jets_.nref] = sd_ptfrac;
  jets_.refSDrm0p5[jets_.nref] = sd_rm0p5;
  jets_.refSDrm1[jets_.nref] = sd_rm1;
  jets_.refSDrm2[jets_.nref] = sd_rm2;
  jets_.refSDrm3[jets_.nref] = sd_rm3;    

  jets_.refConstituentsId.push_back(jcid);
  jets_.refConstituentsE.push_back(jcE);
  jets_.refConstituentsPt.push_back(jcpt);
  jets_.refConstituentsEta.push_back(jceta);
  jets_.refConstituentsPhi.push_back(jcphi);
  jets_.refConstituentsM.push_back(jcm);
  jets_.refSDConstituentsId.push_back(jcSDid);
  jets_.refSDConstituentsE.push_back(jcSDE);
  jets_.refSDConstituentsPt.push_back(jcSDpt);
  jets_.refSDConstituentsEta.push_back(jcSDeta);
  jets_.refSDConstituentsPhi.push_back(jcSDphi);
  jets_.refSDConstituentsM.push_back(jcSDm);

  jets_.refTbeta20p2[jets_.nref] = Tbeta20p2; 
  jets_.refTbeta20p3[jets_.nref] = Tbeta20p3; 
  jets_.refTbeta20p4[jets_.nref] = Tbeta20p4; 
  jets_.refTbeta20p5[jets_.nref] = Tbeta20p5; 

  jets_.refTbeta30p2[jets_.nref] = Tbeta30p2; 
  jets_.refTbeta30p3[jets_.nref] = Tbeta30p3; 
  jets_.refTbeta30p4[jets_.nref] = Tbeta30p4; 
  jets_.refTbeta30p5[jets_.nref] = Tbeta30p5; 

  jets_.refCbeta20p2[jets_.nref] = Cbeta20p2; 
  jets_.refCbeta20p3[jets_.nref] = Cbeta20p3; 
  jets_.refCbeta20p4[jets_.nref] = Cbeta20p4; 
  jets_.refCbeta20p5[jets_.nref] = Cbeta20p5; 

  jets_.refZ11[jets_.nref] = Z11;
  jets_.refZ20[jets_.nref] = Z20;
  jets_.refZ22[jets_.nref] = Z22;
  jets_.refZ31[jets_.nref] = Z31;
  jets_.refZ33[jets_.nref] = Z33;
  jets_.refZ40[jets_.nref] = Z40;
  jets_.refZ42[jets_.nref] = Z42;
  jets_.refZ44[jets_.nref] = Z44;
  jets_.refZ51[jets_.nref] = Z51;
  jets_.refZ53[jets_.nref] = Z53;
  jets_.refZ55[jets_.nref] = Z55;


  jets_.refPhi1[jets_.nref] = Phi1;
  jets_.refPhi2[jets_.nref] = Phi2;
  jets_.refPhi3[jets_.nref] = Phi3;
  jets_.refPhi4[jets_.nref] = Phi4;
  jets_.refPhi5[jets_.nref] = Phi5;
  jets_.refPhi6[jets_.nref] = Phi6;
  jets_.refPhi7[jets_.nref] = Phi7;

  jets_.refSkx[jets_.nref] = Skx;
  jets_.refSky[jets_.nref] = Sky;
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::fillNewJetVarsGenJet(const reco::GenJet jet){

  int nCands = 0;
  int nChCands = 0;
  int nNeCands = 0;

  float jetMByPt = (jet.mass()>0 && jet.pt()!=0) ? jet.mass()/jet.pt() : -999;

  float tot_wt=0;
      
  float RMSCand = 0.0;
  float rmscand_n=0;
  float rmscand_d=0;
      
  float M11=0,M22=0,M12=0,M21=0;
  float Axis1 = 0.0;
  float Axis2 = 0.0;
  float Sigma = 0.0;
      
  float maxCandPt=-999;
  float sumCandPt=0;
  float sumCandPtSq=0;
  float R = 0.0;
  float pTD = 0.0;

  float rm0p5 = 0.0;
  float rm1 = 0.0;
  float rm2 = 0.0;
  float rm3 = 0.0;
      
  TVector2 t_Vect(0,0);
  TVector2 r(0,0);
  float pull = 0.0;
  

  float sd_m = 0.0;
  float sd_pt = 0.0;
  float sd_eta = 0.0;
  float sd_phi = 0.0;
  float sd_ptfrac = 0.0;
  float sd_rm0p5 = 0.0;
  float sd_rm1 = 0.0;
  float sd_rm2 = 0.0;
  float sd_rm3 = 0.0;

  float Tbeta20p2 = 0.0;
  float Tbeta20p3 = 0.0;
  float Tbeta20p4 = 0.0;
  float Tbeta20p5 = 0.0;

  float Tbeta30p2 = 0.0;
  float Tbeta30p3 = 0.0;
  float Tbeta30p4 = 0.0;
  float Tbeta30p5 = 0.0;

  float Cbeta20p2 = 0.0;
  float Cbeta20p3 = 0.0;
  float Cbeta20p4 = 0.0;
  float Cbeta20p5 = 0.0;

  //! Moments shifted to centriod
  float Mu00=0.0, Mu01=0.0, Mu10=0.0, Mu11=0.0;
  float Mu20=0.0, Mu02=0.0, Mu12=0.0, Mu21=0.0;
  float Mu30=0.0, Mu03=0.0;

  float Phi1 = 0.0;
  float Phi2 = 0.0;
  float Phi3 = 0.0;
  float Phi4 = 0.0;
  float Phi5 = 0.0;
  float Phi6 = 0.0;
  float Phi7 = 0.0;

  float Skx=0.0;
  float Sky=0.0;

  float Z11=0.0;
  float Z20=0.0;
  float Z22=0.0;
  float Z31=0.0;
  float Z33=0.0;
  float Z40=0.0;
  float Z42=0.0;
  float Z44=0.0;
  float Z51=0.0;
  float Z53=0.0;
  float Z55=0.0;

  TVector2 tmp_Z11(0,0);
  TVector2 tmp_Z20(0,0);
  TVector2 tmp_Z22(0,0);
  TVector2 tmp_Z31(0,0);
  TVector2 tmp_Z33(0,0);
  TVector2 tmp_Z40(0,0);
  TVector2 tmp_Z42(0,0);
  TVector2 tmp_Z44(0,0);
  TVector2 tmp_Z51(0,0);
  TVector2 tmp_Z53(0,0);
  TVector2 tmp_Z55(0,0);

  float chargedSumConst = 0;
  int   chargedNConst = 0;
  float eSumConst = 0;
  int   eNConst = 0;
  float muSumConst = 0;
  int   muNConst = 0;
  float photonSumConst = 0;
  int   photonNConst = 0;
  float neutralSumConst = 0;
  int   neutralNConst = 0;
  float hfhadSumConst = 0;
  int   hfhadNConst = 0;
  float hfemSumConst = 0;
  int   hfemNConst = 0;

  vector<fastjet::PseudoJet> jetconsts;
  std::vector<int> jcid;
  std::vector<float> jcE;
  std::vector<float> jcpt;
  std::vector<float> jceta;
  std::vector<float> jcphi;
  std::vector<float> jcm;
  std::vector<int> jcSDid;
  std::vector<float> jcSDE;
  std::vector<float> jcSDpt;
  std::vector<float> jcSDeta;
  std::vector<float> jcSDphi;
  std::vector<float> jcSDm;
  
  if(jet.numberOfDaughters()>0) {
    nCands = jet.numberOfDaughters();
    for (unsigned k = 0; k < jet.numberOfDaughters(); ++k) {
      const reco::Candidate & dp = *jet.daughter(k);

      if(dp.charge() == 0){
	nNeCands++;
      }else{
	nChCands++;
      }

      float ptcand = dp.pt();
      int pfid     = dp.pdgId();

      switch(std::abs(pfid)){
      case 211:  //PFCandidate::h charged hadron
      case 321:
      case 2212:
	chargedSumConst += ptcand;
	chargedNConst++;
	break;
      case 11:  //PFCandidate::e // electron
	eSumConst += ptcand;
	eNConst++;
	break;
      case 13: //PFCandidate::mu // muon
	muSumConst += ptcand;
	muNConst++;
	break;
      case 22: //PFCandidate:gamma // gamma
	photonSumConst += ptcand;
	photonNConst++;
	break;
      case 130:  //PFCandidate::h0 //Neutral hadron
      case 2112:
	neutralSumConst += ptcand;
	neutralNConst++;
	break;
      case 1:  //PFCandidate::h_HF //hadron in HF
	hfhadSumConst += ptcand; 
	hfhadNConst++;
      case 2:  //PFCandidate::egamma_HF //electromagnetic in HF
	hfemSumConst += ptcand; 
	hfemNConst++;
      default:
	break;
      }


      if( dp.pt() > maxCandPt )maxCandPt=dp.pt();
      sumCandPt   += dp.pt();
      sumCandPtSq += pow(dp.pt(),2);
	
      float wt = pow(dp.pt(),2);
      tot_wt += wt;
	
      //! RMSCand
      float deta = dp.eta()- jet.eta();
      float dphi = deltaPhi(dp.phi(), jet.phi());
      float dr = deltaR(dp.eta(), dp.phi(), jet.eta(), jet.phi());
      rmscand_n += wt*dr*dr;
      rmscand_d += wt; 
	
      M11 += wt*deta*deta;
      M22 += wt*dphi*dphi;
      M12 += wt*deta*dphi;
      M21 += wt*deta*dphi;
	
      //! Pull
      TLorentzVector pfvec;
      pfvec.SetPtEtaPhiE(dp.pt(), dp.eta(), dp.phi(), dp.energy());
      fastjet::PseudoJet jc = fastjet::PseudoJet(dp.px(), dp.py(), dp.pz(), dp.energy());
      jc.set_user_info(new ExtraInfo(dp.pdgId()));
      jetconsts.push_back(jc);

      jcid.push_back(dp.pdgId());
      jcE.push_back(jc.e());
      jcpt.push_back(jc.pt());
      jcphi.push_back(jc.phi());
      jceta.push_back(jc.eta());
      jcm.push_back(jc.m());

      float pfy = pfvec.Rapidity();
      float dy = pfy - jet.y();
      r.Set( dy, dphi );
      float r_mag = r.Mod();
      t_Vect += ( dp.pt() /  jet.pt() ) * r_mag *r;

      rm0p5+=(dp.pt()*pow(dr,0.5))/jet.pt();
      rm1+=(dp.pt()*pow(dr,1))/jet.pt();
      rm2+=(dp.pt()*pow(dr,2))/jet.pt();
      rm3+=(dp.pt()*pow(dr,3))/jet.pt();      

      //! Added by Pawan ZM 
      tmp_Z11 += ( ( dp.pt() /  jet.pt() ) * r_mag * r );

      tmp_Z20 += ( ( dp.pt() /  jet.pt() ) * (2.*r_mag*r_mag - 1) * r );
      tmp_Z22 += ( ( dp.pt() /  jet.pt() ) * r_mag * r_mag * r );

      tmp_Z31 += ( ( dp.pt() /  jet.pt() ) * ( (3. * r_mag * r_mag * r_mag) - 2.*r_mag ) * r );
      tmp_Z33 += ( ( dp.pt() /  jet.pt() ) * r_mag * r_mag * r_mag * r );

      tmp_Z40 += (( dp.pt() /  jet.pt() ) * ( 6.*pow(r_mag,4) - 6.*pow(r_mag,2) + 1 ) * r );
      tmp_Z42 += (( dp.pt() /  jet.pt() ) * ( 4.*pow(r_mag,4) - 3.*pow(r_mag,2)) * r );
      tmp_Z44 += (( dp.pt() /  jet.pt() ) * pow(r_mag,4) * r);

      tmp_Z51 += (( dp.pt() /  jet.pt() ) * ( 10.*pow(r_mag,5) - 12.*pow(r_mag,3) + 3.*r_mag ) * r );
      tmp_Z53 += (( dp.pt() /  jet.pt() ) * ( 5.*pow(r_mag,5) - 4.*pow(r_mag,3)) * r );
      tmp_Z55 += (( dp.pt() /  jet.pt() ) * pow(r_mag,5) * r );


      //! For two-point moment
      for (unsigned l = k+1; l < jet.numberOfDaughters(); ++l) {
        const reco::Candidate & dl = *jet.daughter(l);
        float dr_kl = deltaR(dl.eta(), dl.phi(), dp.eta(), dp.phi());
	
        Tbeta20p2 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.2) );
        Tbeta20p3 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.3) );
        Tbeta20p4 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.4) );
        Tbeta20p5 += ( dp.pt() * dl.pt() * pow(dr_kl, 0.5) );

	//! For three-point moment
	for (unsigned m = l+1; m < jet.numberOfDaughters(); ++m) {
	  const reco::Candidate & dm = *jet.daughter(m);
	  float dr_km = deltaR(dm.eta(), dm.phi(), dp.eta(), dp.phi());
	  float dr_lm = deltaR(dm.eta(), dm.phi(), dl.eta(), dl.phi());
	  
	  Tbeta30p2 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.2) );
	  Tbeta30p3 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.3) );
	  Tbeta30p4 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.4) );
	  Tbeta30p5 += ( dp.pt() * dl.pt() * dm.pt() * pow(dr_kl*dr_km*dr_lm, 0.5) );
	}
      }

      //! Hu Moment Calculation 
      Mu00 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu01 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));
      Mu10 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu11 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));

      Mu20 += (pow(dp.eta()-jet.eta(),2)*pow(dp.phi()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu02 += (pow(dp.eta()-jet.eta(),0)*pow(dp.phi()-jet.phi(),2)*(dp.pt()/jet.pt()));
      Mu12 += (pow(dp.eta()-jet.eta(),1)*pow(dp.phi()-jet.phi(),2)*(dp.pt()/jet.pt()));
      Mu21 += (pow(dp.eta()-jet.eta(),2)*pow(dp.phi()-jet.phi(),1)*(dp.pt()/jet.pt()));

      Mu30 += (pow(dp.eta()-jet.eta(),3)*pow(dp.eta()-jet.phi(),0)*(dp.pt()/jet.pt()));
      Mu03 += (pow(dp.eta()-jet.eta(),0)*pow(dp.eta()-jet.phi(),3)*(dp.pt()/jet.pt()));
    }
  }
  fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, 999);
  fastjet::ClusterSequence thisClustering_basic(jetconsts, jetDef);
  std::vector<fastjet::PseudoJet> out_jets_basic = thisClustering_basic.inclusive_jets(0);
  if(out_jets_basic.size() == 1){ 
    fastjet::contrib::RecursiveSymmetryCutBase::SymmetryMeasure  symmetry_measure = fastjet::contrib::RecursiveSymmetryCutBase::scalar_z;
    fastjet::contrib::SoftDrop sd(0, 0.1, symmetry_measure, rParam);
    fastjet::PseudoJet sd_jet = sd(out_jets_basic[0]);
    if(sd_jet != 0){
      sd_m = sd_jet.m();
      sd_pt = sd_jet.pt();
      sd_eta = sd_jet.eta();
      sd_phi = sd_jet.phi();
      sd_ptfrac = sd_jet.pt()/jet.pt();    
      std::vector<fastjet::PseudoJet> sd_jetconsts = sd_jet.constituents();
      if(sd_jetconsts.size()!=0){
	for (unsigned k = 0; k < sd_jetconsts.size(); ++k) {
	  const fastjet::PseudoJet dp = sd_jetconsts.at(k);
	  jcSDid.push_back(dp.user_info<ExtraInfo>().part_id());
	  jcSDE.push_back(dp.e());
	  jcSDpt.push_back(dp.pt());
	  jcSDphi.push_back(dp.phi());
	  jcSDeta.push_back(dp.eta());
	  jcSDm.push_back(dp.m());
	  float dr = deltaR(dp.eta(), dp.phi(), jet.eta(), jet.phi());
	  sd_rm0p5+=(dp.pt()*pow(dr,0.5))/sd_jet.pt();
	  sd_rm1+=(dp.pt()*pow(dr,1))/sd_jet.pt();
	  sd_rm2+=(dp.pt()*pow(dr,2))/sd_jet.pt();
	  sd_rm3+=(dp.pt()*pow(dr,3))/sd_jet.pt();
	}
      }
    }
  }

  RMSCand = (rmscand_d > 0 ) ? sqrt ( rmscand_n / rmscand_d ) : -999;
      
  M12 = -1.*M12;
  M21 = -1.*M21;
      
  //! eign values
  float trace = M11 + M22;
  float detrm = (M11*M22) - (M12*M21);
      
  float lam1 = trace/2. + sqrt( pow(trace,2)/4. - detrm );
  float lam2 = trace/2. - sqrt( pow(trace,2)/4. - detrm );
      
  Axis1 = (tot_wt > 0 && lam1>=0 ) ? sqrt( lam1 / tot_wt ) : -999;
  Axis2 = (tot_wt > 0 && lam2>=0 ) ? sqrt( lam2 / tot_wt ) : -999;
  
  Sigma = (tot_wt > 0 ) ? sqrt( pow(Axis1,2) + pow(Axis2,2) ) : -999;
      
  R = (sumCandPt > 0 ) ? maxCandPt / sumCandPt : -999;
      
  pTD = (sumCandPt > 0 ) ? sqrt( sumCandPtSq ) / sumCandPt : -999;
      
  pull = t_Vect.Mod();
    
  Tbeta20p2 /= pow(jet.pt(),2); 
  Tbeta20p3 /= pow(jet.pt(),2); 
  Tbeta20p4 /= pow(jet.pt(),2); 
  Tbeta20p5 /= pow(jet.pt(),2); 

  Tbeta30p2 /= pow(jet.pt(),3); 
  Tbeta30p3 /= pow(jet.pt(),3); 
  Tbeta30p4 /= pow(jet.pt(),3); 
  Tbeta30p5 /= pow(jet.pt(),3); 

  Cbeta20p2 = ( Tbeta20p2 > 0 ) ? Tbeta30p2 / Tbeta20p2 : -999;
  Cbeta20p3 = ( Tbeta20p3 > 0 ) ? Tbeta30p3 / Tbeta20p3 : -999;
  Cbeta20p4 = ( Tbeta20p4 > 0 ) ? Tbeta30p4 / Tbeta20p4 : -999;
  Cbeta20p5 = ( Tbeta20p5 > 0 ) ? Tbeta30p5 / Tbeta20p5 : -999;

  //! Hu Moments
  if( Mu00 > 0 ){
    float Eta02 = Mu20/pow(Mu00,(0+2+1));
    float Eta20 = Mu20/pow(Mu00,(2+0+1));
    float Eta11 = Mu11/pow(Mu00,(1+1+1));
    float Eta12 = Mu12/pow(Mu00,(1+2+1));
    float Eta21 = Mu21/pow(Mu00,(2+1+1));
    float Eta03 = Mu03/pow(Mu00,(0+3+1));
    float Eta30 = Mu30/pow(Mu00,(3+0+1));
    
    Phi1 = (Eta20 + Eta02);
    Phi2 = pow((Eta20 - Eta02),2) + 4.*pow(Eta11,2);
    Phi3 = pow((Eta30 - 3.*Eta12),2) + pow((3.*Eta21 - Eta03),2);
    Phi4 = pow((Eta30 + Eta12),2) + pow((Eta21 + Eta03),2);
    
    Phi5 = (Eta30 - 3.*Eta12)*(Eta30 + Eta12)*(pow((Eta30 + Eta12),2) - 3.*pow((Eta21 + Eta03),2)) + (3.*Eta21 - Eta03)*(Eta21 + Eta03)*(3.*pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2));
    
    Phi6 = ((Eta20 - Eta02 )*(pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2))) + 4.*Eta11*(Eta30 + Eta12)*(Eta21 + Eta03);
    
    Phi7 = (3.*Eta21 - Eta03)*(Eta30 + Eta12)*(pow((Eta30 + Eta12),2) - 3.*pow((Eta21 + Eta03),2)) - (Eta30 - 3.*Eta12)*(Eta21 + Eta03)*(3.*pow((Eta30 + Eta12),2) - pow((Eta21 + Eta03),2));
  }else{
    Phi1 = Phi2 = Phi3 = Phi4 = Phi5 = Phi6 = Phi7 = -999;
  }

  Skx = ( Mu20 > 0 ) ? Mu30/pow(Mu20,1.5) : -999;
  Sky = ( Mu02 > 0 ) ? Mu03/pow(Mu02,1.5) : -999;

  
  Z11 += tmp_Z11.Mod();
  Z20 += tmp_Z20.Mod();
  Z22 += tmp_Z22.Mod();
  Z31 += tmp_Z31.Mod();
  Z33 += tmp_Z33.Mod();
  Z40 += tmp_Z40.Mod();
  Z42 += tmp_Z42.Mod();
  Z44 += tmp_Z44.Mod();
  Z51 += tmp_Z51.Mod();
  Z53 += tmp_Z53.Mod();
  Z55 += tmp_Z55.Mod();


  jets_.gennCands[jets_.ngen] = nCands;
  jets_.gennChCands[jets_.ngen] = nChCands;
  jets_.gennNeCands[jets_.ngen] = nNeCands;
  jets_.genchargedSumConst[jets_.ngen] = chargedSumConst;
  jets_.genchargedNConst  [jets_.ngen] = chargedNConst;  
  jets_.geneSumConst      [jets_.ngen] = eSumConst;      
  jets_.geneNConst        [jets_.ngen] = eNConst;        
  jets_.genmuSumConst     [jets_.ngen] = muSumConst;     
  jets_.genmuNConst       [jets_.ngen] = muNConst;       
  jets_.genphotonSumConst [jets_.ngen] = photonSumConst; 
  jets_.genphotonNConst   [jets_.ngen] = photonNConst;   
  jets_.genneutralSumConst[jets_.ngen] = neutralSumConst;
  jets_.genneutralNConst  [jets_.ngen] = neutralNConst;  
  jets_.genhfhadSumConst  [jets_.ngen] = hfhadSumConst;
  jets_.genhfhadNConst    [jets_.ngen] = hfhadNConst;
  jets_.genhfemSumConst   [jets_.ngen] = hfemSumConst;
  jets_.genhfemNConst     [jets_.ngen] = hfemNConst;

  jets_.genMByPt[jets_.ngen] = jetMByPt;
  jets_.genRMSCand[jets_.ngen] = RMSCand;
  jets_.genAxis1[jets_.ngen] = Axis1;
  jets_.genAxis2[jets_.ngen] = Axis2;
  jets_.genSigma[jets_.ngen] = Sigma;
  jets_.genrm3[jets_.ngen] = rm3;
  jets_.genrm2[jets_.ngen] = rm2;
  jets_.genrm1[jets_.ngen] = rm1;
  jets_.genrm0p5[jets_.ngen] = rm0p5;
  jets_.genR[jets_.ngen] = R;
  jets_.genpull[jets_.ngen] = pull;
  jets_.genpTD[jets_.ngen] = pTD;
  jets_.genSDm[jets_.ngen] = sd_m;
  jets_.genSDpt[jets_.ngen] = sd_pt;
  jets_.genSDeta[jets_.ngen] = sd_eta;  
  jets_.genSDphi[jets_.ngen] = sd_phi;
  jets_.genSDptFrac[jets_.ngen] = sd_ptfrac;
  jets_.genSDrm0p5[jets_.ngen] = sd_rm0p5;
  jets_.genSDrm1[jets_.ngen] = sd_rm1;
  jets_.genSDrm2[jets_.ngen] = sd_rm2;
  jets_.genSDrm3[jets_.ngen] = sd_rm3; 

  jets_.genConstituentsId.push_back(jcid);
  jets_.genConstituentsE.push_back(jcE);
  jets_.genConstituentsPt.push_back(jcpt);
  jets_.genConstituentsEta.push_back(jceta);
  jets_.genConstituentsPhi.push_back(jcphi);
  jets_.genConstituentsM.push_back(jcm);
  jets_.genSDConstituentsId.push_back(jcSDid);
  jets_.genSDConstituentsE.push_back(jcSDE);
  jets_.genSDConstituentsPt.push_back(jcSDpt);
  jets_.genSDConstituentsEta.push_back(jcSDeta);
  jets_.genSDConstituentsPhi.push_back(jcSDphi);
  jets_.genSDConstituentsM.push_back(jcSDm);

  jets_.genTbeta20p2[jets_.ngen] = Tbeta20p2; 
  jets_.genTbeta20p3[jets_.ngen] = Tbeta20p3; 
  jets_.genTbeta20p4[jets_.ngen] = Tbeta20p4; 
  jets_.genTbeta20p5[jets_.ngen] = Tbeta20p5; 

  jets_.genTbeta30p2[jets_.ngen] = Tbeta30p2; 
  jets_.genTbeta30p3[jets_.ngen] = Tbeta30p3; 
  jets_.genTbeta30p4[jets_.ngen] = Tbeta30p4; 
  jets_.genTbeta30p5[jets_.ngen] = Tbeta30p5; 
			       
  jets_.genCbeta20p2[jets_.ngen] = Cbeta20p2; 
  jets_.genCbeta20p3[jets_.ngen] = Cbeta20p3; 
  jets_.genCbeta20p4[jets_.ngen] = Cbeta20p4; 
  jets_.genCbeta20p5[jets_.ngen] = Cbeta20p5; 

  jets_.genZ11[jets_.ngen] = Z11;
  jets_.genZ20[jets_.ngen] = Z20;
  jets_.genZ22[jets_.ngen] = Z22;
  jets_.genZ31[jets_.ngen] = Z31;
  jets_.genZ33[jets_.ngen] = Z33;
  jets_.genZ40[jets_.ngen] = Z40;
  jets_.genZ42[jets_.ngen] = Z42;
  jets_.genZ44[jets_.ngen] = Z44;
  jets_.genZ51[jets_.ngen] = Z51;
  jets_.genZ53[jets_.ngen] = Z53;
  jets_.genZ55[jets_.ngen] = Z55;


  jets_.genPhi1[jets_.ngen] = Phi1;
  jets_.genPhi2[jets_.ngen] = Phi2;
  jets_.genPhi3[jets_.ngen] = Phi3;
  jets_.genPhi4[jets_.ngen] = Phi4;
  jets_.genPhi5[jets_.ngen] = Phi5;
  jets_.genPhi6[jets_.ngen] = Phi6;
  jets_.genPhi7[jets_.ngen] = Phi7;

  jets_.genSkx[jets_.ngen] = Skx;
  jets_.genSky[jets_.ngen] = Sky;
}



//--------------------------------------------------------------------------------------------------
int HiInclusiveJetAnalyzer::getGroomedGenJetIndex(const reco::GenJet jet) const {
  //Find closest soft-dropped gen jet
  double drMin = 100;
  int imatch = -1;
  for(unsigned int i = 0 ; i < gensubjets_->size(); ++i) {
    const reco::Jet& mjet = (*gensubjets_)[i];

    double dr = deltaR(jet,mjet);
    if(dr < drMin){
      imatch = i;
      drMin = dr;
    }
  }
  return imatch;
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::analyzeRefSubjets(const reco::GenJet jet) {

  //Find closest soft-dropped gen jet
  int imatch = getGroomedGenJetIndex(jet);
  double dr = 999.;
  if(imatch>-1) {
    const reco::Jet& mjet =  (*gensubjets_)[imatch];
    dr = deltaR(jet,mjet);
  }

  std::vector<float> sjpt;
  std::vector<float> sjeta;
  std::vector<float> sjphi;
  std::vector<float> sjm;
  if(imatch>-1 && dr<0.4) {
    const reco::Jet& mjet =  (*gensubjets_)[imatch];
    jets_.refptG[jets_.nref]  = mjet.pt();
    jets_.refetaG[jets_.nref] = mjet.eta();
    jets_.refphiG[jets_.nref] = mjet.phi();
    jets_.refmG[jets_.nref]   = mjet.mass();

    if(mjet.numberOfDaughters()>0) {
      for (unsigned k = 0; k < mjet.numberOfDaughters(); ++k) {
        const reco::Candidate & dp = *mjet.daughter(k);
        sjpt.push_back(dp.pt());
        sjeta.push_back(dp.eta());
        sjphi.push_back(dp.phi());
        sjm.push_back(dp.mass());
      }
    }
  }
  else {
    jets_.refptG[jets_.nref]  = -999.;
    jets_.refetaG[jets_.nref] = -999.;
    jets_.refphiG[jets_.nref] = -999.;
    jets_.refmG[jets_.nref]   = -999.;

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
void HiInclusiveJetAnalyzer::analyzeGenSubjets(const reco::GenJet jet) {
  //Find closest soft-dropped gen jet
  int imatch = getGroomedGenJetIndex(jet);
  double dr = 999.;
  if(imatch>-1) {
    const reco::Jet& mjet =  (*gensubjets_)[imatch];
    dr = deltaR(jet,mjet);
  }

  std::vector<float> sjpt;
  std::vector<float> sjeta;
  std::vector<float> sjphi;
  std::vector<float> sjm;
  std::vector<float> sjarea;
  if(imatch>-1 && dr<0.4) {
    const reco::Jet& mjet =  (*gensubjets_)[imatch];
    jets_.genptG[jets_.ngen]  = mjet.pt();
    jets_.genetaG[jets_.ngen] = mjet.eta();
    jets_.genphiG[jets_.ngen] = mjet.phi();
    jets_.genmG[jets_.ngen]   = mjet.mass();
    
    if(mjet.numberOfDaughters()>0) {
      for (unsigned k = 0; k < mjet.numberOfDaughters(); ++k) {
        const reco::Candidate & dp = *mjet.daughter(k);
        sjpt.push_back(dp.pt());
        sjeta.push_back(dp.eta());
        sjphi.push_back(dp.phi());
        sjm.push_back(dp.mass());
        //sjarea.push_back(dp.castTo<reco::JetRef>()->jetArea());
      }
    }
  }
  else {
    jets_.genptG[jets_.ngen]  = -999.;
    jets_.genetaG[jets_.ngen] = -999.;
    jets_.genphiG[jets_.ngen] = -999.;
    jets_.genmG[jets_.ngen]   = -999.;
    
    sjpt.push_back(-999.);
    sjeta.push_back(-999.);
    sjphi.push_back(-999.);
    sjm.push_back(-999.);
    sjarea.push_back(-999.);
  }

  jets_.genSubJetPt.push_back(sjpt);
  jets_.genSubJetEta.push_back(sjeta);
  jets_.genSubJetPhi.push_back(sjphi);
  jets_.genSubJetM.push_back(sjm);
  jets_.genSubJetArea.push_back(sjarea);
}





void HiInclusiveJetAnalyzer::fillInfoSV(const reco::SecondaryVertexTagInfo * tagInfoSV,  math::XYZVector jetDir)
{
  jets_.nsvtx[jets_.nref]     = tagInfoSV->nVertices();
  std::vector<int> svtxntrks;
  std::vector<float> svjetDr, svtxdl, svtxdls, svtxdl2d, svtxdls2d, svtxm, svtxpt, svtxFlPhi, svtxFlEta, svtxmcorr;
  std::vector<int> svType;
  /*  
  if(jets_.nsvtx[jets_.nref]==0){
    svtxntrks.push_back(-999);
    svjetDr.push_back(-999);
    svtxdl.push_back(-999);
    svtxdls.push_back(-999);
    svtxdl2d.push_back(-999);
    svtxm.push_back(-999);
    svtxpt.push_back(-999);	
    svType.push_back(-999);
  }
  */
  for (int ivtx=0; ivtx<jets_.nsvtx[jets_.nref]; ivtx++) {
    svtxntrks.push_back(tagInfoSV->nVertexTracks(ivtx));
    cout<<" nSV tracks "<<tagInfoSV->nVertexTracks(ivtx)<<endl;
    svjetDr.push_back(reco::deltaR(tagInfoSV->flightDirection(ivtx),jetDir));
    // this is the 3d flight distance, for 2-D use (0,true)
    Measurement1D m1D = tagInfoSV->flightDistance(ivtx);
    svtxdl.push_back(m1D.value());
    svtxdls.push_back(m1D.significance());
    Measurement1D m2D = tagInfoSV->flightDistance(ivtx,true);
    svtxdl2d.push_back(m2D.value());
    svtxdls2d.push_back(m2D.significance());
    const Vertex& svtx = tagInfoSV->secondaryVertex(ivtx);
    //cout<<" svtx.px "<<svtx.p4().px()<<" svtx.py "<<svtx.p4().py()<<" svtx.pz "<<svtx.p4().pz()<<endl;
    svtxFlPhi.push_back(tagInfoSV->flightDirection(ivtx).phi());
    svtxFlEta.push_back(tagInfoSV->flightDirection(ivtx).eta());
    double svtxM = svtx.p4().mass();
    double svtxPt = svtx.p4().pt();
    //if(jets_.nsvtx[jets_.nref]>1) cout << "svtxm: "<< svtxM << " svtxpt: "<< svtxPt << endl;
    svtxm.push_back(svtxM); 
    svtxpt.push_back(svtxPt);
    
    //try out the corrected mass (http://arxiv.org/pdf/1504.07670v1.pdf)
    //mCorr=srqt(m^2+p^2sin^2(th)) + p*sin(th)
    double sinth = svtx.p4().Vect().Unit().Cross(tagInfoSV->flightDirection(ivtx).unit()).Mag2();
    sinth = sqrt(sinth);
    svtxmcorr.push_back(sqrt(pow(svtxM,2)+(pow(svtxPt,2)*pow(sinth,2)))+svtxPt*sinth);

    
    TrackRefVector svtxTracks = tagInfoSV->vertexTracks(ivtx);
    unsigned nVtxTypeTrks=0, nVtxTypeVertices=0, nAssocTrks=0;
    for(unsigned int itrk=0; itrk<svtxTracks.size(); itrk++){
      if(tagInfoSV->trackWeight(ivtx, svtxTracks.at(itrk)) >= 0.5) nAssocTrks++; //0.5 is default value
    }
    if(nAssocTrks>0){
      bool isTrackVertex = (nAssocTrks==1);
      ++*(isTrackVertex ? &nVtxTypeTrks : &nVtxTypeVertices); // <-- holy complicated logic, batman! From GhostTrackComputer.cc
    }
    if(nVtxTypeVertices) svType.push_back(0);
    else if(nVtxTypeTrks) svType.push_back(1);
    else svType.push_back(2);	
    
  }
  jets_.svType.push_back(svType);
  jets_.svtxntrk.push_back(svtxntrks);
  jets_.svJetDeltaR.push_back(svjetDr);
  jets_.svtxdl.push_back(svtxdl);
  jets_.svtxdls.push_back(svtxdls);
  jets_.svtxdl2d.push_back(svtxdl2d);
  jets_.svtxdls2d.push_back(svtxdls2d);
  jets_.svtxm.push_back(svtxm);
  jets_.svtxpt.push_back(svtxpt);
  jets_.svtxFlPhi.push_back(svtxFlPhi);
  jets_.svtxFlEta.push_back(svtxFlEta);
  jets_.svtxmcorr.push_back(svtxmcorr);
  
}

void HiInclusiveJetAnalyzer::fillInfoCandSV(const reco::CandSecondaryVertexTagInfo *tagInfoSV,  math::XYZVector jetDir)
{
  jets_.nsvtx[jets_.nref]     = tagInfoSV->nVertices();
  std::vector<int> svtxntrks;
  std::vector<float> svjetDr, svtxdl, svtxdls, svtxdl2d, svtxdls2d, svtxm, svtxpt, svtxFlPhi, svtxFlEta, svtxmcorr;
  std::vector<int> svType;

  /*  
  if(jets_.nsvtx[jets_.nref]==0){
    svtxntrks.push_back(-999);
    svjetDr.push_back(-999);
    svtxdl.push_back(-999);
    svtxdls.push_back(-999);
    svtxdl2d.push_back(-999);
    svtxm.push_back(-999);
    svtxpt.push_back(-999);	
    svType.push_back(-999);
  }
  */
  for (int ivtx=0; ivtx<jets_.nsvtx[jets_.nref]; ivtx++) {
    //  if(jets_.nsvtx[jets_.nref]>1) cout << "ntrks: " << tagInfoSV->nVertexTracks(ivtx) << endl;
    svtxntrks.push_back(tagInfoSV->nVertexTracks(ivtx));
    svjetDr.push_back(reco::deltaR(tagInfoSV->flightDirection(ivtx),jetDir));
    // this is the 3d flight distance, for 2-D use (0,true)
    Measurement1D m1D = tagInfoSV->flightDistance(ivtx);
    svtxdl.push_back(m1D.value());
    svtxdls.push_back(m1D.significance());
    Measurement1D m2D = tagInfoSV->flightDistance(ivtx,true);
    svtxdl2d.push_back(m2D.value());
    svtxdls2d.push_back(m2D.significance());

    const reco::VertexCompositePtrCandidate svtx = tagInfoSV->secondaryVertex(ivtx);
    svtxFlPhi.push_back(tagInfoSV->flightDirection(ivtx).phi());
    svtxFlEta.push_back(tagInfoSV->flightDirection(ivtx).eta());
    
    double svtxM = svtx.p4().mass();
    double svtxPt = svtx.p4().pt();
    //if(jets_.nsvtx[jets_.nref]>1) cout << "svtxm: "<< svtxM << " svtxpt: "<< svtxPt << endl;
    svtxm.push_back(svtxM); 
    svtxpt.push_back(svtxPt);
    
    //try out the corrected mass (http://arxiv.org/pdf/1504.07670v1.pdf)
      //mCorr=srqt(m^2+p^2sin^2(th)) + p*sin(th)
    double sinth = svtx.p4().Vect().Unit().Cross(tagInfoSV->flightDirection(ivtx).unit()).Mag2();
    sinth = sqrt(sinth);
    svtxmcorr.push_back(sqrt(pow(svtxM,2)+(pow(svtxPt,2)*pow(sinth,2)))+svtxPt*sinth);

    
    std::vector<edm::Ptr<reco::Candidate> > svtxTracks = tagInfoSV->vertexTracks(ivtx);
    unsigned nVtxTypeTrks=0, nVtxTypeVertices=0, nAssocTrks=0;
    for(unsigned int itrk=0; itrk<svtxTracks.size(); itrk++){
      if(tagInfoSV->trackWeight(ivtx, svtxTracks.at(itrk)) >= 0.5) nAssocTrks++; //0.5 is default value
    }
    if(nAssocTrks>0){
      bool isTrackVertex = (nAssocTrks==1);
      ++*(isTrackVertex ? &nVtxTypeTrks : &nVtxTypeVertices); // <-- holy complicated logic, batman! From GhostTrackComputer.cc
    }
    if(nVtxTypeVertices) svType.push_back(0);
    else if(nVtxTypeTrks) svType.push_back(1);
    else svType.push_back(2);	
    
  }
  jets_.svType.push_back(svType);
  jets_.svtxntrk.push_back(svtxntrks);
  jets_.svJetDeltaR.push_back(svjetDr);
  jets_.svtxdl.push_back(svtxdl);
  jets_.svtxdls.push_back(svtxdls);
  jets_.svtxdl2d.push_back(svtxdl2d);
  jets_.svtxdls2d.push_back(svtxdls2d);
  jets_.svtxm.push_back(svtxm);
  jets_.svtxpt.push_back(svtxpt);
  jets_.svtxFlPhi.push_back(svtxFlPhi);
  jets_.svtxFlEta.push_back(svtxFlEta);
  jets_.svtxmcorr.push_back(svtxmcorr);


}

void HiInclusiveJetAnalyzer::fillInfoIP(const reco::TrackIPTagInfo *tagInfoIP)
{


  jets_.nIPtrk[jets_.nref] = tagInfoIP->tracks().size();
  jets_.nselIPtrk[jets_.nref] = tagInfoIP->selectedTracks().size();
  if (doLifeTimeTaggingExtras_) {
    TrackRefVector selTracks=tagInfoIP->selectedTracks();
    
    GlobalPoint pv(tagInfoIP->primaryVertex()->position().x(),tagInfoIP->primaryVertex()->position().y(),tagInfoIP->primaryVertex()->position().z());
    
    reco::TrackKinematics allKinematics;	 
    
    for(int it=0;it<jets_.nselIPtrk[jets_.nref] ;it++){
      jets_.ipJetIndex[jets_.nIP + it]= jets_.nref;
      reco::btag::TrackIPData data = tagInfoIP->impactParameterData()[it];
      jets_.ipPt[jets_.nIP + it] = selTracks[it]->pt();
      jets_.ipEta[jets_.nIP + it] = selTracks[it]->eta();
      jets_.ipDxy[jets_.nIP + it] = selTracks[it]->dxy(tagInfoIP->primaryVertex()->position());
      jets_.ipDz[jets_.nIP + it] = selTracks[it]->dz(tagInfoIP->primaryVertex()->position());
      jets_.ipChi2[jets_.nIP + it] = selTracks[it]->normalizedChi2();
      jets_.ipNHit[jets_.nIP + it] = selTracks[it]->numberOfValidHits();
      jets_.ipNHitPixel[jets_.nIP + it] = selTracks[it]->hitPattern().numberOfValidPixelHits();
      jets_.ipNHitStrip[jets_.nIP + it] = selTracks[it]->hitPattern().numberOfValidStripHits();
      jets_.ipIsHitL1[jets_.nIP + it]  = selTracks[it]->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector(1),1); //(inBarrel, layer)
      jets_.ipProb0[jets_.nIP + it] = tagInfoIP->probabilities(0)[it];
      jets_.ip2d[jets_.nIP + it] = data.ip2d.value();
      jets_.ip2dSig[jets_.nIP + it] = data.ip2d.significance();
      jets_.ip3d[jets_.nIP + it] = data.ip3d.value();
      jets_.ip3dSig[jets_.nIP + it] = data.ip3d.significance();
      jets_.ipDist2Jet[jets_.nIP + it] = data.distanceToJetAxis.value();
      jets_.ipClosest2Jet[jets_.nIP + it] = (data.closestToJetAxis - pv).mag();  //decay length
      
    }
    
    jets_.nIP += jets_.nselIPtrk[jets_.nref];
  }
}



void HiInclusiveJetAnalyzer::fillInfoCandIP(const reco::CandIPTagInfo *tagInfoIP)
{


  jets_.nIPtrk[jets_.nref] = tagInfoIP->tracks().size();
  jets_.nselIPtrk[jets_.nref] = tagInfoIP->selectedTracks().size();

}




DEFINE_FWK_MODULE(HiInclusiveJetAnalyzer);
