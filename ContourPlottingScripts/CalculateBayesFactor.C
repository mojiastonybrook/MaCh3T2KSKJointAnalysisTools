#include <vector>
#include <iostream>
#include <iomanip>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

enum OscParams{k_dcp, k_th13, k_th23, k_dm32, n_OscParams};
enum Hierarchy{k_NH, k_IH, n_Hierarchies};
enum Octant{k_Lower, k_Upper, n_Octants};

void CalculateBayesFactor(TString FileName, int BurnIn=0, bool RC=false) {
  //--------------------------------------------------------------------------------------------------------
  //Branches read in from TTree
  std::vector<TString> OscParamBranchNames(n_OscParams);
  OscParamBranchNames[k_dcp] = "dcp";
  OscParamBranchNames[k_th13] = "theta13";
  OscParamBranchNames[k_th23] = "theta23";
  OscParamBranchNames[k_dm32] = "dm23";

  std::vector<TString> OctantNames(n_Octants);
  OctantNames[k_Lower] = "Lower";
  OctantNames[k_Upper] = "Upper";

  std::vector<TString> HierarchyNames(n_Hierarchies);
  HierarchyNames[k_NH] = "Normal Hierarchy";
  HierarchyNames[k_IH] = "Inverse Hierarchy";

  //--------------------------------------------------------------------------------------------------------

  TFile *File = new TFile(FileName);
  if (!File || File->IsZombie()) {
    std::cout << "File:" << FileName << " not found!" << std::endl;
    std::exit(-1);
  }

  TString TreeName = "osc_posteriors";
  TTree *Tree = (TTree*)File->Get(TreeName);
  if (!Tree) {
    std::cout << "Tree:" << TreeName << " not found in File:" << FileName << std::endl;
    std::exit(-1);
  }

  if ((!((TList*)Tree->GetListOfBranches())->FindObject("RCreweight")) && RC) {
    std::cout << "No RCreweight branch found in Tree, Reverting to no RC constraint" << std::endl;
    RC = false;
  }

  //--------------------------------------------------------------------------------------------------------
  std::cout << "Filling histograms from TTree" << std::endl;

  int nEntries = Tree->GetEntries();

  std::vector<float> OscParams(n_OscParams);
  float b_RCw = 1.;
  int b_Step;

  for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
    Tree->SetBranchAddress(OscParamBranchNames[iOscParam],&OscParams[iOscParam]);
  }
  Tree->SetBranchAddress("step",&b_Step);
  if (RC) Tree->SetBranchAddress("RCreweight",&b_RCw);

  std::vector< std::vector<double> > Integral(n_Octants);
  for (int iOctant=0;iOctant<n_Octants;iOctant++) {
    Integral[iOctant].resize(n_Hierarchies);
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      Integral[iOctant][iHierarchy] = 0.;
    }
  }

  double TotalIntegral = 0.;

  for (int iEntry=0;iEntry<nEntries;iEntry++) {
    Tree->GetEntry(iEntry);
    if(b_Step < BurnIn) continue;

    if (iEntry % 1000000 == 0) {
      std::cout << iEntry << "/" << nEntries << " (" << double(iEntry)/double(nEntries)*100. << "%)" << std::endl;
    }
    
    double totWeight = b_RCw;
    TotalIntegral += totWeight;

    int Octant;
    if (OscParams[k_th23] > 0.5) {
      Octant = k_Upper;
    } else {
      Octant = k_Lower;
    }
    
    int Hierarchy;
    if (OscParams[k_dm32] > 0) {
      Hierarchy = k_NH;
    } else {
      Hierarchy = k_IH;
    }

    Integral[Octant][Hierarchy] += totWeight;
  }

  for (int iOctant=0;iOctant<n_Octants;iOctant++) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      Integral[iOctant][iHierarchy] /= TotalIntegral;
    }
  }

  double IntegralCheck = 0.;

  for (int iOctant=0;iOctant<n_Octants;iOctant++) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      IntegralCheck += Integral[iOctant][iHierarchy];
    }
  }

  if (fabs(IntegralCheck-1.0) > 0.001) {
    std::cout << "Total Integral doesn't seem to add to 1." << std::endl;
    std::cout << "Integral Check:" << IntegralCheck << std::endl;
    std::exit(-1);
  }

  std::vector<double> HierarchyIntegral(n_Hierarchies,0);
  std::vector<double> OctantIntegral(n_Octants,0);

  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOctant=0;iOctant<n_Octants;iOctant++) {
      HierarchyIntegral[iHierarchy] += Integral[iOctant][iHierarchy];
      OctantIntegral[iOctant] += Integral[iOctant][iHierarchy];
    }
  }

  std::cout << std::setw(10) << " " << " | " << std::setw(10) << "LO" << " | " << std::setw(10) << "UO" << " | " << std::setw(10) << "Sum" << std::endl;
  std::cout << "--------------------------------------------------------" << std::endl;
  std::cout << std::setw(10) << "NH" << " | " << std::setw(10) << Integral[k_Lower][k_NH] << " | " << std::setw(10) << Integral[k_Upper][k_NH] << " | " << std::setw(10) << HierarchyIntegral[k_NH] << std::endl;
  std::cout << "--------------------------------------------------------" << std::endl;
  std::cout << std::setw(10) << "IH" << " | " << std::setw(10) << Integral[k_Lower][k_IH] << " | " << std::setw(10) << Integral[k_Upper][k_IH] << " | " << std::setw(10) << HierarchyIntegral[k_IH] << std::endl;
  std::cout << "--------------------------------------------------------" << std::endl;
  std::cout << std::setw(10) << "Sum" << " | " << std::setw(10) << OctantIntegral[k_Lower]<< " | " << std::setw(10) << OctantIntegral[k_Upper] << " | " << std::setw(10) << IntegralCheck << std::endl;
  std::cout << "--------------------------------------------------------" << std::endl;

}
