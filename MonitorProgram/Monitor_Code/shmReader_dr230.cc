#include <cstdio>
#include <cstdlib>
#include <cstring> /* strcpy */
#include <cerrno>
#include <ctime>


#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>

#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"


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


bool init_shm(int &id, key_t &key, char* keyFile, struct DR230SHM **data){
  if(( key = ftok(keyFile,'r')) == -1){
    perror("ftok");
    return false;
  }
  if((id = shmget(key, sizeof(struct DR230SHM),  0666 )) == -1){
    perror("shmget");
    return false;
  }
  if(( *data = (struct DR230SHM *)shmat(id,0,0)) == (void *)-1) {
    perror("shmat");
    //return false;
    exit(1);
  }


  return true;
}




#define SHMSZ 4096
int main(int argc , char** argv)
{

  //struct TEMPData temp;
  int  id;
  key_t key;
  char filename[128] = "/home/koto/local/KOTO_MONITOR/shmdr230.dat";
  struct DR230SHM *temperature = new DR230SHM;

  bool test = init_shm(id,key,filename,&temperature);

  if( !test ){
    std::cout << "init failed " << std::endl;
    return -1;
  }
  std::cout << "Initialized" << std::endl;
  double threshold = 1.0;
  

  double  Pressure = 0.0;

  while(1){

    if( (TMath::Power(10,temperature->ch[16]-2) > threshold) ||
	(TMath::Power(10,temperature->ch[17]-2) > threshold) ||
	(TMath::Power(10,temperature->ch[18]-2) > threshold  )){
      for( int ich = 16; ich < 19; ich++){
	if( Pressure  <  temperature->ch[ich] ){
	  Pressure  = temperature->ch[ich];
	}
      }

      std::cout << "Pressure is TOO HIGH PLEASE STOP" << std::endl;
      gSystem->Exec(Form("ssh 192.168.10.21 ~/.scripts/sendPressureAlert.py `date +%s` %3.2f",
			 TMath::Power(10, Pressure)));
      break;
    }

    //    std::cout  << temperature->now << " : "  << temperature->ch[16] << std::endl;
    usleep(1000000);
  }
  
  if(shmdt(temperature)==-1){
    perror("shmdt");
  }
  return 0;
}
