#include <fstream>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "Monitor_Struct.h"
#include "SHMClass.h"

int main(int argc, char *argv[])
{
  char  filename[128] = "/Users/jwlee/work/test/C++/shared_memory/shm2.dat";
  shmTemplate<struct TEMPData> *temp = new shmTemplate<struct TEMPData>(const_cast<char*>(filename),"w");
  
  for( int i = 0; i< 10; i++){
    temp->t->now = time(NULL);
    temp->t->temp[0] = i;
    std::cout <<temp->t->now << "\t" << std::endl;
    usleep(1000);
  }
  temp->closeSHM();
  temp->~shmTemplate();
  
}

