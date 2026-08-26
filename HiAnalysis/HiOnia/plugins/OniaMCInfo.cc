#include "HiAnalysis/HiOnia/interface/HiOniaAnalyzer.h"

reco::GenParticleRef HiOniaAnalyzer::findDaughterRef(reco::GenParticleRef GenParticleDaughter, int GenParticlePDG) {
  reco::GenParticleRef GenParticleTmp = GenParticleDaughter;
  bool foundFirstDaughter = false;

  for (int j = 0; j < 1000; ++j) {
    if (GenParticleTmp.isNonnull() && GenParticleTmp->status() > 0 && GenParticleTmp->status() < 1000 &&
        GenParticleTmp->numberOfDaughters() > 0) {
      if (GenParticleTmp->pdgId() == GenParticlePDG ||
          GenParticleTmp->daughterRef(0)->pdgId() ==
              GenParticlePDG)  //if oscillating B, can take two decays to return to pdgID(B parent)
      {
        GenParticleTmp = GenParticleTmp->daughterRef(0);
      } else if (!foundFirstDaughter)  //if Tmp is not a Bc, it means Tmp is a true daughter
      {
        foundFirstDaughter = true;
        GenParticlePDG = GenParticleTmp->pdgId();
      }
    } else
      break;
  }
  if (GenParticleTmp.isNonnull() && GenParticleTmp->status() > 0 && GenParticleTmp->status() < 1000 &&
      foundFirstDaughter) {  //(GenParticleTmp->pdgId()==GenParticlePDG)) {
    GenParticleDaughter = GenParticleTmp;
  }

  return GenParticleDaughter;
};

void HiOniaAnalyzer::fillGenInfo() {
  if (Gen_QQ_size >= Max_QQ_size) {
    std::cout << "Too many dimuons: " << Gen_QQ_size << std::endl;
    std::cout << "Maximum allowed: " << Max_QQ_size << std::endl;
    return;
  }

  if (Gen_Bc_size >= Max_Bc_size) {
    std::cout << "Too many Bc's: " << Gen_Bc_size << std::endl;
    std::cout << "Maximum allowed: " << Max_Bc_size << std::endl;
    return;
  }

  if (Gen_mu_size >= Max_mu_size) {
    std::cout << "Too many muons: " << Gen_mu_size << std::endl;
    std::cout << "Maximum allowed: " << Max_mu_size << std::endl;
    return;
  }

  if (genInfo.isValid()) {
    if (genInfo->hasBinningValues())
      Gen_pthat = genInfo->binningValues()[0];
    Gen_weight = genInfo->weight();
  }

  if (collGenParticles.isValid()) {
    //Fill the single muons, before the dimuons (important)
    for (std::vector<reco::GenParticle>::const_iterator it = collGenParticles->begin(); it != collGenParticles->end();
         ++it) {
      const reco::GenParticle* gen = &(*it);

      if (abs(gen->pdgId()) == 13 && (gen->status() == 1)) {
        Gen_mu_type[Gen_mu_size] = _isPromptMC ? 0 : 1;  // prompt: 0, non-prompt: 1
        Gen_mu_charge[Gen_mu_size] = gen->charge();

        TLorentzVector vMuon = lorentzMomentum(gen->p4());
        new ((*Gen_mu_4mom)[Gen_mu_size]) TLorentzVector(vMuon);
        Gen_mu_4mom_pt.push_back(vMuon.Pt());
        Gen_mu_4mom_eta.push_back(vMuon.Eta());
        Gen_mu_4mom_phi.push_back(vMuon.Phi());
        Gen_mu_4mom_m.push_back(vMuon.M());

        //Fill map of the muon indices. Use long int keys, to avoid rounding errors on a float key. Implies a precision of 10^-6
        mapGenMuonMomToIndex_[FloatToIntkey(vMuon.Pt())] = Gen_mu_size;

        Gen_mu_size++;
      }
    }

    for (std::vector<reco::GenParticle>::const_iterator it = collGenParticles->begin(); it != collGenParticles->end();
         ++it) {
      const reco::GenParticle* gen = &(*it);

      if (abs(gen->pdgId()) == _oniaPDG && (gen->status() == 2 || (abs(gen->pdgId()) == 23 && gen->status() == 62)) &&
          gen->numberOfDaughters() >= 2) {
        reco::GenParticleRef genMuon1 = findDaughterRef(gen->daughterRef(0), gen->pdgId());
        reco::GenParticleRef genMuon2 = findDaughterRef(gen->daughterRef(1), gen->pdgId());

        if (abs(genMuon1->pdgId()) == 13 && abs(genMuon2->pdgId()) == 13 && (genMuon1->status() == 1) &&
            (genMuon2->status() == 1)) {
          Gen_QQ_Bc_idx[Gen_QQ_size] = -1;
          Gen_QQ_type[Gen_QQ_size] = _isPromptMC ? 0 : 1;  // prompt: 0, non-prompt: 1
          std::pair<std::vector<reco::GenParticleRef>, std::pair<float, float> > MCinfo = findGenMCInfo(gen);
          Gen_QQ_ctau[Gen_QQ_size] = 10.0 * MCinfo.second.first;
          Gen_QQ_ctau3D[Gen_QQ_size] = 10.0 * MCinfo.second.second;

          if (_genealogyInfo) {
            _Gen_QQ_MomAndTrkBro[Gen_QQ_size] = MCinfo.first;
            Gen_QQ_momId[Gen_QQ_size] = _Gen_QQ_MomAndTrkBro[Gen_QQ_size][0]->pdgId();
          }

          TLorentzVector vJpsi = lorentzMomentum(gen->p4());
          new ((*Gen_QQ_4mom)[Gen_QQ_size]) TLorentzVector(vJpsi);
          Gen_QQ_4mom_pt.push_back(vJpsi.Pt());
          Gen_QQ_4mom_eta.push_back(vJpsi.Eta());
          Gen_QQ_4mom_y.push_back(vJpsi.Rapidity());
          Gen_QQ_4mom_phi.push_back(vJpsi.Phi());
          Gen_QQ_4mom_m.push_back(vJpsi.M());

          TLorentzVector vMuon1 = lorentzMomentum(genMuon1->p4());
          TLorentzVector vMuon2 = lorentzMomentum(genMuon2->p4());

          if (genMuon1->charge() > genMuon2->charge()) {
            Gen_QQ_mupl_idx[Gen_QQ_size] = IndexOfThisMuon(&vMuon1, true);
            Gen_QQ_mumi_idx[Gen_QQ_size] = IndexOfThisMuon(&vMuon2, true);
          } else {
            Gen_QQ_mupl_idx[Gen_QQ_size] = IndexOfThisMuon(&vMuon2, true);
            Gen_QQ_mumi_idx[Gen_QQ_size] = IndexOfThisMuon(&vMuon1, true);
          }

          if (_doTrimuons) {
            //GenInfo for the Bc and the daughter muon from the W daughter of the Bc. Beware, this is designed for generated Bc's having QQ as a daughter!!
            std::pair<bool, reco::GenParticleRef> findBcMom = findBcMotherRef(
                findMotherRef(gen->motherRef(), gen->pdgId()), _BcPDG);  //the boolean says if the Bc mother was found

            if (findBcMom.first) {
              if (Gen_QQ_Bc_idx[Gen_QQ_size] > -1) {
                std::cout << "WARNING : Jpsi seems to have more than one Bc mother" << std::endl;
              }

              reco::GenParticleRef genBc = findBcMom.second;
              if (genBc->numberOfDaughters() >= 3) {
                reco::GenParticleRef genDau1 = findDaughterRef(genBc->daughterRef(0), genBc->pdgId());
                reco::GenParticleRef genDau2 = findDaughterRef(genBc->daughterRef(1), genBc->pdgId());
                reco::GenParticleRef genDau3 = findDaughterRef(genBc->daughterRef(2), genBc->pdgId());

                //Which daughter is the mu or nu from the W?
                bool goodDaughters = true;
                const reco::GenParticleRef& gennuW = genDau1;
                const reco::GenParticleRef& genmuW = genDau2;

                if (isNeutrino(genDau1->pdgId()) && (abs(genDau2->pdgId()) == 13)) {
                } else if (isNeutrino(genDau2->pdgId()) && (abs(genDau1->pdgId()) == 13)) {
                } else if (isNeutrino(genDau1->pdgId()) && (abs(genDau3->pdgId()) == 13)) {
                } else if (isNeutrino(genDau3->pdgId()) && (abs(genDau1->pdgId()) == 13)) {
                } else if (isNeutrino(genDau2->pdgId()) && (abs(genDau3->pdgId()) == 13)) {
                } else if (isNeutrino(genDau3->pdgId()) && (abs(genDau2->pdgId()) == 13)) {
                } else {
                  goodDaughters = false;
                }

                //Fill info for Bc and its mu,nu daughters
                if (goodDaughters && (genmuW->charge() == genBc->charge()) && (genmuW->status() == 1)) {
                  Gen_QQ_Bc_idx[Gen_QQ_size] = Gen_Bc_size;
                  Gen_Bc_QQ_idx[Gen_Bc_size] = Gen_QQ_size;

                  Gen_Bc_pdgId[Gen_Bc_size] = genBc->pdgId();
                  std::pair<int, std::pair<float, float> > MCinfo = findGenBcInfo(genBc, gen);
                  Gen_Bc_ctau[Gen_Bc_size] = 10.0 * MCinfo.second.first;

                  TLorentzVector vBc = lorentzMomentum(genBc->p4());
                  new ((*Gen_Bc_4mom)[Gen_Bc_size]) TLorentzVector(vBc);
                  Gen_Bc_4mom_pt.push_back(vBc.Pt());
                  Gen_Bc_4mom_eta.push_back(vBc.Eta());
                  Gen_Bc_4mom_y.push_back(vBc.Rapidity());
                  Gen_Bc_4mom_phi.push_back(vBc.Phi());
                  Gen_Bc_4mom_m.push_back(vBc.M());

                  TLorentzVector vmuW = lorentzMomentum(genmuW->p4());
                  Gen_Bc_muW_idx[Gen_Bc_size] = IndexOfThisMuon(&vmuW, true);

                  TLorentzVector vnuW = lorentzMomentum(gennuW->p4());
                  new ((*Gen_Bc_nuW_4mom)[Gen_Bc_size]) TLorentzVector(vnuW);
                  Gen_Bc_nuW_4mom_pt.push_back(vnuW.Pt());
                  Gen_Bc_nuW_4mom_eta.push_back(vnuW.Eta());
                  Gen_Bc_nuW_4mom_y.push_back(vnuW.Rapidity());
                  Gen_Bc_nuW_4mom_phi.push_back(vnuW.Phi());
                  Gen_Bc_nuW_4mom_m.push_back(vnuW.M());

                  Gen_Bc_size++;
                } else {
                  std::cout << "WARNING : Problem with daughters of the gen Bc, hence Bc and its daughters are not "
                               "written out"
                            << std::endl;
                }
              }
            }
          }
          Gen_QQ_size++;
        }
      }
    }
  }
  return;
};

reco::GenParticleRef HiOniaAnalyzer::findMotherRef(reco::GenParticleRef GenParticleMother, int GenParticlePDG) {
  for (int i = 0; i < 1000; ++i) {
    if (GenParticleMother.isNonnull() && (GenParticleMother->pdgId() == GenParticlePDG) &&
        GenParticleMother->numberOfMothers() > 0) {
      GenParticleMother = GenParticleMother->motherRef();
    } else
      break;
  }
  return GenParticleMother;
};

std::vector<reco::GenParticleRef> HiOniaAnalyzer::GenBrothers(reco::GenParticleRef GenParticleMother, int GenJpsiPDG) {
  bool foundJpsi = false;
  std::vector<reco::GenParticleRef> res;

  if (!GenParticleMother.isNonnull())
    return res;
  //if(Reco_3mu_size>0) cout<<"\nScanning daughters of Jpsi "<<GenJpsiPDG<<" mother, pdg = "<<GenParticleMother->pdgId()<<endl;
  for (int i = 0; i < (int)GenParticleMother->numberOfDaughters(); i++) {
    reco::GenParticleRef dau = findDaughterRef(GenParticleMother->daughterRef(i), GenParticleMother->pdgId());
    for (int l = 0; l < 100; l++) {  //avoid having a daughter of same pdgId
      if (!(dau.isNonnull() && dau->status() > 0 && dau->status() < 1000))
        break;
      if (dau->pdgId() == GenParticleMother->pdgId() && dau->numberOfDaughters() == 1)
        dau = findDaughterRef(dau->daughterRef(0), dau->pdgId());
      else
        break;
    }

    if (!(dau.isNonnull() && dau->status() > 0 && dau->status() < 1000))
      continue;
    //if(Reco_3mu_size>0) cout<<"Daughter #"<<i<<" pdg = "<< dau->pdgId()<<" pt,eta = "<<dau->pt()<<" "<<dau->eta()<<endl;
    if (isChargedTrack(dau->pdgId())) {
      res.push_back(dau);
    }
    if (dau->pdgId() == GenJpsiPDG) {
      foundJpsi = true;  //continue;
    }

    for (int j = 0; j < (int)dau->numberOfDaughters(); j++) {
      reco::GenParticleRef grandDau = findDaughterRef(dau->daughterRef(j), dau->pdgId());
      for (int l = 0; l < 100; l++) {  //avoid having a daughter of same pdgId
        if (!(grandDau.isNonnull() && grandDau->status() > 0 && grandDau->status() < 1000))
          break;
        if (grandDau->pdgId() == dau->pdgId() && grandDau->numberOfDaughters() == 1)
          grandDau = findDaughterRef(grandDau->daughterRef(0), grandDau->pdgId());
        else
          break;
      }

      if (!(grandDau.isNonnull() && grandDau->status() > 0 && grandDau->status() < 1000))
        continue;
      //if(Reco_3mu_size>0) cout<<"    grand-daughter #"<<j<<" pdg = "<< grandDau->pdgId()<<" pt,eta = "<<grandDau->pt()<<" "<<grandDau->eta()<<endl;
      if (isChargedTrack(grandDau->pdgId())) {
        res.push_back(grandDau);
      }
      if (grandDau->pdgId() == GenJpsiPDG) {
        foundJpsi = true;  //continue;
      }

      for (int k = 0; k < (int)grandDau->numberOfDaughters(); k++) {
        reco::GenParticleRef ggrandDau = findDaughterRef(grandDau->daughterRef(k), grandDau->pdgId());
        for (int l = 0; l < 100; l++) {  //avoid having a daughter of same pdgId
          if (!(ggrandDau.isNonnull() && ggrandDau->status() > 0 && ggrandDau->status() < 1000))
            break;
          if (ggrandDau->pdgId() == grandDau->pdgId() && ggrandDau->numberOfDaughters() == 1)
            ggrandDau = findDaughterRef(ggrandDau->daughterRef(0), ggrandDau->pdgId());
          else
            break;
        }

        if (!(ggrandDau.isNonnull() && ggrandDau->status() > 0 && ggrandDau->status() < 1000))
          continue;
        //if(Reco_3mu_size>0) cout<<"        grand-grand-daughter #"<<k<<" pdg = "<< ggrandDau->pdgId()<<" pt,eta = "<<ggrandDau->pt()<<" "<<ggrandDau->eta()<<endl;
        if (isChargedTrack(ggrandDau->pdgId())) {
          res.push_back(ggrandDau);
        }
        if (ggrandDau->pdgId() == GenJpsiPDG) {
          foundJpsi = true;  //continue;
        }
      }
    }
  }

  if (!foundJpsi) {
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!! Incoherence in genealogy: Jpsi not found in the daughters!\n" << endl;
  }
  // if(!isAbHadron(GenParticleMother->pdgId())){
  //   cout<<"\n!!!!!!!!!!!!!!!!!!!!!!!!!!!! Jpsi ancestor is not a b-hadron! pdgID(mother of this ancestor) = "<<findMotherRef(GenParticleMother->motherRef() , GenParticleMother->pdgId())->pdgId()<<endl;
  // }

  return res;
};

std::pair<std::vector<reco::GenParticleRef>, std::pair<float, float> > HiOniaAnalyzer::findGenMCInfo(
    const reco::GenParticle* genJpsi) {
  float trueLife = -99.;
  float trueLife3D = -99.;
  std::vector<reco::GenParticleRef> JpsiBrothers;

  if (genJpsi->numberOfMothers() > 0) {
    TVector3 trueVtx(0.0, 0.0, 0.0);
    TVector3 trueP(0.0, 0.0, 0.0);
    TVector3 trueVtxMom(0.0, 0.0, 0.0);

    trueVtx.SetXYZ(genJpsi->vertex().x(), genJpsi->vertex().y(), genJpsi->vertex().z());
    trueP.SetXYZ(genJpsi->momentum().x(), genJpsi->momentum().y(), genJpsi->momentum().z());

    bool aBhadron = false;
    reco::GenParticleRef Jpsimom_final;
    reco::GenParticleRef Jpsimom = findMotherRef(genJpsi->motherRef(), genJpsi->pdgId());

    if (Jpsimom.isNull()) {
      std::pair<float, float> trueLifePair = std::make_pair(trueLife, trueLife3D);
      std::pair<std::vector<reco::GenParticleRef>, std::pair<float, float> > result =
          std::make_pair(JpsiBrothers, trueLifePair);
      return result;
    } else if (Jpsimom->numberOfMothers() <= 0) {
      if (isAbHadron(Jpsimom->pdgId())) {
        Jpsimom_final = Jpsimom;
        aBhadron = true;
      }
    }

    else {
      reco::GenParticleRef Jpsigrandmom = findMotherRef(Jpsimom->motherRef(), Jpsimom->pdgId());
      if (isAbHadron(Jpsimom->pdgId())) {
        if (Jpsigrandmom.isNonnull() && isAMixedbHadron(Jpsimom->pdgId(), Jpsigrandmom->pdgId())) {
          Jpsimom_final = Jpsigrandmom;
        } else {
          Jpsimom_final = Jpsimom;
        }
        aBhadron = true;
      }

      else if (Jpsigrandmom.isNonnull() && isAbHadron(Jpsigrandmom->pdgId())) {
        if (Jpsigrandmom->numberOfMothers() <= 0) {
          Jpsimom_final = Jpsigrandmom;
        } else {
          reco::GenParticleRef JpsiGrandgrandmom = findMotherRef(Jpsigrandmom->motherRef(), Jpsigrandmom->pdgId());
          if (JpsiGrandgrandmom.isNonnull() && isAMixedbHadron(Jpsigrandmom->pdgId(), JpsiGrandgrandmom->pdgId())) {
            Jpsimom_final = JpsiGrandgrandmom;
          } else {
            Jpsimom_final = Jpsigrandmom;
          }
        }
        aBhadron = true;
      }

      //This is to forcefully find the b-like mother of Jpsi
      else if (Jpsigrandmom.isNonnull() && Jpsigrandmom->numberOfMothers() > 0) {
        reco::GenParticleRef JpsiGrandgrandmom = findMotherRef(Jpsigrandmom->motherRef(), Jpsigrandmom->pdgId());
        if (JpsiGrandgrandmom.isNonnull() && isAbHadron(JpsiGrandgrandmom->pdgId())) {
          Jpsimom_final = JpsiGrandgrandmom;
          aBhadron = true;
        }
      }
    }
    if (!aBhadron) {
      Jpsimom_final = Jpsimom;
    }

    if (Jpsimom_final.isNonnull()) {
      trueVtxMom.SetXYZ(Jpsimom_final->vertex().x(), Jpsimom_final->vertex().y(), Jpsimom_final->vertex().z());
      if (_genealogyInfo && Reco_3mu_size > 0) {
        JpsiBrothers = GenBrothers(Jpsimom_final, genJpsi->pdgId());
      }
      JpsiBrothers.insert(JpsiBrothers.begin(), Jpsimom_final);
    }

    TVector3 vdiff = trueVtx - trueVtxMom;
    trueLife = vdiff.Perp() * JpsiPDGMass / trueP.Perp();
    trueLife3D = vdiff.Mag() * JpsiPDGMass / trueP.Mag();
  }

  std::pair<float, float> trueLifePair = std::make_pair(trueLife, trueLife3D);
  std::pair<std::vector<reco::GenParticleRef>, std::pair<float, float> > result =
      std::make_pair(JpsiBrothers, trueLifePair);
  return result;
};

//Find the indices of the reconstructed J/psi matching each generated J/psi (when the two daughter muons are reconstructed), and vice versa
void HiOniaAnalyzer::fillQQMatchingInfo() {
  for (int igen = 0; igen < Gen_QQ_size; igen++) {
    Gen_QQ_whichRec[igen] = -1;
    int Reco_mupl_idx =
        Gen_mu_whichRec[Gen_QQ_mupl_idx[igen]];  //index of the reconstructed mupl associated to the generated mupl of Jpsi
    int Reco_mumi_idx =
        Gen_mu_whichRec[Gen_QQ_mumi_idx[igen]];  //index of the reconstructed mumi associated to the generated mumi of Jpsi

    if ((Reco_mupl_idx >= 0) && (Reco_mumi_idx >= 0)) {  //Search for Reco_QQ only if both muons are reco
      for (int irec = 0; irec < Reco_QQ_size; irec++) {
        if (((Reco_mupl_idx == Reco_QQ_mupl_idx[irec]) &&
             (Reco_mumi_idx == Reco_QQ_mumi_idx[irec])) ||  //the charges might be wrong in reco
            ((Reco_mupl_idx == Reco_QQ_mumi_idx[irec]) && (Reco_mumi_idx == Reco_QQ_mupl_idx[irec]))) {
          Gen_QQ_whichRec[igen] = irec;
          break;
        }
      }

      if (Gen_QQ_whichRec[igen] == -1)
        Gen_QQ_whichRec[igen] = -2;  //Means the two muons were reconstructed, but the dimuon was not selected
    }
  }

  //Find the index of generated J/psi associated to a reco QQ
  for (int irec = 0; irec < Reco_QQ_size; irec++) {
    Reco_QQ_whichGen[irec] = -1;

    for (int igen = 0; igen < Gen_QQ_size; igen++) {
      if ((Gen_QQ_whichRec[igen] == irec)) {
        Reco_QQ_whichGen[irec] = igen;
        break;
      }
    }
  }
};