#include "TServerSocket.h"
#include "TSocket.h"
#include "TGraph.h"
#include "TMessage.h"

#include "TThread.h"

TThread *t[2]; // [0] Grapher, [1] Communicator;
Bool_t finished;
TServerSocket* ss;
TGraph* gr;


void *AddPoint( void * nb ){
  int index  = (int)nb;
  TThread::Lock();
  gr->SetPoint( ipoint , i%100 , (i%10)*(i%10) );
  TThread::UnLock();
}

void *Communication( void *nb){
  TSocket* s = NULL;
  while( 1 ){
    TMessage mess; 
    if( !s ){ 
      TThread::Lock();
      s = (( TServerSocket *)ss)->Accept();      
      TThread::UnLock();
      s->Recv( mess );
      if( mess->What() = kMESS_STRING ){
	char str[64];
	mess.ReadString( str , 64 );
	mess.Reset();
	std::cout << "RECV:" << str << std::endl;
	if( !strcmp( str, "TRANSPORT") ){ 
	  std::cout << "Start Communication" << std::endl;
	  mess.WriteObject( gr );
	  s->Send( mess );
	}else if( !strcmp( str, "END" ) ){
	  std::cout << "End Communication" << std::endl; 
	  delete mess;
	  s->Close();
	}else{
	  std::cout<< "Invalid Communication Code" << std::endl;
	}
	s->Close();
	s = NULL;
	delete mess;
	continue;       
      }else{
	std::cout<< "Server only Accept STRING MESSAGE" << std::endl;
	s->Close();
	s = NULL;
	delete mess;
      }
    }else{
      s->Close();
      s = NULL;
    }
  }
  ss->Close();
  return 0;
}


void *joiner( void *){
  t[0]->Join();
  t[1]->Join();
  finished = kTRUE;
  return 0;
}
  

int
main( int argc, char** argv ){

  TGraph* gr = new TGraph(); 
  gr->SetName( "graph","gr0");
  



  return 0;

}
