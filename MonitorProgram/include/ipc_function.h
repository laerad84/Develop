#ifndef IPC_FUNCTION__H__
#definde IPC_FUNCTION__H__
#include "MonitorStruct.h"

bool MakeKey( char*, key_t& id);

template < class T >
bool Make_shm( int&, key_t, char*, T** );
template < class T >
bool Close_shm( T** );

template < class T >
bool Connect_shm( int&, key_t, char*, T**);
template < class T >
bool Disconnect_shm( int&, key_t, char*, T**);


bool Make_sem( int&, key_t, 

bool bLoop  = true;
void sigint_handler( int );

int SigAlrm = 0; 
void sigalrm_handler;


#endif


