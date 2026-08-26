#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>

#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"

using namespace std;

class MyCommonHistograms;

class binning {
public:
  //binning();
  binning(int n, float min, float max) {
    nBins = n;
    minVal = min;
    maxVal = max;
  };
  //~binning();

  inline void SetBinning(int n, float min, float max) {
    nBins = n;
    minVal = min;
    maxVal = max;
    return;
  };

  inline void SetNbins(int n) {
    nBins = n;
    return;
  };
  inline void SetMinVal(float min) {
    minVal = min;
    return;
  };
  inline void SetMaxVal(float max) {
    maxVal = max;
    return;
  };

  inline int GetNbins() { return nBins; };
  inline float GetMinVal() { return minVal; };
  inline float GetMaxVal() { return maxVal; };

private:
  int nBins;
  float minVal;
  float maxVal;
};

class MyCommonHistograms {
public:
  //  MyCommonHistograms();
  MyCommonHistograms(std::string theFullName);
  ~MyCommonHistograms() {
    delete hMass;
    delete hE;
    delete hPt;
    delete hEta;
    delete hPhi;
    delete hE_Mass;
    delete hPt_Mass;
    delete hEta_Mass;
    delete hPhi_Mass;
    delete hCent_Mass;
    delete hCtau_Mass;
    delete hE_Eta;
    delete hPt_Eta;
    delete hPhi_Eta;
    delete hE_Phi;
    delete hPt_Phi;

    delete theMassBinning;
    delete theEBinning;
    delete thePtBinning;
    delete theEtaBinning;
    delete thePhiBinning;
    delete theCentBinning;
    delete theCtauBinning;
    delete the3dEBinning;
    delete the3dPtBinning;
    delete the3dEtaBinning;
  };

  inline void SetMassBinning(int n, float min, float max) {
    theMassBinning->SetBinning(n, min, max);
    return;
  };
  inline void SetEBinning(int n, float min, float max) {
    theEBinning->SetBinning(n, min, max);
    return;
  };
  inline void SetPtBinning(int n, float min, float max) {
    thePtBinning->SetBinning(n, min, max);
    return;
  };
  inline void SetEtaBinning(int n, float min, float max) {
    theEtaBinning->SetBinning(n, min, max);
    return;
  };
  inline void SetPhiBinning(int n, float min, float max) {
    thePhiBinning->SetBinning(n, min, max);
    return;
  };
  inline void SetCentBinning(int n, float min, float max) {
    theCentBinning->SetBinning(n, min, max);
    return;
  };
  inline void SetCtauBinning(int n, float min, float max) {
    theCtauBinning->SetBinning(n, min, max);
    return;
  };

  inline void Set3dEBinning(int n, float min, float max) {
    the3dEBinning->SetBinning(n, min, max);
    return;
  };
  inline void Set3dPtBinning(int n, float min, float max) {
    the3dPtBinning->SetBinning(n, min, max);
    return;
  };
  inline void Set3dEtaBinning(int n, float min, float max) {
    the3dEtaBinning->SetBinning(n, min, max);
    return;
  };

  void Fill(const reco::Candidate *p);
  void Fill(const reco::Candidate *p1, const reco::Candidate *p2, std::string hName2);

  void Write(TFile *outf);

private:
  void BookParticleHistos();
  void BookParticleHistos(std::string hName2);
  std::string MakeLabel(std::string hName);

  binning *theMassBinning = nullptr;
  binning *theEBinning = nullptr;
  binning *thePtBinning = nullptr;
  binning *theEtaBinning = nullptr;
  binning *thePhiBinning = nullptr;
  binning *theCentBinning = nullptr;
  binning *theCtauBinning = nullptr;

  binning *the3dEBinning = nullptr;
  binning *the3dPtBinning = nullptr;
  binning *the3dEtaBinning = nullptr;

  std::string hName;
  std::string hLabel;
  std::string hLabel2;
  std::string hAppendix;

  bool useRapidity;

  TH1F *hMass = nullptr;
  TH1F *hE = nullptr;
  TH1F *hPt = nullptr;
  TH1F *hEta = nullptr;
  TH1F *hPhi = nullptr;

  TH2F *hE_Mass = nullptr;
  TH2F *hPt_Mass = nullptr;
  TH2F *hEta_Mass = nullptr;
  TH2F *hPhi_Mass = nullptr;
  TH2F *hCent_Mass = nullptr;
  TH2F *hCtau_Mass = nullptr;

  TH2F *hE_Eta = nullptr;
  TH2F *hPt_Eta = nullptr;
  TH2F *hPhi_Eta = nullptr;

  TH2F *hE_Phi = nullptr;
  TH2F *hPt_Phi = nullptr;

  bool booked1ParticleHistos;
  bool booked2ParticleHistos;
};

class MyCommonHistoManager {
public:
  MyCommonHistoManager(std::string theName) { hName = theName; };
  ~MyCommonHistoManager() {
    for (Size_t i = 0; i < myHistos.size(); i++) {
      delete myHistos[i];
    }
  };

  inline void SetName(std::string theName) {
    hName = theName;
    return;
  };
  void Add(std::string theAppendix, std::string theName2 = "");

  void Fill(const reco::Candidate *p, std::string theFullName);
  void Fill(const reco::Candidate *p1, const reco::Candidate *p2, std::string theFullName, std::string theName2);
  void Print();
  void Write(TFile *outf);

  MyCommonHistograms *GetHistograms(std::string theName) { return myHistos.at(theCategories.find(theName)->second); };
  MyCommonHistograms *GetHistograms(std::string theName, std::string theName2) {
    return myHistos.at(theCategories.find(theName + "_" + theName2)->second);
  };

private:
  std::map<std::string, int> theCategories;
  std::map<std::string, int>::iterator idx;
  std::pair<std::map<std::string, int>::iterator, bool> ret;

  vector<MyCommonHistograms *> myHistos;
  std::string hName;
};
