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
void Loop3(){
  save = fopen("tmpsave","rb");
  ncnt = 0;
  while(fread(&sout, sizeof(struct sout_struct), 1, save) >0) {
    EPcent = (float) sout.cent;
    EPvtx = (float) sout.vtx;
    EPntrk = sout.ntrk;
    EPrun = sout.run;
    vtx = sout.vtx;
    bin = sout.bin;
    centval=sout.cent;
    //if(ncnt < 10) std::cout<<"======================================="<<std::endl;
    for(int j = 0; j<NumEPNames; j++) {
      double psiOffset = -10;
      double psiFlat = -10;
      double psi = -10;
      EPAngs[j] = -10;
      if(sout.msum[j]>0) {
	double soff = flat[j]->getSoffset(sout.ws[j], vtx, bin);
	double coff = flat[j]->getCoffset(sout.wc[j], vtx, bin);
	
	psi = atan2(sout.ws[j],sout.wc[j])/EPOrder[j];
	psiOffset = flat[j]->getOffsetPsi(soff,coff);
	if(centval<80 && fabs(vtx)<15&&bin>=0) {
	  hPsiOffset2[j]->Fill(psiOffset);	
	}
	psiFlat = flatOffset[j]->getFlatPsi(psiOffset,vtx,bin);
	EPAngs[j]=psiFlat;
	if(centval<80 && fabs(vtx)<15&&bin>=0) {
	  hPsiFlat[j]->Fill(psiFlat);
	}
       	//if(ncnt < 4 ) {
	  //flat[j]->dumpxoffDB();
	  //flat[j]->dumpyoffDB(); 
	  //std::cout<<"Loop3: "<<EPNames[j]<<"\t"<<vtx<<"\t"<<bin<<"\t"<<flat[j]->getOffsetIndx(bin,vtx)<<"\t"<<soff<<"\t"<<coff<<"\t"<<psi<<"\t"<<psiOffset<<"\t"<<psiFlat<<std::endl; 
	//} 
      }
    }
    ++ncnt;
    //if(ncnt==100) break;
    EPtree->Fill();

  }
  
}
