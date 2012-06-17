
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
#include <TThread.h>

TMapFile* mapDr230;
TMapFile* mapKeythley;
TCanvas* can ;
TPad* pad[3];
TGraph* grTemp[16];
TGraph* grTempLong[16];
TGraph* grPres[3];
TGraph* grPresLong[3];
TGraph* grTempKEY[60];

TGraph* grShort[80];
TGraph* grLong[80];

int ColorList[10] = {1,2,4,6,8,9,15,28,30,46};
int baseCHinList[8]   = { 0,12,10,11, 5, 1,13,28};
int baseCHoutList[8]  = {26, 8,27,24,30,15,29,25};
int CHHolderList[8]   = {18,16,37,39,23,20, 5,36};
int CHfrontList[9]    = {43,46,41,40,45,47,44,39,42};
int CHSPlateList[6]   = {14,13, 6,31,32,33};
int CHLPlateList[4]   = {35,65,63,69};
int CHSplateUPList[8] = {60,68,51,50,75,72,64,70};
int CHCopperUPList[7] = {73,74,52,54,55,56,57};
int CC03OEVpinList[7] = {23, 7,19,68,69,74,71};


void DrawThread(void *){
  time_t timeRange;

  //number List

  bool isDrawn = false;
  long nLoop = 0;
  while(1){

    std::cout <<" UPDATE:"<< nLoop << "\r" << std::flush;
    for( int grIndex = 0; grIndex < 60; grIndex++){
      grTempKEY[grIndex]->Set(0);
      *grTempKEY[grIndex] = *((TGraph*)mapKeythley->Get(Form("TEMP%02d",grIndex)));
    }
    for( int grIndex = 0; grIndex < 16; grIndex++){
      grTempLong[grIndex]->Set(0);
      //*grTemp[grIndex] = *((TGraph*)mapDr230->Get(Form("TEMPSHORT%02d",grIndex)));
      *grTempLong[grIndex] = *((TGraph*)mapDr230->Get(Form("TEMPLONG%02d",grIndex)));
    }
    for( int grIndex = 0; grIndex < 3; grIndex++){
      grPresLong[grIndex]->Set(0);
      // *grPres[grIndex] = *((TGraph*)mapDr230->Get(Form("PRESSHORT%02d",grIndex)));
      *grPresLong[grIndex] = *((TGraph*)mapDr230->Get(Form("PRESLONG%02d",grIndex)));
    }
    //std::cout << "SetGraphEnd" << std::endl;
    nLoop++;

    // Temperature base inner place

    timeRange = time(NULL);

    pad[0]->cd(1);
    gPad->SetGridx();
    gPad->SetGridy();

    for( int i = 0; i < 8; i++){
      //std::cout << baseCHinList[i] << std::endl;

      grLong[baseCHinList[i]]->SetMarkerColor(ColorList[i]);
      grLong[baseCHinList[i]]->SetMarkerStyle(6);
      grLong[baseCHinList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[baseCHinList[i]]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange + 16*3600);
      grLong[baseCHinList[i]]->GetXaxis()->SetTimeDisplay(1);

      if( i ==0 ){
	grLong[baseCHinList[i]]->Draw("AP");
      }else{
	grLong[baseCHinList[i]]->Draw("P");
      }
    }
    // Temperature base outer place

    pad[0]->cd(2);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<8; i++){
      grLong[baseCHoutList[i]]->SetMarkerColor(ColorList[i]);
      grLong[baseCHoutList[i]]->SetMarkerStyle(6);
      grLong[baseCHoutList[i]]->GetXaxis()->SetTimeDisplay(1);
      grLong[baseCHoutList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[baseCHoutList[i]]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange+16*3600);
      if( i==0 ){
	grLong[baseCHoutList[i]]->Draw("AP");
      }else{
	grLong[baseCHoutList[i]]->Draw("P");
      }
    }

     // Temperature Holder 
    pad[0]->cd(3);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<8; i++){
      grLong[CHHolderList[i]]->SetMarkerColor(ColorList[i]);
      grLong[CHHolderList[i]]->SetMarkerStyle(6);
      grLong[CHHolderList[i]]->GetXaxis()->SetTimeDisplay(1);
      grLong[CHHolderList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[CHHolderList[i]]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange+16*3600);

      if( i==0 ){
	grLong[CHHolderList[i]]->Draw("AP");
      }else{
	grLong[CHHolderList[i]]->Draw("P");
      }
    }

    // Temperature Front
    pad[0]->cd(4);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<9; i++){
      grLong[CHfrontList[i]]->SetMarkerColor(ColorList[i]);
      grLong[CHfrontList[i]]->SetMarkerStyle(6);
      grLong[CHfrontList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[CHfrontList[i]]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange+16*3600);
      grLong[CHfrontList[i]]->GetXaxis()->SetTimeDisplay(1);

      if( i==0 ){
	grLong[CHfrontList[i]]->Draw("AP");
      }else{
	grLong[CHfrontList[i]]->Draw("P");
      }
    }


    // Temperature SmallPlate 
    pad[0]->cd(5);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<6; i++){
      grLong[CHSPlateList[i]]->SetMarkerColor(ColorList[i]);
      grLong[CHSPlateList[i]]->SetMarkerStyle(6);
      grLong[CHSPlateList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[CHSPlateList[i]]->GetXaxis()->SetRangeUser((double)timeRange -9*3600,(double)timeRange+ 16*3600);
      grLong[CHSPlateList[i]]->GetXaxis()->SetTimeDisplay(1);

      if( i==0 ){
	grLong[CHSPlateList[i]]->Draw("AP");
      }else{
	grLong[CHSPlateList[i]]->Draw("P");
      }
    }

     // Temperature LargePlate 
    pad[0]->cd(6);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<4; i++){
      grLong[CHLPlateList[i]]->SetMarkerColor(ColorList[i]);
      grLong[CHLPlateList[i]]->SetMarkerStyle(6);
      grLong[CHLPlateList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[CHLPlateList[i]]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange+ 16*3600);
      grLong[CHLPlateList[i]]->GetXaxis()->SetTimeDisplay(1);

      if( i==0 ){
	grLong[CHLPlateList[i]]->Draw("AP");
      }else{
	grLong[CHLPlateList[i]]->Draw("P");
      }
    }

    // Temperature UpperPlate 
    pad[0]->cd(7);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<8; i++){
      grLong[CHSplateUPList[i]]->SetMarkerColor(ColorList[i]);
      grLong[CHSplateUPList[i]]->SetMarkerStyle(6);
      grLong[CHSplateUPList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[CHSplateUPList[i]]->GetXaxis()->SetRangeUser((double)timeRange-9*3600,(double)timeRange +16*3600);
      grLong[CHSplateUPList[i]]->GetXaxis()->SetTimeDisplay(1);

      if( i==0 ){
	grLong[CHSplateUPList[i]]->Draw("AP");
      }else{
	grLong[CHSplateUPList[i]]->Draw("P");
      }
    }

    // Temperature CopperPlate
    pad[0]->cd(8);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<7; i++){
      grLong[CHCopperUPList[i]]->SetMarkerColor(ColorList[i]);
      grLong[CHCopperUPList[i]]->SetMarkerStyle(6);
      grLong[CHCopperUPList[i]]->GetYaxis()->SetRangeUser(0,50);
      grLong[CHCopperUPList[i]]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange+16*3600);
      grLong[CHCopperUPList[i]]->GetXaxis()->SetTimeDisplay(1);

      if( i==0 ){
	grLong[CHCopperUPList[i]]->Draw("AP");
      }else{
	grLong[CHCopperUPList[i]]->Draw("P");
      }
    }

    // Temperature Holder 
    pad[0]->cd(9);
    gPad->SetGridx();
    gPad->SetGridy();
    for( int i =0; i<7; i++){
      grLong[CC03OEVpinList[i]]->SetMarkerColor(ColorList[i]);
      grLong[CC03OEVpinList[i]]->SetMarkerStyle(6);
      grLong[CC03OEVpinList[i]]->GetYaxis()->SetRangeUser(0,50); 
      grLong[CC03OEVpinList[i]]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange+16*3600);
      grLong[CC03OEVpinList[i]]->GetXaxis()->SetTimeDisplay(1);

      if( i==0 ){
	grLong[CC03OEVpinList[i]]->Draw("AP");
      }else{
	grLong[CC03OEVpinList[i]]->Draw("P");

      }
    }

    // Temperature Holder 
    for( int i =0; i<3; i++){
      grPresLong[i]->SetMarkerColor(ColorList[i]);
      grPresLong[i]->SetMarkerStyle(6);
      grPresLong[i]->GetYaxis()->SetRangeUser(0.01,5);
      grPresLong[i]->GetXaxis()->SetRangeUser((double)timeRange - 9*3600,(double)timeRange+16*3600);
      grPresLong[i]->GetXaxis()->SetTimeDisplay(1);
    }

    pad[1]->cd(1);
    gPad->SetGridx();
    gPad->SetGridy();
    grPresLong[0]->Draw("AP");
    grPresLong[1]->Draw("P");
    grPresLong[2]->Draw("P");

    // Temperature Holder 

    pad[1]->cd(2);
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();
    grPresLong[0]->Draw("AP");
    grPresLong[1]->Draw("P");
    grPresLong[2]->Draw("P");


    can->Update();
    can->Modified();
    can->Update();

    sleep(30);
  }
}
    
    
int main(int argc, char** argv){

  mapDr230    = TMapFile::Create("DR230.map");
  mapKeythley = TMapFile::Create("KEYTHLEY.map");

  TThread* th = new TThread("draw",&DrawThread,NULL);

  TApplication* app = new TApplication("app",&argc, argv);

  can = new TCanvas("can","can1",1920,1000);

  pad[0] = new TPad("temperature","temperature",    0,   0,0.75,   1,  0, 0, 0);
  pad[1] = new TPad("pressure"   ,"pressure"   , 0.75, 0.5,   1,   1,  0, 0, 0);
  pad[2] = new TPad("data"       ,"data"       , 0.75,   0,   1, 0.5,  0, 0, 0);
  pad[0]->Divide(3,3);
  pad[1]->Divide(1,2);

  can->Draw();
  pad[0]->Draw();
  pad[1]->Draw();
  pad[2]->Draw();

  can->Update();
  can->Modified();
  
  for( int i = 0; i< 60; i++){
    grTempKEY[i] = new TGraph();
    // grShort[i]   = grTempKEY[i];
    grLong[i]    = grTempKEY[i];
  }
  for( int i = 0; i< 16; i++){
    //grTemp[i] = new TGraph();
    grTempLong[i] = new TGraph();
    //grShort[i+60] = grTemp[i];
    grLong[i+60]  = grTempLong[i];
  }

  for( int i = 0; i< 3; i++){
    //grPres[i] = new TGraph();
    grPresLong[i] = new TGraph();    
  }

  th->Run();  
  app->Run();

  mapKeythley->Close();
  mapDr230->Close();
  return 0;


}
  
