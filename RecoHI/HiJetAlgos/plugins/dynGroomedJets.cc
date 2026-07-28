
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

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/BTauReco/interface/ShallowTagInfo.h"
#include "CommonTools/UtilAlgos/interface/DeltaR.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "AnalysisDataFormats/TrackInfo/interface/TrackToGenParticleMap.h"
#include "CommonTools/MVAUtils/interface/TMVAEvaluator.h"

#include "HeavyIonsAnalysis/JetAnalysis/interface/HiInclusiveJetAnalyzer.h"

//#include <xgboost/c_api.h> 

template <class T>
class dynGroomedJets : public edm::global::EDProducer<> {
public:
  explicit dynGroomedJets(const edm::ParameterSet&);
  // ~dynGroomedJets() override = default;
  ~dynGroomedJets() {
    //if (aggregateHF_ && withXGB_) XGBoosterFree(*xgbTagger);
  }

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  std::pair<bool,bool> IterativeDeclustering(std::vector<fastjet::PseudoJet>, 
                             fastjet::PseudoJet *, fastjet::PseudoJet *, 
                             std::vector<fastjet::PseudoJet>&, 
                             std::vector<fastjet::PseudoJet>&,
                             reco::PFCandidate) const;
  reco::BasicJet ConvertFJ2BasicJet(fastjet::PseudoJet *, 
                                    std::vector<fastjet::PseudoJet>, 
                                    edm::Handle<std::vector<reco::PFCandidate>>, 
                                    const edm::EventSetup&) const;
  reco::BasicJet ConvertFJ2BasicJet(fastjet::PseudoJet *, 
                                    std::vector<fastjet::PseudoJet>, 
                                    edm::Handle<edm::View<pat::PackedCandidate>>, 
                                    const edm::EventSetup&) const;

  
  typedef std::tuple<std::vector<fastjet::PseudoJet>, std::vector<reco::PFCandidate>, reco::PFCandidate> jetConstituentsPseudoHFTuple;
  jetConstituentsPseudoHFTuple aggregateHFGen(const T&, 
                                             reco::TrackToGenParticleMap) const;
  jetConstituentsPseudoHFTuple aggregateHFReco(const T&, 
                                              reco::TrackToGenParticleMap) const;

  // ------------- member data ----------------------------
  edm::EDGetTokenT<std::vector<T>> jetSrc_;
  edm::EDGetTokenT<std::vector<reco::PFCandidate>> constitSrc_;
  edm::EDGetTokenT<edm::View<pat::PackedCandidate>>  packedConstitSrc_;
  edm::EDGetTokenT<reco::TrackToGenParticleMap> candToGenParticleMapToken_;

  std::unique_ptr<TMVAEvaluator> tmvaTagger;
  //std::unique_ptr<BoosterHandle> xgbTagger;

  bool isMC_;

  bool writeConstits_;
  bool doLateKt_;
  bool chargedOnly_;
  bool doGenJets_;
  
  double zcut_;
  double beta_;
  double dynktcut_;
  double ktcut_;
  double rParam_;
  double ptCut_;
  double trkInefRate_; // 0 by default

  bool aggregateHF_;
  bool withTruthInfo_;
  bool withCuts_;
  //bool withXGB_;
  bool withTMVA_;
  //edm::FileInPath xgb_path_;
  edm::FileInPath tmva_path_;
  std::vector<std::string> tmva_variable_names_;
  std::vector<std::string> tmva_spectator_names_;
  std::string ipTagInfoLabel_;
  std::string svTagInfoLabel_;

  const double elMass = 0.000511169;
  const double piMass = 0.139526;
  const double muMass = 0.105652;
};

template <class T>
dynGroomedJets<T>::dynGroomedJets(const edm::ParameterSet& iConfig) {
  // Get configuration parameters
  isMC_ = iConfig.getParameter<bool>("isMC");
  writeConstits_ = iConfig.getParameter<bool>("writeConstits");
  doLateKt_ = iConfig.getParameter<bool>("doLateKt");
  chargedOnly_ = iConfig.getParameter<bool>("chargedOnly");
  aggregateHF_ = iConfig.getParameter<bool>("aggregateHF"); 
  doGenJets_ = iConfig.getParameter<bool>("doGenJets");
  
  ptCut_ = iConfig.getParameter<double>("ptCut");
  zcut_ = iConfig.getParameter<double>("zcut");
  beta_ = iConfig.getParameter<double>("beta");
  dynktcut_ = iConfig.getParameter<double>("dynktcut");
  ktcut_ = iConfig.getParameter<double>("ktcut");
  rParam_ = iConfig.getParameter<double>("rParam");
  trkInefRate_ = iConfig.getParameter<double>("trkInefRate");
  // std::cout << "trkInefRate_=" << trkInefRate_ << std::endl;

  if (aggregateHF_) {
    withTruthInfo_ = iConfig.getParameter<bool>("aggregateWithTruthInfo");
    withCuts_ = iConfig.getParameter<bool>("aggregateWithCuts");
    //withXGB_ = iConfig.getParameter<bool>("aggregateWithXGB");
    withTMVA_ = iConfig.getParameter<bool>("aggregateWithTMVA");
    /*
    if (withXGB_) {
      xgb_path_ = iConfig.getParameter<edm::FileInPath>("xgb_path");
      }*/
    if (withTMVA_) {
      tmva_path_ = iConfig.getParameter<edm::FileInPath>("tmva_path");
      tmva_variable_names_ = iConfig.getParameter<std::vector<std::string>>("tmva_variables");
      tmva_spectator_names_ = iConfig.getParameter<std::vector<std::string>>("tmva_spectators");
    }
  } 

  // Get labels
  ipTagInfoLabel_ = iConfig.getParameter<std::string>("ipTagInfoLabel");
  svTagInfoLabel_ = iConfig.getParameter<std::string>("svTagInfoLabel");
  
  // Get tokens
  jetSrc_ = consumes<std::vector<T>>(iConfig.getParameter<edm::InputTag>("jetSrc"));
  constitSrc_ = consumes<std::vector<reco::PFCandidate>>(iConfig.getParameter<edm::InputTag>("constitSrc"));
  packedConstitSrc_ = consumes<edm::View<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("constitSrc"));

  if (aggregateHF_ && isMC_) 
    candToGenParticleMapToken_ = consumes<reco::TrackToGenParticleMap>(iConfig.getParameter<edm::InputTag>("candToGenParticleMap"));
  // Initialize objects 
  /*
  if (aggregateHF_ && withXGB_) {
    xgbTagger = std::make_unique<BoosterHandle>();
    XGBoosterCreate(NULL, 0, &(*xgbTagger));
    auto res = XGBoosterLoadModel(*xgbTagger, xgb_path_.fullPath().c_str());    
    }*/

  if (aggregateHF_ && withTMVA_) {
    tmvaTagger = std::make_unique<TMVAEvaluator>();
    tmvaTagger->initialize("Color:Silent:Error",
                            "BDTG",
                            tmva_path_.fullPath(),
                            tmva_variable_names_,
                            tmva_spectator_names_,
                            false,
                            false);
  }

  std::string alias = (iConfig.getParameter<edm::InputTag>("jetSrc")).label();
  produces<std::vector<reco::BasicJet>>().setBranchAlias(alias);
  produces<std::vector<reco::BasicJet>>("SubJets").setBranchAlias(alias);
  produces<std::vector<reco::PFCandidate>>("pseudoHF");
  produces<std::vector<reco::PFCandidate>>("droppedTracks");
}

template <class T>
void dynGroomedJets<T>::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  //std::cout << "In dynGroomedJets" << std::endl;

  auto jetCollection = std::make_unique<reco::BasicJetCollection>();
  auto subjetCollection = std::make_unique<reco::BasicJetCollection>();
  auto pseudoHFCollection = std::make_unique<std::vector<reco::PFCandidate>>();
  auto droppedTrackCollection = std::make_unique<std::vector<reco::PFCandidate>>();

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

  // -- For aggregation -- //
  edm::Handle<reco::TrackToGenParticleMap> candToGenParticleMap;
  //BoosterHandle booster_;
  if (aggregateHF_ && isMC_) {
    iEvent.getByToken(candToGenParticleMapToken_, candToGenParticleMap);
  } 
  // --------------------- //

  indices.resize(jets->size());

  int jetIndex = 0;
  for (const T& jet : *jets) { 
    //std::cout << "new jet with pt: " << jet.pt() << std::endl;
    if (doGenJets_ && isMC_) {
      const reco::GenJet *genJet = jet.genJet();
      if (!genJet) continue;
      p4_hardJets.push_back(math::XYZTLorentzVector(genJet->px(), genJet->py(), genJet->pz(), genJet->energy()));
      area_hardJets.push_back(genJet->jetArea());
    } else {
      p4_hardJets.push_back(math::XYZTLorentzVector(jet.px(), jet.py(), jet.pz(), jet.energy()));
      area_hardJets.push_back(jet.jetArea());
    }
    

    std::vector<fastjet::PseudoJet> jetConstituents = {};

    fastjet::PseudoJet *subFJ1 = new fastjet::PseudoJet();
    fastjet::PseudoJet *subFJ2 = new fastjet::PseudoJet();
    std::vector<fastjet::PseudoJet> constit1;
    std::vector<fastjet::PseudoJet> constit2;

    // if (jet.hadronFlavour() == 5) {
    //   std::cout << "b jet, pt = " << jet.pt() << std::endl;
    // } else {
    //   std::cout << "light jet, pt = " << jet.pt() << std::endl;
    // }

    reco::PFCandidate pseudoHF;
    // aggregate B based on jet type
    if (aggregateHF_) {
      if (doGenJets_ && isMC_) {
		    // std::cout << "------->Aggregating HF for gen jet" << std::endl;
        const reco::GenJet *genJet = jet.genJet();
        if (genJet) {
          auto tempTuple = aggregateHFGen(*genJet, *candToGenParticleMap);
          jetConstituents = std::get<0>(tempTuple);
          pseudoHF = std::get<2>(tempTuple);
        } else {
          continue;
        }
        // std::cout << "genjet i = " << jetIndex << " pt " << jet.pt() << " pt " << pseudoHF.pt() << std::endl;
      } else {
        // std::cout << "------->Aggregating HF for reco jet" << std::endl; 
        // std::cout << "reco jet i = " << jetIndex << " pt " << jet.pt() << std::endl;
        reco::TrackToGenParticleMap recoMap = isMC_ ? *candToGenParticleMap : reco::TrackToGenParticleMap();
        auto tempTuple = aggregateHFReco(jet, recoMap);
        jetConstituents = std::get<0>(tempTuple);
        auto droppedTracks = std::get<1>(tempTuple);
        pseudoHF = std::get<2>(tempTuple);
        // std::cout << "\t pseudoHF m=" << pseudoHF.mass() << std::endl;

        droppedTrackCollection->insert(droppedTrackCollection->end(), droppedTracks.begin(), droppedTracks.end());
        // std::cout << "reco jet i = " << jetIndex << " pt " << jet.pt() << " mb " << pseudoHF.mass() << std::endl;
      } 

      pseudoHFCollection->push_back(pseudoHF);
    } else {
      // std::cout << "\tNot aggregating" << std::endl;
      std::vector<edm::Ptr<reco::Candidate>> constituents = {}; 
      if (doGenJets_ && isMC_) {
        const reco::GenJet *genJet = jet.genJet();
        if (genJet) constituents = genJet->getJetConstituents();
      } else {
        constituents = jet.getJetConstituents();
      }

      for (edm::Ptr<reco::Candidate> constituent : constituents) {
        if ((chargedOnly_) && (constituent->charge() == 0)) continue;
        if (constituent->pt() < ptCut_) continue;

        // fix particle mass-energy
        double mass = 0.;
        if (std::abs(constituent->pdgId()) == 11) mass = elMass;
        else if (std::abs(constituent->pdgId()) == 13) mass = muMass;
        else mass = piMass;
        reco::Candidate::PolarLorentzVector constitV(0., 0., 0., 0.);
        constitV.SetPt(constituent->pt());
        constitV.SetEta(constituent->eta());
        constitV.SetPhi(constituent->phi());
        constitV.SetM(mass);

        jetConstituents.push_back(fastjet::PseudoJet(constituent->px(), constituent->py(), constituent->pz(), constitV.energy()));
      }
    }

    // std::cout << "jet constituents before declustering" << std::endl;
    // for (fastjet::PseudoJet constit : jetConstituents) {
    //   std::cout << "\t\t-m=" << constit.m() << std::endl;
    // }


    // Iterative declustering
    std::pair<bool, bool> temp = IterativeDeclustering(jetConstituents, subFJ1, subFJ2, constit1, constit2, pseudoHF);
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
  iEvent.put(move(pseudoHFCollection), "pseudoHF");
  if (!doGenJets_) iEvent.put(move(droppedTrackCollection), "droppedTracks");
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
  //writeSpecific(basicjet, p4, point, constituents, iSetup);
  writeSpecific(basicjet, p4, point, constituents);

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
  //writeSpecific(basicjet, p4, point, constituents, iSetup);
  writeSpecific(basicjet, p4, point, constituents);

  return basicjet;
}

template <class T>
std::pair<bool, bool> dynGroomedJets<T>::IterativeDeclustering(std::vector<fastjet::PseudoJet> jetConstituents, 
                                              fastjet::PseudoJet *sub1, fastjet::PseudoJet *sub2, 
                                              std::vector<fastjet::PseudoJet> &constit1, std::vector<fastjet::PseudoJet> &constit2,
                                              reco::PFCandidate pseudoHF) const
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

          flagHF = false;
          if (aggregateHF_) {
            // if aggregateHF look for B particle
            std::vector<fastjet::PseudoJet> j1constituents = j1.constituents();
            double e = 1e-3;
            // std::cout << "\tparticles in leading prong:" << std::endl;
            for (size_t icon = 0; icon < j1constituents.size(); icon++) {
              fastjet::PseudoJet constit = j1constituents[icon];
              // std::cout << "\t\t-pt = " << constit.pt() << std::endl;
              // pseudoJet -> packedCandidate 
              reco::Candidate::PolarLorentzVector tempVector(0., 0., 0., 0.);
              tempVector.SetPt(constit.pt());
              tempVector.SetEta(constit.eta());
              tempVector.SetPhi(constit.phi());
              tempVector.SetM(constit.m());
              pat::PackedCandidate packedConstit;
              packedConstit.setP4(tempVector);    
              
            
              if (std::abs(packedConstit.pt() - pseudoHF.pt()) > e) continue;
              flagHF = true;
              // std::cout << "\t --->HF in leading prong" << std::endl;
              break;
            }
          }

          // if (!flagHF) {
          //   std::cout << "\tparticles in subleading prong:" << std::endl;
          //   std::vector<fastjet::PseudoJet> j2constituents = j2.constituents();
          //   for (size_t icon = 0; icon < j2constituents.size(); icon++) {
          //     fastjet::PseudoJet constit = j2constituents[icon];
          //     std::cout << "\t\t-m = " << constit.m() << std::endl;
          //     if (std::abs(constit.m() - pseudoHF.mass()) > e) continue;

          //     flagHF = true;
          //     std::cout << "\t --->HF in subleading prong" << std::endl;
          //     break;
          //   }
          // }

          // if (!flagHF) std::cout << "\tHF is NOT in the leading prong" << std::endl;
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
    } catch (fastjet::Error &) {} //return -1; }

  
  return std::pair<bool, bool>(isHardest, flagHF);
}

// Function to aggregate reconstructed tracks into pseudo-B's 
template <class T>
typename dynGroomedJets<T>::jetConstituentsPseudoHFTuple dynGroomedJets<T>::aggregateHFReco(const T& jet, 
                                                               reco::TrackToGenParticleMap candToGenParticleMap) const
{
  // std::cout << "Aggregating Bs in reco jet" << std::endl;

  // Input and output particle collections
  std::vector<edm::Ptr<reco::Candidate>> inputJetConstituents = jet.getJetConstituents();
  std::vector<fastjet::PseudoJet> outputJetConstituents = {};
  std::vector<reco::PFCandidate> droppedTracks = {};
  reco::PFCandidate outputPseudoHF;

  // Particle collection to aggregate into pseudo-Bs
  std::map<int, std::vector<edm::Ptr<reco::Candidate>>> hfConstituentsMap;
  reco::Candidate::PolarLorentzVector totalPseudoHF(0., 0., 0., 0.);

  // Grab the IP and SV tag info from the jet
  const reco::CandIPTagInfo *ipTagInfo = jet.tagInfoCandIP(ipTagInfoLabel_.c_str());
  const std::vector<reco::btag::TrackIPData> ipData = ipTagInfo->impactParameterData();
  const std::vector<edm::Ptr<reco::Candidate>> ipTracks = ipTagInfo->selectedTracks();

  const reco::CandSecondaryVertexTagInfo *svTagInfo = jet.tagInfoCandSecondaryVertex(svTagInfoLabel_.c_str());

  // Go over jet constituents 
  for (const edm::Ptr<reco::Candidate> & constit : jet.getJetConstituents()) {
    if (chargedOnly_ && constit->charge() == 0) continue;
    if (constit->pt() < ptCut_) continue;
    
    // Look for particle in ipTracks
    auto itIPTrack = std::find(ipTracks.begin(), ipTracks.end(), constit);
    if (itIPTrack == ipTracks.end()) continue;

    // For track inefficiency uncertainty 
    const double range_from = 0;
    const double range_to = 1;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_real_distribution<double> distr(range_from, range_to);
    double rand_uniform = distr(generator);

    if (rand_uniform < trkInefRate_) {
      // std::cout << "skipping track" << std::endl;
      reco::PFCandidate droppedTrack;
      droppedTrack.setCharge(constit->charge());
      droppedTrack.setP4(constit->p4());
      droppedTrack.setPdgId(constit->pdgId());     
      droppedTracks.push_back(droppedTrack);
      continue;
    } 

    // [DEBUG] 
    // std::cout << "constit pt " << constit->pt() << ", m=" << constit->mass() << ", pdgId=" << constit->pdgId() << std::endl;

    // Get status of particle (1 = not from HF, 100+ = from HF) 
    int status = 1;

    if (isMC_ && withTruthInfo_) {
      if (candToGenParticleMap.find(constit) != candToGenParticleMap.end()) { 
        edm::Ptr<pat::PackedGenParticle> matchGenParticle = candToGenParticleMap.at(constit);
        status = matchGenParticle->status();
      }
    } else {
      // Initialize values 
      const double missing_value = -1000000.;

      float ip3dSig = missing_value;
      float ip2dSig = missing_value;
      float distanceToJetAxis = missing_value;
      bool inSV = false;

      float svtxdls = missing_value;
      float svtxdls2d = missing_value;
      float svtxm = missing_value;
      float svtxmcorr = missing_value;
      float svtxNtrk = missing_value;
      float svtxnormchi2 = missing_value;
      float svtxTrkPtOverSv = missing_value;

      float jtpt = jet.pt();

      // Get IP info 
      int trkIPIndex = itIPTrack - ipTracks.begin();
      const reco::btag::TrackIPData trkIPdata = ipData[trkIPIndex];
      ip3dSig = trkIPdata.ip3d.significance();
      ip2dSig = trkIPdata.ip2d.significance();
      distanceToJetAxis = trkIPdata.distanceToJetAxis.value();
      //int pdg = constit->pdgId();
      //isLepton = (std::abs(pdg) == 11) || (std::abs(pdg) == 13);
    //   if (isLepton) {
    //     std::cout << "\tpdg = " << pdg << ", mass = " << constit->mass() << std::endl;
    //   }
    //   if (!isLepton&&abs(constit->mass()-0.139526)>1e-3) std::cout << "\tpdg = " << pdg << ", mass = " << constit->mass() << std::endl;

      // if nan go back to missing_value
      if (ip3dSig != ip3dSig) ip3dSig = missing_value;
      if (ip2dSig != ip2dSig) ip2dSig = missing_value;
      if (distanceToJetAxis != distanceToJetAxis) distanceToJetAxis = missing_value;
         
      // Get SV info
      for (uint ivtx = 0; ivtx < svTagInfo->nVertices(); ivtx++) {
        std::vector<edm::Ptr<reco::Candidate>> isvTracks = svTagInfo->vertexTracks(ivtx);
        auto itSVTrack = std::find(isvTracks.begin(), isvTracks.end(), constit);
        if (itSVTrack == isvTracks.end()) continue;

        inSV = true;

        svtxNtrk = (float) svTagInfo->nVertexTracks(ivtx);
        
        Measurement1D m1D = svTagInfo->flightDistance(ivtx, 0);
        svtxdls = m1D.significance();

        Measurement1D m2D = svTagInfo->flightDistance(ivtx, 2);
        svtxdls2d = m2D.significance();
        
        const reco::VertexCompositePtrCandidate svtx = svTagInfo->secondaryVertex(ivtx);
        svtxm = svtx.p4().mass();

        double svtxpt = svtx.p4().pt();
        svtxTrkPtOverSv = constit->pt() / svtxpt;

        //mCorr=srqt(m^2+p^2sin^2(th)) + p*sin(th) -> http://arxiv.org/pdf/1504.07670v1.pdf
        double sinth = svtx.p4().Vect().Unit().Cross((svTagInfo->flightDirection(ivtx)).unit()).Mag2();
        sinth = sqrt(sinth);
        double underRoot = std::pow(svtxm, 2) + (std::pow(svtxpt, 2) * std::pow(sinth, 2));
        svtxmcorr = std::sqrt(underRoot) + (svtxpt * sinth);

        svtxnormchi2 = svtx.vertexNormalizedChi2();
        svtxTrkPtOverSv = constit->pt() / svtxpt;

        break;
      } // end vtx loop

      if (withCuts_) {
        if (inSV || (ip3dSig > 2.5)) {
          status = 100;
        } 
      }
      /*
      else if (withXGB_) {
        // std::cout << "Aggregating with XGB" << std::endl;
        float inSVBDT = (inSV) ? 1. : 0.;
        float isLeptonBDT = (isLepton) ? 1. : 0.;

        // Initialize BDT related variables
        float threshold = 0.44;

        bst_ulong out_len = 0; // bst_ulong is a typedef of unsigned long
        const float *f; // array to store predictions
        
        DMatrixHandle data_;
        const int nFeatures = 13;
        const int nEntries = 1;
        // if (svtxdls>0) {}
        // if (svtxdls2d>0) {}
        // if (svtxm>0) {}
        // if (svtxmcorr>0) {}
        // if (svtxNtrk>0) {}
        // if (svtxnormchi2>0) {}
        // if (svtxTrkPtOverSv>0) {}
        float trackDataBDT[nEntries][nFeatures] = {{ip3dSig, ip2dSig, distanceToJetAxis,
                                                    isLeptonBDT, inSVBDT,
                                                    svtxdls, svtxdls2d, svtxm, svtxmcorr,
                                                    svtxNtrk, svtxnormchi2, svtxTrkPtOverSv,
                                                    jtpt}};
        // std::cout << "New track" << std::endl;
        // for (auto var : trackDataBDT[0]) {
        //   std::cout << var << std::endl;
        // }
        
        XGDMatrixCreateFromMat((float *)trackDataBDT, nEntries, nFeatures, missing_value, &data_);
        XGBoosterPredict(*xgbTagger, data_, 0, 0, &out_len, &f);
        float prediction = f[0];
        // float prediction = 0.5;
        // std::cout << "trk with pt " << constit->pt()
        //           << ", ip3dsig " << ip3dSig
        //           << ", in SV " << inSVBDT
        //           << ", svtxm " << svtxm
        //           << ", svtxdls " << svtxdls
                  // << ", model prediction " << prediction 
                  // << std::endl;

        if (prediction > threshold) {
          status = 100;
        }
      } 
      */
      else if (withTMVA_) {
        // [TODO]: create a map of all possible variables and 
        // then make the input only include the variables from 
        // tmva_variable_names_
        std::map<std::string, float> inputs;
        inputs["trkIp3dSig"] = ip3dSig;
        inputs["trkIp2dSig"] = ip2dSig;
        inputs["trkDistToAxis"] = distanceToJetAxis;
        inputs["svtxdls"] = svtxdls;
        inputs["svtxdls2d"] = svtxdls2d;
        inputs["svtxm"] = svtxm;
        inputs["svtxmcorr"] = svtxmcorr;
        inputs["svtxnormchi2"] = svtxnormchi2;
        inputs["svtxNtrk"] = svtxNtrk;
        inputs["svtxTrkPtOverSv"] = svtxTrkPtOverSv;
        inputs["jtpt"] = jtpt;

        float prediction = -99.;

        prediction = tmvaTagger->evaluate(inputs);
        // std::cout << "prediction: " << prediction << std::endl;

        if (prediction > -0.3) {
          status = 100;
        }
      } // endif withTMVA_
    } // endif *not* with truth info

    // Make lorentz vector with correct mass, energy
    double mass = 0.;
    if (std::abs(constit->pdgId()) == 11) mass = elMass;
    else if (std::abs(constit->pdgId()) == 13) mass = muMass;
    else mass = piMass;
    reco::Candidate::PolarLorentzVector constitV(0., 0., 0., 0.);
    constitV.SetPt(constit->pt());
    constitV.SetEta(constit->eta());
    constitV.SetPhi(constit->phi());
    constitV.SetM(mass);

    double energy = constitV.energy();

    // Add particle to output collection or from HF map
    if (status == 1) {
      fastjet::PseudoJet outConstit(constit->px(), constit->py(), constit->pz(), energy);
    //   std::cout << "\t\t primary constit pt " << constit->pt() << ", pdgId " << constit->pdgId() 
    //             << ", mass " << constit->mass() << ", energy " << constit->energy() << ", energy2 " << energy 
    //             << ", rapidity " << constit->rapidity() << ", rapidity2 " << constitV.Rapidity() 
    //             << std::endl;
      outputJetConstituents.push_back(outConstit);
    } else if (status >= 100) {
        // std::cout << "\t\t b constit pt " << constit->pt() << ", pdgId " << constit->pdgId() << ", mass " << constit->mass() << ", energy " << constit->energy() << std::endl;
        // if (abs(constit->pdgId())==11) std::cout << "electron constit" << std::endl;
        hfConstituentsMap[status].push_back(constit);
    }
  } // end jet constituents loop
  // Aggregate particles coming from HF decays into pseudo-B/C's 
  // and add them to the collection
  
  for (auto itTrackFromHF = hfConstituentsMap.begin(); itTrackFromHF != hfConstituentsMap.end(); itTrackFromHF++) {
    reco::Candidate::PolarLorentzVector pseudoHF(0., 0., 0., 0.);
    for (edm::Ptr<reco::Candidate> hfConstituent : itTrackFromHF->second) {
      reco::Candidate::PolarLorentzVector productLorentzVector(0., 0., 0., 0.);
      productLorentzVector.SetPt(hfConstituent->pt());
      productLorentzVector.SetEta(hfConstituent->eta());
      productLorentzVector.SetPhi(hfConstituent->phi());
      double mass = 0.;
      if (std::abs(hfConstituent->pdgId()) == 11) mass = elMass;
      else if (std::abs(hfConstituent->pdgId()) == 13) mass = muMass;
      else mass = piMass;
      productLorentzVector.SetM(mass);
    //   std::cout << "px=" << hfConstituent->px() << ", py=" << hfConstituent->py() << ", pz=" << hfConstituent->pz() << ", e=" << hfConstituent->energy() << std::endl;
    //   std::cout << "hfConstituent->mass()=" << hfConstituent->mass() << ", productLorentzVector.M()" << productLorentzVector.M() << std::endl;
      pseudoHF += productLorentzVector;
      totalPseudoHF += productLorentzVector;

      reco::PFCandidate daughter;
      daughter.setP4(productLorentzVector);
      outputPseudoHF.addDaughter(daughter);
    }

    fastjet::PseudoJet outPseudoHFConstit(pseudoHF.Px(), pseudoHF.Py(), pseudoHF.Pz(), pseudoHF.E());
    outputJetConstituents.push_back(outPseudoHFConstit);
  } // end tracks from B loop
  
  outputPseudoHF.setP4(totalPseudoHF);
  // std::cout << "vector pt = " << totalPseudoHF.pt() << ", reco::PFCandidate pt = " << outputPseudoHF.pt() << std::endl;
//   std::cout << "\t vector mass " << totalPseudoHF.M() << " candidate mass " << outputPseudoHF.mass() << std::endl;
//   std::cout << "\t ndaughters " << outputPseudoHF.numberOfDaughters() << std::endl;
//   std::cout << "\t end aggregation" << std::endl;
  return dynGroomedJets<T>::jetConstituentsPseudoHFTuple {outputJetConstituents, droppedTracks, outputPseudoHF};
} // end aggregateHFReco()

// Function to aggregate gen particles coming from B decays into pseudo-B's 
template <class T>
typename dynGroomedJets<T>::jetConstituentsPseudoHFTuple dynGroomedJets<T>::aggregateHFGen(const T& genJet,
                                                                                          reco::TrackToGenParticleMap candToGenParticleMap) const
{
  // std::cout << "Aggregating HF at gen level" << std::endl;

  // Input and output particle collections
  std::vector<edm::Ptr<reco::Candidate>> inputJetConstituents = genJet.getJetConstituents();
  std::vector<fastjet::PseudoJet> outputJetConstituents = {};
  std::vector<reco::PFCandidate> droppedTracks = {}; // this will remain empty
  reco::PFCandidate outputPseudoHF;

  // Particle collection to aggregate into pseudo-Bs
  std::map<int, std::vector<edm::Ptr<reco::Candidate>>> hfConstituentsMap;
  reco::Candidate::PolarLorentzVector totalPseudoHF(0., 0., 0., 0.);

  // Go over jet constituents
  for (edm::Ptr<reco::Candidate> constit : inputJetConstituents) {
    if(chargedOnly_ && constit->charge() == 0) continue;
    if(constit->pt() < ptCut_) continue;

    bool isNeutrino = (constit->pdgId() == 12); // nue
    isNeutrino &= (constit->pdgId() == 14); // numu
    isNeutrino &= (constit->pdgId() == 16); // nutau
    isNeutrino &= (constit->pdgId() == 18); // nutau'
    if (isNeutrino) {
      std::cout << "found a neutrino" << std::endl;
      continue;
    }

    //   if (abs(constit->pdgId())!=211&&abs(constit->mass()-0.139526)>1e-3) std::cout << "\tpdg = " << constit->pdgId() << ", mass = " << constit->mass() << std::endl;
  
    // Get status of matched gen particle
    edm::Ptr<pat::PackedGenParticle> matchGenParticle = candToGenParticleMap.at(constit);
    int status = matchGenParticle->status();

    // Add particle to output collection or from HF map
    if (status == 1) {
      fastjet::PseudoJet outConstit(constit->px(), constit->py(), constit->pz(), constit->energy());
      outputJetConstituents.push_back(outConstit);
    } else if (status >= 100) {
      hfConstituentsMap[status].push_back(constit);
      // std::cout << "constit pt=" << constit->pt() << std::endl;
    }
  } // end constit loop 

  // Aggregate particles coming from HF decays into pseudo-B/D's 
  // and add them to the collection
  for (auto it = hfConstituentsMap.begin(); it != hfConstituentsMap.end(); ++it) {
    // std::cout << "B with code " << it->first << " has " << (it->second).size() << " constituents" << std::endl;
    reco::Candidate::PolarLorentzVector pseudoHF(0., 0., 0., 0.);
    for (edm::Ptr<reco::Candidate> hfConstituent : it->second) {
      reco::Candidate::PolarLorentzVector productLorentzVector(0., 0., 0., 0.);
      productLorentzVector.SetPt(hfConstituent->pt());
      productLorentzVector.SetEta(hfConstituent->eta());
      productLorentzVector.SetPhi(hfConstituent->phi());
      productLorentzVector.SetM(hfConstituent->mass());
      pseudoHF += productLorentzVector;
      totalPseudoHF += productLorentzVector;
      //std::cout << "hf product lorentz vector: " << productLorentzVector << std::endl;

      reco::PFCandidate daughter;
      daughter.setP4(productLorentzVector);
      outputPseudoHF.addDaughter(daughter);
    }
    // std::cout << "added HF with pt=" << pseudoHF.Pt() << " to collection" << std::endl;
    fastjet::PseudoJet outPseudoHFConstit(pseudoHF.Px(), pseudoHF.Py(), pseudoHF.Pz(), pseudoHF.E());
    outputJetConstituents.push_back(outPseudoHFConstit);
  } // end map loop
  // std::cout << "totalPseudoHF pt=" << totalPseudoHF.Pt() << std::endl;
  // std::cout << "\tgenjet has " << hfConstituentsMap.size() << " B's " << std::endl;

  outputPseudoHF.setP4(totalPseudoHF);
//   std::cout << "vector mass " << totalPseudoHF.M() << " candidate mass " << outputPseudoHF.mass() << std::endl;
  // std::cout << "\tjet constituents after aggregation" << std::endl;
  // for (fastjet::PseudoJet constit : outputJetConstituents) {
  //   std::cout << "\t\t-m=" << constit.m() << std::endl;
  // }
  return dynGroomedJets<T>::jetConstituentsPseudoHFTuple {outputJetConstituents, droppedTracks, outputPseudoHF};
}

template <class T>
void dynGroomedJets<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setComment("Dynamically groomed jets");

  // Input collections
  desc.add<edm::InputTag>("jetSrc", edm::InputTag("slimmedJets"));
  desc.add<edm::InputTag>("constitSrc", edm::InputTag("packedPFCandidates"));
  desc.add<edm::InputTag>("candToGenParticleMap", edm::InputTag("TrackToGenParticleMapProducer"));

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

  desc.add<bool>("doGenJets", false);

  desc.add<bool>("aggregateHF", false);
  desc.add<bool>("aggregateWithTruthInfo", true);
  desc.add<bool>("aggregateWithCuts", false);
  //desc.add<bool>("aggregateWithXGB", false);
  desc.add<bool>("aggregateWithTMVA", false);
  //desc.add<edm::FileInPath>("xgb_path", edm::FileInPath("RecoHI/HiJetAlgos/data/dummy.model"));
  desc.add<edm::FileInPath>("tmva_path", edm::FileInPath("RecoHI/HiJetAlgos/data/TMVAClassification_BDTG.weights.xml"));
  desc.add<std::vector<std::string>>("tmva_variables", {});
  desc.add<std::vector<std::string>>("tmva_spectators", {});
  // Tag info labels
  desc.add<std::string>("ipTagInfoLabel", "pfImpactParameter");
  desc.add<std::string>("svTagInfoLabel", "pfInclusiveSecondaryVertexFinder");

  descriptions.add("dynGroomedPATJets", desc);
}

using dynGroomedPATJets = dynGroomedJets<pat::Jet>;

// define this as a plug-in
DEFINE_FWK_MODULE(dynGroomedPATJets);
