// -*- C++ -*-
//
// Package:    OniaPFCandidateProducer
// Class:      OniaPFCandidateProducer
//
/**\class OniaPFCandidateProducer OniaPFCandidateProducer
 RecoHI/OniaPFCandidateProducer/src/OniaPFCandidateProducer

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Yetkin Yilmaz,32 4-A08,+41227673039,
//         Created:  Thu Jan 20 19:53:58 CET 2011
//
//

// system include files
#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <set>

// user include files
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/PatCandidates/interface/GenericParticle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "HeavyIonsAnalysis/JetAnalysis/interface/OniaPFCandidateProducer.h"

#include "TMath.h"

#include "CommonTools/Utils/interface/PtComparator.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Utilities/interface/Exception.h"

//
// constants, enums and typedefs
//
using namespace std;
using namespace edm;
using namespace pat;

//
// static data member definitions
//

//
// constructors and destructor
//
OniaPFCandidateProducer::OniaPFCandidateProducer(const ParameterSet &iConfig) {
  // register your products
  // pfCandToken_ =
  // consumes<reco::PFCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCandTag"));
  pfCandToken_ = consumes<PackedCandidateCollection>(
      iConfig.getParameter<InputTag>("pfCandTag"));
  compositeToken_ = consumes<CompositeCandidateCollection>(
      iConfig.getParameter<InputTag>("compositeTag"));
  // PI = TMath::Pi();
  jpsiTriggFilter_ = iConfig.getParameter<std::string>("jpsiTrigFilter");
  isHI_ = iConfig.getParameter<bool>("isHI");
  replaceOniaMM_ = iConfig.getParameter<bool>("replaceOniaMM");
  massOnia_ = iConfig.getParameter<double>("massOnia");
  massOnia2_ = iConfig.getParameter<double>("massOnia2");
  replaceDKPi_ = iConfig.getParameter<bool>("replaceDKPi");
  checkOnia_ = iConfig.getParameter<bool>("checkOnia");
  oniaMuonID_ = iConfig.getParameter<std::string>("oniaMuonID");
  applyMuonAcceptance_ = iConfig.getParameter<bool>("applyMuonAcceptance");
  assignClosestOniaMass_ = iConfig.getParameter<bool>("assignClosestOniaMass");
  cutMassOnia_ = iConfig.getParameter<double>("cutMassOnia");

  // produces<reco::PFCandidateCollection>();
  produces<PackedCandidateCollection>();
  if ( assignClosestOniaMass_ &&  (massOnia2_ < 0.1 || cutMassOnia_ < 0.1) ){
    std::cout << " OniaPFCandidateProducer is running with closest onia mass assign but massOnia2 "<<massOnia2_<<" or cutMassOnia "<< cutMassOnia_ <<" is not properly set " << std::endl;
  }
  if (!replaceOniaMM_ && !replaceDKPi_)
    std::cout << " OniaPFCandidateProducer ain't doing jack " << std::endl;
  if (replaceOniaMM_ && replaceDKPi_)
    std::cout << " removing multiple species not yet supported " << std::endl;
  if (oniaMuonID_ != "Soft" && oniaMuonID_ != "Tight")
    throw cms::Exception("Configuration")
        << "oniaMuonID must be either 'Soft' or 'Tight', not '"
        << oniaMuonID_ << "'";
}

OniaPFCandidateProducer::~OniaPFCandidateProducer() {

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void OniaPFCandidateProducer::produce(Event &iEvent, const EventSetup &iSetup) {
  using namespace edm;

  Handle<CompositeCandidateCollection> composites;
  iEvent.getByToken(compositeToken_, composites);

  if (not composites.isValid()) {
    std::cout << "Warning: no composite candidates ..." << std::endl;
    return;
  }

  auto nComp = composites->size();

  std::vector<CompositeCandidate> selComposites;
  // Keep every onium candidate passing the configured selection for
  // diagnostics, including candidates later rejected by the legacy duplicate
  // arbitration.
  std::vector<CompositeCandidate> diagnosticOniaCandidates;

  Handle<PackedCandidateCollection> pfCands;
  iEvent.getByToken(pfCandToken_, pfCands);

  auto prod = std::make_unique<PackedCandidateCollection>();

  // auto prod = std::make_unique<reco::PFCandidateCollection>();

  if (nComp == 0) {
    // With no selected onium candidate there is nothing to replace.  Keep the
    // original jet inputs instead of producing an empty collection.
    prod->insert(prod->end(), pfCands->begin(), pfCands->end());
    iEvent.put(std::move(prod));
    return;
  }

  // sort in pt
  // std::sort(composites->begin(), composites->end(), ptComparator);
  // std::sort(composites->begin(), composites->end(),
  // GreaterByPt<CompositeCandidate>());

  // edm::Handle<reco::PFCandidateCollection> pfCands;
  // first pass over composite candidates, apply selections and check for
  // presence in PF candidates
  // for (std::vector<CompositeCandidate>::const_iterator
  // it=composites->begin(); it!=composites->end(); ++it) {
  for (const auto &cand : *composites) {
    if (replaceDKPi_) { // only selection is pt > 3 GeV for now
      if (seld0Cand(cand)) {
        bool isDup = false;
        for (unsigned i = 0; i < selComposites.size(); i++) {
          if (checkDupTrack(cand, selComposites[i])) {
            isDup = true;
            break;
          }
        }
        if (isDup)
          continue;
        selComposites.push_back(cand);

        double candE = sqrt(cand.p() * cand.p() + 1.86484 * 1.86484);
        // reco::Particle::LorentzVector
        // p4(cand.px(),cand.py(),cand.pz(),candE);
        PackedCandidate::LorentzVector p4(cand.px(), cand.py(), cand.pz(),
                                          candE);
        // charge, LorentzVector, type (reco::PFCandidate::ParticleType::X )
        // reco::PFCandidate
        // newPFCand(0,p4,reco::PFCandidate::ParticleType::h0);
        // reco::PFCandidate
        // newPFCand(0,p4,reco::PFCandidate::ParticleType::h_HF);

        // my code
        // PackedCandidate
        // newPFCand(0,p4,reco::PFCandidate::ParticleType::h_HF); Jelena
        PackedCandidate::Point v(0.01, 0.02, 0.);
        PackedCandidate newPFCand(p4, v, 1., 1., 1., 1, reco::VertexRefProd(),
                                  reco::VertexRef().key()); // check

        prod->push_back(newPFCand);
      }
    } else if (replaceOniaMM_) {
      // apply some selections on the j/psi candidates here
      if (!checkOnia_ || 
          (selOniaCand(cand) && 
            selMuonCand(cand, "muon1") &&
            selMuonCand(cand, "muon2"))
          ) {

        diagnosticOniaCandidates.push_back(cand);
      }
    }
  }

  if (replaceOniaMM_) {
    const auto sharesMuon = [](const CompositeCandidate &cand1,
                               const CompositeCandidate &cand2) {
      for (const char *daughter1 : {"muon1", "muon2"}) {
        const Muon *muon1 =
            dynamic_cast<const Muon *>(cand1.daughter(daughter1));
        if (muon1 == nullptr)
          continue;
        for (const char *daughter2 : {"muon1", "muon2"}) {
          const Muon *muon2 =
              dynamic_cast<const Muon *>(cand2.daughter(daughter2));
          if (muon2 == nullptr)
            continue;
          for (unsigned int iSource = 0;
               iSource < muon1->numberOfSourceCandidatePtrs(); ++iSource) {
            const reco::CandidatePtr source1 =
                muon1->sourceCandidatePtr(iSource);
            for (unsigned int jSource = 0;
                 jSource < muon2->numberOfSourceCandidatePtrs(); ++jSource) {
              const reco::CandidatePtr source2 =
                  muon2->sourceCandidatePtr(jSource);
              if (source1.isNonnull() && source2.isNonnull() &&
                  source1.id() == source2.id() &&
                  source1.key() == source2.key())
                return true;
            }
          }
          if (muon1->innerTrack().isNonnull() &&
              muon2->innerTrack().isNonnull() &&
              muon1->innerTrack().id() == muon2->innerTrack().id() &&
              muon1->innerTrack().key() == muon2->innerTrack().key())
            return true;
        }
      }
      return false;
    };

    // Build connected components of candidates sharing reconstructed muons.
    // Keep the highest-pT candidate in each component; disjoint candidates
    // remain independent and are all retained.
    std::vector<bool> assigned(diagnosticOniaCandidates.size(), false);
    for (unsigned int seed = 0; seed < diagnosticOniaCandidates.size(); ++seed) {
      if (assigned[seed])
        continue;
      std::vector<unsigned int> component{seed};
      assigned[seed] = true;
      for (unsigned int position = 0; position < component.size(); ++position) {
        const unsigned int member = component[position];
        for (unsigned int other = 0; other < diagnosticOniaCandidates.size(); ++other) {
          if (!assigned[other] &&
              sharesMuon(diagnosticOniaCandidates[member],
                          diagnosticOniaCandidates[other])) {
            assigned[other] = true;
            component.push_back(other);
          }
        }
      }

      unsigned int winner = component.front();
      for (const unsigned int member : component) {
        if (diagnosticOniaCandidates[member].pt() >
            diagnosticOniaCandidates[winner].pt())
          winner = member;
      }
      if (component.size() > 1) {
        std::cout << "OniaPFCandidateProducer ARBITRATION: componentSize="
                  << component.size() << " winner=" << winner
                  << " winnerPt=" << diagnosticOniaCandidates[winner].pt()
                  << std::endl;
      }

      const auto &cand = diagnosticOniaCandidates[winner];
      selComposites.push_back(cand);
      const double m = assignClosestOniaMass_
                           ? (cand.mass() > cutMassOnia_ ? massOnia2_ : massOnia_)
                           : massOnia_;
      const double candE = sqrt(cand.p() * cand.p() + m * m);
      PackedCandidate::LorentzVector p4(cand.px(), cand.py(), cand.pz(), candE);
      PackedCandidate::Point v(cand.vx(), cand.vy(), cand.vz());
      PackedCandidate newPFCand(p4,
                                v,
                                0.,
                                cand.eta(),
                                cand.phi(),
                                1,
                                reco::VertexRefProd(),
                                reco::VertexRef().key());
      prod->push_back(newPFCand);
    }
  }

  // Diagnose competing onium candidates without changing the selection.  A
  // shared source identity is stronger evidence than kinematic similarity and
  // remains usable even when the referenced product itself is unavailable.
  if (replaceOniaMM_ && diagnosticOniaCandidates.size() > 1) {
    std::cout << "OniaPFCandidateProducer MULTI-ONIA DEBUG: "
              << diagnosticOniaCandidates.size()
              << " selected candidates before duplicate arbitration"
              << std::endl;
    for (unsigned int i = 0; i < diagnosticOniaCandidates.size(); ++i) {
      const auto &cand = diagnosticOniaCandidates[i];
      std::cout << "  onium[" << i << "] pt/eta/phi/mass=" << cand.pt()
                << "/" << cand.eta() << "/" << cand.phi() << "/"
                << cand.mass() << std::endl;
      for (const char *daughterName : {"muon1", "muon2"}) {
        const Muon *muon =
            dynamic_cast<const Muon *>(cand.daughter(daughterName));
        std::cout << "    " << daughterName << " charge=" << muon->charge()
                  << " pt/eta/phi=" << muon->pt() << "/" << muon->eta()
                  << "/" << muon->phi() << " sources=";
        for (unsigned int iSource = 0;
             iSource < muon->numberOfSourceCandidatePtrs(); ++iSource) {
          const reco::CandidatePtr source = muon->sourceCandidatePtr(iSource);
          std::cout << (iSource == 0 ? "" : ",") << "(" << source.id()
                    << "," << source.key() << ")";
        }
        std::cout << std::endl;
      }
    }

    for (unsigned int i = 0; i < diagnosticOniaCandidates.size(); ++i) {
      for (unsigned int j = i + 1; j < diagnosticOniaCandidates.size(); ++j) {
        const auto &cand1 = diagnosticOniaCandidates[i];
        const auto &cand2 = diagnosticOniaCandidates[j];
        bool sharesSourceMuon = false;
        for (const char *daughter1 : {"muon1", "muon2"}) {
          const Muon *muon1 =
              dynamic_cast<const Muon *>(cand1.daughter(daughter1));
          for (const char *daughter2 : {"muon1", "muon2"}) {
            const Muon *muon2 =
                dynamic_cast<const Muon *>(cand2.daughter(daughter2));
            for (unsigned int iSource = 0;
                 iSource < muon1->numberOfSourceCandidatePtrs(); ++iSource) {
              const reco::CandidatePtr source1 =
                  muon1->sourceCandidatePtr(iSource);
              for (unsigned int jSource = 0;
                   jSource < muon2->numberOfSourceCandidatePtrs(); ++jSource) {
                const reco::CandidatePtr source2 =
                    muon2->sourceCandidatePtr(jSource);
                if (source1.isNonnull() && source2.isNonnull() &&
                    source1.id() == source2.id() &&
                    source1.key() == source2.key())
                  sharesSourceMuon = true;
              }
            }
          }
        }
        const double relativeDpt =
            std::abs(cand1.pt() - cand2.pt()) /
            std::max(std::max(cand1.pt(), cand2.pt()), 1.e-9);
        std::cout << "  pair[" << i << "," << j << "] sharesSourceMuon="
                  << sharesSourceMuon << " dR="
                  << reco::deltaR(cand1.eta(), cand1.phi(), cand2.eta(),
                                  cand2.phi())
                  << " relativeDpt=" << relativeDpt
                  << " deltaMass=" << cand1.mass() - cand2.mass()
                  << std::endl;
      }
    }
  }

  // Watch for a PF charged hadron whose track is suspiciously close to any
  // selected onium muon leg.  These loose values are diagnostic thresholds,
  // not selection cuts.
  if (replaceOniaMM_) {
    for (unsigned int iCand = 0; iCand < diagnosticOniaCandidates.size(); ++iCand) {
      const auto &cand = diagnosticOniaCandidates[iCand];
      for (const char *daughterName : {"muon1", "muon2"}) {
        const Muon *muon =
            dynamic_cast<const Muon *>(cand.daughter(daughterName));
        const reco::TrackRef muonTrack = muon->innerTrack();
        std::set<unsigned int> muonPackedSourceKeys;
        for (unsigned int iSource = 0;
             iSource < muon->numberOfSourceCandidatePtrs(); ++iSource) {
          const reco::CandidatePtr source = muon->sourceCandidatePtr(iSource);
          if (source.isNonnull() && source.id() == pfCands.id())
            muonPackedSourceKeys.insert(source.key());
        }
        for (unsigned int iParticle = 0; iParticle < pfCands->size(); ++iParticle) {
          const auto &particle = (*pfCands)[iParticle];
          if (std::abs(particle.pdgId()) != 211 || !particle.hasTrackDetails())
            continue;
          if (muonPackedSourceKeys.count(iParticle))
            continue;
          const reco::Track *pfTrack = particle.bestTrack();
          const double dR = reco::deltaR(muonTrack->eta(), muonTrack->phi(),
                                         pfTrack->eta(), pfTrack->phi());
          const double relativeDpt =
              std::abs(pfTrack->pt() - muonTrack->pt()) /
              std::max(muonTrack->pt(), 1.e-9);
          if (dR < 1.e-3 && relativeDpt < 1.e-2) {
            std::cout << "OniaPFCandidateProducer CHARGED-HADRON DEBUG: candidate="
                      << iCand << " daughter=" << daughterName
                      << " muon charge=" << muon->charge()
                      << " pt/eta/phi=" << muonTrack->pt() << "/"
                      << muonTrack->eta() << "/" << muonTrack->phi()
                      << " packedKey=" << iParticle
                      << " hadron pdgId=" << particle.pdgId()
                      << " pt/eta/phi=" << pfTrack->pt() << "/"
                      << pfTrack->eta() << "/" << pfTrack->phi()
                      << " dR=" << dR << " relativeDpt=" << relativeDpt
                      << std::endl;
          }
        }
      }
    }
  }
  /*
    for(unsigned i=0;i<selComposites.size();i++){
    std::cout<<" pt "<<selComposites[i].pt()<<" mass
    "<<selComposites[i].mass()<<std::endl;
    }
  */

  int replacedCands = 0;

  // The PAT muons retain source pointers into packedPFCandidates.  Use those
  // exact ProductID/key pairs instead of comparing packed pseudo-track
  // kinematics with the muon inner tracks; MiniAOD packing changes the latter
  // enough to make an absolute-pT comparison unreliable.
  std::set<unsigned int> oniaDaughterKeys;
  if (replaceOniaMM_) {
    for (const auto &cand : selComposites) {
      for (const char *daughterName : {"muon1", "muon2"}) {
        const Muon *muon =
            dynamic_cast<const Muon *>(cand.daughter(daughterName));
        for (unsigned int iSource = 0;
             iSource < muon->numberOfSourceCandidatePtrs(); ++iSource) {
          const reco::CandidatePtr source = muon->sourceCandidatePtr(iSource);
          if (source.isNonnull() && source.id() == pfCands.id()) {
            oniaDaughterKeys.insert(source.key());
            break;
          }
        }
      }
    }
  }

  // now loop over PF candidates and replace ones that are part of composites
  // for(reco::PFCandidateCollection::const_iterator ci  = pfCands->begin();
  // ci!=pfCands->end(); ++ci)  { for(PackedCandidateCollection::const_iterator
  // ci  = pfCands->begin(); ci!=pfCands->end(); ++ci)  {
  for (unsigned int iParticle = 0; iParticle < pfCands->size(); ++iParticle) {
    const auto &particle = (*pfCands)[iParticle];

    bool writeCand = true;

    if (replaceOniaMM_ && oniaDaughterKeys.count(iParticle)) {
      writeCand = false;
      ++replacedCands;
    }

    // if(particle.trackRef().isNonnull()){
    if (replaceDKPi_ && particle.hasTrackDetails()) {

      // reco::TrackRef pfTrack = particle.trackRef();
      const reco::Track *pfTrack = particle.bestTrack();

      double pfPt = pfTrack->pt();
      double pfEta = pfTrack->eta();
      double pfPhi = pfTrack->phi();

      // for(std::vector<CompositeCandidate>::const_iterator
      // it=selComposites.begin(); it!=selComposites.end(); ++it) {
      for (const auto &cand : selComposites) {
        double eps = 0.005;

        if (replaceDKPi_) {

          double dau1Pt = cand.daughter("track1")->pt();
          double dau1Eta = cand.daughter("track1")->eta();
          double dau1Phi = cand.daughter("track1")->phi();

          double dau2Pt = cand.daughter("track2")->pt();
          double dau2Eta = cand.daughter("track2")->eta();
          double dau2Phi = cand.daughter("track2")->phi();

          if ((fabs(dau1Pt - pfPt) < eps && fabs(dau1Eta - pfEta) < eps &&
               fabs(dau1Phi - pfPhi) < eps) ||
              (fabs(dau2Pt - pfPt) < eps && fabs(dau2Eta - pfEta) < eps &&
               fabs(dau2Phi - pfPhi) < eps)) {
            writeCand = false;
            replacedCands++;
          }
        }
      }

    }
    // cout<<" replacedCands = "<<replacedCands <<std::endl;
    if (writeCand)
      prod->push_back(particle);
  }
  if (replaceOniaMM_ && replacedCands != static_cast<int>(oniaDaughterKeys.size())) {
    std::cout << "OniaPFCandidateProducer MATCH DEBUG: expected "
              << oniaDaughterKeys.size()
              << " source-linked PF candidates to be removed but found "
              << replacedCands << std::endl;
    for (unsigned int iCand = 0; iCand < selComposites.size(); ++iCand) {
      const auto &cand = selComposites[iCand];
      for (const char *daughterName : {"muon1", "muon2"}) {
        const Muon *muon =
            dynamic_cast<const Muon *>(cand.daughter(daughterName));
        const reco::TrackRef muonTrack = muon->innerTrack();
        std::cout << "  candidate " << iCand << " " << daughterName
                  << " sourceCandidatePtrs="
                  << muon->numberOfSourceCandidatePtrs()
                  << " packedPFCandidates ProductID=" << pfCands.id()
                  << std::endl;
        for (unsigned int iSource = 0;
             iSource < muon->numberOfSourceCandidatePtrs(); ++iSource) {
          const reco::CandidatePtr source = muon->sourceCandidatePtr(iSource);
          std::cout << "    source[" << iSource << "]: nonnull="
                    << source.isNonnull() << " available=" << source.isAvailable()
                    << " ProductID=" << source.id() << " key=" << source.key()
                    << " pointsToPackedPF=" << (source.id() == pfCands.id());
          if (source.isAvailable())
            std::cout << " pdgId=" << source->pdgId() << " pt/eta/phi="
                      << source->pt() << "/" << source->eta() << "/"
                      << source->phi();
          std::cout << std::endl;
        }
        unsigned int strictMatches = 0;
        double closestTrackDr = 1.e9;
        double closestTrackDpt = 1.e9;
        double closestTrackDeta = 1.e9;
        double closestTrackDphi = 1.e9;
        int closestTrackPdgId = 0;
        double closestP4Dr = 1.e9;
        double closestP4Dpt = 1.e9;
        int closestP4PdgId = 0;
        bool closestP4HasTrackDetails = false;

        for (const auto &particle : *pfCands) {
          const double p4Dr = reco::deltaR(muonTrack->eta(), muonTrack->phi(),
                                            particle.eta(), particle.phi());
          if (p4Dr < closestP4Dr) {
            closestP4Dr = p4Dr;
            closestP4Dpt = particle.pt() - muonTrack->pt();
            closestP4PdgId = particle.pdgId();
            closestP4HasTrackDetails = particle.hasTrackDetails();
          }

          if (!particle.hasTrackDetails())
            continue;
          const reco::Track *pfTrack = particle.bestTrack();
          const double dpt = pfTrack->pt() - muonTrack->pt();
          const double deta = pfTrack->eta() - muonTrack->eta();
          const double dphi = reco::deltaPhi(pfTrack->phi(), muonTrack->phi());
          const double trackDr = std::hypot(deta, dphi);
          if (trackDr < closestTrackDr) {
            closestTrackDr = trackDr;
            closestTrackDpt = dpt;
            closestTrackDeta = deta;
            closestTrackDphi = dphi;
            closestTrackPdgId = particle.pdgId();
          }
          if (std::abs(dpt) < 0.005 && std::abs(deta) < 0.005 &&
              std::abs(dphi) < 0.005)
            ++strictMatches;
        }

        std::cout << "  candidate " << iCand << " " << daughterName
                  << ": innerTrack pt/eta/phi=" << muonTrack->pt() << "/"
                  << muonTrack->eta() << "/" << muonTrack->phi()
                  << ", strictMatches=" << strictMatches
                  << "; closest track-bearing PF: pdgId=" << closestTrackPdgId
                  << " dpt/deta/dphi/dR=" << closestTrackDpt << "/"
                  << closestTrackDeta << "/" << closestTrackDphi << "/"
                  << closestTrackDr
                  << "; closest PF p4: pdgId=" << closestP4PdgId
                  << " dpt/dR=" << closestP4Dpt << "/" << closestP4Dr
                  << " hasTrackDetails=" << closestP4HasTrackDetails
                  << std::endl;
      }
    }
  }
  if (!selComposites.empty()) {
    std::cout << "OniaPFCandidateProducer: selected " << selComposites.size()
              << " onium candidate(s), found " << oniaDaughterKeys.size()
              << " daughter candidate(s) in the PF jet input, removed "
              << replacedCands << ", output size " << prod->size() << std::endl;
  }
  iEvent.put(std::move(prod));
}

// ------------ method called once each job just before starting event loop
// ------------
void OniaPFCandidateProducer::beginJob() {}

// ------------ method called once each job just after ending the event loop
// ------------
void OniaPFCandidateProducer::endJob() {}

bool OniaPFCandidateProducer::seld0Cand(const CompositeCandidate d0Cand) {
  if (d0Cand.pt() < 3.)
    return false;
  return true;
}

bool OniaPFCandidateProducer::selOniaCand(const CompositeCandidate oniaCand) {
  if (!oniaCand.hasUserFloat("vProb") || oniaCand.userFloat("vProb") < 0.01)
    return false;
  if (fabs(oniaCand.rapidity()) > 2.4)
    return false;
  const Muon *muon1 = dynamic_cast<const Muon *>(oniaCand.daughter("muon1"));
  const Muon *muon2 = dynamic_cast<const Muon *>(oniaCand.daughter("muon2"));

  if (muon1 == nullptr || muon2 == nullptr)
    return false;
  if (muon1->charge() == muon2->charge())
    return false;
  return true;
}

bool OniaPFCandidateProducer::selMuonCand(const CompositeCandidate jpsiCand,
                                          const char *muonName) {
  const Muon *muon = dynamic_cast<const Muon *>(jpsiCand.daughter(muonName));

  if (muon == nullptr)
    return false;

  if (oniaMuonID_ == "Soft") {
    if (!muon->isGlobalMuon() || !muon->isTrackerMuon() ||
        muon->innerTrack().isNull())
      return false;
    const reco::Vertex *refVertex = nullptr;
    if (!isHI_ && jpsiCand.hasUserData("muonlessPV"))
      refVertex = jpsiCand.userData<reco::Vertex>("muonlessPV");
    if (refVertex == nullptr && jpsiCand.hasUserData("PVwithmuons"))
      refVertex = jpsiCand.userData<reco::Vertex>("PVwithmuons");
    if (refVertex == nullptr)
      return false;
    const math::XYZPoint refVtx = refVertex->position();
    const reco::TrackRef iTrack = muon->innerTrack();
    if (iTrack->hitPattern().trackerLayersWithMeasurement() <= 5 ||
        iTrack->hitPattern().pixelLayersWithMeasurement() <= 0 ||
        fabs(iTrack->dxy(refVtx)) >= 0.3 || fabs(iTrack->dz(refVtx)) >= 20.)
      return false;
  } else if (!muon->passed(reco::Muon::CutBasedIdTight)) {
    return false;
  }

  if (applyMuonAcceptance_) {
    const double absEta = fabs(muon->eta());
    const double pt = muon->pt();
    const bool isMuonInAcc =
        absEta < 2.4 &&
        ((absEta < 1.2 && pt >= 3.5) ||
         (1.2 <= absEta && absEta < 2.1 && pt >= 5.47 - 1.89 * absEta) ||
         (2.1 <= absEta && pt >= 1.5));
    if (!isMuonInAcc)
      return false;
  }

  return true;
}

bool OniaPFCandidateProducer::checkDupTrack(const CompositeCandidate cand1,
                                            const CompositeCandidate cand2) {

  double eps = 0.0001;
  if (fabs(cand1.daughter("track1")->pt() - cand2.daughter("track1")->pt()) <
      eps)
    return true;
  if (fabs(cand1.daughter("track1")->pt() - cand2.daughter("track2")->pt()) <
      eps)
    return true;
  if (fabs(cand1.daughter("track2")->pt() - cand2.daughter("track1")->pt()) <
      eps)
    return true;
  if (fabs(cand1.daughter("track2")->pt() - cand2.daughter("track2")->pt()) <
      eps)
    return true;

  return false;
}

bool OniaPFCandidateProducer::checkDupMuon(const CompositeCandidate cand1,
                                           const CompositeCandidate cand2) {

  double eps = 0.0001;
  if (fabs(cand1.daughter("muon1")->pt() - cand2.daughter("muon1")->pt()) < eps)
    return true;
  if (fabs(cand1.daughter("muon1")->pt() - cand2.daughter("muon2")->pt()) < eps)
    return true;
  if (fabs(cand1.daughter("muon2")->pt() - cand2.daughter("muon1")->pt()) < eps)
    return true;
  if (fabs(cand1.daughter("muon2")->pt() - cand2.daughter("muon2")->pt()) < eps)
    return true;

  return false;
}

// define this as a plug-in
DEFINE_FWK_MODULE(OniaPFCandidateProducer);
