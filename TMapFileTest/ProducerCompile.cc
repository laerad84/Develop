#include "TMapFile.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TSystem.h"
#include "TRandom.h"
#include <iostream>

int main( int argc, char** argv ){
  
  TMapFile* mfile = TMapFile::Create("hsimple.map","RECREATE", 24*1024*1024,
				     "Demo memory mapped file with histograms");

   // Create a 1d, a 2d and a profile histogram. These objects will
   // be automatically added to the current directory, i.e. mfile.
  TH1F* hpx    = new TH1F("hpx","This is the px distribution",100,-4,4);
  TH2F* hpxpy  = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
  TProfile* hprof  = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);
  
  // Set a fill color for the TH1F
  hpx->SetFillColor(48);
  
  // Print status of mapped file
  mfile->Print();
  
  // Endless loop filling histograms with random numbers
  Float_t px, py, pz;
  int ii = 0;
  std::cout<< "Start" << std::endl;
   while (1) {
     gRandom->Rannor(px,py);
     pz = px*px + py*py;
     hpx->Fill(px);
     hpxpy->Fill(px,py);
     hprof->Fill(px,pz);

     if (!(ii % 10)) {
       std::cout<< "Update" << std::endl;
       mfile->Update();       // updates all objects in shared memory
       std::cout<< "Updated" << std::endl;
       if(!ii){ std::cout<< "ls" << std::endl;mfile->ls();}  // print contents of mapped file after
       std::cout<< ii << std::endl;
     }                         // first update
     ii++;
     gSystem->Sleep(50);
   }
}
