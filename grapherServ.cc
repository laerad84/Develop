#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMKEYPATH  "/dev/null"
#define SEMKEYID    1
#define SHMKEYPATH  "/dev/null"
#define SHMKEYID    1

#define NUMSEMS 2
#define SIZEOFSHMSEG 50;
#define NUMSG 1000

class Semaphore {
  int rx, semid, i;
  key_t semkey;
  struct sembuf operations[2];
  short sarray;
  std::string keyFile;
public:
  Semaphore(int);
  ~Semaphore();
  void Lock();
  void UnLock();

  bool BuildKey();
  bool GetSem();

};


int
main ( int argc , char* argv ){
  int rc, semid , shmid, i;
  key_t semkey, shmkey;
  void *shm_address;
  struct sembuf operations[2];
  struct shmid_ds shmid_struct;
  short sarray[NUMSEMS];
  
  semkey = ftok(SEMKEYPATH, SEMKEYID);
  if( semkey = (key_t)-1 ){
    std::cout<< __FUNCTION__ << " : ftok() for sem failed\n";
    return -1;
  }
  
  shmkey = ftok(SHMKEYPATH, SHMKEYID);
  if( shmkey  == (key_t)-1 ){
    std::cout<< __FUNCTION__ << " : ftok() for shm failed\n";
    return -1; 
  }

  

}


