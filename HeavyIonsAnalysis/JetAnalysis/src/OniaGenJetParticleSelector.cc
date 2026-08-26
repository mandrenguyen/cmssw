#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <unordered_set>
#include <vector>

namespace {
  bool hasDescendantWithId(const reco::Candidate* particle, int pdgId) {
    for (unsigned int i = 0; i < particle->numberOfDaughters(); ++i) {
      const auto* daughter = particle->daughter(i);
      if (daughter->pdgId() == pdgId || hasDescendantWithId(daughter, pdgId))
        return true;
    }
    return false;
  }

  bool hasDaughterWithAbsId(const reco::Candidate* particle, int absPdgId) {
    for (unsigned int i = 0; i < particle->numberOfDaughters(); ++i) {
      if (std::abs(particle->daughter(i)->pdgId()) == absPdgId)
        return true;
    }
    return false;
  }

  bool descendsFrom(const reco::Candidate* particle,
                    const std::unordered_set<const reco::Candidate*>& ancestors,
                    std::unordered_set<const reco::Candidate*>& visited) {
    if (!visited.insert(particle).second)
      return false;
    for (unsigned int i = 0; i < particle->numberOfMothers(); ++i) {
      const auto* mother = particle->mother(i);
      if (ancestors.count(mother) || descendsFrom(mother, ancestors, visited))
        return true;
    }
    return false;
  }
}  // namespace

class OniaGenJetParticleSelector : public edm::global::EDProducer<> {
public:
  explicit OniaGenJetParticleSelector(const edm::ParameterSet& config)
      : srcToken_(consumes<reco::CandidateView>(config.getParameter<edm::InputTag>("src"))),
        resonancePdgIds_(config.getParameter<std::vector<int>>("resonancePdgIds")),
        ignoredPdgIds_(config.getParameter<std::vector<int>>("ignoreParticleIDs")) {
    for (auto& id : resonancePdgIds_)
      id = std::abs(id);
    for (auto& id : ignoredPdgIds_)
      id = std::abs(id);
    produces<reco::CandidatePtrVector>();
  }

  void produce(edm::StreamID, edm::Event& event, const edm::EventSetup&) const override {
    edm::Handle<reco::CandidateView> particles;
    event.getByToken(srcToken_, particles);

    std::unordered_set<const reco::Candidate*> selectedOnia;
    std::vector<size_t> selectedOniaIndices;
    for (size_t i = 0; i < particles->size(); ++i) {
      const auto* particle = &particles->at(i);
      const int absId = std::abs(particle->pdgId());
      if (std::find(resonancePdgIds_.begin(), resonancePdgIds_.end(), absId) == resonancePdgIds_.end())
        continue;
      // Keep the last copy that actually leads to mu+mu-.  This avoids adding
      // duplicate generator-history copies of the same resonance.
      if (hasDaughterWithAbsId(particle, absId))
        continue;
      if (!hasDescendantWithId(particle, 13) || !hasDescendantWithId(particle, -13))
        continue;
      selectedOnia.insert(particle);
      selectedOniaIndices.push_back(i);
    }

    auto output = std::make_unique<reco::CandidatePtrVector>();
    unsigned int removedStableDescendants = 0;
    for (size_t i = 0; i < particles->size(); ++i) {
      const auto* particle = &particles->at(i);
      if (particle->status() != 1)
        continue;
      if (std::find(ignoredPdgIds_.begin(), ignoredPdgIds_.end(), std::abs(particle->pdgId())) !=
          ignoredPdgIds_.end())
        continue;
      std::unordered_set<const reco::Candidate*> visited;
      if (!descendsFrom(particle, selectedOnia, visited))
        output->push_back(particles->ptrAt(i));
      else
        ++removedStableDescendants;
    }
    for (const auto index : selectedOniaIndices)
      output->push_back(particles->ptrAt(index));

    if (!selectedOniaIndices.empty())
      edm::LogInfo("OniaGenJetParticleSelector")
          << "Replaced " << removedStableDescendants << " stable decay products with "
          << selectedOniaIndices.size() << " generated onia candidate(s).";

    event.put(std::move(output));
  }

private:
  edm::EDGetTokenT<reco::CandidateView> srcToken_;
  std::vector<int> resonancePdgIds_;
  std::vector<int> ignoredPdgIds_;
};

DEFINE_FWK_MODULE(OniaGenJetParticleSelector);
