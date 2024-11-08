
/*

  Perform dynamic grooming on jets
  Cannot be done in FastjetJetProducer, as this algorithm is not part of fastjet
  I borrowed liberally from CompoundJetProducer
  If I was a better programmer, I would just call that class

  -Matt Nguyen, 02-02-2022 (Groundhog's day) 

*/

#include <memory>
#include <vector>
#include <cmath>
#include <map>
#include <random>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/SoftDrop.hh"

#include "RecoJets/JetProducers/interface/JetSpecific.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

/*
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/BTauReco/interface/ShallowTagInfo.h"
#include "CommonTools/UtilAlgos/interface/DeltaR.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/JetReco/interface/Jet.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "AnalysisDataFormats/TrackInfo/interface/TrackToGenParticleMap.h"
#include "CommonTools/MVAUtils/interface/TMVAEvaluator.h"

#include "HeavyIonsAnalysis/JetAnalysis/interface/HiInclusiveJetAnalyzer.h"
*/

template <class T>
class dynGroomedJets : public edm::global::EDProducer<> {
public:
  explicit dynGroomedJets(const edm::ParameterSet&);
  // ~dynGroomedJets() override = default;
  ~dynGroomedJets() {  }

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  std::pair<bool,bool> IterativeDeclustering(std::vector<fastjet::PseudoJet>, 
                             fastjet::PseudoJet *, fastjet::PseudoJet *, 
                             std::vector<fastjet::PseudoJet>&, 
                             std::vector<fastjet::PseudoJet>&) const;
  reco::BasicJet ConvertFJ2BasicJet(fastjet::PseudoJet *, 
                                    std::vector<fastjet::PseudoJet>, 
                                    edm::Handle<std::vector<reco::PFCandidate>>, 
                                    const edm::EventSetup&) const;
  reco::BasicJet ConvertFJ2BasicJet(fastjet::PseudoJet *, 
                                    std::vector<fastjet::PseudoJet>, 
                                    edm::Handle<edm::View<pat::PackedCandidate>>, 
                                    const edm::EventSetup&) const;

  
  typedef std::tuple<std::vector<fastjet::PseudoJet>, std::vector<reco::PFCandidate>, reco::PFCandidate> jetConstituentsPseudoHFTuple;

  // ------------- member data ----------------------------
  edm::EDGetTokenT<std::vector<T>> jetSrc_;
  edm::EDGetTokenT<std::vector<reco::PFCandidate>> constitSrc_;
  edm::EDGetTokenT<edm::View<pat::PackedCandidate>>  packedConstitSrc_;

  bool isMC_;

  bool writeConstits_;
  bool doLateKt_;
  bool chargedOnly_;
  
  double zcut_;
  double beta_;
  double dynktcut_;
  double ktcut_;
  double rParam_;
  double ptCut_;
  double trkInefRate_; // 0 by default

  bool withTruthInfo_;
  bool withCuts_;

};

template <class T>
dynGroomedJets<T>::dynGroomedJets(const edm::ParameterSet& iConfig) {
  // Get configuration parameters
  isMC_ = iConfig.getParameter<bool>("isMC");
  writeConstits_ = iConfig.getParameter<bool>("writeConstits");
  doLateKt_ = iConfig.getParameter<bool>("doLateKt");
  chargedOnly_ = iConfig.getParameter<bool>("chargedOnly");
  
  ptCut_ = iConfig.getParameter<double>("ptCut");
  zcut_ = iConfig.getParameter<double>("zcut");
  beta_ = iConfig.getParameter<double>("beta");
  dynktcut_ = iConfig.getParameter<double>("dynktcut");
  ktcut_ = iConfig.getParameter<double>("ktcut");
  rParam_ = iConfig.getParameter<double>("rParam");
  trkInefRate_ = iConfig.getParameter<double>("trkInefRate");

  // Get tokens
  jetSrc_ = consumes<std::vector<T>>(iConfig.getParameter<edm::InputTag>("jetSrc"));
  //constitSrc_ = consumes<edm::View<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("constitSrc"));
  constitSrc_ = consumes<std::vector<reco::PFCandidate>>(iConfig.getParameter<edm::InputTag>("constitSrc"));
  packedConstitSrc_ = consumes<edm::View<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("constitSrc"));


  std::string alias = (iConfig.getParameter<edm::InputTag>("jetSrc")).label();
  produces<std::vector<reco::BasicJet>>().setBranchAlias(alias);
  produces<std::vector<reco::BasicJet>>("SubJets").setBranchAlias(alias);

}

template <class T>
void dynGroomedJets<T>::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {

  auto jetCollection = std::make_unique<reco::BasicJetCollection>();
  auto subjetCollection = std::make_unique<reco::BasicJetCollection>();

  // This will store the handle for the subjets after we write them
  edm::OrphanHandle<std::vector<reco::BasicJet>> subjetHandleAfterPut;
  // this is the mapping of subjet to hard jet
  std::vector<std::vector<int>> indices;
  // this is the list of hardjet 4-momenta
  std::vector<math::XYZTLorentzVector> p4_hardJets;
  // this is the hardjet areas
  std::vector<double> area_hardJets;
  std::vector<bool> isHardest;
  std::vector<bool> leadingHF;

  edm::Handle<std::vector<T>> jets;
  iEvent.getByToken(jetSrc_, jets);
  
  edm::Handle<std::vector<reco::PFCandidate>> pfcands;
  bool isPF = iEvent.getByToken(constitSrc_, pfcands);

  edm::Handle<edm::View<pat::PackedCandidate>> pfcandsPacked;
  //bool isPackedPF = iEvent.getByToken(packedConstitSrc_, pfcandsPacked);
  iEvent.getByToken(packedConstitSrc_, pfcandsPacked);
  // std::cout << "DEBUG: pfcandPacked ok" << std::endl;


  indices.resize(jets->size());

  int jetIndex = 0;
  for (const T& jet : *jets) { 
    //std::cout << "new jet with pt: " << jet.pt() << std::endl;

    p4_hardJets.push_back(math::XYZTLorentzVector(jet.px(), jet.py(), jet.pz(), jet.energy()));
    area_hardJets.push_back(jet.jetArea());

    std::vector<fastjet::PseudoJet> jetConstituents = {};

    fastjet::PseudoJet *subFJ1 = new fastjet::PseudoJet();
    fastjet::PseudoJet *subFJ2 = new fastjet::PseudoJet();
    std::vector<fastjet::PseudoJet> constit1;
    std::vector<fastjet::PseudoJet> constit2;



    std::vector<edm::Ptr<reco::Candidate>> constituents = jet.getJetConstituents();
    
    for (edm::Ptr<reco::Candidate> constituent : constituents) {
      //std::cout<< "charge  "<<constituent->charge()<<" pt  "<<constituent->pt()<<" mass "<<constituent->mass()<<std::endl;
      if ((chargedOnly_) && (constituent->charge() == 0)) continue;
      if (constituent->pt() < ptCut_) continue;
      jetConstituents.push_back(fastjet::PseudoJet(constituent->px(), constituent->py(), constituent->pz(), constituent->energy()));
    }
  

    // std::cout << "jet constituents before declustering" << std::endl;
    // for (fastjet::PseudoJet constit : jetConstituents) {
    //   std::cout << "\t\t-m=" << constit.m() << std::endl;
    // }


    // Iterative declustering
    std::pair<bool, bool> temp = IterativeDeclustering(jetConstituents, subFJ1, subFJ2, constit1, constit2);
    isHardest.push_back(temp.first);
    leadingHF.push_back(temp.second);
    
    
    
    // Convert fastjets to basicjets 
    reco::BasicJet subjet1, subjet2;
    if(isPF){
      subjet1 = ConvertFJ2BasicJet(subFJ1, constit1, pfcands, iSetup);
      subjet2 = ConvertFJ2BasicJet(subFJ2, constit2, pfcands, iSetup);
    }
    else{
      subjet1 = ConvertFJ2BasicJet(subFJ1, constit1, pfcandsPacked, iSetup);
      subjet2 = ConvertFJ2BasicJet(subFJ2, constit2, pfcandsPacked, iSetup);
    }
    
    if (subjet1.pt() > 1.0e-3) {
      indices[jetIndex].push_back(subjetCollection->size());
      if (subFJ1->has_area()) subjet1.setJetArea(subFJ1->area());
      subjetCollection->push_back(subjet1);
    }
    if (subjet2.pt() > 1.0e-3) {
      indices[jetIndex].push_back(subjetCollection->size());
      if (subFJ2->has_area()) subjet2.setJetArea(subFJ2->area());
      subjetCollection->push_back(subjet2);
    }
    
    jetIndex++;
  } // end jet loop

  // put subjets into event record
  subjetHandleAfterPut = iEvent.put(move(subjetCollection), "SubJets"); 

  // Now create the hard jets with ptr's to the subjets as constituents
  std::vector<math::XYZTLorentzVector>::const_iterator ip4 = p4_hardJets.begin(), ip4Begin = p4_hardJets.begin(), ip4End = p4_hardJets.end();
  
  for (; ip4 != ip4End; ++ip4) {
    int p4_index = ip4 - ip4Begin;
    std::vector<int>& ind = indices[p4_index];
    std::vector<edm::Ptr<reco::Candidate>> i_hardJetConstituents;

    // Add the subjets to the hard jet
    for (std::vector<int>::const_iterator isub = ind.begin(); isub != ind.end(); ++isub) {
      edm::Ptr<reco::Candidate> candPtr(subjetHandleAfterPut, *isub, false);
      i_hardJetConstituents.push_back(candPtr);
    }
    
    reco::Particle::Point point(0, 0, 0);
    //cout<<" size of i_hardJetConstituents "<<i_hardJetConstituents.size()<<endl;
    reco::BasicJet toput(*ip4, point, i_hardJetConstituents);
    // ---- hijack jet area to get the hf in leading prong flag
    // if (isHardest[ip4-ip4Begin]) toput.setJetArea(0.8);
    // else toput.setJetArea(0.4);
    if (leadingHF[ip4-ip4Begin]) toput.setJetArea(0.8);
    else toput.setJetArea(0.4);
    jetCollection->push_back(toput);
  }

  //cout << "jetCollection size: " << jetCollection->size() << endl; 
  iEvent.put(move(jetCollection));
  // std::cout << "End of dynGroomedJets" << std::endl;

}

template <class T>
reco::BasicJet dynGroomedJets<T>::ConvertFJ2BasicJet(fastjet::PseudoJet *fj, 
                                                     std::vector<fastjet::PseudoJet> constit, 
                                                     edm::Handle<std::vector<reco::PFCandidate>> pfcands, 
                                                     const edm::EventSetup& iSetup) const
{
  math::XYZTLorentzVector p4(fj->px(), fj->py(), fj->pz(), fj->e());  
  reco::Particle::Point point(0, 0, 0);
  std::vector<edm::Ptr<reco::Candidate>> constituents;
  if (writeConstits_) {
    for(uint j = 0; j < constit.size(); j++){
      double constitPt = constit[j].pt();
      double constitEta = constit[j].eta();
      
      int iCand = -1;
      for (const reco::PFCandidate& pfcand : *pfcands) {
        iCand++;
        
        if (std::fabs(constitPt - pfcand.pt()) < 0.0001 && std::fabs(constitEta - pfcand.eta()) < 0.0001 ){
          constituents.push_back(edm::Ptr<reco::Candidate>(pfcands, iCand));
          break;
        }
      }
    }
  }
  reco::BasicJet basicjet;
  writeSpecific(basicjet, p4, point, constituents, iSetup);

  return basicjet;
}

template <class T>
reco::BasicJet dynGroomedJets<T>::ConvertFJ2BasicJet(fastjet::PseudoJet *fj, 
                                                     std::vector<fastjet::PseudoJet> constit, 
                                                     edm::Handle<edm::View<pat::PackedCandidate>> pfcands, 
                                                     const edm::EventSetup& iSetup) const
{
  math::XYZTLorentzVector p4(fj->px(), fj->py(), fj->pz(), fj->e());  
  reco::Particle::Point point(0, 0, 0);
  std::vector<edm::Ptr<reco::Candidate>> constituents;
  if (writeConstits_) {
    for(uint j = 0; j < constit.size(); j++){
      double constitPt = constit[j].pt();
      double constitEta = constit[j].eta();
      
      int iCand = -1;
      for (const pat::PackedCandidate& pfcand : *pfcands) {
        iCand++;
        
        if (std::fabs(constitPt - pfcand.pt()) < 0.0001 && std::fabs(constitEta - pfcand.eta()) < 0.0001 ){
          constituents.push_back(edm::Ptr<reco::Candidate>(pfcands, iCand));
          break;
        }
      }
    }
  }
  reco::BasicJet basicjet;
  writeSpecific(basicjet, p4, point, constituents, iSetup);

  return basicjet;
}

template <class T>
std::pair<bool, bool> dynGroomedJets<T>::IterativeDeclustering(std::vector<fastjet::PseudoJet> jetConstituents, 
                                              fastjet::PseudoJet *sub1, fastjet::PseudoJet *sub2, 
                                              std::vector<fastjet::PseudoJet> &constit1, std::vector<fastjet::PseudoJet> &constit2) const
{
  //  std::cout << "--- Declustering --- " << std::endl;
  // Iterative declustering for any type of jet
  // given its constituents
  // returns true/false about whether the selected split is the hardest

  bool flagSubjet=false;
  bool isHardest=false;
  bool flagHF=false;
  double kt1=-1;
  double nsplit=0;
  double nsel=0;
  double nsdin=-1;
  double jet_radius_ca = 1.0;
  fastjet::JetDefinition jet_def(fastjet::genkt_algorithm, jet_radius_ca, 0, static_cast <fastjet::RecombinationScheme>(0), fastjet::Best);

  // Return false if no constituents
  if (jetConstituents.size() == 0) return std::pair<bool, bool>(false, false);

  // Reclustering jet constituents with new algorithm                                                                                          
  try
    {
      fastjet::ClusterSequence csiter(jetConstituents, jet_def);
      std::vector<fastjet::PseudoJet> output_jets = csiter.inclusive_jets(0);
      output_jets = sorted_by_pt(output_jets);
      
      
      fastjet::PseudoJet jj = output_jets[0];
      fastjet::PseudoJet j1;
      fastjet::PseudoJet j2;                                                                                                               
      fastjet::PseudoJet j1first;
      fastjet::PseudoJet j2first;

      while (jj.has_parents(j1, j2)) {
        if (j1.perp() < j2.perp()) std::swap(j1,j2);
        
        double delta_R = j1.delta_R(j2);
        double cut = zcut_ * pow(delta_R / rParam_, beta_);
        double z = j2.perp() / (j1.perp() + j2.perp());
        double kt = j2.perp() * delta_R;
        bool passCut = (z > cut);
        if (doLateKt_) passCut = (kt > ktcut_);
        if (passCut && (doLateKt_ || !flagSubjet) ) {
          flagSubjet = true;
          j1first = j1;
          j2first = j2;
          *sub1 = j1first;
          *sub2 = j2first;
          nsdin = nsplit;

        }
        // if (!passCut) {
        //   std::cout << "\tsplit didn't pass the cut" << std::endl;
        // }
        double dyn= z * (1-z) * j2.perp() * pow(delta_R / rParam_, dynktcut_);
        
        if (dyn > kt1) {
          nsel = nsplit;
          kt1 = dyn;
        }
        nsplit = nsplit + 1;
        jj = j1;
      }
      
      if (!flagSubjet) *sub1 = output_jets[0];

      if (sub1->has_constituents()) constit1 = sub1->constituents(); 
      if (sub2->has_constituents()) constit2 = sub2->constituents(); 
      if (nsel == nsdin) isHardest = true; 
    } catch (fastjet::Error) {} //return -1; }

  
  return std::pair<bool, bool>(isHardest, flagHF);
}



template <class T>
void dynGroomedJets<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setComment("Dynamically groomed jets");

  // Configuration parameters
  desc.add<bool>("isMC", true);
  desc.add<bool>("writeConstits", false);
  desc.add<bool>("doLateKt", false);
  desc.add<bool>("chargedOnly", false);
  
  desc.add<double>("zcut", 0.1);
  desc.add<double>("beta", 0.0);
  desc.add<double>("dynktcut", 1.0);
  desc.add<double>("ktcut", 1.0);
  desc.add<double>("rParam", 0.4);
  desc.add<double>("ptCut", 1.);
  desc.add<double>("trkInefRate", 0.); // between 0 and 1
  
  if (typeid(T) == typeid(pat::Jet)) {
    desc.add<edm::InputTag>("jetSrc", edm::InputTag("slimmedJets"));
    desc.add<edm::InputTag>("constitSrc", edm::InputTag("packedPFCandidates"));
    descriptions.add("dynGroomedPatJets", desc);
  }
  else if (typeid(T) == typeid(reco::GenJet)) {
    desc.add<edm::InputTag>("jetSrc", edm::InputTag("ak4GenJets"));
    desc.add<edm::InputTag>("constitSrc", edm::InputTag("genParticles"));
    descriptions.add("dynGroomedGenJets", desc);
  }
  
}

using dynGroomedPatJets = dynGroomedJets<pat::Jet>;
using dynGroomedGenJets = dynGroomedJets<reco::GenJet>;

// define this as a plug-in
DEFINE_FWK_MODULE(dynGroomedPatJets);
DEFINE_FWK_MODULE(dynGroomedGenJets);
