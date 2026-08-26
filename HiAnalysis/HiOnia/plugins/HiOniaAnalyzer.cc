#include "HiAnalysis/HiOnia/interface/HiOniaAnalyzer.h"

HiOniaAnalyzer::HiOniaAnalyzer(const edm::ParameterSet& iConfig)
    : _patMuonToken(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("srcMuon"))),
      _patMuonNoTrigToken(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("srcMuonNoTrig"))),
      _patJpsiToken(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter<edm::InputTag>("srcDimuon"))),
      _patTrimuonToken(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter<edm::InputTag>(
          "srcTrimuon"))),  //the names of userData are the same as for dimuons, but with 'trimuon' product instance name. Ignored if the collection does not exist
      _patDimuTrkToken(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter<edm::InputTag>(
          "srcDimuTrk"))),  //the names of userData are the same as for dimuons, but with 'dimutrk' product instance name. Ignored if the collection does not exist
      _recoTracksToken(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("srcTracks"))),
      _genParticleToken(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticles"))),
      _genInfoToken(consumes<GenEventInfoProduct>(edm::InputTag("generator"))),
      _thePVsToken(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("primaryVertexTag"))),
      _SVToken(
          consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("srcSV"))),  //consumes<edm::View<VTX>>
      _tagTriggerResultsToken(
          consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggerResultsLabel"))),
      _centralityTagToken(consumes<reco::Centrality>(iConfig.getParameter<edm::InputTag>("CentralitySrc"))),
      _centralityBinTagToken(consumes<int>(iConfig.getParameter<edm::InputTag>("CentralityBinSrc"))),
      _evtPlaneTagToken(consumes<reco::EvtPlaneCollection>(iConfig.getParameter<edm::InputTag>("EvtPlane"))),
      _histfilename(iConfig.getParameter<std::string>("histFileName")),
      _datasetname(iConfig.getParameter<std::string>("dataSetName")),
      _mom4format(iConfig.getParameter<std::string>("mom4format")),
      _muonSel(iConfig.getParameter<std::string>("muonSel")),
      _centralityranges(iConfig.getParameter<std::vector<double> >("centralityRanges")),
      _ptbinranges(iConfig.getParameter<std::vector<double> >("pTBinRanges")),
      _etabinranges(iConfig.getParameter<std::vector<double> >("etaBinRanges")),
      _dblTriggerPathNames(iConfig.getParameter<std::vector<string> >("dblTriggerPathNames")),
      _sglTriggerPathNames(iConfig.getParameter<std::vector<string> >("sglTriggerPathNames")),
      _onlythebest(iConfig.getParameter<bool>("onlyTheBest")),
      _applycuts(iConfig.getParameter<bool>("applyCuts")),
      _SofterSgMuAcceptance(iConfig.getParameter<bool>("SofterSgMuAcceptance")),
      _SumETvariables(iConfig.getParameter<bool>("SumETvariables")),
      _selTightGlobalMuon(iConfig.getParameter<bool>("selTightGlobalMuon")),
      _muonLessPrimaryVertex(iConfig.getParameter<bool>("muonLessPV")),
      _useSVfinder(iConfig.getParameter<bool>("useSVfinder")),
      _useBS(iConfig.getParameter<bool>("useBeamSpot")),
      _useRapidity(iConfig.getParameter<bool>("useRapidity")),
      _removeSignal(iConfig.getUntrackedParameter<bool>("removeSignalEvents", false)),
      _removeMuons(iConfig.getUntrackedParameter<bool>("removeTrueMuons", false)),
      _storeSs(iConfig.getParameter<bool>("storeSameSign")),
      _AtLeastOneCand(iConfig.getParameter<bool>("AtLeastOneCand")),
      _combineCategories(iConfig.getParameter<bool>("combineCategories")),
      _fillRooDataSet(iConfig.getParameter<bool>("fillRooDataSet")),
      _fillTree(iConfig.getParameter<bool>("fillTree")),
      _fillHistos(iConfig.getParameter<bool>("fillHistos")),
      _theMinimumFlag(iConfig.getParameter<bool>("minimumFlag")),
      _fillSingleMuons(iConfig.getParameter<bool>("fillSingleMuons")),
      _onlySingleMuons(iConfig.getParameter<bool>("onlySingleMuons")),
      _fillRecoTracks(iConfig.getParameter<bool>("fillRecoTracks")),
      _isHI(iConfig.getUntrackedParameter<bool>("isHI", false)),
      _isPA(iConfig.getUntrackedParameter<bool>("isPA", true)),
      _isMC(iConfig.getUntrackedParameter<bool>("isMC", false)),
      _isPromptMC(iConfig.getUntrackedParameter<bool>("isPromptMC", true)),
      _useEvtPlane(iConfig.getUntrackedParameter<bool>("useEvtPlane", false)),
      _useGeTracks(iConfig.getUntrackedParameter<bool>("useGeTracks", false)),
      _doTrimuons(iConfig.getParameter<bool>("doTrimuons")),
      _doDimuTrk(iConfig.getParameter<bool>("DimuonTrk")),
      _flipJpsiDirection(iConfig.getParameter<int>("flipJpsiDirection")),
      _genealogyInfo(iConfig.getParameter<bool>("genealogyInfo")),
      _miniAODcut(iConfig.getParameter<bool>("miniAODcut")),
      _oniaPDG(iConfig.getParameter<int>("oniaPDG")),
      _BcPDG(iConfig.getParameter<int>("BcPDG")),
      _OneMatchedHLTMu(iConfig.getParameter<int>("OneMatchedHLTMu")),
      _checkTrigNames(iConfig.getParameter<bool>("checkTrigNames")),
      _genOnly(iConfig.getParameter<bool>("genOnly")),
      hltPrescaleProvider(iConfig, consumesCollector(), *this),
      _iConfig(iConfig) {
  usesResource(TFileService::kSharedResource);

  if (_doTrimuons && _doDimuTrk) {
    cout << "FATAL ERROR: _doTrimuons and _doDimuTrk cannot be both true! Code not designed to do both at a time; "
            "Return now."
         << endl;
    return;
  }
  if (_doDimuTrk) {
    if (!_useGeTracks) {
      cout << "Have to use generalTracks if doDimuonTrk==true. _useGeTracks = true is forced." << endl;
      _useGeTracks = true;
    }
    if (!_fillRecoTracks) {
      cout << "Have to use generalTracks if doDimuonTrk==true. _fillRecoTracks = true is forced." << endl;
      _fillRecoTracks = true;
    }
  }

  //now do whatever initialization is needed
  nEvents = 0;
  passedCandidates = 0;

  theRegions.push_back("All");
  theRegions.push_back("Barrel");
  theRegions.push_back("EndCap");

  std::stringstream centLabel;
  for (unsigned int iCent = 0; iCent < _centralityranges.size(); ++iCent) {
    if (iCent == 0)
      centLabel << "00" << _centralityranges.at(iCent);
    else
      centLabel << _centralityranges.at(iCent - 1) << _centralityranges.at(iCent);

    theCentralities.push_back(centLabel.str());
    centLabel.str("");
  }
  theCentralities.push_back("MinBias");

  theSign.push_back("pm");
  if (_storeSs) {
    theSign.push_back("pp");
    theSign.push_back("mm");
  }

  NTRIGGERS_DBL = _dblTriggerPathNames.size();
  NTRIGGERS = NTRIGGERS_DBL + _sglTriggerPathNames.size() + 1;  // + 1 for "NoTrigger"
  std::cout << "NTRIGGERS_DBL = " << NTRIGGERS_DBL << "\t NTRIGGERS_SGL = " << _sglTriggerPathNames.size()
            << "\t NTRIGGERS = " << NTRIGGERS << std::endl;
  nTrig = NTRIGGERS - 1;

  isTriggerMatched[0] = true;  // first entry 'hardcoded' true to accept "all" events
  theTriggerNames.push_back("NoTrigger");

  for (unsigned int iTr = 1; iTr < NTRIGGERS; ++iTr) {
    isTriggerMatched[iTr] = false;

    if (iTr <= NTRIGGERS_DBL) {
      theTriggerNames.push_back(_dblTriggerPathNames.at(iTr - 1));
    } else {
      theTriggerNames.push_back(_sglTriggerPathNames.at(iTr - NTRIGGERS_DBL - 1));
    }
    std::cout << " Trigger " << iTr << "\t" << theTriggerNames[iTr] << std::endl;
  }

  if (_OneMatchedHLTMu >= (int)NTRIGGERS) {
    std::cout
        << "WARNING: the _OneMatchedHLTMu parameter is asking for a wrong trigger number. No matching will be done."
        << std::endl;
    _OneMatchedHLTMu = -1;
  }
  if (_OneMatchedHLTMu > -1)
    std::cout << " Will keep only dimuons (trimuons) that have one (two) daughters matched to "
              << theTriggerNames[_OneMatchedHLTMu] << " filter." << std::endl;

  etaMax = 2.5;

  JpsiMassMin = 2.6;
  JpsiMassMax = 3.5;

  JpsiPtMin = _ptbinranges[0];
  //std::cout << "Pt min = " << JpsiPtMin << std::endl;
  JpsiPtMax = _ptbinranges[_ptbinranges.size() - 1];
  //std::cout << "Pt max = " << JpsiPtMax << std::endl;

  JpsiRapMin = _etabinranges[0];
  //std::cout << "Rap min = " << JpsiRapMin << std::endl;
  JpsiRapMax = _etabinranges[_etabinranges.size() - 1];
  //std::cout << "Rap max = " << JpsiRapMax << std::endl;

  for (std::vector<std::string>::iterator it = theTriggerNames.begin(); it != theTriggerNames.end(); ++it) {
    mapTriggerNameToIntFired_[*it] = -9999;
    mapTriggerNameToPrescaleFac_[*it] = -1;
  }
};

HiOniaAnalyzer::~HiOniaAnalyzer() {
  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)
  Reco_mu_4mom->Delete();
  Reco_mu_L1_4mom->Delete();
  Reco_QQ_4mom->Delete();
  Reco_QQ_mumi_4mom->Delete();
  Reco_QQ_mupl_4mom->Delete();
  Reco_QQ_vtx->Delete();
  if (_useGeTracks && _fillRecoTracks) {
    Reco_trk_4mom->Delete();
    Reco_trk_vtx->Delete();
  }
  if (_doTrimuons || _doDimuTrk) {
    Reco_3mu_4mom->Delete();
    Reco_3mu_vtx->Delete();
    if (_isMC) {
      Gen_Bc_4mom->Delete();
      Gen_Bc_nuW_4mom->Delete();
      Gen_3mu_4mom->Delete();
    }
  }
  if (_isMC) {
    Gen_mu_4mom->Delete();
    Gen_QQ_4mom->Delete();
  }
};

void HiOniaAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  //   using namespace edm;
  InitEvent();
  nEvents++;
  hStats->Fill(BIN_nEvents);

  runNb = iEvent.id().run();
  eventNb = iEvent.id().event();
  lumiSection = iEvent.luminosityBlock();


  edm::Handle<reco::VertexCollection> privtxs;
  iEvent.getByToken(_thePVsToken, privtxs);
  reco::VertexCollection::const_iterator privtx;

  edm::Handle<reco::Centrality> centrality;
  edm::Handle<int> cbin_;

  if(_genOnly) goto genOnly;


  if (privtxs.isValid()) {
    nPV = privtxs->size();

    if (privtxs->begin() != privtxs->end()) {
      privtx = privtxs->begin();
      RefVtx = privtx->position();
      RefVtx_xError = privtx->xError();
      RefVtx_yError = privtx->yError();
      RefVtx_zError = privtx->zError();
    } else {
      RefVtx.SetXYZ(0., 0., 0.);
      RefVtx_xError = 0.0;
      RefVtx_yError = 0.0;
      RefVtx_zError = 0.0;
    }

    zVtx = RefVtx.Z();

    hZVtx->Fill(zVtx);
    hPileUp->Fill(nPV);
  } else {
    std::cout << "ERROR: privtxs is NULL or not isValid ! Return now" << std::endl;
    return;
  }

  this->hltReport(iEvent, iSetup);

  for (unsigned int iTr = 1; iTr < theTriggerNames.size(); iTr++) {
    if (mapTriggerNameToIntFired_[theTriggerNames.at(iTr)] == 3) {
      HLTriggers += pow(2, iTr - 1);
      hStats->Fill(iTr);  // event info
    }
    trigPrescale[iTr - 1] = mapTriggerNameToPrescaleFac_[theTriggerNames.at(iTr)];
  }

  
  if (_isHI || _isPA) {
    iEvent.getByToken(_centralityTagToken, centrality);
    iEvent.getByToken(_centralityBinTagToken, cbin_);
  }
  if (centrality.isValid() && cbin_.isValid()) {
    centBin = *cbin_;
    hCent->Fill(centBin);

    for (unsigned int iCent = 0; iCent < _centralityranges.size(); ++iCent) {
      if ((_isHI && centBin < _centralityranges.at(iCent) / 0.5) || (_isPA && centBin < _centralityranges.at(iCent))) {
        theCentralityBin = iCent;
        break;
      }
    }

    Npix = (Short_t)centrality->multiplicityPixel();
    NpixelTracks = (Short_t)centrality->NpixelTracks();
    Ntracks = (Short_t)centrality->Ntracks();
    NtracksPtCut = centrality->NtracksPtCut();
    NtracksEtaCut = centrality->NtracksEtaCut();
    NtracksEtaPtCut = centrality->NtracksEtaPtCut();
    

    if (_SumETvariables) {
      SumET_HF = centrality->EtHFtowerSum();
      SumET_HFplus = centrality->EtHFtowerSumPlus();
      SumET_HFminus = centrality->EtHFtowerSumMinus();
      SumET_HFplusEta4 = centrality->EtHFtruncatedPlus();
      SumET_HFminusEta4 = centrality->EtHFtruncatedMinus();

      SumET_HFhit = centrality->EtHFhitSum();
      SumET_HFhitPlus = centrality->EtHFhitSumPlus();
      SumET_HFhitMinus = centrality->EtHFhitSumMinus();

      SumET_ZDC = centrality->zdcSum();
      SumET_ZDCplus = centrality->zdcSumPlus();
      SumET_ZDCminus = centrality->zdcSumMinus();

      SumET_EEplus = centrality->EtEESumPlus();
      SumET_EEminus = centrality->EtEESumMinus();
      SumET_EE = centrality->EtEESum();
      SumET_EB = centrality->EtEBSum();
      SumET_ET = centrality->EtMidRapiditySum();
    }
  } else {
    centBin = 0;
    theCentralityBin = 0;

    Npix = 0;
    NpixelTracks = 0;
    Ntracks = 0;
    NtracksPtCut = 0;
    NtracksEtaCut = 0;
    NtracksEtaPtCut = 0;

    SumET_HF = 0;
    SumET_HFplus = 0;
    SumET_HFminus = 0;
    SumET_HFplusEta4 = 0;
    SumET_HFminusEta4 = 0;

    SumET_HFhit = 0;
    SumET_HFhitPlus = 0;
    SumET_HFhitMinus = 0;

    SumET_ZDC = 0;
    SumET_ZDCplus = 0;
    SumET_ZDCminus = 0;
    SumET_EEplus = 0;
    SumET_EEminus = 0;
    SumET_EE = 0;
    SumET_EB = 0;
    SumET_ET = 0;
  }

  if ((_isHI || _isPA) && _useEvtPlane) {
    nEP = 0;
    edm::Handle<reco::EvtPlaneCollection> flatEvtPlanes;
    iEvent.getByToken(_evtPlaneTagToken, flatEvtPlanes);
    if (flatEvtPlanes.isValid()) {
      for (reco::EvtPlaneCollection::const_iterator rp = flatEvtPlanes->begin(); rp != flatEvtPlanes->end(); rp++) {
        rpAng_origin[nEP] = rp->angle(0);   // Using Event Plane Level 0 -> w/o recentering and w/o flattening. 
        rpSin_origin[nEP] = rp->sumSin(0);  // Using Event Plane Level 0 -> w/o recentering and w/o flattening. 
        rpCos_origin[nEP] = rp->sumCos(0);  // Using Event Plane Level 0 -> w/o recentering and w/o flattening.
        rpAng[nEP] = rp->angle(2);   // Using Event Plane Level 2 -> Includes recentering and flattening.
        rpSin[nEP] = rp->sumSin(2);  // Using Event Plane Level 2 -> Includes recentering and flattening.
        rpCos[nEP] = rp->sumCos(2);  // Using Event Plane Level 2 -> Includes recentering and flattening.
        nEP++;
      }
    } else if (!_isMC) {
      std::cout << "Warning! Can't get flattened hiEvtPlane product!" << std::endl;
    }
  }

  iEvent.getByToken(_patJpsiToken, collJpsi);
  if (_doTrimuons)
    iEvent.getByToken(_patTrimuonToken, collTrimuon);
  if (_doDimuTrk)
    iEvent.getByToken(_patDimuTrkToken, collDimutrk);
  iEvent.getByToken(_patMuonToken, collMuon);
  iEvent.getByToken(_patMuonNoTrigToken, collMuonNoTrig);

  if (_useSVfinder)
    iEvent.getByToken(_SVToken, SVs);

  if (!_onlySingleMuons) {
    // APPLY CUTS
    this->makeCuts(_storeSs);

    // APPLY CUTS for Bc (trimuon)
    if (_doTrimuons)
      this->makeBcCuts(_storeSs);

    // APPLY CUTS for Bc (dimuon+track)
    if (_doDimuTrk)
      this->makeDimutrkCuts(_storeSs);
  }

  if (_fillSingleMuons || !_AtLeastOneCand || !_doTrimuons || !_isMC ||
      !_thePassedBcCands
           .empty()) {  //not storing the mu reconstructed info if we do a trimuon MC and there is no reco trimuon
    //_fillSingleMuons is checked within the fillRecoMuons function: the info on the wanted muons was stored in the makeCuts function
    this->fillRecoMuons(theCentralityBin);

    if (_useGeTracks) {
      iEvent.getByToken(_recoTracksToken, collTracks);
      if (_fillRecoTracks) {
        if (!collTracks.isValid()) {
          cout << " collTrack is not valid !!!! Abandoning fillRecoTracks()" << endl;
        } else
          this->fillRecoTracks();
      }
    }
  }

  if (!_onlySingleMuons)
    this->fillRecoHistos();

  //for pp, record Ntracks as well
  if (!(_isHI) && !(_isPA)) {
    iEvent.getByToken(_recoTracksToken, collTracks);
    if (collTracks.isValid()) {
      for (unsigned int tidx = 0; tidx < collTracks->size(); tidx++) {
        const reco::TrackRef track(collTracks, tidx);
        if (track->qualityByName("highPurity") && track->eta() < 2.4 &&
            fabs(track->dxy(RefVtx) / track->dxyError()) < 3 && fabs(track->dz(RefVtx) / track->dzError()) < 3 &&
            track->dz(RefVtx) < 0.5 && fabs(track->ptError() / track->pt()) < 0.1) {
          Ntracks++;
        }
      }
    }
  }
 genOnly:
  if (_isMC) {
    //GEN info
    iEvent.getByToken(_genParticleToken, collGenParticles);
    iEvent.getByToken(_genInfoToken, genInfo);
    this->fillGenInfo();

    //MC MATCHING info
    this->fillMuMatchingInfo();  //Needs to be done after fillGenInfo, and the filling of reco muons collections
    if (!_onlySingleMuons)
      this->fillQQMatchingInfo();  //Needs to be done after fillMuMatchingInfo
    if (_doTrimuons || _doDimuTrk) {
      if (!_onlySingleMuons)
        this->fillBcMatchingInfo();  //Needs to be done after fillQQMatchingInfo
    }
  }

  //keeping events with at least ONE CANDIDATE when asked
  bool oneGoodCand = !_AtLeastOneCand;  //if !_AtLeastOneCand, pass in all cases
  if (_AtLeastOneCand) {
    if (_doTrimuons || _doDimuTrk) {
      if (Reco_3mu_size > 0)
        oneGoodCand = true;
    } else if (Reco_QQ_size > 0)
      oneGoodCand = true;
  }

  // ---- Fill the tree with this event only if AtLeastOneCand=false OR if there is at least one dimuon candidate in the event (or at least one trimuon cand if doTrimuons=true) ----
  if (_fillTree && oneGoodCand)
    myTree->Fill();

  return;
};

void HiOniaAnalyzer::fillRecoHistos() {
  if (!_doTrimuons || !_isMC ||
      !_thePassedBcCands
           .empty()) {  //not storing the mu and QQ reconstructed info if we do a trimuon MC and there is no reco trimuon
    // BEST J/PSI?
    if (_onlythebest) {  // yes, fill simply the best (possibly same-sign)

      pair<unsigned int, const pat::CompositeCandidate*> theBest = theBestQQ();
      if (theBest.first < 10)
        this->fillHistosAndDS(theBest.first, theBest.second);

    } else {  // no, fill all candidates passing cuts (possibly same-sign)

      for (unsigned int count = 0; count < _thePassedCands.size(); count++) {
        const pat::CompositeCandidate* aJpsiCand = _thePassedCands.at(count);

        this->checkTriggers(aJpsiCand);
        if (_fillTree)
          this->fillTreeJpsi(count);

        for (unsigned int iTr = 0; iTr < NTRIGGERS; ++iTr) {
          if (isTriggerMatched[iTr]) {
            this->fillRecoJpsi(count, theTriggerNames.at(iTr), theCentralities.at(theCentralityBin));
          }
        }
      }
    }
  }

  //Fill Bc (trimuon)
  if (_fillTree && _doTrimuons) {
    for (unsigned int count = 0; count < _thePassedBcCands.size(); count++) {
      this->fillTreeBc(count);
    }
  }
  //Fill Bc (dimuon+track)
  if (_fillTree && _doDimuTrk) {
    for (unsigned int count = 0; count < _thePassedBcCands.size(); count++) {
      this->fillTreeDimuTrk(count);
    }
  }

  return;
};

void HiOniaAnalyzer::fillTreeMuon(const pat::Muon* muon, int iType, ULong64_t trigBits) {
  if (Reco_mu_size >= Max_mu_size) {
    std::cout << "Too many muons: " << Reco_mu_size << std::endl;
    std::cout << "Maximum allowed: " << Max_mu_size << std::endl;
    return;
  }

  if (muon != nullptr) {
    Reco_mu_charge[Reco_mu_size] = muon->charge();
    Reco_mu_type[Reco_mu_size] = iType;

    TLorentzVector vMuon = lorentzMomentum(muon->p4());
    new ((*Reco_mu_4mom)[Reco_mu_size]) TLorentzVector(vMuon);
    Reco_mu_4mom_pt.push_back(vMuon.Pt());
    Reco_mu_4mom_eta.push_back(vMuon.Eta());
    Reco_mu_4mom_phi.push_back(vMuon.Phi());
    Reco_mu_4mom_m.push_back(vMuon.M());

    TLorentzVector vMuonL1;
    if (muon->hasUserFloat("l1Eta") && muon->hasUserFloat("l1Phi")) {
      vMuonL1.SetPtEtaPhiM(vMuon.Pt(), muon->userFloat("l1Eta"), muon->userFloat("l1Phi"), vMuon.M());
    } else {
      vMuonL1.SetPtEtaPhiM(0, 0, 0, 0);
    }
    new ((*Reco_mu_L1_4mom)[Reco_mu_size]) TLorentzVector(vMuonL1);
    Reco_mu_L1_4mom_pt.push_back(vMuonL1.Pt());
    Reco_mu_L1_4mom_eta.push_back(vMuonL1.Eta());
    Reco_mu_L1_4mom_phi.push_back(vMuonL1.Phi());
    Reco_mu_L1_4mom_m.push_back(vMuonL1.M());

    //Fill map of the muon indices. Use long int keys, to avoid rounding errors on a float key. Implies a precision of 10^-6
    mapMuonMomToIndex_[FloatToIntkey(vMuon.Pt())] = Reco_mu_size;

    Reco_mu_trig[Reco_mu_size] = trigBits;

    reco::TrackRef iTrack = muon->innerTrack();
    reco::TrackRef bestTrack = muon->muonBestTrack();

    if (!_theMinimumFlag) {
      Reco_mu_InTightAcc[Reco_mu_size] = isMuonInAccept(muon, "GLB");
      Reco_mu_InLooseAcc[Reco_mu_size] = isMuonInAccept(muon, "GLBSOFT");
      Reco_mu_SelectionType[Reco_mu_size] = muonIDmask(muon);
      Reco_mu_StationsMatched[Reco_mu_size] = muon->numberOfMatchedStations();
      Reco_mu_isPF[Reco_mu_size] = muon->isPFMuon();
      Reco_mu_isTracker[Reco_mu_size] = muon->isTrackerMuon();
      Reco_mu_isGlobal[Reco_mu_size] = muon->isGlobalMuon();
      Reco_mu_isSoftCutBased[Reco_mu_size] = muon->passed(reco::Muon::SoftCutBasedId);
      Reco_mu_softMvaRun3Value[Reco_mu_size] = muon->softMvaRun3Value();
      Reco_mu_isHybridSoft[Reco_mu_size] = isHybridSoftMuon(muon);
      Reco_mu_isMediumCutBased[Reco_mu_size] = muon->passed(reco::Muon::CutBasedIdMedium);
      Reco_mu_isTightCutBased[Reco_mu_size] = muon->passed(reco::Muon::CutBasedIdTight);
      Reco_mu_candType[Reco_mu_size] = (Short_t)(muon->hasUserInt("candType")) ? (muon->userInt("candType")) : (-1);

      Reco_mu_TMOneStaTight[Reco_mu_size] = muon::isGoodMuon(*muon, muon::TMOneStationTight);

      Reco_mu_localChi2[Reco_mu_size] = muon->combinedQuality().chi2LocalPosition;
      Reco_mu_kink[Reco_mu_size] = muon->combinedQuality().trkKink;
      Reco_mu_segmentComp[Reco_mu_size] = muon->segmentCompatibility(reco::Muon::SegmentAndTrackArbitration);

      Reco_mu_normChi2_bestTracker[Reco_mu_size] = bestTrack->normalizedChi2();

      if (!iTrack.isNull()) {
        Reco_mu_highPurity[Reco_mu_size] = iTrack->quality(reco::TrackBase::highPurity);
        Reco_mu_nTrkHits[Reco_mu_size] = iTrack->found();
        Reco_mu_normChi2_inner[Reco_mu_size] =
            (muon->hasUserFloat("trackChi2") ? muon->userFloat("trackChi2") : iTrack->normalizedChi2());
        Reco_mu_nPixValHits[Reco_mu_size] = iTrack->hitPattern().numberOfValidPixelHits();
        Reco_mu_nPixWMea[Reco_mu_size] = iTrack->hitPattern().pixelLayersWithMeasurement();
        Reco_mu_nTrkWMea[Reco_mu_size] = iTrack->hitPattern().trackerLayersWithMeasurement();
        Reco_mu_dxy[Reco_mu_size] = iTrack->dxy(RefVtx);
        Reco_mu_dxyErr[Reco_mu_size] = iTrack->dxyError();
        Reco_mu_dz[Reco_mu_size] = iTrack->dz(RefVtx);
        Reco_mu_dzErr[Reco_mu_size] = iTrack->dzError();
        //Reco_mu_pt_inner[Reco_mu_size] = iTrack->pt();
        Reco_mu_ptErr_inner[Reco_mu_size] = iTrack->ptError();
        Reco_mu_validFraction[Reco_mu_size] = iTrack->validFraction();
      } else if (_muonSel != (std::string)("All")) {
        std::cout << "ERROR: 'iTrack' pointer in fillTreeMuon is NULL ! Return now" << std::endl;
        return;
      }

      if (muon->isGlobalMuon()) {
        reco::TrackRef gTrack = muon->globalTrack();
        Reco_mu_nMuValHits[Reco_mu_size] = gTrack->hitPattern().numberOfValidMuonHits();
        Reco_mu_normChi2_global[Reco_mu_size] = gTrack->normalizedChi2();
        //Reco_mu_pt_global[Reco_mu_size] = gTrack->pt();
        //Reco_mu_ptErr_global[Reco_mu_size] = gTrack->ptError();
      } else {
        Reco_mu_nMuValHits[Reco_mu_size] = -1;
        Reco_mu_normChi2_global[Reco_mu_size] = 999;
        //Reco_mu_pt_global[Reco_mu_size] = -1;
        //Reco_mu_ptErr_global[Reco_mu_size] = -1;
      }
    }

    if (_isMC) {
      Reco_mu_pTrue[Reco_mu_size] =
          ((muon->genParticleRef()).isNonnull()) ? ((float)(muon->genParticleRef())->p()) : (-1);
      if (_genealogyInfo) {
        Reco_mu_simExtType[Reco_mu_size] = muon->simExtType();
      }
    }
  } else {
    std::cout << "ERROR: 'muon' pointer in fillTreeMuon is NULL ! Return now" << std::endl;
    return;
  }

  Reco_mu_size++;
  return;
};

void HiOniaAnalyzer::fillTreeJpsi(int count) {
  if (Reco_QQ_size >= Max_QQ_size) {
    std::cout << "Too many dimuons: " << Reco_QQ_size << std::endl;
    std::cout << "Maximum allowed: " << Max_QQ_size << std::endl;
    return;
  }

  const pat::CompositeCandidate* aJpsiCand = _thePassedCands.at(count);

  if (aJpsiCand != nullptr) {
    const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(aJpsiCand->daughter("muon1"));
    const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(aJpsiCand->daughter("muon2"));

    ULong64_t trigBits = 0;
    for (unsigned int iTr = 1; iTr < NTRIGGERS; ++iTr) {
      if (isTriggerMatched[iTr]) {
        trigBits += pow(2, iTr - 1);
      }
    }

    if (muon1 == nullptr || muon2 == nullptr) {
      std::cout << "ERROR: 'muon1' or 'muon2' pointer in fillTreeJpsi is NULL ! Return now" << std::endl;
      return;
    } else {
      Reco_QQ_sign[Reco_QQ_size] = muon1->charge() + muon2->charge();
      Reco_QQ_type[Reco_QQ_size] = _thePassedCats.at(count);

      Reco_QQ_trig[Reco_QQ_size] = trigBits;

      if (!(_isHI) && _muonLessPrimaryVertex && aJpsiCand->hasUserData("muonlessPV")) {
        RefVtx = (*aJpsiCand->userData<reco::Vertex>("muonlessPV")).position();
        RefVtx_xError = (*aJpsiCand->userData<reco::Vertex>("muonlessPV")).xError();
        RefVtx_yError = (*aJpsiCand->userData<reco::Vertex>("muonlessPV")).yError();
        RefVtx_zError = (*aJpsiCand->userData<reco::Vertex>("muonlessPV")).zError();
      } else if (!_muonLessPrimaryVertex && aJpsiCand->hasUserData("PVwithmuons")) {
        RefVtx = (*aJpsiCand->userData<reco::Vertex>("PVwithmuons")).position();
        RefVtx_xError = (*aJpsiCand->userData<reco::Vertex>("PVwithmuons")).xError();
        RefVtx_yError = (*aJpsiCand->userData<reco::Vertex>("PVwithmuons")).yError();
        RefVtx_zError = (*aJpsiCand->userData<reco::Vertex>("PVwithmuons")).zError();
      } else {
        cout << "HiOniaAnalyzer::fillTreeJpsi: no PVfor muon pair stored" << endl;
        return;
      }

      new ((*Reco_QQ_vtx)[Reco_QQ_size]) TVector3(RefVtx.X(), RefVtx.Y(), RefVtx.Z());

      TLorentzVector vMuon1 = lorentzMomentum(muon1->p4());
      TLorentzVector vMuon2 = lorentzMomentum(muon2->p4());

      reco::Track iTrack_mupl, iTrack_mumi, mu1Trk, mu2Trk;
      if (_flipJpsiDirection > 0 && aJpsiCand->hasUserData("muon1Track") && aJpsiCand->hasUserData("muon2Track")) {
        mu1Trk = *(aJpsiCand->userData<reco::Track>("muon1Track"));
        mu2Trk = *(aJpsiCand->userData<reco::Track>("muon2Track"));
      }

      Reco_QQ_flipJpsi[Reco_QQ_size] = _flipJpsiDirection;
      if (aJpsiCand->hasUserInt("flipJpsi"))
        Reco_QQ_flipJpsi[Reco_QQ_size] = aJpsiCand->userInt("flipJpsi");

      if ((muon1->innerTrack()).isNull() || (muon2->innerTrack()).isNull()) {
        std::cout << "ERROR: 'iTrack_mupl' or 'iTrack_mumi' pointer in fillTreeJpsi is NULL ! Return now" << std::endl;
        return;
      }

      if (muon1->charge() > muon2->charge()) {
        Reco_QQ_mupl_idx[Reco_QQ_size] = IndexOfThisMuon(&vMuon1);  //needs the non-flipped muon momentum
        Reco_QQ_mumi_idx[Reco_QQ_size] = IndexOfThisMuon(&vMuon2);

        if (_flipJpsiDirection > 0) {
          iTrack_mupl = mu1Trk;
          iTrack_mumi = mu2Trk;
          new ((*Reco_QQ_mupl_4mom)[Reco_QQ_size]) TLorentzVector(
              mu1Trk.px(), mu1Trk.py(), mu1Trk.pz(), vMuon1.E());  //only the direction of the 3-momentum changes
          Reco_QQ_mupl_4mom_pt.push_back(mu1Trk.pt());
          Reco_QQ_mupl_4mom_eta.push_back(mu1Trk.eta());
          Reco_QQ_mupl_4mom_phi.push_back(mu1Trk.phi());
          Reco_QQ_mupl_4mom_m.push_back(vMuon1.M());

          new ((*Reco_QQ_mumi_4mom)[Reco_QQ_size]) TLorentzVector(mu2Trk.px(), mu2Trk.py(), mu2Trk.pz(), vMuon2.E());
          Reco_QQ_mumi_4mom_pt.push_back(mu2Trk.pt());
          Reco_QQ_mumi_4mom_eta.push_back(mu2Trk.eta());
          Reco_QQ_mumi_4mom_phi.push_back(mu2Trk.phi());
          Reco_QQ_mumi_4mom_m.push_back(vMuon2.M());

        } else if (_muonLessPrimaryVertex || _useGeTracks) {
          iTrack_mupl = *(muon1->innerTrack());
          iTrack_mumi = *(muon2->innerTrack());
        }

      } else {
        Reco_QQ_mupl_idx[Reco_QQ_size] = IndexOfThisMuon(&vMuon2);  //needs the non-flipped muon momentum
        Reco_QQ_mumi_idx[Reco_QQ_size] = IndexOfThisMuon(&vMuon1);

        if (_flipJpsiDirection > 0) {
          iTrack_mupl = mu2Trk;
          iTrack_mumi = mu1Trk;
          new ((*Reco_QQ_mumi_4mom)[Reco_QQ_size]) TLorentzVector(
              mu1Trk.px(), mu1Trk.py(), mu1Trk.pz(), vMuon1.E());  //only the direction of the 3-momentum changes
          Reco_QQ_mumi_4mom_pt.push_back(mu1Trk.pt());
          Reco_QQ_mumi_4mom_eta.push_back(mu1Trk.eta());
          Reco_QQ_mumi_4mom_phi.push_back(mu1Trk.phi());
          Reco_QQ_mumi_4mom_m.push_back(vMuon1.M());
          new ((*Reco_QQ_mupl_4mom)[Reco_QQ_size]) TLorentzVector(mu2Trk.px(), mu2Trk.py(), mu2Trk.pz(), vMuon2.E());
          Reco_QQ_mupl_4mom_pt.push_back(mu2Trk.pt());
          Reco_QQ_mupl_4mom_eta.push_back(mu2Trk.eta());
          Reco_QQ_mupl_4mom_phi.push_back(mu2Trk.phi());
          Reco_QQ_mupl_4mom_m.push_back(vMuon2.M());
        } else if (_muonLessPrimaryVertex || _useGeTracks) {
          iTrack_mupl = *(muon2->innerTrack());
          iTrack_mumi = *(muon1->innerTrack());
        }
      }

      if ((!_theMinimumFlag && _muonLessPrimaryVertex) || (_flipJpsiDirection > 0)) {
        Reco_QQ_mupl_dxy[Reco_QQ_size] = iTrack_mupl.dxy(RefVtx);
        Reco_QQ_mumi_dxy[Reco_QQ_size] = iTrack_mumi.dxy(RefVtx);
        Reco_QQ_mupl_dz[Reco_QQ_size] = iTrack_mupl.dz(RefVtx);
        Reco_QQ_mumi_dz[Reco_QQ_size] = iTrack_mumi.dz(RefVtx);
      }

      TLorentzVector vJpsi = lorentzMomentum(aJpsiCand->p4());
      new ((*Reco_QQ_4mom)[Reco_QQ_size]) TLorentzVector(vJpsi);
      Reco_QQ_4mom_pt.push_back(vJpsi.Pt());
      Reco_QQ_4mom_eta.push_back(vJpsi.Eta());
      Reco_QQ_4mom_y.push_back(vJpsi.Rapidity());
      Reco_QQ_4mom_phi.push_back(vJpsi.Phi());
      Reco_QQ_4mom_m.push_back(vJpsi.M());

      if (_useBS) {
        if (aJpsiCand->hasUserFloat("ppdlBS")) {
          Reco_QQ_ctau[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlBS");
        } else {
          Reco_QQ_ctau[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlBS was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("ppdlErrBS")) {
          Reco_QQ_ctauErr[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlErrBS");
        } else {
          Reco_QQ_ctauErr[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlErrBS was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("ppdlBS3D")) {
          Reco_QQ_ctau3D[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlBS3D");
        } else {
          Reco_QQ_ctau3D[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlBS3D was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("ppdlErrBS3D")) {
          Reco_QQ_ctauErr3D[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlErrBS3D");
        } else {
          Reco_QQ_ctauErr3D[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlErrBS3D was not found" << std::endl;
        }
      } else {
        if (aJpsiCand->hasUserFloat("ppdlPV")) {
          Reco_QQ_ctau[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlPV");
        } else {
          Reco_QQ_ctau[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlPV was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("ppdlErrPV")) {
          Reco_QQ_ctauErr[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlErrPV");
        } else {
          Reco_QQ_ctauErr[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlErrPV was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("ppdlPV3D")) {
          Reco_QQ_ctau3D[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlPV3D");
        } else {
          Reco_QQ_ctau3D[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlPV3D was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("ppdlErrPV3D")) {
          Reco_QQ_ctauErr3D[Reco_QQ_size] = 10.0 * aJpsiCand->userFloat("ppdlErrPV3D");
        } else {
          Reco_QQ_ctau3D[Reco_QQ_size] = -100;
          std::cout << "Warning: User Float ppdlErrPV3D was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("cosAlpha")) {
          Reco_QQ_cosAlpha[Reco_QQ_size] = aJpsiCand->userFloat("cosAlpha");
        } else {
          Reco_QQ_cosAlpha[Reco_QQ_size] = -10;
          std::cout << "Warning: User Float cosAlpha was not found" << std::endl;
        }
        if (aJpsiCand->hasUserFloat("cosAlpha3D")) {
          Reco_QQ_cosAlpha3D[Reco_QQ_size] = aJpsiCand->userFloat("cosAlpha3D");
        } else {
          Reco_QQ_cosAlpha3D[Reco_QQ_size] = -10;
          std::cout << "Warning: User Float cosAlpha3D was not found" << std::endl;
        }
      }
      if (aJpsiCand->hasUserFloat("vProb")) {
        Reco_QQ_VtxProb[Reco_QQ_size] = aJpsiCand->userFloat("vProb");
      } else {
        Reco_QQ_VtxProb[Reco_QQ_size] = -1;
        std::cout << "Warning: User Float vProb was not found" << std::endl;
      }
      if (aJpsiCand->hasUserFloat("DCA")) {
        Reco_QQ_dca[Reco_QQ_size] = aJpsiCand->userFloat("DCA");
      } else {
        Reco_QQ_dca[Reco_QQ_size] = -10;
        std::cout << "Warning: User Float DCA was not found" << std::endl;
      }
      if (aJpsiCand->hasUserFloat("MassErr")) {
        Reco_QQ_MassErr[Reco_QQ_size] = aJpsiCand->userFloat("MassErr");
      } else {
        Reco_QQ_MassErr[Reco_QQ_size] = -10;
        std::cout << "Warning: User Float MassErr was not found" << std::endl;
      }

      Reco_QQ_NtrkDeltaR03[Reco_QQ_size] = 0;
      Reco_QQ_NtrkDeltaR04[Reco_QQ_size] = 0;
      Reco_QQ_NtrkDeltaR05[Reco_QQ_size] = 0;

      Reco_QQ_NtrkPt02[Reco_QQ_size] = 0;
      Reco_QQ_NtrkPt03[Reco_QQ_size] = 0;
      Reco_QQ_NtrkPt04[Reco_QQ_size] = 0;

      //--- counting tracks around Jpsi direction ---
      if (_useGeTracks && !_doDimuTrk && collTracks.isValid()) {
        for (std::vector<reco::Track>::const_iterator it = collTracks->begin(); it != collTracks->end(); ++it) {
          const reco::Track* track = &(*it);

          if (track == nullptr) {
            std::cout << "ERROR: 'track' pointer in fillTreeJpsi is NULL ! Return now" << std::endl;
            return;
          } else {
            double dz = track->dz(RefVtx);
            double dzsigma = sqrt(track->dzError() * track->dzError() + RefVtx_zError * RefVtx_zError);
            double dxy = track->dxy(RefVtx);
            double dxysigma = sqrt(track->dxyError() * track->dxyError() + RefVtx_xError * RefVtx_yError);

            if (track->qualityByName("highPurity") && track->pt() > 0.2 && fabs(track->eta()) < 2.4 &&
                track->ptError() / track->pt() < 0.1 && fabs(dz / dzsigma) < 3.0 && fabs(dxy / dxysigma) < 3.0) {
              Reco_QQ_NtrkPt02[Reco_QQ_size]++;
              if (track->pt() > 0.3)
                Reco_QQ_NtrkPt03[Reco_QQ_size]++;
              if (track->pt() > 0.4) {
                Reco_QQ_NtrkPt04[Reco_QQ_size]++;

                if (iTrack_mupl.charge() == track->charge()) {
                  double Reco_QQ_mupl_NtrkDeltaR =
                      deltaR(iTrack_mupl.eta(), iTrack_mupl.phi(), track->eta(), track->phi());
                  double Reco_QQ_mupl_RelDelPt = abs(1.0 - iTrack_mupl.pt() / track->pt());

                  if (Reco_QQ_mupl_NtrkDeltaR < 0.001 && Reco_QQ_mupl_RelDelPt < 0.001)
                    continue;
                } else {
                  double Reco_QQ_mumi_NtrkDeltaR =
                      deltaR(iTrack_mumi.eta(), iTrack_mumi.phi(), track->eta(), track->phi());
                  double Reco_QQ_mumi_RelDelPt = abs(1.0 - iTrack_mumi.pt() / track->pt());
                  if (Reco_QQ_mumi_NtrkDeltaR < 0.001 && Reco_QQ_mumi_RelDelPt < 0.001)
                    continue;
                }

                double Reco_QQ_NtrkDeltaR = deltaR(aJpsiCand->eta(), aJpsiCand->phi(), track->eta(), track->phi());
                if (Reco_QQ_NtrkDeltaR < 0.3)
                  Reco_QQ_NtrkDeltaR03[Reco_QQ_size]++;
                if (Reco_QQ_NtrkDeltaR < 0.4)
                  Reco_QQ_NtrkDeltaR04[Reco_QQ_size]++;
                if (Reco_QQ_NtrkDeltaR < 0.5)
                  Reco_QQ_NtrkDeltaR05[Reco_QQ_size]++;
              }
            }
          }
        }
      }
    }
  } else {
    std::cout << "ERROR: 'aJpsiCand' pointer in fillTreeJpsi is NULL ! Return now" << std::endl;
    return;
  }

  Reco_QQ_size++;
  return;
};

void HiOniaAnalyzer::fillRecoJpsi(int count, std::string trigName, std::string centName) {
  pat::CompositeCandidate* aJpsiCand = _thePassedCands.at(count)->clone();

  if (aJpsiCand == nullptr) {
    std::cout << "ERROR: 'aJpsiCand' pointer in fillTreeJpsi is NULL ! Return now" << std::endl;
    return;
  }
  aJpsiCand->addUserInt("centBin", centBin);
  const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(aJpsiCand->daughter("muon1"));
  const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(aJpsiCand->daughter("muon2"));

  if (muon1 == nullptr || muon2 == nullptr) {
    std::cout << "ERROR: 'muon1' or 'muon2' pointer in fillTreeJpsi is NULL ! Return now" << std::endl;
    return;
  }
  int iSign = muon1->charge() + muon2->charge();
  if (iSign != 0) {
    (iSign == 2) ? (iSign = 1) : (iSign = 2);
  }

  std::string theLabel = trigName + "_" + centName + "_" + theSign.at(iSign);

  bool isBarrel = (fabs(aJpsiCand->rapidity()) < 1.2);

  if (iSign == 0 && aJpsiCand->mass() >= JpsiMassMin && aJpsiCand->mass() < JpsiMassMax &&
      aJpsiCand->pt() >= JpsiPtMin && aJpsiCand->pt() < JpsiPtMax && abs(aJpsiCand->rapidity()) >= JpsiRapMin &&
      fabs(aJpsiCand->rapidity()) < JpsiRapMax)
    passedCandidates++;

  if (_fillHistos) {
    if (_combineCategories &&
        _thePassedCats.at(count) <= Trk_Trk) {  // for the moment consider Glb+Glb, GlbTrk+GlbTrk, Trk+Trk
      myRecoJpsiHistos->Fill(aJpsiCand, "All_" + theLabel);
      if (isBarrel)
        myRecoJpsiHistos->Fill(aJpsiCand, "Barrel_" + theLabel);
      else
        myRecoJpsiHistos->Fill(aJpsiCand, "EndCap_" + theLabel);
    } else {
      switch (_thePassedCats.at(count)) {
        case Glb_Glb:
          myRecoJpsiGlbGlbHistos->Fill(aJpsiCand, "All_" + theLabel);
          if (isBarrel)
            myRecoJpsiGlbGlbHistos->Fill(aJpsiCand, "Barrel_" + theLabel);
          else
            myRecoJpsiGlbGlbHistos->Fill(aJpsiCand, "EndCap_" + theLabel);
          break;
        case GlbTrk_GlbTrk:
          myRecoJpsiGlbTrkHistos->Fill(aJpsiCand, "All_" + theLabel);
          if (isBarrel)
            myRecoJpsiGlbTrkHistos->Fill(aJpsiCand, "Barrel_" + theLabel);
          else
            myRecoJpsiGlbTrkHistos->Fill(aJpsiCand, "EndCap_" + theLabel);
          break;
        case Trk_Trk:
          myRecoJpsiTrkTrkHistos->Fill(aJpsiCand, "All_" + theLabel);
          if (isBarrel)
            myRecoJpsiTrkTrkHistos->Fill(aJpsiCand, "Barrel_" + theLabel);
          else
            myRecoJpsiTrkTrkHistos->Fill(aJpsiCand, "EndCap_" + theLabel);
          break;
        default:
          break;
      }
    }
  }
  this->fillHistosAndDS(_thePassedCats.at(count), aJpsiCand);
  delete aJpsiCand;
  return;
};

void HiOniaAnalyzer::fillHistosAndDS(unsigned int theCat, const pat::CompositeCandidate* aJpsiCand) { return; };

void HiOniaAnalyzer::checkTriggers(const pat::CompositeCandidate* aJpsiCand) {
  if (aJpsiCand == nullptr) {
    std::cout << "ERROR: 'aJpsiCand' pointer in checkTriggers is NULL ! Return now" << std::endl;
    return;
  }

  const pat::Muon* muon1 = dynamic_cast<const pat::Muon*>(aJpsiCand->daughter("muon1"));
  const pat::Muon* muon2 = dynamic_cast<const pat::Muon*>(aJpsiCand->daughter("muon2"));

  if (muon1 == nullptr || muon2 == nullptr) {
    std::cout << "ERROR: 'muon1' or 'muon2' pointer in checkTriggers is NULL ! Return now" << std::endl;
    return;
  }

  // Trigger passed
  for (unsigned int iTr = 1; iTr < NTRIGGERS; ++iTr) {
    const auto& lastFilter = filterNameMap.at(theTriggerNames[iTr]);
    const auto& mu1HLTMatchesFilter = muon1->triggerObjectMatchesByFilter(lastFilter);
    const auto& mu2HLTMatchesFilter = muon2->triggerObjectMatchesByFilter(lastFilter);

    bool pass1 = !mu1HLTMatchesFilter.empty();
    bool pass2 = !mu2HLTMatchesFilter.empty();

    if (iTr > NTRIGGERS_DBL) {  // single triggers here
      isTriggerMatched[iTr] = pass1 || pass2;
    } else {  // double triggers here
      isTriggerMatched[iTr] = pass1 && pass2;
    }
  }

  for (unsigned int iTr = 1; iTr < NTRIGGERS; ++iTr) {
    if (isTriggerMatched[iTr]) {
      // since we have bins for event info, let's try to fill here the trigger info for each pair
      // also if there are several pairs matched to the same kind of trigger
      hStats->Fill(iTr + NTRIGGERS);  // pair info
    }
  }
  return;
};

void HiOniaAnalyzer::InitEvent() {
  for (unsigned int iTr = 1; iTr < NTRIGGERS; ++iTr) {
    alreadyFilled[iTr] = false;
  }
  HLTriggers = 0;
  nEP = 0;

  _thePassedCats.clear();
  _thePassedCands.clear();

  Reco_QQ_size = 0;
  Reco_mu_size = 0;
  Reco_trk_size = 0;

  Reco_QQ_4mom->Clear();
  Reco_QQ_4mom_pt.clear();
  Reco_QQ_4mom_eta.clear();
  Reco_QQ_4mom_y.clear();
  Reco_QQ_4mom_phi.clear();
  Reco_QQ_4mom_m.clear();
  Reco_QQ_mupl_4mom->Clear();
  Reco_QQ_mupl_4mom_pt.clear();
  Reco_QQ_mupl_4mom_eta.clear();
  Reco_QQ_mupl_4mom_phi.clear();
  Reco_QQ_mupl_4mom_m.clear();
  Reco_QQ_mumi_4mom->Clear();
  Reco_QQ_mumi_4mom_pt.clear();
  Reco_QQ_mumi_4mom_eta.clear();
  Reco_QQ_mumi_4mom_phi.clear();
  Reco_QQ_mumi_4mom_m.clear();
  Reco_QQ_vtx->Clear();
  Reco_mu_4mom->Clear();
  Reco_mu_4mom_pt.clear();
  Reco_mu_4mom_eta.clear();
  Reco_mu_4mom_phi.clear();
  Reco_mu_4mom_m.clear();
  Reco_mu_L1_4mom->Clear();
  Reco_mu_L1_4mom_pt.clear();
  Reco_mu_L1_4mom_eta.clear();
  Reco_mu_L1_4mom_phi.clear();
  Reco_mu_L1_4mom_m.clear();

  if (_useGeTracks && _fillRecoTracks) {
    Reco_trk_4mom->Clear();
    Reco_trk_4mom_pt.clear();
    Reco_trk_4mom_eta.clear();
    Reco_trk_4mom_phi.clear();
    Reco_trk_4mom_m.clear();
    Reco_trk_vtx->Clear();
  }

  if (_isMC) {
    Gen_QQ_4mom->Clear();
    Gen_QQ_4mom_pt.clear();
    Gen_QQ_4mom_eta.clear();
    Gen_QQ_4mom_y.clear();
    Gen_QQ_4mom_phi.clear();
    Gen_QQ_4mom_m.clear();
    Gen_mu_4mom->Clear();
    Gen_mu_4mom_pt.clear();
    Gen_mu_4mom_eta.clear();
    Gen_mu_4mom_phi.clear();
    Gen_mu_4mom_m.clear();
    Gen_QQ_size = 0;
    Gen_mu_size = 0;

    Gen_weight = -1.;
    Gen_pthat = -1.;

    mapGenMuonMomToIndex_.clear();
  }

  if (_doTrimuons || _doDimuTrk) {
    _thePassedBcCats.clear();
    _thePassedBcCands.clear();

    Reco_3mu_size = 0;
    Reco_3mu_vtx->Clear();
    Reco_3mu_4mom->Clear();
    Reco_3mu_4mom_pt.clear();
    Reco_3mu_4mom_eta.clear();
    Reco_3mu_4mom_y.clear();
    Reco_3mu_4mom_phi.clear();
    Reco_3mu_4mom_m.clear();

    if (_isMC) {
      Gen_Bc_size = 0;
      Gen_Bc_4mom->Clear();
      Gen_Bc_4mom_pt.clear();
      Gen_Bc_4mom_eta.clear();
      Gen_Bc_4mom_y.clear();
      Gen_Bc_4mom_phi.clear();
      Gen_Bc_4mom_m.clear();
      Gen_Bc_nuW_4mom->Clear();
      Gen_Bc_nuW_4mom_pt.clear();
      Gen_Bc_nuW_4mom_eta.clear();
      Gen_Bc_nuW_4mom_y.clear();
      Gen_Bc_nuW_4mom_phi.clear();
      Gen_Bc_nuW_4mom_m.clear();
      Gen_3mu_4mom->Clear();
      Gen_3mu_4mom_pt.clear();
      Gen_3mu_4mom_eta.clear();
      Gen_3mu_4mom_y.clear();
      Gen_3mu_4mom_phi.clear();
      Gen_3mu_4mom_m.clear();
    }
  }

  mapMuonMomToIndex_.clear();
  mapTrkMomToIndex_.clear();
  for (std::map<std::string, int>::iterator clearIt = mapTriggerNameToIntFired_.begin();
       clearIt != mapTriggerNameToIntFired_.end();
       clearIt++) {
    clearIt->second = 0;
  }
  for (std::map<std::string, int>::iterator clearIt = mapTriggerNameToPrescaleFac_.begin();
       clearIt != mapTriggerNameToPrescaleFac_.end();
       clearIt++) {
    clearIt->second = -1;
  }

  return;
};

void HiOniaAnalyzer::fillRecoTracks() {
  if (!collTracks.isValid())
    return;
  for (unsigned int tidx = 0; tidx < collTracks->size(); tidx++) {
    const reco::TrackRef track(collTracks, tidx);
    if (!track.isNonnull()) {
      std::cout << "ERROR: 'track' pointer in fillRecoTracks is NULL ! Go to next track." << endl;
      return;
    }
    bool WantedTrack = false;
    for (int k = 0; k < (int)EtaOfWantedTracks.size(); k++) {
      if (fabs(track->eta() - EtaOfWantedTracks[k]) < 1e-5) {
        WantedTrack = true;
        break;
      }
    }
    if (_doDimuTrk && !WantedTrack)
      continue;

    if (selTrk(track)) {
      if (Reco_trk_size >= Max_trk_size) {
        std::cout << "Too many tracks: " << Reco_trk_size << std::endl;
        std::cout << "Maximum allowed: " << Max_trk_size << std::endl;
        break;
      }
      TLorentzVector vTrack;
      vTrack.SetPtEtaPhiM(track->pt(), track->eta(), track->phi(), 0.13957018);  //0.13957018 for the pion

      if (_isMC) {
	Reco_trk_whichGenmu[Reco_trk_size] = -1;

        float dRmax = 0.05;  //dR max of the matching to gen muons//same than for reco-gen muon matching
        float dR;
        float dPtmax = 0.5;
        for (int igen = 0; igen < Gen_mu_size; igen++) {
          TLorentzVector* genmu = (TLorentzVector*)Gen_mu_4mom->ConstructedAt(igen);
          dR = genmu->DeltaR(vTrack);
          if (dR <= dRmax && track->charge() == Gen_mu_charge[igen] &&
              fabs(genmu->Pt() - vTrack.Pt()) / genmu->Pt() < dPtmax) {
            dRmax = dR;
            Reco_trk_whichGenmu[Reco_trk_size] = igen;
          }
        }

	if (Reco_trk_whichGenmu[Reco_trk_size] == -1) continue;
      }

      Reco_trk_charge[Reco_trk_size] = track->charge();
      Reco_trk_highPurity[Reco_trk_size] = track->qualityByName("highPurity");

      Reco_trk_originalAlgo[Reco_mu_size] = track->originalAlgo();
      Reco_trk_nPixWMea[Reco_mu_size] = track->hitPattern().pixelLayersWithMeasurement();
      Reco_trk_nTrkWMea[Reco_mu_size] = track->hitPattern().trackerLayersWithMeasurement();
      Reco_trk_dxyError[Reco_trk_size] = track->dxyError();
      Reco_trk_dzError[Reco_trk_size] = track->dzError();
      Reco_trk_dxy[Reco_trk_size] = track->dxy(RefVtx);
      Reco_trk_dz[Reco_trk_size] = track->dz(RefVtx);
      Reco_trk_ptErr[Reco_trk_size] = track->ptError();

      new ((*Reco_trk_vtx)[Reco_trk_size]) TVector3(RefVtx.X(), RefVtx.Y(), RefVtx.Z());


      mapTrkMomToIndex_[FloatToIntkey(vTrack.Pt())] = Reco_trk_size;

      Reco_trk_InLooseAcc[Reco_trk_size] = isTrkInMuonAccept(vTrack, "GLBSOFT");
      Reco_trk_InTightAcc[Reco_trk_size] = isTrkInMuonAccept(vTrack, "GLB");

      new ((*Reco_trk_4mom)[Reco_trk_size]) TLorentzVector(vTrack);
      Reco_trk_4mom_pt.push_back(vTrack.Pt());
      Reco_trk_4mom_eta.push_back(vTrack.Eta());
      Reco_trk_4mom_phi.push_back(vTrack.Phi());
      Reco_trk_4mom_m.push_back(vTrack.M());
      Reco_trk_size++;
    }
  }
  return;
};

void HiOniaAnalyzer::fillRecoMuons(int iCent) {
  int nL1DoubleMu0Muons = 0;
  int nGoodMuons = 0;
  int nGoodMuonsNoTrig = 0;

  if (collMuonNoTrig.isValid()) {
    for (std::vector<pat::Muon>::const_iterator it = collMuonNoTrig->begin(); it != collMuonNoTrig->end(); ++it) {
      const pat::Muon* muon = &(*it);
      if (muon == nullptr) {
        std::cout << "ERROR: 'muon' pointer in fillRecoMuons is NULL ! Return now" << std::endl;
        return;
      }
      if (selGlobalMuon(muon))
        nGoodMuonsNoTrig++;
    }
  }

  if (collMuon.isValid()) {
    for (vector<pat::Muon>::const_iterator it = collMuon->begin(); it != collMuon->end(); ++it) {
      const pat::Muon* muon = &(*it);
      if (muon == nullptr) {
        std::cout << "ERROR: 'muon' pointer in fillRecoMuons is NULL ! Return now" << std::endl;
        return;
      }

      //Trick to recover feature of filling only muons from selected dimuons
      if (!_fillSingleMuons) {
        bool WantedMuon = false;
        for (int k = 0; k < (int)EtaOfWantedMuons.size(); k++) {
          if (fabs(muon->eta() - EtaOfWantedMuons[k]) < 1e-5) {
            WantedMuon = true;
            break;
          }
        }
        if (!WantedMuon)
          continue;
      }

      bool isBarrel = (fabs(muon->eta() < 1.2));
      std::string theLabel = theTriggerNames.at(0) + "_" + theCentralities.at(iCent);

      if (_fillHistos) {
        if (_combineCategories) {
          if (selGlobalMuon(muon) || selTrackerMuon(muon)) {
            myRecoMuonHistos->Fill(muon, "All_" + theLabel);
            if (isBarrel)
              myRecoMuonHistos->Fill(muon, "Barrel_" + theLabel);
            else
              myRecoMuonHistos->Fill(muon, "EndCap_" + theLabel);
          }
        } else {
          if (selGlobalMuon(muon)) {
            myRecoGlbMuonHistos->Fill(muon, "All_" + theLabel);
            if (isBarrel)
              myRecoGlbMuonHistos->Fill(muon, "Barrel_" + theLabel);
            else
              myRecoGlbMuonHistos->Fill(muon, "EndCap_" + theLabel);
          } else if (selTrackerMuon(muon)) {
            myRecoTrkMuonHistos->Fill(muon, "All_" + theLabel);
            if (isBarrel)
              myRecoTrkMuonHistos->Fill(muon, "Barrel_" + theLabel);
            else
              myRecoTrkMuonHistos->Fill(muon, "EndCap_" + theLabel);
          }
        }
      }

      muType = -99;
      if (_muonSel == (std::string)("Glb") && selGlobalMuon(muon))
        muType = Glb;
      if (_muonSel == (std::string)("GlbTrk") && selGlobalMuon(muon))
        muType = GlbTrk;
      if (_muonSel == (std::string)("Trk") && selTrackerMuon(muon))
        muType = Trk;
      if (_muonSel == (std::string)("TwoGlbAmongThree") && selGlobalOrTrackerMuon(muon))
        muType = GlbOrTrk;
      if (_muonSel == (std::string)("GlbOrTrk") && selGlobalOrTrackerMuon(muon))
        muType = GlbOrTrk;
      if (_muonSel == (std::string)("All") && selAllMuon(muon))
        muType = All;

      if (muType == GlbOrTrk || muType == GlbTrk || muType == Trk || muType == Glb || muType == All) {
        nGoodMuons++;

        ULong64_t trigBits = 0;
        for (unsigned int iTr = 1; iTr < NTRIGGERS; ++iTr) {
          const pat::TriggerObjectStandAloneCollection muHLTMatchesFilter =
              muon->triggerObjectMatchesByFilter(filterNameMap.at(theTriggerNames[iTr]));

          // apparently matching by path gives false positives so we use matching by filter for all triggers for which we know the filter name
          if (!muHLTMatchesFilter.empty()) {
            std::string theLabel = theTriggerNames.at(iTr) + "_" + theCentralities.at(iCent);

            if (_fillHistos) {
              if (_combineCategories) {
                myRecoMuonHistos->Fill(muon, "All_" + theLabel);
                if (isBarrel)
                  myRecoMuonHistos->Fill(muon, "Barrel_" + theLabel);
                else
                  myRecoMuonHistos->Fill(muon, "EndCap_" + theLabel);
              } else if (muType == Glb || muType == GlbTrk) {
                myRecoGlbMuonHistos->Fill(muon, "All_" + theLabel);
                if (isBarrel)
                  myRecoGlbMuonHistos->Fill(muon, "Barrel_" + theLabel);
                else
                  myRecoGlbMuonHistos->Fill(muon, "EndCap_" + theLabel);
              } else if (muType == Trk || muType == GlbOrTrk || muType == All) {
                myRecoTrkMuonHistos->Fill(muon, "All_" + theLabel);
                if (isBarrel)
                  myRecoTrkMuonHistos->Fill(muon, "Barrel_" + theLabel);
                else
                  myRecoTrkMuonHistos->Fill(muon, "EndCap_" + theLabel);
              }
            }
            trigBits += pow(2, iTr - 1);
            if (iTr == 1)
              nL1DoubleMu0Muons++;
          }
        }
        if (_fillTree)
          this->fillTreeMuon(muon, muType, trigBits);
      }
    }
  }

  hGoodMuonsNoTrig->Fill(nGoodMuonsNoTrig);
  hGoodMuons->Fill(nGoodMuons);
  hL1DoubleMu0->Fill(nL1DoubleMu0Muons);

  return;
};

void HiOniaAnalyzer::InitTree() {
  Reco_mu_4mom = new TClonesArray("TLorentzVector", Max_mu_size);
  Reco_mu_L1_4mom = new TClonesArray("TLorentzVector", Max_mu_size);
  Reco_QQ_4mom = new TClonesArray("TLorentzVector", Max_QQ_size);
  Reco_QQ_mumi_4mom = new TClonesArray("TLorentzVector", Max_QQ_size);
  Reco_QQ_mupl_4mom = new TClonesArray("TLorentzVector", Max_QQ_size);
  Reco_QQ_vtx = new TClonesArray("TVector3", Max_QQ_size);

  if (_useGeTracks && _fillRecoTracks) {
    Reco_trk_4mom = new TClonesArray("TLorentzVector", Max_trk_size);
    Reco_trk_vtx = new TClonesArray("TVector3", Max_trk_size);
  }

  if (_isMC) {
    Gen_mu_4mom = new TClonesArray("TLorentzVector", 10);
    Gen_QQ_4mom = new TClonesArray("TLorentzVector", 10);
  }

  if (_doTrimuons || _doDimuTrk) {
    Reco_3mu_4mom = new TClonesArray("TLorentzVector", Max_Bc_size);
    Reco_3mu_vtx = new TClonesArray("TVector3", Max_Bc_size);

    if (_isMC) {
      Gen_Bc_4mom = new TClonesArray("TLorentzVector", 10);
      Gen_Bc_nuW_4mom = new TClonesArray("TLorentzVector", 10);
      Gen_3mu_4mom = new TClonesArray("TLorentzVector", 10);
    }
  }

  //myTree = new TTree("myTree","My TTree of dimuons");
  myTree = fs->make<TTree>("myTree", "My TTree of dimuons");

  myTree->Branch("eventNb", &eventNb, "eventNb/i");
  if(_genOnly) goto genOnly2;
  if (!_isMC) {
    myTree->Branch("runNb", &runNb, "runNb/i");
    myTree->Branch("LS", &lumiSection, "LS/i");
  }
  myTree->Branch("zVtx", &zVtx, "zVtx/F");
  myTree->Branch("nPV", &nPV, "nPV/S");
  if (_isHI || _isPA) {
    myTree->Branch("Centrality", &centBin, "Centrality/I");
    myTree->Branch("Npix", &Npix, "Npix/S");
    myTree->Branch("NpixelTracks", &NpixelTracks, "NpixelTracks/S");
  }
  myTree->Branch("Ntracks", &Ntracks, "Ntracks/S");

  //myTree->Branch("nTrig", &nTrig, "nTrig/I");
  myTree->Branch("trigPrescale", trigPrescale, Form("trigPrescale[%d]/I", nTrig));
  myTree->Branch("HLTriggers", &HLTriggers, "HLTriggers/l");

  if ((_isHI || _isPA) && _SumETvariables) {
    myTree->Branch("SumET_HF", &SumET_HF, "SumET_HF/F");
    myTree->Branch("SumET_HFplus", &SumET_HFplus, "SumET_HFplus/F");
    myTree->Branch("SumET_HFminus", &SumET_HFminus, "SumET_HFminus/F");
    myTree->Branch("SumET_HFplusEta4", &SumET_HFplusEta4, "SumET_HFplusEta4/F");
    myTree->Branch("SumET_HFminusEta4", &SumET_HFminusEta4, "SumET_HFminusEta4/F");
    myTree->Branch("SumET_ET", &SumET_ET, "SumET_ET/F");
    myTree->Branch("SumET_EE", &SumET_EE, "SumET_EE/F");
    myTree->Branch("SumET_EB", &SumET_EB, "SumET_EB/F");
    //myTree->Branch("SumET_EEplus", &SumET_EEplus, "SumET_EEplus/F");
    //myTree->Branch("SumET_EEminus", &SumET_EEminus, "SumET_EEminus/F");
    //myTree->Branch("SumET_ZDC", &SumET_ZDC, "SumET_ZDC/F");
    //myTree->Branch("SumET_ZDCplus", &SumET_ZDCplus, "SumET_ZDCplus/F");
    //myTree->Branch("SumET_ZDCminus", &SumET_ZDCminus, "SumET_ZDCminus/F");
  }

  if ((_isHI || _isPA) && _useEvtPlane) {
    myTree->Branch("nEP", &nEP, "nEP/I");
    myTree->Branch("rpAng", &rpAng, "rpAng[nEP]/F");
    myTree->Branch("rpSin", &rpSin, "rpSin[nEP]/F");
    myTree->Branch("rpCos", &rpCos, "rpCos[nEP]/F");
    myTree->Branch("rpAng_origin", &rpAng_origin, "rpAng_origin[nEP]/F");
    myTree->Branch("rpSin_origin", &rpSin_origin, "rpSin_origin[nEP]/F");
    myTree->Branch("rpCos_origin", &rpCos_origin, "rpCos_origin[nEP]/F");
  }

  if (!_onlySingleMuons) {
    if (_doTrimuons || _doDimuTrk) {
      myTree->Branch("Reco_3mu_size", &Reco_3mu_size, "Reco_3mu_size/S");
      myTree->Branch("Reco_3mu_charge", Reco_3mu_charge, "Reco_3mu_charge[Reco_3mu_size]/S");
      if (std::strcmp("array", _mom4format.c_str()) == 0)
        myTree->Branch("Reco_3mu_4mom", "TClonesArray", &Reco_3mu_4mom, 32000, 0);
      if (std::strcmp("vector", _mom4format.c_str()) == 0){
        myTree->Branch("Reco_3mu_4mom_pt", &Reco_3mu_4mom_pt, 32000, 0);
        myTree->Branch("Reco_3mu_4mom_eta", &Reco_3mu_4mom_eta, 32000, 0);
        myTree->Branch("Reco_3mu_4mom_y", &Reco_3mu_4mom_y, 32000, 0);
        myTree->Branch("Reco_3mu_4mom_phi", &Reco_3mu_4mom_phi, 32000, 0);
        myTree->Branch("Reco_3mu_4mom_m", &Reco_3mu_4mom_m, 32000, 0);
      }
      
      myTree->Branch("Reco_3mu_mupl_idx", Reco_3mu_mupl_idx, "Reco_3mu_mupl_idx[Reco_3mu_size]/S");
      myTree->Branch("Reco_3mu_mumi_idx", Reco_3mu_mumi_idx, "Reco_3mu_mumi_idx[Reco_3mu_size]/S");
      myTree->Branch("Reco_3mu_muW_idx", Reco_3mu_muW_idx, "Reco_3mu_muW_idx[Reco_3mu_size]/S");
      if (!_doDimuTrk)
        myTree->Branch("Reco_3mu_muW2_idx", Reco_3mu_muW2_idx, "Reco_3mu_muW2_idx[Reco_3mu_size]/S");
      myTree->Branch("Reco_3mu_QQ1_idx", Reco_3mu_QQ1_idx, "Reco_3mu_QQ1_idx[Reco_3mu_size]/S");
      if (!_doDimuTrk) {
        myTree->Branch("Reco_3mu_QQ2_idx", Reco_3mu_QQ2_idx, "Reco_3mu_QQ2_idx[Reco_3mu_size]/S");
        myTree->Branch("Reco_3mu_QQss_idx", Reco_3mu_QQss_idx, "Reco_3mu_QQss_idx[Reco_3mu_size]/S");
      }
      if (_isMC && _genealogyInfo) {
        myTree->Branch(
            "Reco_3mu_muW_isGenJpsiBro", Reco_3mu_muW_isGenJpsiBro, "Reco_3mu_muW_isGenJpsiBro[Reco_3mu_size]/O");
        myTree->Branch("Reco_3mu_muW_trueId", Reco_3mu_muW_trueId, "Reco_3mu_muW_trueId[Reco_3mu_size]/I");
      }

      myTree->Branch("Reco_3mu_ctau", Reco_3mu_ctau, "Reco_3mu_ctau[Reco_3mu_size]/F");
      myTree->Branch("Reco_3mu_ctauErr", Reco_3mu_ctauErr, "Reco_3mu_ctauErr[Reco_3mu_size]/F");
      myTree->Branch("Reco_3mu_cosAlpha", Reco_3mu_cosAlpha, "Reco_3mu_cosAlpha[Reco_3mu_size]/F");
      myTree->Branch("Reco_3mu_ctau3D", Reco_3mu_ctau3D, "Reco_3mu_ctau3D[Reco_3mu_size]/F");
      myTree->Branch("Reco_3mu_ctauErr3D", Reco_3mu_ctauErr3D, "Reco_3mu_ctauErr3D[Reco_3mu_size]/F");
      myTree->Branch("Reco_3mu_cosAlpha3D", Reco_3mu_cosAlpha3D, "Reco_3mu_cosAlpha3D[Reco_3mu_size]/F");

      if (_isMC) {
        myTree->Branch("Reco_3mu_whichGen", Reco_3mu_whichGen, "Reco_3mu_whichGen[Reco_3mu_size]/S");
      }
      myTree->Branch("Reco_3mu_VtxProb", Reco_3mu_VtxProb, "Reco_3mu_VtxProb[Reco_3mu_size]/F");

      if (_doDimuTrk) {
        myTree->Branch("Reco_3mu_KCVtxProb", Reco_3mu_KCVtxProb, "Reco_3mu_KCVtxProb[Reco_3mu_size]/F");
        myTree->Branch("Reco_3mu_KCctau", Reco_3mu_KCctau, "Reco_3mu_KCctau[Reco_3mu_size]/F");
        myTree->Branch("Reco_3mu_KCctauErr", Reco_3mu_KCctauErr, "Reco_3mu_KCctauErr[Reco_3mu_size]/F");
        myTree->Branch("Reco_3mu_KCcosAlpha", Reco_3mu_KCcosAlpha, "Reco_3mu_KCcosAlpha[Reco_3mu_size]/F");
        myTree->Branch("Reco_3mu_KCctau3D", Reco_3mu_KCctau3D, "Reco_3mu_KCctau3D[Reco_3mu_size]/F");
        myTree->Branch("Reco_3mu_KCctauErr3D", Reco_3mu_KCctauErr3D, "Reco_3mu_KCctauErr3D[Reco_3mu_size]/F");
        myTree->Branch("Reco_3mu_KCcosAlpha3D", Reco_3mu_KCcosAlpha3D, "Reco_3mu_KCcosAlpha3D[Reco_3mu_size]/F");
      }
      if ((!_theMinimumFlag && _muonLessPrimaryVertex) || (_flipJpsiDirection > 0)) {
        myTree->Branch(
            "Reco_3mu_muW_dxy_muonlessVtx", Reco_3mu_muW_dxy, "Reco_3mu_muW_dxy_muonlessVtx[Reco_3mu_size]/F");
        myTree->Branch("Reco_3mu_muW_dz_muonlessVtx", Reco_3mu_muW_dz, "Reco_3mu_muW_dz_muonlessVtx[Reco_3mu_size]/F");
        myTree->Branch(
            "Reco_3mu_mumi_dxy_muonlessVtx", Reco_3mu_mumi_dxy, "Reco_3mu_mumi_dxy_muonlessVtx[Reco_3mu_size]/F");
        myTree->Branch(
            "Reco_3mu_mumi_dz_muonlessVtx", Reco_3mu_mumi_dz, "Reco_3mu_mumi_dz_muonlessVtx[Reco_3mu_size]/F");
        myTree->Branch(
            "Reco_3mu_mupl_dxy_muonlessVtx", Reco_3mu_mupl_dxy, "Reco_3mu_mupl_dxy_muonlessVtx[Reco_3mu_size]/F");
        myTree->Branch(
            "Reco_3mu_mupl_dz_muonlessVtx", Reco_3mu_mupl_dz, "Reco_3mu_mupl_dz_muonlessVtx[Reco_3mu_size]/F");
      }

      myTree->Branch("Reco_3mu_MassErr", Reco_3mu_MassErr, "Reco_3mu_MassErr[Reco_3mu_size]/F");
      myTree->Branch("Reco_3mu_CorrM", Reco_3mu_CorrM, "Reco_3mu_CorrM[Reco_3mu_size]/F");
      if (_useSVfinder && SVs.isValid() && !SVs->empty()) {
        myTree->Branch("Reco_3mu_NbMuInSameSV", Reco_3mu_NbMuInSameSV, "Reco_3mu_NbMuInSameSV[Reco_3mu_size]/S");
      }
      myTree->Branch("Reco_3mu_vtx", "TClonesArray", &Reco_3mu_vtx, 32000, 0);
    }

    myTree->Branch("Reco_QQ_size", &Reco_QQ_size, "Reco_QQ_size/S");
    myTree->Branch("Reco_QQ_type", Reco_QQ_type, "Reco_QQ_type[Reco_QQ_size]/S");
    myTree->Branch("Reco_QQ_sign", Reco_QQ_sign, "Reco_QQ_sign[Reco_QQ_size]/S");
    if (std::strcmp("array", _mom4format.c_str()) == 0)
      myTree->Branch("Reco_QQ_4mom", "TClonesArray", &Reco_QQ_4mom, 32000, 0);
    if (std::strcmp("vector", _mom4format.c_str()) == 0) {
      myTree->Branch("Reco_QQ_4mom_pt", &Reco_QQ_4mom_pt, 32000, 0);
      myTree->Branch("Reco_QQ_4mom_eta", &Reco_QQ_4mom_eta, 32000, 0);
      myTree->Branch("Reco_QQ_4mom_y", &Reco_QQ_4mom_y, 32000, 0);
      myTree->Branch("Reco_QQ_4mom_phi", &Reco_QQ_4mom_phi, 32000, 0);
      myTree->Branch("Reco_QQ_4mom_m", &Reco_QQ_4mom_m, 32000, 0);
    }
    myTree->Branch("Reco_QQ_mupl_idx", Reco_QQ_mupl_idx, "Reco_QQ_mupl_idx[Reco_QQ_size]/S");
    myTree->Branch("Reco_QQ_mumi_idx", Reco_QQ_mumi_idx, "Reco_QQ_mumi_idx[Reco_QQ_size]/S");

    myTree->Branch("Reco_QQ_trig", Reco_QQ_trig, "Reco_QQ_trig[Reco_QQ_size]/l");
    myTree->Branch("Reco_QQ_ctau", Reco_QQ_ctau, "Reco_QQ_ctau[Reco_QQ_size]/F");
    myTree->Branch("Reco_QQ_ctauErr", Reco_QQ_ctauErr, "Reco_QQ_ctauErr[Reco_QQ_size]/F");
    myTree->Branch("Reco_QQ_cosAlpha", Reco_QQ_cosAlpha, "Reco_QQ_cosAlpha[Reco_QQ_size]/F");
    myTree->Branch("Reco_QQ_ctau3D", Reco_QQ_ctau3D, "Reco_QQ_ctau3D[Reco_QQ_size]/F");
    myTree->Branch("Reco_QQ_ctauErr3D", Reco_QQ_ctauErr3D, "Reco_QQ_ctauErr3D[Reco_QQ_size]/F");
    myTree->Branch("Reco_QQ_cosAlpha3D", Reco_QQ_cosAlpha3D, "Reco_QQ_cosAlpha3D[Reco_QQ_size]/F");

    if (_isMC) {
      myTree->Branch("Reco_QQ_whichGen", Reco_QQ_whichGen, "Reco_QQ_whichGen[Reco_QQ_size]/S");
    }
    myTree->Branch("Reco_QQ_VtxProb", Reco_QQ_VtxProb, "Reco_QQ_VtxProb[Reco_QQ_size]/F");
    myTree->Branch("Reco_QQ_dca", Reco_QQ_dca, "Reco_QQ_dca[Reco_QQ_size]/F");
    //myTree->Branch("Reco_QQ_MassErr", Reco_QQ_MassErr, "Reco_QQ_MassErr[Reco_QQ_size]/F");
    myTree->Branch("Reco_QQ_vtx", "TClonesArray", &Reco_QQ_vtx, 32000, 0);

    if ((!_theMinimumFlag && _muonLessPrimaryVertex) || (_flipJpsiDirection > 0)) {
      myTree->Branch("Reco_QQ_mupl_dxy_muonlessVtx", Reco_QQ_mupl_dxy, "Reco_QQ_mupl_dxy_muonlessVtx[Reco_QQ_size]/F");
      myTree->Branch("Reco_QQ_mumi_dxy_muonlessVtx", Reco_QQ_mumi_dxy, "Reco_QQ_mumi_dxy_muonlessVtx[Reco_QQ_size]/F");
      myTree->Branch("Reco_QQ_mupl_dz_muonlessVtx", Reco_QQ_mupl_dz, "Reco_QQ_mupl_dz_muonlessVtx[Reco_QQ_size]/F");
      myTree->Branch("Reco_QQ_mumi_dz_muonlessVtx", Reco_QQ_mumi_dz, "Reco_QQ_mumi_dz_muonlessVtx[Reco_QQ_size]/F");
    }
    if (_flipJpsiDirection > 0) {
      myTree->Branch("Reco_QQ_flipJpsi", Reco_QQ_flipJpsi, "Reco_QQ_flipJpsi[Reco_QQ_size]/S");
      if (std::strcmp("array", _mom4format.c_str()) == 0)
        myTree->Branch("Reco_QQ_mumi_4mom", "TClonesArray", &Reco_QQ_mumi_4mom, 32000, 0);
      if (std::strcmp("vector", _mom4format.c_str()) == 0) {
        myTree->Branch("Reco_QQ_mumi_4mom_pt", &Reco_QQ_mumi_4mom_pt, 32000, 0);
        myTree->Branch("Reco_QQ_mumi_4mom_eta", &Reco_QQ_mumi_4mom_eta, 32000, 0);
        myTree->Branch("Reco_QQ_mumi_4mom_phi", &Reco_QQ_mumi_4mom_phi, 32000, 0);
        myTree->Branch("Reco_QQ_mumi_4mom_m", &Reco_QQ_mumi_4mom_m, 32000, 0);
      }
      if (std::strcmp("array", _mom4format.c_str()) == 0)
        myTree->Branch("Reco_QQ_mupl_4mom", "TClonesArray", &Reco_QQ_mupl_4mom, 32000, 0);
      if (std::strcmp("vector", _mom4format.c_str()) == 0) {
        myTree->Branch("Reco_QQ_mupl_4mom_pt", &Reco_QQ_mupl_4mom_pt, 32000, 0);
        myTree->Branch("Reco_QQ_mupl_4mom_eta", &Reco_QQ_mupl_4mom_eta, 32000, 0);
        myTree->Branch("Reco_QQ_mupl_4mom_phi", &Reco_QQ_mupl_4mom_phi, 32000, 0);
        myTree->Branch("Reco_QQ_mupl_4mom_m", &Reco_QQ_mupl_4mom_m, 32000, 0);
      }
    }
  }

  myTree->Branch("Reco_mu_size", &Reco_mu_size, "Reco_mu_size/S");
  myTree->Branch("Reco_mu_type", Reco_mu_type, "Reco_mu_type[Reco_mu_size]/S");
  if (_isMC) {
    myTree->Branch("Reco_mu_whichGen", Reco_mu_whichGen, "Reco_mu_whichGen[Reco_mu_size]/S");
  }
  //myTree->Branch("Reco_mu_SelectionType", Reco_mu_SelectionType, "Reco_mu_SelectionType[Reco_mu_size]/I");
  myTree->Branch("Reco_mu_charge", Reco_mu_charge, "Reco_mu_charge[Reco_mu_size]/S");
  if (std::strcmp("array", _mom4format.c_str()) == 0) {
    myTree->Branch("Reco_mu_4mom", "TClonesArray", &Reco_mu_4mom, 32000, 0);
    myTree->Branch("Reco_mu_L1_4mom", "TClonesArray", &Reco_mu_L1_4mom, 32000, 0);
  }
  if (std::strcmp("vector", _mom4format.c_str()) == 0) {
    myTree->Branch("Reco_mu_4mom_pt", &Reco_mu_4mom_pt, 32000, 0);
    myTree->Branch("Reco_mu_4mom_eta", &Reco_mu_4mom_eta, 32000, 0);
    myTree->Branch("Reco_mu_4mom_phi", &Reco_mu_4mom_phi, 32000, 0);
    myTree->Branch("Reco_mu_4mom_m", &Reco_mu_4mom_m, 32000, 0);
    myTree->Branch("Reco_mu_L1_4mom_pt", &Reco_mu_L1_4mom_pt, 32000, 0);
    myTree->Branch("Reco_mu_L1_4mom_eta", &Reco_mu_L1_4mom_eta, 32000, 0);
    myTree->Branch("Reco_mu_L1_4mom_phi", &Reco_mu_L1_4mom_phi, 32000, 0);
    myTree->Branch("Reco_mu_L1_4mom_m", &Reco_mu_L1_4mom_m, 32000, 0);
  }
  myTree->Branch("Reco_mu_trig", Reco_mu_trig, "Reco_mu_trig[Reco_mu_size]/l");

  if (!_theMinimumFlag) {
    myTree->Branch("Reco_mu_InTightAcc", Reco_mu_InTightAcc, "Reco_mu_InTightAcc[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_InLooseAcc", Reco_mu_InLooseAcc, "Reco_mu_InLooseAcc[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_highPurity", Reco_mu_highPurity, "Reco_mu_highPurity[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_TMOneStaTight", Reco_mu_TMOneStaTight, "Reco_mu_TMOneStaTight[Reco_mu_size]/O");
    // myTree->Branch("Reco_mu_TrkMuArb", Reco_mu_TrkMuArb,   "Reco_mu_TrkMuArb[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_isPF", Reco_mu_isPF, "Reco_mu_isPF[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_isTracker", Reco_mu_isTracker, "Reco_mu_isTracker[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_isGlobal", Reco_mu_isGlobal, "Reco_mu_isGlobal[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_isSoftCutBased", Reco_mu_isSoftCutBased, "Reco_mu_isSoftCutBased[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_softMvaRun3Value", Reco_mu_softMvaRun3Value, "Reco_mu_softMvaRun3Value[Reco_mu_size]/F");
    myTree->Branch("Reco_mu_isHybridSoft", Reco_mu_isHybridSoft, "Reco_mu_isHybridSoft[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_isMediumCutBased", Reco_mu_isMediumCutBased, "Reco_mu_isMediumCutBased[Reco_mu_size]/O");
    myTree->Branch("Reco_mu_isTightCutBased", Reco_mu_isTightCutBased, "Reco_mu_isTightCutBased[Reco_mu_size]/O");

    //myTree->Branch("Reco_mu_candType", Reco_mu_candType, "Reco_mu_candType[Reco_mu_size]/S");
    myTree->Branch("Reco_mu_nPixValHits", Reco_mu_nPixValHits, "Reco_mu_nPixValHits[Reco_mu_size]/I");
    myTree->Branch("Reco_mu_nMuValHits", Reco_mu_nMuValHits, "Reco_mu_nMuValHits[Reco_mu_size]/I");
    myTree->Branch("Reco_mu_nTrkHits", Reco_mu_nTrkHits, "Reco_mu_nTrkHits[Reco_mu_size]/I");
    //myTree->Branch("Reco_mu_segmentComp", Reco_mu_segmentComp, "Reco_mu_segmentComp[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_kink", Reco_mu_kink, "Reco_mu_kink[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_localChi2", Reco_mu_localChi2, "Reco_mu_localChi2[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_validFraction", Reco_mu_validFraction, "Reco_mu_validFraction[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_normChi2_bestTracker", Reco_mu_normChi2_bestTracker, "Reco_mu_normChi2_bestTracker[Reco_mu_size]/F");
    myTree->Branch("Reco_mu_normChi2_inner", Reco_mu_normChi2_inner, "Reco_mu_normChi2_inner[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_normChi2_global", Reco_mu_normChi2_global, "Reco_mu_normChi2_global[Reco_mu_size]/F");
    myTree->Branch("Reco_mu_nPixWMea", Reco_mu_nPixWMea, "Reco_mu_nPixWMea[Reco_mu_size]/I");
    myTree->Branch("Reco_mu_nTrkWMea", Reco_mu_nTrkWMea, "Reco_mu_nTrkWMea[Reco_mu_size]/I");
    //myTree->Branch("Reco_mu_StationsMatched", Reco_mu_StationsMatched, "Reco_mu_StationsMatched[Reco_mu_size]/I");
    //myTree->Branch("Reco_mu_dxy", Reco_mu_dxy, "Reco_mu_dxy[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_dxyErr", Reco_mu_dxyErr, "Reco_mu_dxyErr[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_dz", Reco_mu_dz, "Reco_mu_dz[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_dzErr", Reco_mu_dzErr, "Reco_mu_dzErr[Reco_mu_size]/F");
    // myTree->Branch("Reco_mu_pt_inner",Reco_mu_pt_inner, "Reco_mu_pt_inner[Reco_mu_size]/F");
    // myTree->Branch("Reco_mu_pt_global",Reco_mu_pt_global, "Reco_mu_pt_global[Reco_mu_size]/F");
    //myTree->Branch("Reco_mu_ptErr_inner", Reco_mu_ptErr_inner, "Reco_mu_ptErr_inner[Reco_mu_size]/F");
    // myTree->Branch("Reco_mu_ptErr_global",Reco_mu_ptErr_global, "Reco_mu_ptErr_global[Reco_mu_size]/F");
  }

  if (_useGeTracks && _fillRecoTracks) {
    if (!_doDimuTrk) {
      myTree->Branch("Reco_QQ_NtrkPt02", Reco_QQ_NtrkPt02, "Reco_QQ_NtrkPt02[Reco_QQ_size]/I");
      myTree->Branch("Reco_QQ_NtrkPt03", Reco_QQ_NtrkPt03, "Reco_QQ_NtrkPt03[Reco_QQ_size]/I");
      myTree->Branch("Reco_QQ_NtrkPt04", Reco_QQ_NtrkPt04, "Reco_QQ_NtrkPt04[Reco_QQ_size]/I");
      myTree->Branch("Reco_QQ_NtrkDeltaR03", Reco_QQ_NtrkDeltaR03, "Reco_QQ_NtrkDeltaR03[Reco_QQ_size]/I");
      myTree->Branch("Reco_QQ_NtrkDeltaR04", Reco_QQ_NtrkDeltaR04, "Reco_QQ_NtrkDeltaR04[Reco_QQ_size]/I");
      myTree->Branch("Reco_QQ_NtrkDeltaR05", Reco_QQ_NtrkDeltaR05, "Reco_QQ_NtrkDeltaR05[Reco_QQ_size]/I");
    }

    myTree->Branch("Reco_trk_size", &Reco_trk_size, "Reco_trk_size/S");
    myTree->Branch("Reco_trk_charge", Reco_trk_charge, "Reco_trk_charge[Reco_trk_size]/S");
    myTree->Branch("Reco_trk_highPurity", Reco_trk_highPurity, "Reco_trk_highPurity[Reco_trk_size]/O");
    myTree->Branch("Reco_trk_InLooseAcc", Reco_trk_InLooseAcc, "Reco_trk_InLooseAcc[Reco_trk_size]/O");
    myTree->Branch("Reco_trk_InTightAcc", Reco_trk_InTightAcc, "Reco_trk_InTightAcc[Reco_trk_size]/O");
    if (std::strcmp("array", _mom4format.c_str()) == 0) {
      myTree->Branch("Reco_trk_4mom", "TClonesArray", &Reco_trk_4mom, 32000, 0);
    }
    if (std::strcmp("vector", _mom4format.c_str()) == 0) {
      myTree->Branch("Reco_trk_4mom_pt", &Reco_trk_4mom_pt, 32000, 0);
      myTree->Branch("Reco_trk_4mom_eta", &Reco_trk_4mom_eta, 32000, 0);
      myTree->Branch("Reco_trk_4mom_phi", &Reco_trk_4mom_phi, 32000, 0);
      myTree->Branch("Reco_trk_4mom_m", &Reco_trk_4mom_m, 32000, 0);
    }
    myTree->Branch("Reco_trk_vtx", "TClonesArray", &Reco_trk_vtx, 32000, 0);
    myTree->Branch("Reco_trk_dxyError", Reco_trk_dxyError, "Reco_trk_dxyError[Reco_trk_size]/F");
    myTree->Branch("Reco_trk_dzError", Reco_trk_dzError, "Reco_trk_dzError[Reco_trk_size]/F");
    myTree->Branch("Reco_trk_dxy", Reco_trk_dxy, "Reco_trk_dxy[Reco_trk_size]/F");
    myTree->Branch("Reco_trk_dz", Reco_trk_dz, "Reco_trk_dz[Reco_trk_size]/F");
    myTree->Branch("Reco_trk_ptErr", Reco_trk_ptErr, "Reco_trk_ptErr[Reco_trk_size]/F");
    //myTree->Branch("Reco_trk_originalAlgo", Reco_trk_originalAlgo, "Reco_trk_originalAlgo[Reco_trk_size]/I");
    myTree->Branch("Reco_trk_nPixWMea", Reco_trk_nPixWMea, "Reco_trk_nPixWMea[Reco_trk_size]/I");
    myTree->Branch("Reco_trk_nTrkWMea", Reco_trk_nTrkWMea, "Reco_trk_nTrkWMea[Reco_trk_size]/I");
    if (_isMC) {
      myTree->Branch("Reco_trk_whichGenmu", Reco_trk_whichGenmu, "Reco_trk_whichGenmu[Reco_trk_size]/S");
    }
  }

genOnly2: 
  if (_isMC) {
    if (_genealogyInfo) {
      myTree->Branch("Reco_mu_simExtType", Reco_mu_simExtType, "Reco_mu_simExtType[Reco_mu_size]/I");
    }
    
    myTree->Branch("Gen_weight", &Gen_weight, "Gen_weight/F");
    myTree->Branch("Gen_pthat", &Gen_pthat, "Gen_pthat/F");

    if (!_onlySingleMuons) {
      myTree->Branch("Gen_QQ_size", &Gen_QQ_size, "Gen_QQ_size/S");
      //myTree->Branch("Gen_QQ_type",      Gen_QQ_type,    "Gen_QQ_type[Gen_QQ_size]/S");
      if (std::strcmp("array", _mom4format.c_str()) == 0) {
	      myTree->Branch("Gen_QQ_4mom", "TClonesArray", &Gen_QQ_4mom, 32000, 0);
      }
      if (std::strcmp("vector", _mom4format.c_str()) == 0) {
	      myTree->Branch("Gen_QQ_4mom_pt", &Gen_QQ_4mom_pt, 32000, 0);
	      myTree->Branch("Gen_QQ_4mom_eta", &Gen_QQ_4mom_eta, 32000, 0);
        myTree->Branch("Gen_QQ_4mom_y", &Gen_QQ_4mom_y, 32000, 0);
	      myTree->Branch("Gen_QQ_4mom_phi", &Gen_QQ_4mom_phi, 32000, 0);
	      myTree->Branch("Gen_QQ_4mom_m", &Gen_QQ_4mom_m, 32000, 0);
      }
      myTree->Branch("Gen_QQ_ctau", Gen_QQ_ctau, "Gen_QQ_ctau[Gen_QQ_size]/F");
      myTree->Branch("Gen_QQ_ctau3D", Gen_QQ_ctau3D, "Gen_QQ_ctau3D[Gen_QQ_size]/F");
      myTree->Branch("Gen_QQ_mupl_idx", Gen_QQ_mupl_idx, "Gen_QQ_mupl_idx[Gen_QQ_size]/S");
      myTree->Branch("Gen_QQ_mumi_idx", Gen_QQ_mumi_idx, "Gen_QQ_mumi_idx[Gen_QQ_size]/S");
      myTree->Branch("Gen_QQ_whichRec", Gen_QQ_whichRec, "Gen_QQ_whichRec[Gen_QQ_size]/S");
      if (_genealogyInfo) {
        myTree->Branch("Gen_QQ_momId", Gen_QQ_momId, "Gen_QQ_momId[Gen_QQ_size]/I");
      }

      if (_doTrimuons || _doDimuTrk) {
        myTree->Branch("Gen_QQ_Bc_idx", Gen_QQ_Bc_idx, "Gen_QQ_Bc_idx[Gen_QQ_size]/S");
        myTree->Branch("Gen_Bc_size", &Gen_Bc_size, "Gen_Bc_size/S");
        if (std::strcmp("array", _mom4format.c_str()) == 0) {
          myTree->Branch("Gen_Bc_4mom", "TClonesArray", &Gen_Bc_4mom, 32000, 0);
          myTree->Branch("Gen_Bc_nuW_4mom", "TClonesArray", &Gen_Bc_nuW_4mom, 32000, 0);
          myTree->Branch("Gen_3mu_4mom", "TClonesArray", &Gen_3mu_4mom, 32000, 0);
        }
        if (std::strcmp("vector", _mom4format.c_str()) == 0) {
          myTree->Branch("Gen_Bc_4mom_pt", &Gen_Bc_4mom_pt, 32000, 0);
          myTree->Branch("Gen_Bc_4mom_eta", &Gen_Bc_4mom_eta, 32000, 0);
          myTree->Branch("Gen_Bc_4mom_y", &Gen_Bc_4mom_y, 32000, 0);
          myTree->Branch("Gen_Bc_4mom_phi", &Gen_Bc_4mom_phi, 32000, 0);
          myTree->Branch("Gen_Bc_4mom_m", &Gen_Bc_4mom_m, 32000, 0);
          myTree->Branch("Gen_Bc_nuW_4mom_pt", &Gen_Bc_nuW_4mom_pt, 32000, 0);
          myTree->Branch("Gen_Bc_nuW_4mom_eta", &Gen_Bc_nuW_4mom_eta, 32000, 0);
          myTree->Branch("Gen_Bc_nuW_4mom_y", &Gen_Bc_nuW_4mom_y, 32000, 0);
          myTree->Branch("Gen_Bc_nuW_4mom_phi", &Gen_Bc_nuW_4mom_phi, 32000, 0);
          myTree->Branch("Gen_Bc_nuW_4mom_m", &Gen_Bc_nuW_4mom_m, 32000, 0);
          myTree->Branch("Gen_3mu_4mom_pt", &Gen_3mu_4mom_pt, 32000, 0);
          myTree->Branch("Gen_3mu_4mom_eta", &Gen_3mu_4mom_eta, 32000, 0);
          myTree->Branch("Gen_3mu_4mom_y", &Gen_3mu_4mom_y, 32000, 0);
          myTree->Branch("Gen_3mu_4mom_phi", &Gen_3mu_4mom_phi, 32000, 0);
          myTree->Branch("Gen_3mu_4mom_m", &Gen_3mu_4mom_m, 32000, 0);
        }
        myTree->Branch("Gen_Bc_QQ_idx", Gen_Bc_QQ_idx, "Gen_Bc_QQ_idx[Gen_Bc_size]/S");
        myTree->Branch("Gen_Bc_muW_idx", Gen_Bc_muW_idx, "Gen_Bc_muW_idx[Gen_Bc_size]/S");
        myTree->Branch("Gen_Bc_pdgId", Gen_Bc_pdgId, "Gen_Bc_pdgId[Gen_Bc_size]/I");
        myTree->Branch("Gen_Bc_ctau", Gen_Bc_ctau, "Gen_Bc_ctau[Gen_Bc_size]/F");

        myTree->Branch("Gen_3mu_whichRec", Gen_3mu_whichRec, "Gen_3mu_whichRec[Gen_Bc_size]/S");
      }
    }

    myTree->Branch("Gen_mu_size", &Gen_mu_size, "Gen_mu_size/S");
    //myTree->Branch("Gen_mu_type",   Gen_mu_type,   "Gen_mu_type[Gen_mu_size]/S");
    myTree->Branch("Gen_mu_charge", Gen_mu_charge, "Gen_mu_charge[Gen_mu_size]/S");
    if (std::strcmp("array", _mom4format.c_str()) == 0) {
      myTree->Branch("Gen_mu_4mom", "TClonesArray", &Gen_mu_4mom, 32000, 0);
    }
    if (std::strcmp("vector", _mom4format.c_str()) == 0) {
      myTree->Branch("Gen_mu_4mom_pt", &Gen_mu_4mom_pt, 32000, 0);
      myTree->Branch("Gen_mu_4mom_eta", &Gen_mu_4mom_eta, 32000, 0);
      myTree->Branch("Gen_mu_4mom_phi", &Gen_mu_4mom_phi, 32000, 0);
      myTree->Branch("Gen_mu_4mom_m", &Gen_mu_4mom_m, 32000, 0);
    }
    myTree->Branch("Gen_mu_whichRec", Gen_mu_whichRec, "Gen_mu_whichRec[Gen_mu_size]/S");
  }

  return;
};

// ------------ method called once each job just before starting event loop  ------------
void HiOniaAnalyzer::beginJob() {
  InitTree();

  // book histos
  hGoodMuonsNoTrig = fs->make<TH1F>("hGoodMuonsNoTrig", "hGoodMuonsNoTrig", 10, 0, 10);
  hGoodMuons = fs->make<TH1F>("hGoodMuons", "hGoodMuons", 10, 0, 10);
  hL1DoubleMu0 = fs->make<TH1F>("hL1DoubleMu0", "hL1DoubleMu0", 10, 0, 10);

  hGoodMuonsNoTrig->Sumw2();
  hGoodMuons->Sumw2();
  hL1DoubleMu0->Sumw2();

  // muons
  if (_combineCategories)
    myRecoMuonHistos = new MyCommonHistoManager("RecoMuon");
  else {
    myRecoGlbMuonHistos = new MyCommonHistoManager("GlobalMuon");
    myRecoTrkMuonHistos = new MyCommonHistoManager("TrackerMuon");
  }

  // J/psi
  if (_combineCategories)
    myRecoJpsiHistos = new MyCommonHistoManager("RecoJpsi");
  else {
    myRecoJpsiGlbGlbHistos = new MyCommonHistoManager("GlbGlbJpsi");
    myRecoJpsiGlbTrkHistos = new MyCommonHistoManager("GlbTrkJpsi");
    myRecoJpsiTrkTrkHistos = new MyCommonHistoManager("TrkTrkJpsi");
  }

  for (unsigned int i = 0; i < theRegions.size(); ++i) {
    for (unsigned int j = 0; j < NTRIGGERS; ++j) {
      for (unsigned int k = 0; k < theCentralities.size(); ++k) {
        std::string theAppendix = theRegions.at(i);
        theAppendix += "_" + theTriggerNames.at(j);
        theAppendix += "_" + theCentralities.at(k);

        // muons
        if (_combineCategories) {
          myRecoMuonHistos->Add(theAppendix);
          myRecoMuonHistos->GetHistograms(theAppendix)->SetMassBinning(1, 0.10, 0.11);
          myRecoMuonHistos->GetHistograms(theAppendix)->SetPtBinning(200, 0.0, 100.0);
        } else {
          myRecoGlbMuonHistos->Add(theAppendix);
          myRecoTrkMuonHistos->Add(theAppendix);

          myRecoGlbMuonHistos->GetHistograms(theAppendix)->SetMassBinning(1, 0.10, 0.11);
          myRecoGlbMuonHistos->GetHistograms(theAppendix)->SetPtBinning(200, 0.0, 100.0);

          myRecoTrkMuonHistos->GetHistograms(theAppendix)->SetMassBinning(1, 0.10, 0.11);
          myRecoTrkMuonHistos->GetHistograms(theAppendix)->SetPtBinning(200, 0.0, 100.0);
        }

        for (unsigned int l = 0; l < theSign.size(); ++l) {
          // J/psi
          if (_combineCategories)
            myRecoJpsiHistos->Add(theAppendix + "_" + theSign.at(l));
          else {
            myRecoJpsiGlbGlbHistos->Add(theAppendix + "_" + theSign.at(l));
            myRecoJpsiGlbTrkHistos->Add(theAppendix + "_" + theSign.at(l));
            myRecoJpsiTrkTrkHistos->Add(theAppendix + "_" + theSign.at(l));
          }
        }
      }
    }
  }
  hStats = fs->make<TH1F>("hStats", "hStats;;Number of Events", 2 * NTRIGGERS + 1, 0, 2 * NTRIGGERS + 1);
  hStats->GetXaxis()->SetBinLabel(1, "All");
  for (int i = 2; i < (int)theTriggerNames.size() + 1; ++i) {
    hStats->GetXaxis()->SetBinLabel(i, theTriggerNames.at(i - 1).c_str());              // event info
    hStats->GetXaxis()->SetBinLabel(i + NTRIGGERS, theTriggerNames.at(i - 1).c_str());  // muon pair info
  }
  hStats->Sumw2();

  hCent = fs->make<TH1F>("hCent", "hCent;centrality bin;Number of Events", 200, 0, 200);
  hCent->Sumw2();

  hPileUp = fs->make<TH1F>("hPileUp", "Number of Primary Vertices;n_{PV};counts", 50, 0, 50);
  hPileUp->Sumw2();

  hZVtx = fs->make<TH1F>("hZVtx", "Primary z-vertex distribution;z_{vtx} [cm];counts", 120, -30, 30);
  hZVtx->Sumw2();

  return;
};

void HiOniaAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  //init HLTConfigProvider

  EDConsumerBase::Labels labelTriggerResults;
  EDConsumerBase::labelsForToken(_tagTriggerResultsToken, labelTriggerResults);
  const std::string pro = labelTriggerResults.process;

  bool changed = true;
  hltConfigInit = hltConfig.init(iRun, iSetup, pro, changed);

  changed = true;
  hltPrescaleInit = hltPrescaleProvider.init(iRun, iSetup, pro, changed);

  //extract trigger path names
  for (const auto& pathLabel : theTriggerNames)
    triggerNameMap[pathLabel] = "";

  for (const auto& hltPath : hltConfig.triggerNames())
    if (hltPath.rfind("HLT_", 0) == 0)
      for (const auto& pathLabel : theTriggerNames)
        if (hltPath != "NoString" && TString(hltPath).Contains(TRegexp(TString(pathLabel)))) {
          triggerNameMap.at(pathLabel) = hltPath;
          break;
        }

  //extract last filter names
  for (const auto& p : triggerNameMap) {
    filterNameMap[p.first] = "";
    if (p.second.empty())
      continue;
    const auto& m = hltConfig.moduleLabels(hltConfig.triggerIndex(p.second));
    for (int j = m.size() - 1; j >= 0; j--)
      if ( (m[j].rfind("hltL", 0) == 0 && m[j].rfind("Filtered") != std::string::npos) || (m[j].rfind("hltL1s", 0) == 0) ) {
        filterNameMap.at(p.first) = m[j];
        break;
      }
  }
  return;
};

// ------------ method called once each job just after ending the event loop  ------------
void HiOniaAnalyzer::endJob() {
  std::cout << "Total number of events = " << nEvents << std::endl;
  std::cout << "Total number of passed candidates = " << passedCandidates << std::endl;
  return;
};

//define this as a plug-in
DEFINE_FWK_MODULE(HiOniaAnalyzer);