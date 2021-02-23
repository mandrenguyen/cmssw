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
void Loop0(){
  int goodev = 0;
    for(int i = 0; i<nentries; i++) {
      if(test && ncnt>ntest) break;
      if(tree->GetEntry(i)<=0) continue;
      hruns->Fill(runno_);
      if(runno_ < minRun_ || runno_ > maxRun_) continue;
      hcent->Fill(centval);
      if(useNtrk) bin = NtrkToBin(ntrkval);
      hcentbin->Fill(bin);
      hvtx->Fill(vtx);
      ++goodev;
      ++totentries;
      for(int j = 0; j<NumEPNames; j++) {


	int indx = flat[j]->getOffsetIndx(bin,vtx);
  	double order = EPOrder[j];

  	if(wsv[j]==0&&wcv[j]==0) continue;
	pt_[j][indx]+=pt[j];
	pt2_[j][indx]+=pt2[j];
	ptcnt_[j][indx]+=ptcnt[j];
	if(order>1) continue;
	if(MomConsWeight[j][0]=='y') {
	  ptav[j]+=pt[j];
	  pt2av[j]+=pt2[j];
	  ptcntav[j]+=ptcnt[j];
	}
      }
      ++ncnt;
    }
}
