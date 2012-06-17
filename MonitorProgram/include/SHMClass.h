#ifndef SHMTEMPLATE__H__
#define SHMTEMPLATE__H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <iostream>
#include "Monitor_Struct.h"

template< class T >
class shmTemplate {
public: 
  int id;
  key_t key;
  char  keyfile[128];
  T *t;
  int shmflag;

  shmTemplate(char* , char* flag ="r");
  ~shmTemplate();
  bool makeKey();
  bool openSHM();
  bool attachSHM();
  bool closeSHM();
  bool initSHM();

};

template <class T>
shmTemplate<T>::shmTemplate(char* filename,char* flag) 
  : id(0), key(0), shmflag(0)
{
  strcpy(keyfile, filename);
  t = new T;

  if( flag[0] == 'w'){
    shmflag = IPC_CREAT | 0666;
  }else if( flag[0] == 'r' ){
    shmflag = 0666;
  }else{
    shmflag = 0666;
  }

   bool b = initSHM();
  if( b ){
    std::cout << "make shm" << std::endl;
  }else{
    std::cout << "fail to make shm" << std::endl;
  }
}

template<class T>
shmTemplate<T>::~shmTemplate(){  
  //delete t;
}

template<class T>
bool shmTemplate<T>::makeKey(){
  if(( this->key = ftok(this->keyfile,'r')) == -1){
    perror("ftok");
    return false;
  }else{
    return true;
  }
}
template<class T>
bool shmTemplate<T>::openSHM(){
  if((this->id = shmget(key, sizeof(T), shmflag )) == -1){
    perror("shmget");
    return false;
  }else{
    return true;
  }
}

template<class T>
bool shmTemplate<T>::attachSHM(){
  if((this->t = (T*)shmat(id,0,0)) == (void *)-1) {
    perror("shmat");
    return false;
  }else{
    true;
  }
}

template<class T>
bool shmTemplate<T>::initSHM(){
  if( makeKey() ){
    if( openSHM() ){
      if( attachSHM() ){
	return true;
      }
    }
  }
  return false;
}

template<class T>
bool shmTemplate<T>::closeSHM(){
  if( shmdt(t) == -1){
    std::cerr << "Failed to Destruct shared memory" << std::endl;
    return false;
  }else{
    std::cout << "Destruct shared memory" << std::endl;
    return true;
  }
}

#endif
