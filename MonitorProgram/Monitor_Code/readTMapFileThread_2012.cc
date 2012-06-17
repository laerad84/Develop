#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#include <TApplication.h>
#include <TMapFile.h>
#include <TAxis.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TH1.h>
#include <TText.h>
#include <TGraph.h>
#include "TThread.h"
#include "TPaveText.h"
#include "TStyle.h"
//time variable
time_t timeRange;
//TGraph
TGraph* grTempKEY[60];

//TMapFile
TMapFile* mapKeythley;

TCanvas* can ;

void Draw(void *){
  long nLoop = 0;
  can->Draw();  
  can->Update();
  can->Modified();
  
  while(1){
    
    std::cout <<" UPDATE:"<< nLoop << "\r" << std::flush;
    std::cout <<" TEMPKEY" << "\r" << std::flush;
    //TThread::Lock();
    for( int grIndex = 0; grIndex < 60; grIndex++){
      grTempKEY[grIndex] = (TGraph*)mapKeythley->Get(Form("TEMP%02d",grIndex),grTempKEY[grIndex]);
      grTempKEY[grIndex]->SetMarkerColor(grIndex%5+1);
      //grTempKEY[grIndex]->SetMarkerStyle(2);      
      grTempKEY[grIndex]->GetYaxis()->SetRangeUser(0,50);
      grTempKEY[grIndex]->GetXaxis()->SetTimeDisplay(1);
      if( (grIndex % 5) == 0){
	can->cd(grIndex/5+1);
	grTempKEY[grIndex]->Draw("AP");
      }else{
	can->cd(grIndex/5+1);
	grTempKEY[grIndex]->Draw("P");
      }
    }
    
    
    
    std::cout << " UNLOCK" << "\r" << std::flush;

    nLoop++;
    timeRange = time(NULL);
    
    std::cout <<" CANVAS UPDATE" << "\r" << std::flush;

    can->Update();
    can->Modified();
    can->Update();
    //    TThread::UnLock();

    sleep(20);

    TThread::Lock();
    std::cout << " LOCK" << "\r" << std::flush;
    for( int i = 0; i< 60; i++){
      delete grTempKEY[i];
      grTempKEY[i] = NULL;
    }
    TThread::UnLock();

    std::cout << " UNLOCK" << "\r" << std::flush;    
  }
}


int main( int argc, char** argv ){
  mapKeythley = NULL;
  mapKeythley = TMapFile::Create("KEYTHLEY.map");

  gROOT->SetStyle("Plain");

  TApplication* app     = new TApplication("TempMonitor",&argc, argv);

  can = new TCanvas("keythley","keythley"   ,1024,700);
  can->Divide(3,4);
  can->Draw();
  can->Update();
  can->Modified();

  TThread* th = new TThread("draw",&Draw,NULL);  
  bool isDrawn = false;
  
  th->Run();
  
  app->Run();

  mapKeythley->Close();
  return 0;
}
    
    
