#include "TMapFile.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TThread.h"

#include <iostream>
#include "TApplication.h"
TMapFile* mfile = NULL;

TThread* th[2];
TCanvas* c1;
TPad* pad1;
TPad* pad2;
TPad* pad3;
TH1F     *hpx    = 0;
TH2F     *hpxpy  = 0;
TProfile *hprof  = 0;
Bool_t   finished = kFALSE;

void *Draw(void *nb ){
  std::cout<< "Loop" <<std::endl;
  int nloop = 0;
  int index=0;
  while (1) {
    std::cout<< index << std::endl;
    index++;
    TThread::Lock();
    hpx    = (TH1F *) mfile->Get("hpx", hpx);
    hpxpy  = (TH2F *) mfile->Get("hpxpy", hpxpy);
    hprof  = (TProfile *) mfile->Get("hprof", hprof);
    pad1->cd();
    hpx->Draw();
    pad2->cd();
    hprof->Draw();
    pad3->cd();
    hpxpy->Draw("cont");
    c1->cd();
    TThread::UnLock();
    c1->Modified();
    c1->Update();
    nloop++;
    gSystem->Sleep(100);   // sleep for 0.1 seconds
    std::cout<< nloop << std::endl;
    if( nloop > 1000 ){ break; }
  }
  return 0;
}

void *Joiner( void *){
  th[0]->Join();
  finished = kTRUE;
  return 0;
}


int
main(int argc, char** argv ){ 
  
  mfile = TMapFile::Create("hsimple.map");
  TApplication* app = new TApplication("app",&argc, argv);
  if (!gROOT->IsBatch()) {
    c1 = new TCanvas("c1","Shared Memory Consumer Example",200,10,700,780);
    pad1 = new TPad("pad1","This is pad1",0.02,0.52,0.98,0.98,21);
    pad2 = new TPad("pad2","This is pad2",0.02,0.02,0.48,0.48,21);
    pad3 = new TPad("pad3","This is pad3",0.52,0.02,0.98,0.48,21);
    c1->Draw();
    pad1->Draw();
    pad2->Draw();
    pad3->Draw();
  }

  
  // Open the memory mapped file "hsimple.map" in "READ" (default) mode.
  std::cout<< "Read1" << std::endl;
  std::cout<< "Read2" << std::endl;
  
  // Print status of mapped file and list its contents
  
  std::cout<< "LS" << std::endl;
  mfile->Print();
  std::cout<< "LS" << std::endl;
  mfile->ls();
  std::cout<< "LS" << std::endl;
  
  // Create pointers to the objects in shared memory.
  // Loop displaying the histograms. Once the producer stops this
  // script will break out of the loop.
  Int_t oldentries = 0;
  
  th[0] = new TThread("t0", Draw  , (void*) 1);
  th[1] = new TThread("t1", Joiner, (void*) 2);
  th[0]->Run();
  th[1]->Run();
  TThread::Ps();

  while( !finished ){    
    gSystem->Sleep(100);
    gSystem->ProcessEvents();
  }
  
  th[2]->Join();
  TThread::Ps();
  delete th[0];
  delete th[1];
  mfile->Close();
  delete pad1;
  delete pad2;
  delete pad3;
  delete c1;
  app->Run();
}
