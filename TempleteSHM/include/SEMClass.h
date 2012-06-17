#ifndef SEMTEMPLATE__H__
#define SEMTEMPLATE__H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <unistd.h>
#include <iostream>

class SemClass {
 private:
  int   m_id;
  int   m_semflag;
  key_t m_key;
  char  m_keyfile[128];
  struct sembuf operations[4];
 public:
  SemClass(char* , char* flag = "r");
  ~SemClass();  
  bool MakeKey();
  bool OpenSEM();  
  bool InitSHM();
  bool CloseSEM();
  bool Lock( int );
  bool UnLock( int );

};

SemClass::SemClass( char* filename ,char* flag )
: id(0), key(0), shmflag(0)
{
  strcpy( keyfile, filename );
  if( flag[0] ='w' ){
    m_semflag = IPC_CREAT | 0666;
  }else if( flag[0] =='r' ){
    m_semflag = 0666;
  }else{
    m_semflag = 0666;
  }

  bool b = InitSEM();
  if( b ){
    std::cout<< "Make Semaphore" << std::endl;
  }else{
    std::cout<< "Fail to make shm" << std::endl;
  }
}					       

SemClass::~SemClass(){

}

bool SemClass::MakeKey(){
  if( ( this->m_key = ftok( this->keyfile,'r') ) == -1 ){
    perror ("ftok");
    return false;
  }else{
    return true;
  }
}

bool SemClass::OpenSEM(){
  if( ( this->m_id = semget( m_key, m_nsem, m_flag ) ) == -1 ){
    perror( "semget" );
    return false;
  }else{
    return true;
  }
}

bool SemClass::InitSem(){
  int rc = semctl ( m_id, 1, SETALL );
  reuturn true;
}

bool SemClass::CloseSem(){
  int rc = semctl( m_id, 1, IPC_RMID );
  if( rc == -1 ){
    perror( "semctl()" );
    return false;
  }else{
    return true;
  }
}
#endif
