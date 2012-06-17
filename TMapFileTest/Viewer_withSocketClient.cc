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
#include "TServerSocket.h"
#include "TSocket.h"
#include "TMessage.h"

TThread* th[2];
TCanvas* c1;
TPad* pad1;
TPad* pad2;
TPad* pad3;
TH1F     *hpx    = 0;
TH2F     *hpxpy  = 0;
TProfile *hprof  = 0;
Bool_t   finished = kFALSE;

void *Draw( void *nb ) {
  std::cout<< "Loop" <<std::endl;
  int nloop = 0;
  while (1) {
    TThread::Lock();
    TSocket* sock  = new TSocket("localhost" , 9090 );
    TMessage *mess;
    sock->SetCompressionLevel(1);
    std::cout<< "SRT" << std::endl;
    sock->Send("SRT");

    std::cout<< "hpx" << std::endl;
    sock->Send("hpx");
    sock->Recv(mess);
    std::cout<< "recieved" << std::endl;
    hpx = (TH1F *)mess->ReadObject(mess->GetClass());
    mess->Reset();

    std::cout<< "hpxpy" << std::endl;
    sock->Send("hpxpy");
    sock->Recv(mess);
    std::cout<< "recieved" << std::endl;
    hpxpy = (TH2F *)mess->ReadObject(mess->GetClass());
    mess->Reset();
    
    std::cout<< "hprof" << std::endl;	
    sock->Send("hprof");
    sock->Recv(mess);
    std::cout<< "recieved" << std::endl;
    hprof = (TProfile *)mess->ReadObject(mess->GetClass());
    mess->Reset();

    std::cout<< "END" << std::endl;

    sock->Send("END");
    sock->Close();
    delete mess;
    if( hpx == NULL || hpxpy == NULL || hprof == NULL ){ continue; }

    pad1->cd();
    hpx->SetFillColor(2);
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
    gSystem->Sleep(1000);   // sleep for 0.1 seconds
    //std::cout<< nloop << std::endl;
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
  TApplication* app = new TApplication("app",&argc, argv);
  if (!gROOT->IsBatch()) {
    c1 = new TCanvas("c1","Shared Memory Consumer Example",200,10,700,780);
    pad1 = new TPad("pad1","This is pad1",0.02,0.52,0.98,0.98,21);
    pad2 = new TPad("pad2","This is pad2",0.02,0.02,0.48,0.48,21);
    pad3 = new TPad("pad3","This is pad3",0.52,0.02,0.98,0.48,21);
    pad1->Draw();
    pad2->Draw();
    pad3->Draw();
  }
  
  // Create pointers to the objects in shared memory.
  // Loop displaying the histograms. Once the producer stops this
  // script will break out of the loop.
  Int_t oldentries = 0;
  
  th[0] = new TThread("t0", Draw  , (void*) 1);
  th[1] = new TThread("t1", Joiner, (void*) 2);
  th[0]->Run();
  th[1]->Run();
  TThread::Ps();
  int index = 0; 
  while( !finished ){
    c1->Update();
    std::cout<< index << std::endl;
    index++;
    gSystem->ProcessEvents();
    gSystem->Sleep(1000);
  }
  
  th[2]->Join();
  TThread::Ps();
  delete th[0];
  delete th[1];
  delete pad1;
  delete pad2;
  delete pad3;
  delete c1;
  app->Run();
}

