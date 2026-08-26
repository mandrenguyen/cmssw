#include "HiAnalysis/HiOnia/interface/HiOniaAnalyzer.h"

void HiOniaAnalyzer::fillTreeBc(int count) {
  if (Reco_3mu_size >= Max_Bc_size) {
    std::cout << "Too many trimuons: " << Reco_3mu_size << std::endl;
    std::cout << "Maximum allowed: " << Max_Bc_size << std::endl;
    return;
  }

  const pat::CompositeCandidate* aBcCand = _thePassedBcCands.at(count);

  if (aBcCand == nullptr) {
    std::cout << "ERROR: 'aBcCand' pointer in fillTreeBc is NULL ! Return now" << std::endl;
    return;
  } else {
    const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(aBcCand->daughter("muon1"));
    const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(aBcCand->daughter("muon2"));
    const pat::Muon* muon3 = dynamic_cast<const pat::Muon*>(aBcCand->daughter("muon3"));

    if (muon1 == nullptr || muon2 == nullptr || muon3 == nullptr) {
      std::cout << "ERROR: 'muon1' or 'muon2' or 'muon3' pointer in fillTreeBc is NULL ! Return now" << std::endl;
      return;
    } else {
      int charge = muon1->charge() + muon2->charge() + muon3->charge();

      TLorentzVector vMuon1 = lorentzMomentum(muon1->p4());
      TLorentzVector vMuon2 = lorentzMomentum(muon2->p4());
      TLorentzVector vMuon3 = lorentzMomentum(muon3->p4());

      int mu1_idx = IndexOfThisMuon(&vMuon1);  //the muon list contains unchanged muons (even in jpsiFlipping case)
      int mu2_idx = IndexOfThisMuon(&vMuon2);
      int mu3_idx = IndexOfThisMuon(&vMuon3);

      int flipJ = (aBcCand->hasUserInt("flipJpsi")) ? aBcCand->userInt("flipJpsi") : 0;

      //One dimuon combination has to pass the Jpsi kinematic cuts
      if (IndexOfThisJpsi(mu1_idx, mu2_idx, flipJ) == -1 && IndexOfThisJpsi(mu2_idx, mu3_idx, flipJ) == -1 &&
          IndexOfThisJpsi(mu1_idx, mu3_idx, flipJ) == -1) {
        return;
      }

      if (_flipJpsiDirection > 0) {  //in case of Jpsi flipping, we know that mu1-mu2 is the chosen Jpsi OS dimuon
        Reco_3mu_QQ1_idx[Reco_3mu_size] = IndexOfThisJpsi(mu1_idx, mu2_idx, flipJ);
        Reco_3mu_muW_idx[Reco_3mu_size] = mu3_idx;
        Reco_3mu_mumi_idx[Reco_3mu_size] = (Reco_mu_charge[mu1_idx] == -1) ? mu1_idx : mu2_idx;
        Reco_3mu_mupl_idx[Reco_3mu_size] = (Reco_mu_charge[mu1_idx] == 1) ? mu1_idx : mu2_idx;

        if (Reco_mu_charge[mu1_idx] ==
            Reco_mu_charge[mu3_idx]) {  //supposing mu1 and mu2 are precedently chosen as opposite-sign
          Reco_3mu_QQ2_idx[Reco_3mu_size] = IndexOfThisJpsi(mu3_idx, mu2_idx, flipJ);
          Reco_3mu_QQss_idx[Reco_3mu_size] = IndexOfThisJpsi(mu3_idx, mu1_idx, flipJ);
        } else {
          Reco_3mu_QQ2_idx[Reco_3mu_size] = IndexOfThisJpsi(mu3_idx, mu1_idx, flipJ);
          Reco_3mu_QQss_idx[Reco_3mu_size] = IndexOfThisJpsi(mu3_idx, mu2_idx, flipJ);
        }
      }  // end jpsi flipping case

      else {
        //If Bc charge is OK, write out the QQ indices for the two opposite-sign pairs
        if (fabs(charge) == 1) {
          int mu_loneCharge = mu1_idx;
          int mu_SameCharge1 = mu2_idx;
          int mu_SameCharge2 = mu3_idx;
          //Look for the muon that has a different charge than the two others
          if (Reco_mu_charge[mu1_idx] == Reco_mu_charge[mu2_idx]) {
            mu_loneCharge = mu3_idx;
            mu_SameCharge2 = mu1_idx;
          } else if (Reco_mu_charge[mu1_idx] == Reco_mu_charge[mu3_idx]) {
            mu_loneCharge = mu2_idx;
            mu_SameCharge1 = mu1_idx;
          }

          //Look for the Jpsi indices corresponding to two muons
          Reco_3mu_QQ1_idx[Reco_3mu_size] = IndexOfThisJpsi(mu_loneCharge, mu_SameCharge1);
          Reco_3mu_QQ2_idx[Reco_3mu_size] = IndexOfThisJpsi(mu_loneCharge, mu_SameCharge2);
          if (Reco_3mu_QQ1_idx[Reco_3mu_size] == -1 && Reco_3mu_QQ2_idx[Reco_3mu_size] == -1)
            return;  //need one OS pair to pass the dimuon selection
          Reco_3mu_QQss_idx[Reco_3mu_size] = IndexOfThisJpsi(mu_SameCharge1, mu_SameCharge2);

          //Split the muon types according to one valid QQ hypothesis among the two OS pairs
          if (Reco_3mu_QQ1_idx[Reco_3mu_size] > -1) {  //The second os pair could also pass
            Reco_3mu_mumi_idx[Reco_3mu_size] = (Reco_mu_charge[mu_loneCharge] == -1) ? mu_loneCharge : mu_SameCharge1;
            Reco_3mu_mupl_idx[Reco_3mu_size] = (Reco_mu_charge[mu_SameCharge1] == 1) ? mu_SameCharge1 : mu_loneCharge;
            Reco_3mu_muW_idx[Reco_3mu_size] = mu_SameCharge2;
          } else {  //Only the second pair is valid
            Reco_3mu_mumi_idx[Reco_3mu_size] = (Reco_mu_charge[mu_loneCharge] == -1) ? mu_loneCharge : mu_SameCharge2;
            Reco_3mu_mupl_idx[Reco_3mu_size] = (Reco_mu_charge[mu_SameCharge2] == 1) ? mu_SameCharge2 : mu_loneCharge;
            Reco_3mu_muW_idx[Reco_3mu_size] = mu_SameCharge1;
          }
          //pointing to the muW corresponding to the 2nd OS pair
          Reco_3mu_muW2_idx[Reco_3mu_size] = mu_SameCharge1;

        }  //end Jpsi attribution for good Bc charge

        //If charge of the Bc is wrong, simpler procedure : random attribution of the three same-sign pairs
        else {
          Reco_3mu_QQ1_idx[Reco_3mu_size] = IndexOfThisJpsi(mu1_idx, mu2_idx);
          Reco_3mu_QQ2_idx[Reco_3mu_size] = IndexOfThisJpsi(mu1_idx, mu3_idx);
          Reco_3mu_QQss_idx[Reco_3mu_size] = IndexOfThisJpsi(mu2_idx, mu3_idx);

          Reco_3mu_mumi_idx[Reco_3mu_size] = mu1_idx;  //Which muon is mumi or mupl is random
          Reco_3mu_mupl_idx[Reco_3mu_size] = mu2_idx;
          Reco_3mu_muW_idx[Reco_3mu_size] = mu3_idx;
          Reco_3mu_muW2_idx[Reco_3mu_size] = mu2_idx;
        }
      }  // end "no Jpsi flipping"

      //*********
      //Fill all remaining Bc variables
      Reco_3mu_charge[Reco_3mu_size] = charge;
      TLorentzVector vBc = lorentzMomentum(aBcCand->p4());
      new ((*Reco_3mu_4mom)[Reco_3mu_size]) TLorentzVector(vBc);
      Reco_3mu_4mom_pt.push_back(vBc.Pt());
      Reco_3mu_4mom_eta.push_back(vBc.Eta());
      Reco_3mu_4mom_y.push_back(vBc.Rapidity());
      Reco_3mu_4mom_phi.push_back(vBc.Phi());
      Reco_3mu_4mom_m.push_back(vBc.M());
      // if(_flipJpsiDirection>0){
      // 	cout<<"Bc mass, old vs new = "<< (vMuon1+vMuon2+vMuon3).M()<<" "<<vBc.M()<<endl;
      // }

      if (!(_isHI) && _muonLessPrimaryVertex && aBcCand->hasUserData("muonlessPV")) {
        RefVtx = (*aBcCand->userData<reco::Vertex>("muonlessPV")).position();
        RefVtx_xError = (*aBcCand->userData<reco::Vertex>("muonlessPV")).xError();
        RefVtx_yError = (*aBcCand->userData<reco::Vertex>("muonlessPV")).yError();
        RefVtx_zError = (*aBcCand->userData<reco::Vertex>("muonlessPV")).zError();
      } else if (aBcCand->hasUserData("PVwithmuons")) {
        RefVtx = (*aBcCand->userData<reco::Vertex>("PVwithmuons")).position();
        RefVtx_xError = (*aBcCand->userData<reco::Vertex>("PVwithmuons")).xError();
        RefVtx_yError = (*aBcCand->userData<reco::Vertex>("PVwithmuons")).yError();
        RefVtx_zError = (*aBcCand->userData<reco::Vertex>("PVwithmuons")).zError();
      } else {
        cout << "HiOniaAnalyzer::fillTreeBc: no PVfor muon pair stored" << endl;
        return;
      }

      new ((*Reco_3mu_vtx)[Reco_3mu_size]) TVector3(RefVtx.X(), RefVtx.Y(), RefVtx.Z());

      reco::Track iTrack_mupl, iTrack_mumi, iTrack_muW, mu1Trk, mu2Trk;
      if (_flipJpsiDirection > 0 && aBcCand->hasUserData("muon1Track") && aBcCand->hasUserData("muon2Track")) {
        mu1Trk = *(aBcCand->userData<reco::Track>("muon1Track"));
        mu2Trk = *(aBcCand->userData<reco::Track>("muon2Track"));
      }

      //Long here but nothing much is happening -- just have to find back the mumi-pl-W indices
      if ((!_theMinimumFlag && _muonLessPrimaryVertex) || (_flipJpsiDirection > 0)) {
        if (_flipJpsiDirection > 0) {
          iTrack_mupl = (muon1->charge() > 0) ? mu1Trk : mu2Trk;
          iTrack_mumi = (muon1->charge() > 0) ? mu2Trk : mu1Trk;
          iTrack_muW = *(muon3->innerTrack());
        }

        else {  //find indices of mumi-pl-W
          if (Reco_3mu_muW_idx[Reco_3mu_size] == mu1_idx) {
            iTrack_muW = *(muon1->innerTrack());
            if (Reco_3mu_mumi_idx[Reco_3mu_size] == mu2_idx) {
              iTrack_mumi = *(muon2->innerTrack());
              iTrack_mupl = *(muon3->innerTrack());
            } else {
              iTrack_mumi = *(muon3->innerTrack());
              iTrack_mupl = *(muon2->innerTrack());
            }
          } else if (Reco_3mu_muW_idx[Reco_3mu_size] == mu2_idx) {
            iTrack_muW = *(muon2->innerTrack());
            if (Reco_3mu_mumi_idx[Reco_3mu_size] == mu1_idx) {
              iTrack_mumi = *(muon1->innerTrack());
              iTrack_mupl = *(muon3->innerTrack());
            } else {
              iTrack_mumi = *(muon3->innerTrack());
              iTrack_mupl = *(muon1->innerTrack());
            }
          } else {
            iTrack_muW = *(muon3->innerTrack());
            if (Reco_3mu_mumi_idx[Reco_3mu_size] == mu2_idx) {
              iTrack_mumi = *(muon2->innerTrack());
              iTrack_mupl = *(muon1->innerTrack());
            } else {
              iTrack_mumi = *(muon1->innerTrack());
              iTrack_mupl = *(muon2->innerTrack());
            }
          }
        }

        Reco_3mu_muW_dxy[Reco_3mu_size] = iTrack_muW.dxy(RefVtx);
        Reco_3mu_muW_dz[Reco_3mu_size] = iTrack_muW.dz(RefVtx);
        Reco_3mu_mumi_dxy[Reco_3mu_size] = iTrack_mumi.dxy(RefVtx);
        Reco_3mu_mumi_dz[Reco_3mu_size] = iTrack_mumi.dz(RefVtx);
        Reco_3mu_mupl_dxy[Reco_3mu_size] = iTrack_mupl.dxy(RefVtx);
        Reco_3mu_mupl_dz[Reco_3mu_size] = iTrack_mupl.dz(RefVtx);
      }

      //*********
      //Lifetime related variables

      if (_useBS) {
        if (aBcCand->hasUserFloat("ppdlBS")) {
          Reco_3mu_ctau[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlBS");
        } else {
          Reco_3mu_ctau[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlBS was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("ppdlErrBS")) {
          Reco_3mu_ctauErr[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlErrBS");
        } else {
          Reco_3mu_ctauErr[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlErrBS was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("ppdlBS3D")) {
          Reco_3mu_ctau3D[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlBS3D");
        } else {
          Reco_3mu_ctau3D[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlBS3D was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("ppdlErrBS3D")) {
          Reco_3mu_ctauErr3D[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlErrBS3D");
        } else {
          Reco_3mu_ctauErr3D[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlErrBS3D was not found" << std::endl;
        }
      } else {
        if (aBcCand->hasUserFloat("ppdlPV")) {
          Reco_3mu_ctau[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlPV");
        } else {
          Reco_3mu_ctau[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlPV was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("ppdlErrPV")) {
          Reco_3mu_ctauErr[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlErrPV");
        } else {
          Reco_3mu_ctauErr[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlErrPV was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("ppdlPV3D")) {
          Reco_3mu_ctau3D[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlPV3D");
        } else {
          Reco_3mu_ctau3D[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlPV3D was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("ppdlErrPV3D")) {
          Reco_3mu_ctauErr3D[Reco_3mu_size] = 10.0 * aBcCand->userFloat("ppdlErrPV3D");
        } else {
          Reco_3mu_ctau3D[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float ppdlErrPV3D was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("cosAlpha")) {
          Reco_3mu_cosAlpha[Reco_3mu_size] = aBcCand->userFloat("cosAlpha");
        } else {
          Reco_3mu_cosAlpha[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float cosAlpha was not found" << std::endl;
        }
        if (aBcCand->hasUserFloat("cosAlpha3D")) {
          Reco_3mu_cosAlpha3D[Reco_3mu_size] = aBcCand->userFloat("cosAlpha3D");
        } else {
          Reco_3mu_cosAlpha3D[Reco_3mu_size] = -10;
          std::cout << "Warning: User Float cosAlpha3D was not found" << std::endl;
        }
      }

      if (aBcCand->hasUserFloat("vProb")) {
        Reco_3mu_VtxProb[Reco_3mu_size] = aBcCand->userFloat("vProb");
      } else {
        Reco_3mu_VtxProb[Reco_3mu_size] = -1;
        std::cout << "Warning: User Float vProb was not found" << std::endl;
      }
      if (aBcCand->hasUserFloat("MassErr")) {
        Reco_3mu_MassErr[Reco_3mu_size] = aBcCand->userFloat("MassErr");
      } else {
        Reco_3mu_MassErr[Reco_3mu_size] = -10;
        std::cout << "Warning: User Float MassErr was not found" << std::endl;
      }

      //Correct the Bc mass for the momentum of the neutrino, transverse to the Bc flight direction
      float Mtrimu = vBc.M();
      float Ptrimu = vBc.P();
      float sinalpha = sin(acos(Reco_3mu_cosAlpha3D[Reco_3mu_size]));
      float PperpTrimu = sinalpha * Ptrimu;
      Reco_3mu_CorrM[Reco_3mu_size] = sqrt(Mtrimu * Mtrimu + PperpTrimu * PperpTrimu) + PperpTrimu;

      if (_useSVfinder && SVs.isValid() && !SVs->empty()) {
        Reco_3mu_NbMuInSameSV[Reco_3mu_size] = MuInSV(vMuon1, vMuon2, vMuon3);
      }

      Reco_3mu_size++;
    }
  }

  return;
};

void HiOniaAnalyzer::makeBcCuts(bool keepWrongSign) {
  math::XYZPoint RefVtx_tmp = RefVtx;

  if (collTrimuon.isValid()) {
    for (std::vector<pat::CompositeCandidate>::const_iterator it = collTrimuon->begin(); it != collTrimuon->end();
         ++it) {
      const pat::CompositeCandidate* cand = &(*it);

      if (cand == nullptr) {
        std::cout << "ERROR: 'cand' pointer in makeBcCuts is NULL ! Return now" << std::endl;
        return;
      } else {
        const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(cand->daughter("muon1"));
        const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(cand->daughter("muon2"));
        const pat::Muon* muon3 = dynamic_cast<const pat::Muon*>(cand->daughter("muon3"));

        if (muon1 == nullptr || muon2 == nullptr || muon3 == nullptr) {
          std::cout << "ERROR: 'muon1' or 'muon2' or 'muon3' pointer in makeBcCuts is NULL ! Return now" << std::endl;
          return;
        } else {
          if (!keepWrongSign && (muon1->charge() + muon2->charge() + muon3->charge() != 1) &&
              (muon1->charge() + muon2->charge() + muon3->charge() != -1))
            continue;

          if (!(_isHI) && _muonLessPrimaryVertex && cand->hasUserData("muonlessPV"))
            RefVtx = (*cand->userData<reco::Vertex>("muonlessPV")).position();
          else if (!_muonLessPrimaryVertex && cand->hasUserData("PVwithmuons"))
            RefVtx = (*cand->userData<reco::Vertex>("PVwithmuons")).position();
          else {
            std::cout << "HiOniaAnalyzer::makeCuts: no PV for muon pair stored ! Go to next candidate." << std::endl;
            continue;
          }

          if (fabs(RefVtx.Z()) > _iConfig.getParameter<double>("maxAbsZ"))
            continue;

          if (fabs(muon1->eta()) >= etaMax || fabs(muon2->eta()) >= etaMax || fabs(muon3->eta()) >= etaMax)
            continue;

          //Pass muon selection?
          if ((_muonSel == (std::string)("GlbOrTrk")) && checkBcCuts(cand,
                                                                     muon1,
                                                                     muon2,
                                                                     muon3,
                                                                     &HiOniaAnalyzer::selGlobalOrTrackerMuon,
                                                                     &HiOniaAnalyzer::selGlobalOrTrackerMuon,
                                                                     &HiOniaAnalyzer::selGlobalOrTrackerMuon)) {
            _thePassedBcCats.push_back(Glb_Glb);
            _thePassedBcCands.push_back(cand);
            if (!_fillSingleMuons) {
              EtaOfWantedMuons.push_back(muon1->eta());
              EtaOfWantedMuons.push_back(muon2->eta());
              EtaOfWantedMuons.push_back(muon3->eta());
            }
          } else if ((_muonSel ==
                      (std::string)(
                          "TwoGlbAmongThree")) &&  //argument functions 2 and 3 have to be the same for good symmetrization
                     checkBcCuts(cand,
                                 muon1,
                                 muon2,
                                 muon3,
                                 &HiOniaAnalyzer::selTrackerMuon,
                                 &HiOniaAnalyzer::selGlobalMuon,
                                 &HiOniaAnalyzer::selGlobalMuon)) {
            _thePassedBcCats.push_back(TwoGlbAmongThree);
            _thePassedBcCands.push_back(cand);
            if (!_fillSingleMuons) {
              EtaOfWantedMuons.push_back(muon1->eta());
              EtaOfWantedMuons.push_back(muon2->eta());
              EtaOfWantedMuons.push_back(muon3->eta());
            }
          } else if ((_muonSel == (std::string)("Glb")) && checkBcCuts(cand,
                                                                       muon1,
                                                                       muon2,
                                                                       muon3,
                                                                       &HiOniaAnalyzer::selGlobalMuon,
                                                                       &HiOniaAnalyzer::selGlobalMuon,
                                                                       &HiOniaAnalyzer::selGlobalMuon)) {
            _thePassedBcCats.push_back(Glb_Glb);
            _thePassedBcCands.push_back(cand);
            if (!_fillSingleMuons) {
              EtaOfWantedMuons.push_back(muon1->eta());
              EtaOfWantedMuons.push_back(muon2->eta());
              EtaOfWantedMuons.push_back(muon3->eta());
            }
          } else if ((_muonSel == (std::string)("GlbTrk")) && checkBcCuts(cand,
                                                                          muon1,
                                                                          muon2,
                                                                          muon3,
                                                                          &HiOniaAnalyzer::selGlobalMuon,
                                                                          &HiOniaAnalyzer::selGlobalMuon,
                                                                          &HiOniaAnalyzer::selGlobalMuon)) {
            _thePassedBcCats.push_back(GlbTrk_GlbTrk);
            _thePassedBcCands.push_back(cand);
            if (!_fillSingleMuons) {
              EtaOfWantedMuons.push_back(muon1->eta());
              EtaOfWantedMuons.push_back(muon2->eta());
              EtaOfWantedMuons.push_back(muon3->eta());
            }
          } else if ((_muonSel == (std::string)("Trk")) && checkBcCuts(cand,
                                                                       muon1,
                                                                       muon2,
                                                                       muon3,
                                                                       &HiOniaAnalyzer::selTrackerMuon,
                                                                       &HiOniaAnalyzer::selTrackerMuon,
                                                                       &HiOniaAnalyzer::selTrackerMuon)) {
            _thePassedBcCats.push_back(Trk_Trk);
            _thePassedBcCands.push_back(cand);
            if (!_fillSingleMuons) {
              EtaOfWantedMuons.push_back(muon1->eta());
              EtaOfWantedMuons.push_back(muon2->eta());
              EtaOfWantedMuons.push_back(muon3->eta());
            }
          } else if ((_muonSel == (std::string)("All")) && checkBcCuts(cand,
                                                                       muon1,
                                                                       muon2,
                                                                       muon3,
                                                                       &HiOniaAnalyzer::selAllMuon,
                                                                       &HiOniaAnalyzer::selAllMuon,
                                                                       &HiOniaAnalyzer::selAllMuon)) {
            _thePassedBcCats.push_back(All_All);
            _thePassedBcCands.push_back(cand);
            if (!_fillSingleMuons) {
              EtaOfWantedMuons.push_back(muon1->eta());
              EtaOfWantedMuons.push_back(muon2->eta());
              EtaOfWantedMuons.push_back(muon3->eta());
            }
          } else {
            //std::cout << "[HiOniaAnalyzer::makeCuts] trimuon --- muon did not pass selection: " << _muonSel << std::endl;
          }
        }
      }
    }
  }

  RefVtx = RefVtx_tmp;
  return;
};

std::pair<bool, reco::GenParticleRef> HiOniaAnalyzer::findBcMotherRef(reco::GenParticleRef GenParticleMother,
                                                                      int BcPDG) {
  bool FoundBc = false;
  for (int i = 0; i < 1000; ++i) {
    if ((fabs(GenParticleMother->pdgId()) == BcPDG) && (GenParticleMother->status() == 2) &&
        (GenParticleMother->numberOfDaughters() >= 2)) {
      FoundBc = true;
      break;
    } else if (GenParticleMother.isNonnull() && GenParticleMother->numberOfMothers() > 0) {
      GenParticleMother = GenParticleMother->motherRef();
    } else {
      break;
    }
  }

  std::pair<bool, reco::GenParticleRef> res;
  res.first = FoundBc;
  res.second = GenParticleMother;
  return res;
};

//Build the visible Bc when the three daugther muons are reconstructed. Record indices between Gen and Rec
void HiOniaAnalyzer::fillBcMatchingInfo() {
  for (int igen = 0; igen < Gen_Bc_size; igen++) {
    Gen_3mu_whichRec[igen] = -1;

    //Build the visible Gen Bc (sum of lorentzvectors of the three gen muons)
    TLorentzVector gen_3mu_4mom = *((TLorentzVector*)Gen_QQ_4mom->ConstructedAt(Gen_Bc_QQ_idx[igen])) +
                                  *((TLorentzVector*)Gen_mu_4mom->ConstructedAt(Gen_Bc_muW_idx[igen]));
    new ((*Gen_3mu_4mom)[igen]) TLorentzVector(gen_3mu_4mom);
    Gen_3mu_4mom_pt.push_back(gen_3mu_4mom.Pt());
    Gen_3mu_4mom_eta.push_back(gen_3mu_4mom.Eta());
    Gen_3mu_4mom_y.push_back(gen_3mu_4mom.Rapidity());
    Gen_3mu_4mom_phi.push_back(gen_3mu_4mom.Phi());
    Gen_3mu_4mom_m.push_back(gen_3mu_4mom.M());

    //Look for index of reconstructed muon
    int Reco_muW_idx =
        Gen_mu_whichRec[Gen_Bc_muW_idx[igen]];  //index of the reconstructed muW associated to the generated muW of Bc
    int Reco_mupl_idx = Gen_mu_whichRec
        [Gen_QQ_mupl_idx[Gen_Bc_QQ_idx[igen]]];  //index of the reconstructed mupl associated to the generated mupl of Jpsi
    int Reco_mumi_idx = Gen_mu_whichRec
        [Gen_QQ_mumi_idx[Gen_Bc_QQ_idx[igen]]];  //index of the reconstructed mumi associated to the generated mumi of Jpsi

    if ((Reco_mupl_idx >= 0) && (Reco_mumi_idx >= 0) &&
        (Reco_muW_idx >= 0)) {  //Search for Reco_Bc only if the three muons are reco

      for (int irec = 0; irec < Reco_3mu_size; irec++) {
        if (((Reco_mupl_idx == Reco_3mu_mupl_idx[irec]) && (Reco_mumi_idx == Reco_3mu_mumi_idx[irec]) &&
             (Reco_muW_idx == Reco_3mu_muW_idx[irec])) ||  //the charges might be wrong in reco
            ((Reco_mupl_idx == Reco_3mu_mupl_idx[irec]) && (Reco_mumi_idx == Reco_3mu_muW_idx[irec]) &&
             (Reco_muW_idx == Reco_3mu_mumi_idx[irec])) ||
            ((Reco_mupl_idx == Reco_3mu_muW_idx[irec]) && (Reco_mumi_idx == Reco_3mu_mumi_idx[irec]) &&
             (Reco_muW_idx == Reco_3mu_mupl_idx[irec])) ||
            ((Reco_mupl_idx == Reco_3mu_muW_idx[irec]) && (Reco_mumi_idx == Reco_3mu_mupl_idx[irec]) &&
             (Reco_muW_idx == Reco_3mu_mumi_idx[irec])) ||
            ((Reco_mupl_idx == Reco_3mu_mumi_idx[irec]) && (Reco_mumi_idx == Reco_3mu_muW_idx[irec]) &&
             (Reco_muW_idx == Reco_3mu_mupl_idx[irec])) ||
            ((Reco_mupl_idx == Reco_3mu_mumi_idx[irec]) && (Reco_mumi_idx == Reco_3mu_mupl_idx[irec]) &&
             (Reco_muW_idx == Reco_3mu_muW_idx[irec]))) {
          Gen_3mu_whichRec[igen] = irec;
          break;
        }
      }

      if ((Gen_3mu_whichRec[igen] == -1)) {
        Gen_3mu_whichRec[igen] = -2;  //The three muons were reconstructed, but the associated trimuon was not selected
      }
    }
  }

  for (int irec = 0; irec < Reco_3mu_size; irec++) {
    //Find the index of generated Bc associated to a reco trimuon
    Reco_3mu_whichGen[irec] = -1;
    for (int igen = 0; igen < Gen_Bc_size; igen++) {
      if ((Gen_3mu_whichRec[igen] == irec)) {
        Reco_3mu_whichGen[irec] = igen;
        break;
      }
    }

    //Match the muW to a gen particle, and find out if it comes from the B meson hard process
    if (_genealogyInfo) {
      //cout<<"Checking genealogy for Reco_3mu #"<<irec<<endl;
      //cout<<"Reco_muW simExtType,simPdgId,simMotherPdgId = "<<Reco_mu_simExtType[Reco_3mu_muW_idx[irec]]<<" "<<Reco_mu_simPdgId[Reco_3mu_muW_idx[irec]]<<" "<<Reco_mu_simMotherPdgId[Reco_3mu_muW_idx[irec]]<<endl;
      Reco_3mu_muW_isGenJpsiBro[irec] = false;
      Reco_3mu_muW_trueId[irec] = 0;

      int goodRecQQidx = (Reco_3mu_QQ1_idx[irec] > -1) ? (Reco_3mu_QQ1_idx[irec]) : (Reco_3mu_QQ2_idx[irec]);
      if (Reco_3mu_QQ1_idx[irec] > -1 &&
          Reco_3mu_QQ2_idx[irec] >
              -1) {  //If both OS pairs are reconstructed, choose the reco QQ that is matched to a gen
        goodRecQQidx =
            (Reco_QQ_whichGen[Reco_3mu_QQ1_idx[irec]] > -1) ? (Reco_3mu_QQ1_idx[irec]) : (Reco_3mu_QQ2_idx[irec]);
      }
      int genQQidx = Reco_QQ_whichGen[goodRecQQidx];
      int newmuWidx = (goodRecQQidx == Reco_3mu_QQ1_idx[irec])
                          ? (Reco_3mu_muW_idx[irec])
                          : (Reco_3mu_muW2_idx[irec]);  //use the muW_idx corresponding to the chosen QQ dimuon
      int genMuWidx = Reco_mu_whichGen[newmuWidx];

      //If the muon is fake, match it to whatever generated particles
      if (genMuWidx == -1) {
        TLorentzVector* recmuW = (TLorentzVector*)Reco_mu_4mom->ConstructedAt(newmuWidx);
        bool SureDecayInFlight = (fabs(Reco_mu_simExtType[newmuWidx]) == 4);
        bool Unmatched = (fabs(Reco_mu_simExtType[newmuWidx]) == 0);
        float dRmax = SureDecayInFlight ? 0.3 : (Unmatched ? 0.15 : 0.1);
        float dRmin = dRmax;
        float matchedPhi = 5;

        //Loop over gen particles
        //cout<<"Looking for a (charged track) gen particle matching the fake Reco_muW of pt,eta = "<<recmuW->Pt()<<" "<<recmuW->Eta()<<endl;
        for (std::vector<reco::GenParticle>::const_iterator it = collGenParticles->begin();
             it != collGenParticles->end();
             ++it) {
          const reco::GenParticle* gen = &(*it);

          if (isChargedTrack(gen->pdgId())  //&& gen->status() == 1
          ) {
            TLorentzVector genP = lorentzMomentum(gen->p4());
            float dR = recmuW->DeltaR(genP);
            if (dR < dRmin &&
                (!Unmatched ||
                 (genP.Pt() >
                  0.6 *
                      recmuW->Pt()))  // if unmatched, probably a ghost, so at least have a decent Pt agreement (leaving possibility for energy loss of a decayInFlight) for the particle causing the ghost
            ) {
              dRmin = dR;
              Reco_3mu_muW_trueId[irec] = gen->pdgId();
              matchedPhi = genP.Phi();
              //cout<<"   ........Found matched gen particle for fake reco muon: ID, pt, eta, deltaR = "<<Reco_3mu_muW_trueId[irec]<<" "<<genP.Pt()<<" "<<genP.Eta()<<" "<<dRmin<<endl;
            }
          }
        }
        //end loop over gen particles

        //If the Jpsi is true, check if the matched gen particle was part of the B-parent process
        if (genQQidx > -1) {
          for (auto&& bro : _Gen_QQ_MomAndTrkBro[genQQidx]) {
            if (fabs(bro->phi() - matchedPhi) < 1e-6 && isChargedTrack(bro->pdgId())) {
              Reco_3mu_muW_isGenJpsiBro[irec] = true;
              //cout<<"   !!!!!!!!!!!!    FAKE muW is from B parent"<<endl;
              break;
            }
          }
        }
      }

      //If the muon is true, take the associated gen muon, and find if it comes from the B parent of Jpsi
      if (genMuWidx > -1) {
        Reco_3mu_muW_trueId[irec] = (Gen_mu_charge[genMuWidx] == -1) ? 13 : (-13);

        if (genQQidx > -1) {
          for (auto&& bro : _Gen_QQ_MomAndTrkBro[genQQidx]) {
            if (fabs(bro->pdgId()) == 13 &&
                fabs(bro->phi() - ((TLorentzVector*)Gen_mu_4mom->ConstructedAt(genMuWidx))->Phi()) < 1e-6) {
              Reco_3mu_muW_isGenJpsiBro[irec] = true;
              break;
            }
          }
        } else {
        }
      }
    }
  }
};

std::pair<int, std::pair<float, float> > HiOniaAnalyzer::findGenBcInfo(reco::GenParticleRef genBc,
                                                                       const reco::GenParticle* genJpsi) {
  int momBcID = 0;
  float trueLife = -99.;

  TVector3 trueVtx(0.0, 0.0, 0.0);
  TVector3 trueP(0.0, 0.0, 0.0);
  TVector3 trueVtxMom(0.0, 0.0, 0.0);

  trueVtx.SetXYZ(genJpsi->vertex().x(), genJpsi->vertex().y(), genJpsi->vertex().z());
  trueVtxMom.SetXYZ(genBc->vertex().x(), genBc->vertex().y(), genBc->vertex().z());
  trueP.SetXYZ(genBc->momentum().x(), genBc->momentum().y(), genBc->momentum().z());

  TVector3 vdiff = trueVtx - trueVtxMom;
  trueLife = vdiff.Perp() * BcPDGMass / trueP.Perp();

  std::pair<float, float> trueLifePair = std::make_pair(trueLife, trueLife);
  std::pair<int, std::pair<float, float> > result = std::make_pair(momBcID, trueLifePair);
  return result;
};