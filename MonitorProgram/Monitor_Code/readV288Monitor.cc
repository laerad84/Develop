
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


struct VIdata{
  time_t now;
  double voltage[32];
  double current[32];
  unsigned short status[32];
};
  

bool bLoop=true;

void
sigint_handler(int)
{
  bLoop = false;
}
int SigAlrm = 0;
void sigalrm_handler(int)
{
  SigAlrm++;
}

bool init_shm(int &id, key_t &key, char* keyFile,struct VIdata** viAddr){
  if(( key = ftok(keyFile,'r')) == -1){
    perror("ftok");
    return false;
    //exit(1);
  }
  if((id = shmget(key, sizeof(struct VIdata),  0666 )) == -1){
    perror("shmget");
    return false;
    //exit(1);
  }

  if(( *viAddr = (struct VIdata *)shmat(id,0,0)) == (void *)-1) {
    perror("shmat");
    return false;
    //exit(1);
  }

  //std::cout << temperature << std::endl;
  return true;
}

bool close_shm(struct VIdata** viAddr){
  if( shmdt(*viAddr) == -1){
    perror("shmdt");
    return false;
  }
  return true;
}



int main( int argc, char **argv){
  int ColorMap[8] = {1,2,4,6,8,9,kOrange+1,kViolet-6};
  int   id;
  key_t key;
  char  filename[128] = "/home/koto/local/KOTO_MONITOR/shmv288cntl.dat";

  struct VIdata *vi = new VIdata;

  long nevent=0;
  const long nLoop = 3600;
  std::cout << "init SHM " << std::endl;
  bool binit_shm = init_shm(id, key, filename, &vi);  
  if( binit_shm ){
    std::cout << "shm connected" << std::endl;
  }else{
    return -1;
  }
  std::cout << "Set signal " << std::endl;
  //signal(SIGINT,sigint_handler);
  signal(SIGALRM,sigalrm_handler);
  
  std::cout << "Set MapFile " << std::endl;

  TMapFile* map = TMapFile::Create("tmap.map","RECREATE",20*1024*1024,"test TMapFile");
  TGraph*   grVoltage[32];
  TGraph*   grCurrent[32];
  TGraph*   grStatus[32];
  for( int i = 0; i< 32; i++){

    grVoltage[i] = new TGraph();
    grVoltage[i]->SetNameTitle(Form("V%02d",i),Form("V%02d",i));
    grVoltage[i]->SetMarkerStyle(3);
    grCurrent[i] = new TGraph();
    grCurrent[i]->SetNameTitle(Form("I%02d",i),Form("I%02d",i));
    grStatus[i] = new TGraph();
    grStatus[i]->SetNameTitle(Form("S%02d",i),Form("S%02d",i));
    map->Add( grVoltage[i] , grVoltage[i]->GetName() );
    map->Add( grCurrent[i] , grCurrent[i]->GetName() );
    map->Add( grStatus[i]  , grStatus[i]->GetName() );

    grVoltage[i]->SetMarkerStyle(6);
    grCurrent[i]->SetMarkerStyle(6);
    grStatus[i]->SetMarkerStyle(6);

    grVoltage[i]->SetMarkerColor(ColorMap[i%8]);
    grCurrent[i]->SetMarkerColor(ColorMap[i%8]);
    grStatus[i]->SetMarkerColor(ColorMap[i%8]);
    grVoltage[i]->SetLineColor(ColorMap[i%8]);
    grCurrent[i]->SetLineColor(ColorMap[i%8]);
    grStatus[i]->SetLineColor(ColorMap[i%8]);

  }

  std::cout << "Set canvas, timer and start Loop" << std::endl;
  TApplication* app = new TApplication("v288cntl",&argc, argv);
  TCanvas* can = new TCanvas("CC03HV","CC03HVMonitor",1200,1024);
  can->Divide(4,3);
  int TimerInterval = 1000000;
  int  tsec = TimerInterval/1000000;
  int  usec = TimerInterval - tsec*1000000;
  struct timeval ctv = { tsec,usec }; // sec, usec
  struct timeval ntv = ctv;
  struct itimerval itv = { ntv,ctv }; // next,current
  setitimer( ITIMER_REAL, &itv, NULL);
  bool isDrawn = false;
  while( bLoop ){
    if( SigAlrm > 0 ){
      std::cout <<" nevent "<< nevent << std::endl;
      for( int ich = 0; ich < 32; ich++ ){
	//std::cout << vi->voltage[ich] << "\t";

	grVoltage[ich]->SetPoint( nevent%nLoop, (double)vi->now+15*3600, vi->voltage[ich]);
	grCurrent[ich]->SetPoint( nevent%nLoop, (double)vi->now+15*3600, vi->current[ich]);
	grStatus[ich]->SetPoint( nevent%nLoop,  (double)vi->now+15*3600, TMath::Log2(vi->status[ich]));

	grVoltage[ich]->GetXaxis()->SetTimeDisplay(1);
	grCurrent[ich]->GetXaxis()->SetTimeDisplay(1);
	grStatus[ich]->GetXaxis()->SetTimeDisplay(1);

	grVoltage[ich]->GetYaxis()->SetRangeUser(0,1200);
	grCurrent[ich]->GetYaxis()->SetRangeUser(0,400);
	grStatus[ich]->GetYaxis()->SetRangeUser(0,20);
	map->Update(grVoltage[ich]);
	map->Update(grCurrent[ich]);
	map->Update(grStatus[ich]);

	if( !isDrawn ){
	  if( ich%8 ==0 ){
	    can->cd(ich/8+1);
	    gPad->SetGridx();
	    gPad->SetGridy();
	    grVoltage[ich]->Draw("AL");
	    can->cd(ich/8+1+4);
	    gPad->SetGridx();
	    gPad->SetGridy();
	    grCurrent[ich]->Draw("AL");
	    can->cd(ich/8+1+8);
	    gPad->SetGridx();
	    gPad->SetGridy();
	    //gPad->SetLogy();
	    grStatus[ich]->Draw("AL");
	  }else{
	    can->cd(ich/8+1);
	    grVoltage[ich]->Draw("L");
	    can->cd(ich/8+1+4);
	    grCurrent[ich]->Draw("L");
	    can->cd(ich/8+1+8);
	    grStatus[ich]->Draw("L");
	  }
	}
      }
      can->Update();
      can->Modified();
      can->Update();      
      nevent++;
      SigAlrm = 0;
    }
    usleep(200000);
  }

  app->Run();
  map->Close();
  close_shm(&vi);
  signal(SIGINT,SIG_DFL);
  signal(SIGALRM,SIG_DFL);  
  return 0;
}

