/*
----------------------
reduceDataSet.cpp

- Eliminates the number of branches in the MCMC output so it's quicker to plot oscillation contours

Usage:
./reduceDataSet inputFile outputFile

Output:
ROOT file with oscillation contours, LLH and step branches
----------------------
*/

#include <iostream>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"
#include "TObjArray.h"
#include "TBranch.h"
#include "TString.h"

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    std::cout << "usage: reduceDataSet \"/some/files/blah*.root\" outputfile.root \"some information string\"" << std::endl;
    std::cout << "note: the last argument is optional, enter a sentence to describe whats in the file." << std::endl;
    return 1;
  }

  double t_sin2th_23;
  double t_sin2th_13;
  double t_sin2th_12;
  double t_delm2_23;
  double t_delta_cp;
  double t_LogL;
  /*
  double t_xsec_66_w;
  double t_xsec_67_w;
  double t_xsec_24_w;
  */
  int t_step;
  TNamed info;

  if (argc == 4)
    info = TNamed("info", argv[3]);

  TChain* myChain = new TChain("posteriors","");
  std::cout << "Adding files " << argv[1] << " to TTree " << myChain->GetName() << "..." << std::endl;
  myChain->Add(argv[1]);
  // get branches names from chain
  // get the list of branches
  TObjArray* brlis = (TObjArray*)myChain->GetListOfBranches();
  // branch numbers
  int nbr = brlis->GetEntries();
  // arrays of the nuisance parmaters' branch names
  std::vector<TString> atmflux_brs;
  std::vector<TString> xsec_brs;
  std::vector<TString> corrfddet_brs;
  // loop over the branches to get the names
  for (int i=0; i<nbr; i++){
    TBranch* br = (TBranch*)brlis->At(i);
    TString bname = br->GetName();
    if (bname.BeginsWith("ndd_")||bname.BeginsWith("LogL")) continue;

    if (bname.BeginsWith("atmflux_")) {
      atmflux_brs.push_back(bname);
    }else if (bname.BeginsWith("xsec_")){
      xsec_brs.push_back(bname);	    
    }else if (bname.BeginsWith("corrfddet_")){
      corrfddet_brs.push_back(bname);
    }
  }
  // nuisance parameter numbers
  int n_atmflux = atmflux_brs.size();
  int n_xsec = xsec_brs.size();
  int n_corrfddet = corrfddet_brs.size();
  // arrays to hold the values
  std::cout << "number of parameters: " << n_atmflux << " " << n_xsec << " " << n_corrfddet << std::endl;
  double* t_atmflux_array = new double[n_atmflux];
  double* t_xsec_array = new double[n_xsec];
  double* t_corrfddet_array = new double[n_corrfddet];

  // Turn off all the branches
  myChain->SetBranchStatus("*",0);

  myChain->SetBranchStatus("sin2th_23",1);
  myChain->SetBranchAddress("sin2th_23", &t_sin2th_23);

  myChain->SetBranchStatus("sin2th_12",1);
  myChain->SetBranchAddress("sin2th_12", &t_sin2th_12);

  myChain->SetBranchStatus("sin2th_13",1);
  myChain->SetBranchAddress("sin2th_13", &t_sin2th_13);

  myChain->SetBranchStatus("delm2_23",1);
  myChain->SetBranchAddress("delm2_23", &t_delm2_23);

  myChain->SetBranchStatus("delta_cp",1);
  myChain->SetBranchAddress("delta_cp", &t_delta_cp);

  myChain->SetBranchStatus("LogL",1);
  myChain->SetBranchAddress("LogL", &t_LogL);

  myChain->SetBranchStatus("step",1);
  myChain->SetBranchAddress("step", &t_step);

  for(int i=0; i<n_atmflux; i++){
    myChain->SetBranchStatus(atmflux_brs[i], 1);
    myChain->SetBranchAddress(atmflux_brs[i], &t_atmflux_array[i]);
  }
  for(int i=0; i<n_xsec; i++){
    myChain->SetBranchStatus(xsec_brs[i], 1);
    myChain->SetBranchAddress(xsec_brs[i], &t_xsec_array[i]);
  }
  for(int i=0; i<n_corrfddet; i++){
    myChain->SetBranchStatus(corrfddet_brs[i], 1);
    myChain->SetBranchAddress(corrfddet_brs[i], &t_corrfddet_array[i]);
  }

  /*
  myChain->SetBranchStatus("xsec_66_w", 1);
  myChain->SetBranchAddress("xsec_66_w", &t_xsec_66_w);

  myChain->SetBranchStatus("xsec_67_w", 1);
  myChain->SetBranchAddress("xsec_67_w", &t_xsec_67_w);

  myChain->SetBranchStatus("xsec_24_w", 1);
  myChain->SetBranchAddress("xsec_24_w", &t_xsec_24_w);
  */

  TFile *newfile = new TFile(argv[2],"recreate");
  TTree *newtree = new TTree("posteriors","oscillation, atmflux, xsec, and corrfddet parameter posteriors, marginalized");
  float t_sin2th_23_float;
  float t_sin2th_13_float;
  float t_sin2th_12_float;
  float t_delm2_23_float;
  float t_delta_cp_float;
  float t_LogL_float;
  /*
  double t_xsec_66_w_double;
  double t_xsec_67_w_double;
  double t_xsec_24_w_double;
  */
  float* t_atmflux_float_array = new float[n_atmflux];
  float* t_xsec_float_array = new float[n_xsec];
  float* t_corrfddet_float_array = new float[n_corrfddet];

  newtree->Branch("theta23",  &t_sin2th_23_float, "theta23/F");
  newtree->Branch("theta13",  &t_sin2th_13_float, "theta13/F");
  newtree->Branch("theta12",  &t_sin2th_12_float, "theta12/F");
  newtree->Branch("dm23",     &t_delm2_23_float,  "dm23/F");
  newtree->Branch("dcp",      &t_delta_cp_float,  "dcp/F");
  newtree->Branch("step",     &t_step,            "step/I");
  newtree->Branch("LogL",     &t_LogL_float,      "LogL/F");
  for(int i=0; i<n_atmflux; i++){
    newtree->Branch(atmflux_brs[i], &t_atmflux_float_array[i], atmflux_brs[i]+"/F");
  }
  for(int i=0; i<n_xsec; i++){
    newtree->Branch(xsec_brs[i], &t_xsec_float_array[i], xsec_brs[i]+"/F");
  }
  for(int i=0; i<n_corrfddet; i++){
    newtree->Branch(corrfddet_brs[i], &t_corrfddet_float_array[i], corrfddet_brs[i]+"/F");
  }
  
  /*
  newtree->Branch("xsec_66_w",&t_xsec_66_w_double,      "xsec_66_w/D");
  newtree->Branch("xsec_67_w",&t_xsec_67_w_double,      "xsec_67_w/D");
  newtree->Branch("xsec_24_w",&t_xsec_24_w_double,      "xsec_24_w/D");
  */

  newtree->SetAutoSave(1E7);

  int nentries = myChain->GetEntries();
  for (int i = 0; i < nentries; ++i) {
    myChain->GetEntry(i);
    if (i % 100000 == 0) {
      std::cout << "on step " << i << "/" << nentries << " (" << double(i)/double(nentries)*100. << "%)" << std::endl;
    }

    t_sin2th_23_float = t_sin2th_23;
    t_sin2th_13_float = t_sin2th_13;
    t_sin2th_12_float = t_sin2th_12;
    t_delm2_23_float = t_delm2_23;
    t_delta_cp_float = t_delta_cp;
    t_LogL_float = t_LogL;
    for(int j=0; j<n_atmflux; j++){
      t_atmflux_float_array[j] = t_atmflux_array[j];
    }
    for(int j=0; j<n_xsec; j++){
      t_xsec_float_array[j] = t_xsec_array[j];
    }
    for(int j=0; j<n_corrfddet; j++){
      t_corrfddet_float_array[j] = t_corrfddet_array[j];
    }

    /*
    t_xsec_66_w_double = t_xsec_66_w;
    t_xsec_67_w_double = t_xsec_67_w;
    t_xsec_24_w_double = t_xsec_24_w;
    */

    //myChain->GetEntry(i);
    newtree->Fill();
  }

  if (argc == 4) info.Write();

  newtree->Print();
  newtree->AutoSave();

  delete newfile;

  return 0;
}
