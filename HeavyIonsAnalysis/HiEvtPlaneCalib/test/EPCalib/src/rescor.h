#include "TTree.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLatex.h"
#include "TMath.h"
#include "TPaveText.h"
#include <iostream>
#include <iomanip>

using namespace std;

#include "EPCalib/HiEvtPlaneList.h"
using namespace hi;


void ResCor(Double_t mincent, Double_t maxcent, Double_t delcent, Double_t minvtx, Double_t maxvtx ){
TFile * tf;
TTree * tr;
  FILE * fout[NumEPNames];
  for(int i = 0; i<NumEPNames; i++) {
    if(mincent==0) {
      fout[i] = fopen(Form("RescorTables/%s_%04.1f.dat",EPNames[i].data(),delcent),"w");
    } else {
      fout[i] = fopen(Form("RescorTables/%s_%04.1f_Special_%04.1f_%04.1f.dat",EPNames[i].data(),delcent,mincent,maxcent),"w");
    }
  }
  tf = new TFile("EP.root");
  tr = (TTree *) tf->Get("EPtree");
  int nbins = (int) ( (maxcent-mincent)/delcent+0.1 );
  Float_t full[200];
  Float_t Cent;
  Float_t Vtx;
  tr->SetBranchAddress("EPAngs",full);
  tr->SetBranchAddress("Cent",&Cent);
  tr->SetBranchAddress("Vtx",&Vtx);
  double cnt[200][200];
  double cora[200][200];
  double corb[200][200];
  double corc[200][200];
  double cora2[200][200];
  double corb2[200][200];
  double corc2[200][200];
  double siga;
  double sigb;
  double sigc;
  for(int i = 0; i<200; i++) {
    for(int j = 0; j<200; j++) {
      cnt[i][j]=0; cora[i][j]=0; corb[i][j]=0; corc[i][j]=0; cora2[i][j]=0; corb2[i][j]=0; corc2[i][j]=0;
    }
  }
  for(int ievent = 0; ievent<tr->GetEntries(); ievent++) {
    if(tr->GetEntry(ievent)<=0) continue;
    if(Cent < mincent) continue;
    if(Cent > maxcent) continue;
    if(Vtx < minvtx) continue;
    if(Vtx > maxvtx) continue;
    for(int i = 0; i<NumEPNames; i++) {
    double order = EPOrder[i];
    double ang[200];
    double ang1[200];
    double ang2[200];
    ang[i] = full[i];
    ang1[i] = full[RCMate1[i]];
    ang2[i] = full[RCMate2[i]];
    int lbin = (Cent-mincent)/delcent;
    if(ResCalcType[i][0]=='3') {
      if(ang[i]>-5&&ang1[i]>-5&&ang2[i]>-5) {
	cora[lbin][i]+=TMath::Cos( order*(ang[i] - ang1[i]) );
	cora2[lbin][i]+=pow(TMath::Cos( order*(ang[i] - ang1[i]) ),2);
	corb[lbin][i]+=TMath::Cos( order*(ang[i] - ang2[i]) );
	corb2[lbin][i]+=pow(TMath::Cos( order*(ang[i] - ang2[i]) ),2);
	corc[lbin][i]+=TMath::Cos( order*(ang2[i] - ang1[i]) );
	corc2[lbin][i]+=pow(TMath::Cos( order*(ang2[i] - ang1[i]) ),2);
	++cnt[lbin][i];
      }
    } else {
      if(ang[i]>-5 && ang1[i]>-5) {
	cora[lbin][i]+=TMath::Cos( order*(ang[i] - ang1[i]) );
	cora2[lbin][i]+=pow(TMath::Cos( order*(ang[i] - ang1[i]) ), 2);
	++cnt[lbin][i];
      }
    }
    }
  }
  for(int i = 0; i<nbins; i++) {
    for(int j = 0; j<NumEPNames; j++) {
      if(cnt[i][j]<=0) continue; 
      cora[i][j]/=cnt[i][j];
      corb[i][j]/=cnt[i][j];
      corc[i][j]/=cnt[i][j];
      cora2[i][j]/=cnt[i][j];
      corb2[i][j]/=cnt[i][j];
      corc2[i][j]/=cnt[i][j];
      siga = sqrt(cora2[i][j] - pow(cora[i][j],2))/sqrt(cnt[i][j]);
      if(ResCalcType[j][0]=='3') {
	sigb = sqrt(corb2[i][j] - pow(corb[i][j],2))/sqrt(cnt[i][j]);
	sigc = sqrt(corc2[i][j] - pow(corc[i][j],2))/sqrt(cnt[i][j]);
      }
      double resc = 0;
      double err = 0;
      if(ResCalcType[j][0]=='3') {
	resc = cora[i][j] * corb[i][j]/corc[i][j];
	err = resc*sqrt(pow(siga/cora[i][j],2)+pow(sigb/corb[i][j],2)+pow(sigc/corc[i][j],2));
	if(resc>0) {
	  resc = TMath::Sqrt(resc);
	  err = 0.5*err/resc;
	} else {
	  resc = -resc;
	  resc = TMath::Sqrt(resc);
	  err = -0.5*err/resc;
	}
	
      } else {
	resc = cora[i][j] ;
	if(resc>0) {
	  resc = sqrt(resc);
	  err = 0.5*siga/resc;
	} else {
	  resc = -resc;
	  resc = sqrt(resc);
	  err = 0.5*fabs(siga)/resc;
	}
      }
      double cmin = mincent+i*delcent;
      double cmax = cmin+delcent;
      fprintf(fout[j],"%5.1f\t%5.1f\t%7.5f\t%7.5f\n",cmin,cmax,resc,err);

    }
  }
  for(int i = 0; i<NumEPNames; i++) fclose(fout[i]);
  return;
}

void rescor(){
  ResCor(0,100,1,-15,15);
  ResCor(0,100,2,-15,15);
  ResCor(0,100,5,-15,15);
  ResCor(0,100,10,-15,15);
  ResCor(0,100,20,-15,15);
  ResCor(0,100,25,-15,15);
  ResCor(0,90,30,-15,15);
  ResCor(0,80,40,-15,15);
  ResCor(20,10,90,-15,15);
}
