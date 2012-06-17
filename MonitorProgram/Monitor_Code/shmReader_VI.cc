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

struct VIdata{
  time_t now;
  double voltage[32];
  double current[32];
  unsigned short status[32];
};


bool init_shm(int &id, key_t &key, char* keyFile, struct VIdata **data){
  if(( key = ftok(keyFile,'r')) == -1){
    perror("ftok");
    return false;
  }
  if((id = shmget(key, sizeof(struct VIdata),  0666 )) == -1){
    perror("shmget");
    return false;
  }
  if(( *data = (struct VIdata *)shmat(id,0,0)) == (void *)-1) {
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
  char filename[128] = "/home/koto/local/KOTO_MONITOR/shmv288cntl.dat";
  struct VIdata *temperature = new VIdata;

  bool test = init_shm(id,key,filename,&temperature);
  if( !test ){
    std::cout << "init failed " << std::endl;
    return -1;
  }
  std::cout << "Initialized" << std::endl;
  while(1){
    std::cout  << temperature->now << " : "  << temperature->voltage[0] << std::endl;
    usleep(1000000);
  }
  
  if(shmdt(temperature)==-1){
    perror("shmdt");
  }
  return 0;
}
