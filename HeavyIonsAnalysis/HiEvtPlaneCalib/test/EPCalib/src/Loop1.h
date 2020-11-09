#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TDirectory.h"
#include <iostream>
#include "string.h"
#include "stdio.h"

#include "EPCalib/HiEvtPlaneList.h"
#include "EPCalib/HiEvtPlaneFlatten.h"
void Loop1(){

    for(int i = 0; i<nentries; i++) {
      if(tree->GetEntry(i)<=0) continue;
      if(runno_ < minRun_ || runno_ > maxRun_) continue;
      if(useNtrk) bin=NtrkToBin(ntrkval);
      sout.vtx = vtx;
      sout.bin = bin;
      sout.cent = centval;
      sout.run = runno_;
      sout.ntrk = ntrkval;
      ++totentries;
      for(int j = 0; j<NumEPNames; j++) {
	int indx = flat[j]->getOffsetIndx(bin,vtx);
  	sout.ws[j] = 0;
  	sout.wc[j] = 0;
  	sout.msum[j] = 0;
  	sout.wsum[j] = 0;
  	double order = EPOrder[j];
  	if(wsv[j]==0&&wcv[j]==0) continue;
	double scale = flat[j]->getEtScale(vtx,bin);
	double s = wsv[j]*scale;
	double c = wcv[j]*scale;
	double snow = wsv_no_w[j]*scale;
	double cnow = wcv_no_w[j]*scale;
	double psiin = atan2(s,c)/order;
	double sin = s;
	double cin = c;
	double pts = pt2_[j][indx]/pt_[j][indx];
	if(MomConsWeight[j][0]=='y' && ptav[j]>0) {
	  s=s-(pt2_[j][indx]/pt_[j][indx])*snow;
	  c=c-(pt2_[j][indx]/pt_[j][indx])*cnow;
	}
  	double psi = atan2(s,c)/order;
	//if(i<2) {
	//  std::cout<<"Loop1: "<<s<<"\t"<<c<<"\t"<<sin<<"\t"<<cin<<"\t"<<psi<<"\t"<<scale<<std::endl;
	//}
  	flat[j]->fill(psi,vtx,bin);
  	flat[j]->fillOffset(s,c,ptcnt[j],vtx,bin);
  	flatOffset[j]->fillOffset(s,c,ptcnt[j],vtx,bin);
  	sout.ws[j] = s;
  	sout.wc[j] = c;
  	sout.wsum[j] = ptcnt[j];
  	sout.msum[j] = ptcnt[j];
  	if(centval<80 && fabs(vtx)<15&&bin>=0) hPsi[j]->Fill(psi);
	int k = centval/10;
	if(ptcnt[j]>0) {
	  hpt[j][k]->Fill(pt[j]*scale/ptcnt[j]);
	  hscale[j][k]->Fill(scale);
	}
      }
      ++ncnt;
      fwrite(&sout,sizeof(struct sout_struct),1,save);
    }

}
