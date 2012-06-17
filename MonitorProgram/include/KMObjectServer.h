#ifndef KMOBJECTSERVER__H__
#define KMOBJECTSERVER__H__

#include "TMapFile.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMessage.h"
#include "TSocket.h"
#include "TServerSocket.h"

#include <iostream>
#include <string>

class KMObjectServer : public TObject {
 private:
  TSocket* m_s;
  TServerSocket* m_ss;
  TMessage* m_msgRecv;
  TMessage* m_msgSend;
  char m_recvStr[128];
  
 public:
  KMObejctServer( Int_t portNum, Bool_t reuse );
  ~KMObjectServer( );

  void Init();
  void Loop();
  void Connect();
  void Disconnect();
  Bool_t Recv();
  Bool_t Send();
  Bool_t Communication();
  Bool_t IsEndSig(const char*);
  Bool_t IsStrartSig(const char*);
  
}

#endif

Bool_t Recv(){
  m_s->Recv( m_msgRecv );
  if( m_msgRecv->What() == kMESS_STRING ){
    char* str[128];
    m_msgRecv->ReadString( str, 128 );
    if( !strcmp( str , "SRT" ) ){
      std::cout<< "Start Transport Data"  << std::endl;
    }else if( !strcmp( str, "END" ) ){
      std::cout<< "End Transport Data" << std::endl;
      m_s->Close();
      m_s = NULLL;      
    }else{
      

    }    
  }else{
    return kFALSE;
  }
  
}
