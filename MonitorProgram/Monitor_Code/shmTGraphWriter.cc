
#include <fstream>
#include <iostream>
#include <cmath>
#include <ctime>

#include <cstdio>
#include <cstdlib>
#include <cstring> /* strcpy */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include <TGraph.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TMapFile.h>

#define SHMSZ 4096
int main(int argc, char *argv[])
{
  long nevent;
  TMapFile* map = TMapFile::Create("tmap.map","RECREATE",20*1024*1024,"test TMapFile");
  TGraph* gr = NULL;
  TGraph* grN = NULL;
  std::cout << "create graph " << std::endl;
  gr = new TGraph();
  gr->SetNameTitle("testGr","testGr");
  grN = new TGraph();
  grN->SetNameTitle("grN","grN");
  map->Add(gr,gr->GetName());
  map->Add(grN,grN->GetName());
  nevent = 0;


  for( int i = 0; i<100; i++){
    std::cout << i << std::endl;
    gr->SetPoint( nevent%75 ,nevent ,i%75 );
    grN->SetPoint( 0, nevent, 0);		  
    nevent++;
    std::cout << gr->GetN() << " : " << grN->GetX()[0] << std::endl;
    std::cout << "update" << std::endl;
    map->Update(gr);
    map->Update(grN);
    std::cout << "end update" << std::endl;
    sleep(1);
  }
  map->Close();
}

