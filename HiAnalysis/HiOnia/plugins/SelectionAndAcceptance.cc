#include "HiAnalysis/HiOnia/interface/HiOniaAnalyzer.h"

bool HiOniaAnalyzer::isTrkInMuonAccept(TLorentzVector trk4mom, std::string muonType) {
  if (muonType == (std::string)("GLB")) {
    return (fabs(trk4mom.Eta()) < 2.4 && ((fabs(trk4mom.Eta()) < 1.2 && trk4mom.Pt() >= 3.5) ||
                                          (1.2 <= fabs(trk4mom.Eta()) && fabs(trk4mom.Eta()) < 2.1 &&
                                           trk4mom.Pt() >= 5.47 - 1.89 * fabs(trk4mom.Eta())) ||
                                          (2.1 <= fabs(trk4mom.Eta()) && trk4mom.Pt() >= 1.5)));
  } else if (muonType == (std::string)("TRK") || muonType == (std::string)("TRKSOFT")) {
    return (fabs(trk4mom.Eta()) < 2.4 &&
            ((fabs(trk4mom.Eta()) < 1.1 && trk4mom.Pt() >= 3.3) ||
             (1.1 <= fabs(trk4mom.Eta()) && fabs(trk4mom.Eta()) < 1.3 &&
              trk4mom.Pt() >= 13.2 - 9.0 * fabs(trk4mom.Eta())) ||
             (1.3 <= fabs(trk4mom.Eta()) && trk4mom.Pt() >= 0.8 && trk4mom.Pt() >= 3.02 - 1.17 * fabs(trk4mom.Eta()))));
  } else if (muonType == (std::string)("GLBSOFT")) {
    return (
        fabs(trk4mom.Eta()) < 2.4 &&
        ((fabs(trk4mom.Eta()) < 0.3 && trk4mom.Pt() >= 3.4) ||
         (fabs(trk4mom.Eta()) > 0.3 && fabs(trk4mom.Eta()) < 1.1 && trk4mom.Pt() >= 3.3) ||
         (fabs(trk4mom.Eta()) > 1.1 && fabs(trk4mom.Eta()) < 1.4 && trk4mom.Pt() >= 7.7 - 4.0 * fabs(trk4mom.Eta())) ||
         (fabs(trk4mom.Eta()) > 1.4 && fabs(trk4mom.Eta()) < 1.55 && trk4mom.Pt() >= 2.1) ||
         (fabs(trk4mom.Eta()) > 1.55 && fabs(trk4mom.Eta()) < 2.2 &&
          trk4mom.Pt() >= 4.25 - 1.39 * fabs(trk4mom.Eta())) ||
         (fabs(trk4mom.Eta()) > 2.2 && trk4mom.Pt() >= 1.2)));
  } else
    std::cout << "ERROR: Incorrect Muon Type" << std::endl;

  return false;
};

bool HiOniaAnalyzer::isMuonInAccept(const pat::Muon* aMuon, const std::string muonType) {
  if (muonType == (std::string)("GLB")) {
    return (fabs(aMuon->eta()) < 2.4 && ((fabs(aMuon->eta()) < 1.2 && aMuon->pt() >= 3.5) ||
                                         (1.2 <= fabs(aMuon->eta()) && fabs(aMuon->eta()) < 2.1 &&
                                          aMuon->pt() >= 5.47 - 1.89 * fabs(aMuon->eta())) ||
                                         (2.1 <= fabs(aMuon->eta()) && aMuon->pt() >= 1.5)));
  } else if (muonType == (std::string)("Acceptance2015")) {
    return (fabs(aMuon->eta()) < 2.4 && ((fabs(aMuon->eta()) < 1.2 && aMuon->pt() >= 3.5) ||
                                         (1.2 <= fabs(aMuon->eta()) && fabs(aMuon->eta()) < 2.1 &&
                                          aMuon->pt() >= 5.77 - 1.89 * fabs(aMuon->eta())) ||
                                         (2.1 <= fabs(aMuon->eta()) && aMuon->pt() >= 1.8)));
  } else if (muonType == (std::string)("TRK")) {  //This is actually softer than the "TRKSOFT" acceptance
    return (fabs(aMuon->eta()) < 2.4 && ((fabs(aMuon->eta()) < 0.8 && aMuon->pt() >= 3.3) ||
                                         (0.8 <= fabs(aMuon->eta()) && fabs(aMuon->eta()) < 2. && aMuon->p() >= 2.9) ||
                                         (2. <= fabs(aMuon->eta()) && aMuon->pt() >= 0.8)));
  } else if (muonType == (std::string)("GLBSOFT")) {
    return (
        fabs(aMuon->eta()) < 2.4 &&
        ((fabs(aMuon->eta()) < 0.3 && aMuon->pt() >= 3.4) ||
         (fabs(aMuon->eta()) > 0.3 && fabs(aMuon->eta()) < 1.1 && aMuon->pt() >= 3.3) ||
         (fabs(aMuon->eta()) > 1.1 && fabs(aMuon->eta()) < 1.4 && aMuon->pt() >= 7.7 - 4.0 * fabs(aMuon->eta())) ||
         (fabs(aMuon->eta()) > 1.4 && fabs(aMuon->eta()) < 1.55 && aMuon->pt() >= 2.1) ||
         (fabs(aMuon->eta()) > 1.55 && fabs(aMuon->eta()) < 2.2 && aMuon->pt() >= 4.25 - 1.39 * fabs(aMuon->eta())) ||
         (fabs(aMuon->eta()) > 2.2 && aMuon->pt() >= 1.2)));
  } else if (muonType == (std::string)("TRKSOFT")) {
    return (
        fabs(aMuon->eta()) < 2.4 &&
        ((fabs(aMuon->eta()) < 1.1 && aMuon->pt() >= 3.3) ||
         (1.1 <= fabs(aMuon->eta()) && fabs(aMuon->eta()) < 1.3 && aMuon->pt() >= 13.2 - 9.0 * fabs(aMuon->eta())) ||
         (1.3 <= fabs(aMuon->eta()) && aMuon->pt() >= 0.8 && aMuon->pt() >= 3.02 - 1.17 * fabs(aMuon->eta()))));
  } else
    std::cout << "ERROR: Incorrect Muon Type" << std::endl;

  return false;
};

bool HiOniaAnalyzer::isSoftMuonBase(const pat::Muon* aMuon) {
  return (aMuon->isTrackerMuon() && aMuon->innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5 &&
          aMuon->innerTrack()->hitPattern().pixelLayersWithMeasurement() > 0 &&
          fabs(aMuon->innerTrack()->dxy(RefVtx)) < 0.3 && fabs(aMuon->innerTrack()->dz(RefVtx)) < 20.);
};

bool HiOniaAnalyzer::isHybridSoftMuon(const pat::Muon* aMuon) {
  return (isSoftMuonBase(aMuon) && aMuon->isGlobalMuon());
};

bool HiOniaAnalyzer::selGlobalMuon(const pat::Muon* aMuon) {
  if (!aMuon->isGlobalMuon())
    return false;

  if (_muonSel == (std::string)("GlbTrk") && !aMuon->isTrackerMuon())
    return false;

  if (!_applycuts)
    return true;

  bool isInAcc = isMuonInAccept(aMuon, (std::string)(_SofterSgMuAcceptance ? "GLBSOFT" : "GLB"));
  bool isGood = (_selTightGlobalMuon ? aMuon->passed(reco::Muon::CutBasedIdTight) : isSoftMuonBase(aMuon));

  return (isInAcc && isGood && (!_miniAODcut || PassMiniAODcut(aMuon)));
};

bool HiOniaAnalyzer::selTrackerMuon(const pat::Muon* aMuon) {
  if (!aMuon->isTrackerMuon())
    return false;

  if (!_applycuts)
    return true;

  bool isInAcc = isMuonInAccept(aMuon, (std::string)(_SofterSgMuAcceptance ? "TRKSOFT" : "TRK"));
  bool isGood = isSoftMuonBase(aMuon);

  return (isInAcc && isGood && (!_miniAODcut || PassMiniAODcut(aMuon)));
};

bool HiOniaAnalyzer::selGlobalOrTrackerMuon(const pat::Muon* aMuon) {
  if (!aMuon->isGlobalMuon() && !aMuon->isTrackerMuon())
    return false;

  if (!_applycuts)
    return true;

  bool isInAcc = isMuonInAccept(aMuon, (std::string)(_SofterSgMuAcceptance ? "TRKSOFT" : "TRK"));
  bool isGood = isSoftMuonBase(aMuon);

  return (isInAcc && isGood && (!_miniAODcut || PassMiniAODcut(aMuon)));
};

bool HiOniaAnalyzer::selAllMuon(const pat::Muon* aMuon) { return !_miniAODcut || PassMiniAODcut(aMuon); };

bool HiOniaAnalyzer::selTrk(const reco::TrackRef aTrk) {
  if (!(aTrk->qualityByName("highPurity") && aTrk->ptError() / aTrk->pt() < 0.1))
    return false;

  if (!_applycuts)
    return true;

  bool isInAcc =
      aTrk->pt() > 1.2 &&
      fabs(aTrk->eta()) <
          2.4;  //(aTrk->pt())>0.2 && fabs(aTrk->eta())<2.4 && aTrk->ptError()/aTrk->pt()<0.1 && fabs(aTrk->dxy(RefVtx))<0.35 && fabs(aTrk->dz(RefVtx))<20; //keep margin in dxy and dz, if the RefVtx is not the good one due to muonlessPV

  return (isInAcc);
};

bool HiOniaAnalyzer::isAbHadron(int pdgID) {
  return (abs(pdgID) == 511 || abs(pdgID) == 521 || abs(pdgID) == 531 || abs(pdgID) == 5122 || abs(pdgID) == 541);
};

bool HiOniaAnalyzer::isNeutrino(int pdgID) { return (abs(pdgID) == 14 || abs(pdgID) == 16 || abs(pdgID) == 18); };

bool HiOniaAnalyzer::isChargedTrack(int pdgId) {
  return ((fabs(pdgId) == 211) || (fabs(pdgId) == 321) || (fabs(pdgId) == 2212) || (fabs(pdgId) == 11) ||
          (fabs(pdgId) == 13));
};

bool HiOniaAnalyzer::isAMixedbHadron(int pdgID, int momPdgID) {
  if ((abs(pdgID) == 511 && abs(momPdgID) == 511 && pdgID * momPdgID < 0) ||
      (abs(pdgID) == 531 && abs(momPdgID) == 531 && pdgID * momPdgID < 0))
    return true;
  return false;
};