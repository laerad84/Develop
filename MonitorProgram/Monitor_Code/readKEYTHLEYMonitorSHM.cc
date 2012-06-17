
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
#include <TPad.h>

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


struct KEYTHLEYdata{
  time_t now; 
  char   data[4096];
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

bool init_shm(int &id, key_t &key, char* keyFile,struct KEYTHLEYdata** viAddr){
  if(( key = ftok(keyFile,'r')) == -1){
    perror("ftok");
    return false;
    //exit(1);
  }
  if((id = shmget(key, sizeof(struct KEYTHLEYdata),  0666 )) == -1){
    perror("shmget");
    return false;
    //exit(1);
  }

  if(( *viAddr = (struct KEYTHLEYdata *)shmat(id,0,0)) == (void *)-1) {
    perror("shmat");
    return false;
    //exit(1);
  }

  //std::cout << temperature << std::endl;
  return true;
}

bool close_shm(struct KEYTHLEYdata** viAddr){
  if( shmdt(*viAddr) == -1){
    perror("shmdt");
    return false;
  }
  return true;
}

void convertData( char* data, double *temperature ){
  char chtemp[180][30];
  int  ich   = 0; 
  int  icomp = 0;
  for( int cnt  = 1; cnt  < 4096; cnt++){
    if( data[cnt] == ',' ){
      chtemp[ich][icomp] = '\0';
      ich++;
      icomp = 0;
    }else{
      chtemp[ich][icomp] = data[cnt];
      icomp++;
    }
    if(data[cnt] =='\0'){
      break;
    }
  }
  for( int ich  = 0; ich < 60; ich++){
    temperature[ich] = atof(chtemp[ich*3]);
  }
}


int main( int argc, char **argv){
  int ColorMap[8] = {1,2,4,6,8,9};
  int   id;
  key_t key;
  char  filename[128] = "/home/koto/local/KOTO_MONITOR/shmkeythley.dat";

  struct KEYTHLEYdata *vi = new KEYTHLEYdata;

  long   nevent=0;
  double temperature[60]={};

  const long nLoop = 120*24;
  const long nLoopShort = 60;

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
  const int nCH = 60;
  TMapFile* map = TMapFile::Create("KEYTHLEY.map","RECREATE",
				   20*1024*1024,
				   "test TMapFile");
  TGraph*   grTemp[nCH];
  
  for( int i = 0; i< nCH; i++){
    grTemp[i] = new TGraph();
    grTemp[i]->SetNameTitle(Form("TEMP%02d",i),Form("TEMP%02d",i));
    grTemp[i]->SetMarkerStyle(3);
    grTemp[i]->SetMarkerStyle(6);
    grTemp[i]->SetMarkerColor(ColorMap[i%5]);
    grTemp[i]->SetLineColor(ColorMap[i%5]);
    map->Add( grTemp[i] ,grTemp[i]->GetName());
  }
  
  std::cout << "Set timer and start Loop" << std::endl;
  
  int TimerInterval = 30*1000*1000;
  int  tsec = TimerInterval/1000000;
  int  usec = TimerInterval - tsec*1000000;
  struct timeval ctv = { tsec,usec }; // sec, usec
  struct timeval ntv = ctv;
  struct itimerval itv = { ntv,ctv }; // next,current
  setitimer( ITIMER_REAL, &itv, NULL);

  SigAlrm = 1;

  while( bLoop ){

    if( SigAlrm > 0 ){
      SigAlrm = 0;
      std::cout <<" nevent "<< nevent << "\r" << std::flush;
      convertData( vi->data, temperature );
      for( int ich = 0; ich <nCH; ich++){
	
	if(temperature[ich] >1000){
	  grTemp[ich]->SetPoint(nevent % nLoop, 
				vi->now + 3600*15, 
				-273);
	}else{
	  grTemp[ich]->SetPoint(nevent % nLoop,
				vi->now + 3600*15,
				temperature[ich]);
	}
	map->Update(grTemp[ich]);
      }      
      nevent++;      
    }      
    usleep(1*1000*1000);
  }
  
  map->Close();
  
  close_shm(&vi);
  signal(SIGINT,SIG_DFL);
  signal(SIGALRM,SIG_DFL);  
  return 0;

}

