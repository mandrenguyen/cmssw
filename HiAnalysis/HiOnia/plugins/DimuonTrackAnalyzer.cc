#include "HiAnalysis/HiOnia/interface/HiOniaAnalyzer.h"

void HiOniaAnalyzer::fillTreeDimuTrk(int count) {
  if (Reco_3mu_size >= Max_Bc_size) {
    std::cout << "Too many dimuon+track candidates: " << Reco_3mu_size << std::endl;
    std::cout << "Maximum allowed: " << Max_Bc_size << std::endl;
    return;
  }

  const pat::CompositeCandidate* aBcCand = _thePassedBcCands.at(count);

  if (aBcCand == nullptr) {
    std::cout << "ERROR: 'aBcCand' pointer in fillTreeDimuTrk is NULL ! Return now" << std::endl;
    return;
  } else {
    const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(aBcCand->daughter("muon1"));
    const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(aBcCand->daughter("muon2"));
    const reco::RecoChargedCandidate* trk3 =
        dynamic_cast<const reco::RecoChargedCandidate*>(aBcCand->daughter("track"));

    if (muon1 == nullptr || muon2 == nullptr || trk3 == nullptr) {
      std::cout << "ERROR: 'muon1' or 'muon2' or 'trk3' pointer in fillTreeDimuTrk is NULL ! Return now" << std::endl;
      return;
    } else {
      Reco_3mu_charge[Reco_3mu_size] = muon1->charge() + muon2->charge() + trk3->charge();

      TLorentzVector vMuon1 = lorentzMomentum(muon1->p4());
      TLorentzVector vMuon2 = lorentzMomentum(muon2->p4());
      TLorentzVector vTrk3 = lorentzMomentum(trk3->p4());

      int mu1_idx = IndexOfThisMuon(&vMuon1);
      int mu2_idx = IndexOfThisMuon(&vMuon2);
      int trk3_idx = IndexOfThisTrack(&vTrk3);

      //The dimuon has to pass the Jpsi kinematic cuts
      Reco_3mu_QQ1_idx[Reco_3mu_size] = IndexOfThisJpsi(mu1_idx, mu2_idx);
      if (Reco_3mu_QQ1_idx[Reco_3mu_size] == -1) {
        return;
      }

      TLorentzVector vBc = lorentzMomentum(aBcCand->p4());
      new ((*Reco_3mu_4mom)[Reco_3mu_size]) TLorentzVector(vBc);
      Reco_3mu_4mom_pt.push_back(vBc.Pt());
      Reco_3mu_4mom_eta.push_back(vBc.Eta());
      Reco_3mu_4mom_y.push_back(vBc.Rapidity());
      Reco_3mu_4mom_phi.push_back(vBc.Phi());
      Reco_3mu_4mom_m.push_back(vBc.M());

      Reco_3mu_mumi_idx[Reco_3mu_size] = (Reco_mu_charge[mu1_idx] == -1) ? mu1_idx : mu2_idx;
      Reco_3mu_mupl_idx[Reco_3mu_size] = (Reco_mu_charge[mu1_idx] == -1) ? mu2_idx : mu1_idx;
      Reco_3mu_muW_idx[Reco_3mu_size] =
          trk3_idx;  //let's keep the same variables names as for the trimuon (for internal use...)

      //*********
      //Fill all remaining Bc variables
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
        cout << "HiOniaAnalyzer::fillTreeDimuTrk: no PVfor muon pair stored" << endl;
        return;
      }

      new ((*Reco_3mu_vtx)[Reco_3mu_size]) TVector3(RefVtx.X(), RefVtx.Y(), RefVtx.Z());

      Reco_3mu_muW_dxy[Reco_3mu_size] = trk3->track()->dxy(RefVtx);
      Reco_3mu_muW_dz[Reco_3mu_size] = trk3->track()->dz(RefVtx);

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

        if (_doDimuTrk) {
          if (aBcCand->hasUserFloat("KCppdlPV")) {
            Reco_3mu_KCctau[Reco_3mu_size] = 10.0 * aBcCand->userFloat("KCppdlPV");
          } else {
            Reco_3mu_KCctau[Reco_3mu_size] = -10;
            std::cout << "Warning: User Float KCppdlPV was not found" << std::endl;
          }
          if (aBcCand->hasUserFloat("KCppdlErrPV")) {
            Reco_3mu_KCctauErr[Reco_3mu_size] = 10.0 * aBcCand->userFloat("KCppdlErrPV");
          } else {
            Reco_3mu_KCctauErr[Reco_3mu_size] = -10;
            std::cout << "Warning: User Float KCppdlErrPV was not found" << std::endl;
          }
          if (aBcCand->hasUserFloat("KCppdlPV3D")) {
            Reco_3mu_KCctau3D[Reco_3mu_size] = 10.0 * aBcCand->userFloat("KCppdlPV3D");
          } else {
            Reco_3mu_KCctau3D[Reco_3mu_size] = -10;
            std::cout << "Warning: User Float KCppdlPV3D was not found" << std::endl;
          }
          if (aBcCand->hasUserFloat("KCppdlErrPV3D")) {
            Reco_3mu_KCctauErr3D[Reco_3mu_size] = 10.0 * aBcCand->userFloat("KCppdlErrPV3D");
          } else {
            Reco_3mu_KCctau3D[Reco_3mu_size] = -10;
            std::cout << "Warning: User Float KCppdlErrPV3D was not found" << std::endl;
          }
          if (aBcCand->hasUserFloat("KCcosAlpha")) {
            Reco_3mu_KCcosAlpha[Reco_3mu_size] = aBcCand->userFloat("KCcosAlpha");
          } else {
            Reco_3mu_KCcosAlpha[Reco_3mu_size] = -10;
            std::cout << "Warning: User Float KCcosAlpha was not found" << std::endl;
          }
          if (aBcCand->hasUserFloat("KCcosAlpha3D")) {
            Reco_3mu_KCcosAlpha3D[Reco_3mu_size] = aBcCand->userFloat("KCcosAlpha3D");
          } else {
            Reco_3mu_KCcosAlpha3D[Reco_3mu_size] = -10;
            std::cout << "Warning: User Float KCcosAlpha3D was not found" << std::endl;
          }
        }
      }

      if (aBcCand->hasUserFloat("vProb")) {
        Reco_3mu_VtxProb[Reco_3mu_size] = aBcCand->userFloat("vProb");
      } else {
        Reco_3mu_VtxProb[Reco_3mu_size] = -1;
        std::cout << "Warning: User Float vProb was not found" << std::endl;
      }
      if (aBcCand->hasUserFloat("KinConstrainedVtxProb")) {
        Reco_3mu_KCVtxProb[Reco_3mu_size] = aBcCand->userFloat("KinConstrainedVtxProb");
      } else {
        Reco_3mu_KCVtxProb[Reco_3mu_size] = -10;
        std::cout << "Warning: User Float KinConstrainedVtxProb was not found" << std::endl;
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
        Reco_3mu_NbMuInSameSV[Reco_3mu_size] = MuInSV(vMuon1, vMuon2, vTrk3);
      }

      Reco_3mu_size++;
    }
  }

  return;
};
