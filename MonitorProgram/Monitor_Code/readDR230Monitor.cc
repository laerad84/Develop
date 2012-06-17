
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

struct DR230SHM{
  time_t now;
  double ch[20];
};

bool bLoop = true;

void sigint_handler(int)
{
  bLoop = false;
}

int SigAlrm = 0;
void sigalrm_handler(int)
{
  SigAlrm++;
}

bool init_shm(int &id, key_t &key, char* keyFile,struct DR230SHM** viAddr){
  if(( key = ftok(keyFile,'r')) == -1){
    perror("ftok");
    return false;
    //exit(1);
  }
  if((id = shmget(key, sizeof(struct DR230SHM),  0666 )) == -1){
    perror("shmget");
    return false;
    //exit(1);
  }

  if(( *viAddr = (struct DR230SHM *)shmat(id,0,0)) == (void *)-1) {
    perror("shmat");
    return false;
    //exit(1);
  }

  //std::cout << temperature << std::endl;
  return true;
}

bool close_shm(struct DR230SHM** viAddr){
  if( shmdt(*viAddr) == -1){
    perror("shmdt");
    return false;
  }
  return true;
}



int main( int argc, char **argv){
  int ColorMap[8] = {1,2,4,6,8,9};
  int   id;
  key_t key;
  char  filename[128] = "/home/koto/local/KOTO_MONITOR/shmdr230.dat";

  struct DR230SHM *vi = new DR230SHM;

  long nevent=0;
  const long nLoop = 3600*24;
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
  const int nCH = 20;
  TMapFile* map = TMapFile::Create("tmap_dr230.map","RECREATE",40*1024*1024,"test TMapFile");
  TGraph*   grTemperature[16];
  TGraph*   grPressure[4];
  TText*    text[20];
  for( int i = 0; i< 20 ; i++){
    text[i] = new TText();
  }


  for( int i = 0; i< 16; i++){

    grTemperature[i] = new TGraph();
    grTemperature[i]->SetNameTitle(Form("TEMP%02d",i),Form("TEMP%02d",i));
    grTemperature[i]->SetMarkerStyle(6);
    map->Add( grTemperature[i] ,grTemperature[i]->GetName());
    grTemperature[i]->SetMarkerColor(ColorMap[i%4]);
    grTemperature[i]->SetLineColor(ColorMap[i%4]);

  }

  for( int i = 0 ; i< 3; i++){
    grPressure[i] = new TGraph();
    grPressure[i]->SetNameTitle(Form("PRESSURE%02d",i),Form("PRESSURE%02d",i));
    grPressure[i]->SetMarkerStyle(6);
    map->Add( grPressure[i] ,grPressure[i]->GetName());
    grPressure[i]->SetMarkerColor(ColorMap[i%8]);
    grPressure[i]->SetLineColor(ColorMap[i%8]);
  }


  std::cout << "Set canvas, timer and start Loop" << std::endl;
  TApplication* app = new TApplication("v288cntl",&argc, argv);
  TCanvas* can = new TCanvas("DR230","DR230",680,800);
  can->Divide(2,3);
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
      std::cout <<" nevent "<< nevent << "\r" << std::flush;
      for( int ich = 0; ich < 16; ich++ ){
	//std::cout << vi->voltage[ich] << "\t";
	grTemperature[ich]->SetPoint( nevent%nLoop, (double)vi->now+15*3600, vi->ch[ich]);
	grTemperature[ich]->GetXaxis()->SetTimeDisplay(1);
	grTemperature[ich]->GetYaxis()->SetRangeUser(0,50);
	map->Update(grTemperature[ich]);
      }

      for( int ich = 0; ich < 3; ich++){
	grPressure[ich]->SetPoint( nevent%nLoop, (double)vi->now+15*3600, TMath::Power(10,vi->ch[ich+16]-2));
	grPressure[ich]->GetXaxis()->SetTimeDisplay(1);
	grPressure[ich]->GetYaxis()->SetRangeUser(0.001,100);
	map->Update(grPressure[ich]);
      }	
      
      if( !isDrawn ){
	for( int i = 0; i< 16; i++){
	  if( i%4 ==0 ){
	    can->cd(i/4+1);
	    gPad->SetGridx();
	    gPad->SetGridy();
	    grTemperature[i]->Draw("AL");
	  }else{
	    can->cd(i/4+1);
	    grTemperature[i]->Draw("L");
	  }
	}

	can->cd(5);
	gPad->SetLogy();
	gPad->SetGridx();
	gPad->SetGridy();
	grPressure[0]->Draw("AL");
	grPressure[1]->Draw("L");
	grPressure[2]->Draw("L");
      }
      can->cd(6);
      for( int ich = 0; ich < 19; ich++){
	if( ich < 16 ){
	  text[ich]->SetText(0.5*(ich/10)+0.1,1-0.1*(ich%10+1)+0.05,
			     Form("%02d:%3.1lf[C]",ich, vi->ch[ich]));
	}else{
	  text[ich]->SetText(0.5*(ich/10)+0.1,1-0.1*(ich%10+1)+0.05,
			     Form("%02d:%3.3lf[Pa]",ich, TMath::Power(10,vi->ch[ich]-2)));
	}
	text[ich]->Draw();
      }
      
      
      can->Update();
      can->Modified();
      can->Update();      
      nevent++;
      SigAlrm=0;
    }
    usleep(200000);
  }

  app->Run();
  map->Close();
  close_shm(&vi);
  //signal(SIGINT,SIG_DFL);
  signal(SIGALRM,SIG_DFL);  
  return 0;
}

