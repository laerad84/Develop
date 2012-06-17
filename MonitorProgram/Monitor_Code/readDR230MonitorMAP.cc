
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
  long neventLong = 0;
  const long nLoopShort = 3600;//1hour
  const long nLoopLong  = 72*60*2;//24hour

  std::cout << "init SHM " << std::endl;
  bool binit_shm = init_shm(id, key, filename, &vi);  
  if( binit_shm ){
    std::cout << "shm connected" << std::endl;
  }else{
    return -1;
  }
  std::cout << "Set signal " << std::endl;
  signal(SIGINT,sigint_handler);
  signal(SIGALRM,sigalrm_handler);
  
  std::cout << "Set MapFile " << std::endl;
  const int nCH = 20;
  TMapFile* map = TMapFile::Create("DR230.map","RECREATE",40*1024*1024,"test TMapFile");

  std::cout << "Define TGraph" << std::endl;
  TGraph*   grTemperatureLong[16];
  TGraph*   grTemperatureShort[16];
  TGraph*   grPressureLong[4];
  TGraph*   grPressureShort[4];

  std::cout << "Add to Map" << std::endl;

  for( int i = 0; i< 16; i++){
    grTemperatureLong[i]=NULL;
    grTemperatureLong[i] = new TGraph();
    grTemperatureLong[i]->SetNameTitle(Form("TEMPLONG%02d",i),Form("TEMPLONG%02d",i));
    grTemperatureLong[i]->SetMarkerStyle(6);
    grTemperatureLong[i]->SetMarkerColor(ColorMap[i%4]);
    grTemperatureLong[i]->SetLineColor(ColorMap[i%4]);
    map->Add( grTemperatureLong[i] ,grTemperatureLong[i]->GetName());

    grTemperatureShort[i]=NULL;
    grTemperatureShort[i] = new TGraph();
    grTemperatureShort[i]->SetNameTitle(Form("TEMPSHORT%02d",i),Form("TEMPSHORT%02d",i));
    grTemperatureShort[i]->SetMarkerStyle(6);
    grTemperatureShort[i]->SetMarkerColor(ColorMap[i%4]);
    grTemperatureShort[i]->SetLineColor(ColorMap[i%4]);
    map->Add( grTemperatureShort[i] ,grTemperatureShort[i]->GetName());

  }

  for( int i = 0 ; i< 3; i++){
    grPressureLong[i] = NULL;
    grPressureLong[i] = new TGraph();
    grPressureLong[i]->SetNameTitle(Form("PRESLONG%02d",i),Form("PRESLONG%02d",i));
    grPressureLong[i]->SetMarkerStyle(6);
    grPressureLong[i]->SetMarkerColor(ColorMap[i%8]);
    grPressureLong[i]->SetLineColor(ColorMap[i%8]);
    map->Add( grPressureLong[i] ,grPressureLong[i]->GetName());

    grPressureShort[i] = NULL;
    grPressureShort[i] = new TGraph();
    grPressureShort[i]->SetNameTitle(Form("PRESSHORT%02d",i),Form("PRESSHORT%02d",i));
    grPressureShort[i]->SetMarkerStyle(6);
    grPressureShort[i]->SetMarkerColor(ColorMap[i%8]);
    grPressureShort[i]->SetLineColor(ColorMap[i%8]);
    map->Add( grPressureShort[i] ,grPressureShort[i]->GetName());
  }

  
  std::cout << "Set canvas, timer and start Loop" << std::endl;
  int TimerInterval = 1000000;
  int  tsec = TimerInterval/1000000;
  int  usec = TimerInterval - tsec*1000000;
  struct timeval ctv = { tsec,usec }; // sec, usec
  struct timeval ntv = ctv;
  struct itimerval itv = { ntv,ctv }; // next,current
  setitimer( ITIMER_REAL, &itv, NULL);
  
  nevent = 0;
  neventLong = 0;
  while( bLoop ){
    if( SigAlrm > 0 ){
      std::cout <<" nevent "<< nevent << "\r" << std::flush;
      if( (double)vi->now <= 0){ continue;}
      for( int ich = 0; ich < 16; ich++ ){
	if( nevent %30 ==0){
	  grTemperatureLong[ich]->SetPoint( neventLong % nLoopLong, (double)vi->now+15*3600, vi->ch[ich]);
	  map->Update(grTemperatureLong[ich]);
	}
	grTemperatureShort[ich]->SetPoint( nevent % nLoopShort, (double)vi->now+15*3600, vi->ch[ich]);
	map->Update(grTemperatureShort[ich]);
      }

      for( int ich = 0; ich < 3; ich++){
	if( nevent % 30 ==0){
	grPressureLong[ich]->SetPoint( neventLong % nLoopLong, (double)vi->now+15*3600,
				       TMath::Power(10,vi->ch[ich+16]-2));
	map->Update(grPressureLong[ich]);
	}
	grPressureShort[ich]->SetPoint( nevent % nLoopShort, (double)vi->now+15*3600, 
					TMath::Power(10,vi->ch[ich+16]-2));
	map->Update(grPressureShort[ich]);
      }

      if( nevent % 30 == 0){neventLong++;}
      nevent++;

      SigAlrm=0;
    }
    usleep(200000);
  }

  //app->Run();
  map->Close();
  close_shm(&vi);
  signal(SIGINT,SIG_DFL);
  signal(SIGALRM,SIG_DFL);  
  return 0;
}

