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
#include <TApplication.h>
#include <TCanvas.h>

#include <TROOT.h>
#include <TH1.h>
#include <TGraph.h>
#include <TMapFile.h>

#define SHMSZ 4096
int main(int argc , char** argv)
{
  std::cout << 1 << std::endl;
  TMapFile* map = TMapFile::Create("tmap.map","read");
  TApplication* app = new TApplication( "app", &argc, argv);
  TCanvas* can  = new TCanvas("can","",600,600);
  can->Draw();
  std::cout << 1 << std::endl;

  TGraph* gr;
  TGraph* grN;
  bool isDrawn = false;
  std::cout << 1 << std::endl;

  for( int i = 0; i< 100; i++){


    gr = (TGraph*)map->Get("testGr");
    grN= (TGraph*)map->Get("grN");
    std::cout << i << " : "<<  grN->GetX()[0] << std::endl;
    if( !isDrawn ){
      can->cd();
      gr->Draw("AP");
      isDrawn = true;
    }else{
      std::cout <<"Update" << std::endl;
      gr->Draw("AP");
      can->Update();
      can->Modified();
      can->Update();
    }
    delete gr;
    delete grN;
    gr  = NULL;
    grN = NULL;
    sleep(1);
  }
  

  app->Run();
  return 0;
}
