#include <vector>
#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TH2D.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TMath.h"
#include "TStyle.h"

enum RCReweight{k_woRC,k_wRC,n_RCrw};
enum OscParams{k_dcp, k_th13, k_th23, k_dm32, n_OscParams};
enum Hierarchy{k_BH, k_NH, k_IH, n_Hierarchies};
enum IntervalType{k_CredibleInterval, k_ConfidenceInterval, n_IntervalTypes};
enum Intervals{k_0, k_1sig, k_90, k_2sig, k_99, k_3sig, k_4sig, n_Intervals};
enum LegendPos{kLegendTopLeft,kLegendTopMiddle,kLegendTopRight,kLegendMiddleRight};

//#include "Binning.h"
#include "Binning_Atm.h"  // special binning setting for atmospheric samples only fits

TH2D* ReturnBinning(TString HistName, int xOscParam, int yOscParam, int Hierarchy, bool RC);
void FormatHistogram(TH2D* Hist, int Hierarchy, std::vector<TString> HierarchyTitleOptions, int xOscParam, int yOscParam, std::vector<TString> OscParamAxisTitles, bool RC, std::vector<TString> RCTitleOptions, TString IntervalAxisLabel, TString IntervalName="");
std::vector< std::vector< std::vector< std::vector<TH2D*> > > > SplitIntervalByLevel(std::vector< std::vector< std::vector<TH2D*> > > vec_HistPost, std::vector< std::vector< std::vector< std::vector<double> > > > vec_HistIntvLevels);

void MakeContours2D(TString FileName, 
    int BurnIn=0, 
    bool RC=false, 
    int IntervalType=k_CredibleInterval, 
    bool Draw=false, 
    TString AdditionalNameInfo="") {
  //--------------------------------------------------------------------------------------------------------

  gStyle->SetNumberContours(255);
  gStyle->SetLineStyleString(11,"[9 15]");

  //Used in Histogram Z Axis Caption
  std::vector<TString> IntervalTypeAxisNames(n_IntervalTypes);
  IntervalTypeAxisNames[k_CredibleInterval] = "Posterior probability";
  IntervalTypeAxisNames[k_ConfidenceInterval] = "#Delta #chi^{2}";

  //Used in Histogram Name (Not Title)
  std::vector<TString> IntervalTypeNames(n_IntervalTypes);
  IntervalTypeNames[k_CredibleInterval] = "Credible";
  IntervalTypeNames[k_ConfidenceInterval] = "Confidence";

  //Used in Histogram Name (Not Title)
  std::vector<TString> OscParamNames(n_OscParams);
  OscParamNames[k_dcp] = "dcp";
  OscParamNames[k_th13] = "th13";
  OscParamNames[k_th23] = "th23";
  OscParamNames[k_dm32] = "dm32";

  //Branches read in from TTree
  std::vector<TString> OscParamBranchNames(n_OscParams);
  OscParamBranchNames[k_dcp] = "dcp";
  OscParamBranchNames[k_th13] = "theta13";
  OscParamBranchNames[k_th23] = "theta23";
  OscParamBranchNames[k_dm32] = "dm23";

  //Used in Histogram Axis Titles
  std::vector<TString> OscParamAxisTitles(n_OscParams);
  OscParamAxisTitles[k_dcp] = "#delta_{CP}";
  OscParamAxisTitles[k_th13] = "sin^{2} #theta_{13}";
  OscParamAxisTitles[k_th23] = "sin^{2} #theta_{23}";
  OscParamAxisTitles[k_dm32] = "#Delta m^{2}_{23}";

  //Used in Histogram Name (Not Title)
  std::vector<TString> HierarchyNames(n_Hierarchies);
  HierarchyNames[k_BH] = "BH";
  HierarchyNames[k_NH] = "NH";
  HierarchyNames[k_IH] = "IH";

  //Used in Histogram Title (Not Name)
  std::vector<TString> HierarchyTitleOptions(n_Hierarchies);
  HierarchyTitleOptions[k_BH] = "Both Hierarchies";
  HierarchyTitleOptions[k_NH] = "Normal Hierarchy";
  HierarchyTitleOptions[k_IH] = "Inverted Hierarchy";

  //Used in Histogram Title (Not Name)
  std::vector<TString> RCReweightTitleOptions(n_RCrw);
  RCReweightTitleOptions[k_woRC] = "Without Reactor Constraint";
  RCReweightTitleOptions[k_wRC] = "With Reactor Constraint";

  //Credible Interval Values
  std::vector<double> CredibleInterval(n_Intervals);
  CredibleInterval[k_0] = 0.;
  CredibleInterval[k_1sig] = 0.68;
  CredibleInterval[k_90] = 0.90;
  CredibleInterval[k_2sig] = 0.954;
  CredibleInterval[k_99] = 0.99;
  CredibleInterval[k_3sig] = 0.9973;
  CredibleInterval[k_4sig] = 0.999937;

  //Confidence Interval Values (Table 40.2: https://pdg.lbl.gov/2020/reviews/rpp2020-rev-statistics.pdf)
  std::vector<double> ConfidenceInterval(n_Intervals);
  ConfidenceInterval[k_0] = 0.;
  ConfidenceInterval[k_1sig] = 2.30;
  ConfidenceInterval[k_90] = 4.61;
  ConfidenceInterval[k_2sig] = 6.18;
  ConfidenceInterval[k_99] = 9.21;
  ConfidenceInterval[k_3sig] = 11.83;
  ConfidenceInterval[k_4sig] = 16.0; //DB This value needs calculating

  //Interval Legend Entries
  std::vector<TString> IntervalCaption(n_Intervals);
  IntervalCaption[k_0] = "Posterior";
  IntervalCaption[k_1sig] = "68% "+IntervalTypeNames[IntervalType]+" Interval";
  IntervalCaption[k_90] = "90% "+IntervalTypeNames[IntervalType]+" Interval";
  IntervalCaption[k_2sig] = "2#sigma "+IntervalTypeNames[IntervalType]+" Interval";
  IntervalCaption[k_99] = "99% "+IntervalTypeNames[IntervalType]+" Interval";
  IntervalCaption[k_3sig] = "3#sigma "+IntervalTypeNames[IntervalType]+" Interval";
  IntervalCaption[k_4sig] = "4#sigma "+IntervalTypeNames[IntervalType]+" Interval";

  //Used in Histogram Title (Not Name) and Directory Name
  std::vector<TString> IntervalName(n_Intervals);
  IntervalName[k_0] = "All";
  IntervalName[k_1sig] = "1sig";
  IntervalName[k_90] = "90";
  IntervalName[k_2sig] = "2Sig";
  IntervalName[k_99] = "99";
  IntervalName[k_3sig] = "3Sig";
  IntervalName[k_4sig] = "4Sig";

  //Histogram Line Style
  std::vector<int> IntervalStyle(n_Intervals);
  IntervalStyle[k_0] = 1;
  IntervalStyle[k_1sig] = 1;
  IntervalStyle[k_90] = 3;
  IntervalStyle[k_2sig] = 3;
  IntervalStyle[k_99] = 11;
  IntervalStyle[k_3sig] = 11;
  IntervalStyle[k_4sig] = 11;

  /*
  IntervalStyle[k_0] = 1;
  IntervalStyle[k_1sig] = 1;
  IntervalStyle[k_90] = 3;
  IntervalStyle[k_2sig] = 3;
  IntervalStyle[k_99] = 2;
  IntervalStyle[k_3sig] = 2;
  IntervalStyle[k_4sig] = 2;
  */

  //Which contours do you want to overlay per canvas
  std::vector< std::vector<int> > DrawGroups(2);

  std::vector<int> DrawGroup_1(3);
  DrawGroup_1[2] = k_1sig;
  DrawGroup_1[1] = k_90;
  DrawGroup_1[0] = k_99;

  DrawGroups[0] = DrawGroup_1;

  std::vector<int> DrawGroup_2(3);
  DrawGroup_2[2] = k_1sig;
  DrawGroup_2[1] = k_2sig;
  DrawGroup_2[0] = k_3sig;

  DrawGroups[1] = DrawGroup_2;

  //--------------------------------------------------------------------------------------------------------

  //Where the Legend is position on a OscParam/Hierearchy basis
  std::vector< std::vector< std::vector<int> > > LegendPosition(n_Hierarchies);
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    LegendPosition[iHierarchy].resize(n_OscParams);
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      LegendPosition[iHierarchy][iOscParam].resize(n_OscParams);
      for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	LegendPosition[iHierarchy][iOscParam][jOscParam] = kLegendTopRight;
      }
    }
  }

  for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
    LegendPosition[k_BH][iOscParam][k_dm32] = kLegendMiddleRight;
  }

  for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
    LegendPosition[k_BH][k_dm32][iOscParam] = kLegendTopMiddle;
  }

  //--------------------------------------------------------------------------------------------------------
  bool FoundIntervalType = false;
  for (int iIntervalType=0;iIntervalType<n_IntervalTypes;iIntervalType++) {
    if (IntervalType == iIntervalType) {
      FoundIntervalType = true;
      break;
    }
  }
  if (!FoundIntervalType) {
    std::cout << "IntervalType:" << IntervalType << " not found!" << std::endl;
    std::exit(-1);
  }
  std::cout << "Calculating "+IntervalTypeNames[IntervalType]+" Interval" << std::endl;

  //--------------------------------------------------------------------------------------------------------
  gStyle->SetOptStat(0);
  TGaxis::SetMaxDigits(4);

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
  std::vector<int> IntervalIndex(n_Intervals);
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    IntervalIndex[iInterval] = n_Intervals-iInterval;
  }

  std::vector< std::vector< std::vector<TH2D*> > > vec_HistPost(n_Hierarchies);
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    vec_HistPost[iHierarchy].resize(n_OscParams);
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      vec_HistPost[iHierarchy][iOscParam].resize(n_OscParams);
    }
  }

  TString HistName;
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	if (jOscParam == iOscParam) continue;
	HistName = "h_"+OscParamNames[iOscParam]+"_"+OscParamNames[jOscParam]+"_"+HierarchyNames[iHierarchy];
	vec_HistPost[iHierarchy][iOscParam][jOscParam] = (TH2D*)ReturnBinning(HistName,iOscParam,jOscParam,iHierarchy,RC)->Clone();
	FormatHistogram(vec_HistPost[iHierarchy][iOscParam][jOscParam],iHierarchy,HierarchyTitleOptions,iOscParam,jOscParam,OscParamAxisTitles,RC,RCReweightTitleOptions,IntervalTypeAxisNames[IntervalType]);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  std::cout << "Filling histograms from TTree" << std::endl;

  int nEntries = Tree->GetEntries();

  std::vector<float> OscParams(n_OscParams);
  float b_RCw = 1.;
  int b_Step;

  for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
    Tree->SetBranchAddress(OscParamBranchNames[iOscParam], &OscParams[iOscParam]);
  }
  Tree->SetBranchAddress("step", &b_Step);
  if (RC) {
    std::cout << "Using RC reweight" << std::endl;
    Tree->SetBranchAddress("RCreweight", &b_RCw);
  }

  //JJ: a hard-coded LogL cut
  float LogL = 0.;
  Tree->SetBranchStatus("LogL",true);
  Tree->SetBranchAddress("LogL",&LogL);

  for (int iEntry=0;iEntry<nEntries;iEntry++) {
    Tree->GetEntry(iEntry);

    if (b_Step < BurnIn) continue;
    //JJ: a hard-coded LogL cut
    //if (LogL<1000.) continue;

    if (iEntry % 1000000 == 0) {
      std::cout << iEntry << "/" << nEntries << " (" << double(iEntry)/double(nEntries)*100. << "%)" << std::endl;
    }
    

    double totWeight = b_RCw;

    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	if (jOscParam == iOscParam) continue;

	vec_HistPost[k_BH][iOscParam][jOscParam]->Fill(OscParams[iOscParam],OscParams[jOscParam],totWeight);
	
	if (OscParams[k_dm32] >= 0) {
	  vec_HistPost[k_NH][iOscParam][jOscParam]->Fill(OscParams[iOscParam],OscParams[jOscParam],totWeight);
	} else {
	  vec_HistPost[k_IH][iOscParam][jOscParam]->Fill(OscParams[iOscParam],OscParams[jOscParam],totWeight);
	}

      }
    }

  }

  //Smooth after filling histograms from TTree
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
        if (jOscParam == iOscParam) continue;
	vec_HistPost[iHierarchy][iOscParam][jOscParam]->Smooth(1);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  if (IntervalType == k_ConfidenceInterval) {
    std::cout << "Calculating confidence posterior" << std::endl;;
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  double MaxBinContent = vec_HistPost[iHierarchy][iOscParam][jOscParam]->GetMaximum();
	  for (int xBin=1;xBin<=vec_HistPost[iHierarchy][iOscParam][jOscParam]->GetNbinsX();xBin++) {
	    for (int yBin=1;yBin<=vec_HistPost[iHierarchy][iOscParam][jOscParam]->GetNbinsY();yBin++) {
	      double BinContent = vec_HistPost[iHierarchy][iOscParam][jOscParam]->GetBinContent(xBin,yBin);
	      if (BinContent == 0) BinContent = 1;
	      BinContent = -2.0 * TMath::Log(BinContent/MaxBinContent);
	      vec_HistPost[iHierarchy][iOscParam][jOscParam]->SetBinContent(xBin,yBin,BinContent);
	    }
	  }
	}
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  if (IntervalType == k_CredibleInterval) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  vec_HistPost[iHierarchy][iOscParam][jOscParam]->Scale(1.0/vec_HistPost[iHierarchy][iOscParam][jOscParam]->Integral());
	}
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  std::vector< std::vector< std::vector< std::vector<double> > > > vec_HistIntvLevels(n_Intervals);
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    vec_HistIntvLevels[iInterval].resize(n_Hierarchies);
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      vec_HistIntvLevels[iInterval][iHierarchy].resize(n_OscParams);
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	vec_HistIntvLevels[iInterval][iHierarchy][iOscParam].resize(n_OscParams);
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  vec_HistIntvLevels[iInterval][iHierarchy][iOscParam][jOscParam] = 0.;
	}
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  std::cout << "Calculating intervals from histograms" << std::endl;

  double Integral, Sum = 0.;
  int MaxBin = 0;
  TH2D* Hist;

  if (IntervalType == k_CredibleInterval) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      std::cout << "\tHierarchy: " << iHierarchy << std::endl;
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  std::cout << "\t\t iOscParam:jOscParam: " << iOscParam << ":" << jOscParam << std::endl;
	  if (jOscParam == iOscParam) continue;
	  
	  Hist = (TH2D*)vec_HistPost[iHierarchy][iOscParam][jOscParam]->Clone();
	  Integral = Hist->Integral();
	  Sum = 0.;
	  MaxBin = 0;
	  
	  while((Sum/Integral) < CredibleInterval[n_Intervals-1]) {
	    
	    Sum += Hist->GetMaximum();
	    MaxBin = Hist->GetMaximumBin();
	    
	    for (int iCredibleInterval=1;iCredibleInterval<n_Intervals;iCredibleInterval++) {
	      if ((Sum/Integral >= CredibleInterval[iCredibleInterval-1]) && (Sum/Integral < CredibleInterval[iCredibleInterval])) {
		vec_HistIntvLevels[iCredibleInterval][iHierarchy][iOscParam][jOscParam] = Hist->GetMaximum();
		Hist->SetBinContent(MaxBin,0.);
	      }
	    }
	    
	  }
	  
	}
      }
    }
  } else if (IntervalType == k_ConfidenceInterval) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  double MinChi2 = vec_HistPost[iHierarchy][iOscParam][jOscParam]->GetMinimum();
	  for (int iConfidenceInterval=1;iConfidenceInterval<n_Intervals;iConfidenceInterval++) {
	    vec_HistIntvLevels[iConfidenceInterval][iHierarchy][iOscParam][jOscParam] = MinChi2+ConfidenceInterval[iConfidenceInterval];
	  }
	}
      }
    }
  }
  
  //--------------------------------------------------------------------------------------------------------
  std::cout << "Splitting histograms by interval level" << std::endl;
  
  std::vector< std::vector< std::vector< std::vector<TH2D*> > > > vec_HistIntv_ByIntv = SplitIntervalByLevel(vec_HistPost,vec_HistIntvLevels);
  
  /* 
  //Smooth after Contour Levels have been found
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  vec_HistIntv_ByIntv[iInterval][iHierarchy][iOscParam][jOscParam]->Smooth(1);
	}
      }
    }
  }
  */

  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  FormatHistogram(vec_HistIntv_ByIntv[iInterval][iHierarchy][iOscParam][jOscParam],iHierarchy,HierarchyTitleOptions,iOscParam,jOscParam,OscParamAxisTitles,RC,RCReweightTitleOptions,IntervalTypeAxisNames[IntervalType]);
	}
      }
    }
  }
  
  //--------------------------------------------------------------------------------------------------------

  for (int iInterval=1;iInterval<n_Intervals;iInterval++) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  vec_HistIntv_ByIntv[iInterval][iHierarchy][iOscParam][jOscParam]->SetLineColor(kRed);
	  vec_HistIntv_ByIntv[iInterval][iHierarchy][iOscParam][jOscParam]->SetLineStyle(IntervalStyle[iInterval]);
	}
      }
    }
  } 
  
  //--------------------------------------------------------------------------------------------------------
  if (Draw) {
    std::cout << "Drawing posteriors" << std::endl;
    
    TCanvas* Canvas = new TCanvas();
    Canvas->SetRightMargin(0.15);
    TString PDFName;
    
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  
	  for (int iDrawGroup=0;iDrawGroup<(int)DrawGroups.size();iDrawGroup++) {
	    PDFName = "Contours_2D_"+OscParamNames[iOscParam]+"_"+OscParamNames[jOscParam]+"_"+HierarchyNames[iHierarchy]+"_"+Form("%i",iDrawGroup);
	    if (AdditionalNameInfo!="") {
	      PDFName += "_"+AdditionalNameInfo;
	    }
	    PDFName += ".pdf";
	    
	    //vec_HistPost[iHierarchy][iOscParam][jOscParam]->Draw("COLZ");
	    vec_HistPost[iHierarchy][iOscParam][jOscParam]->Draw("AXIS");
	    
	    TLegend* Legend;
	    if (LegendPosition[iHierarchy][iOscParam][jOscParam] == kLegendTopRight) {
	      Legend = new TLegend(0.5,0.7,0.89,0.89);
	    } else if (LegendPosition[iHierarchy][iOscParam][jOscParam] == kLegendTopMiddle) {
	      Legend = new TLegend(0.3,0.7,0.69,0.89);
	    } else if (LegendPosition[iHierarchy][iOscParam][jOscParam] == kLegendTopLeft) {
	      Legend = new TLegend(0.12,0.7,0.45,0.89);
	    } else if (LegendPosition[iHierarchy][iOscParam][jOscParam] == kLegendMiddleRight) {
	      Legend = new TLegend(0.5,0.3,0.89,0.59);
	    } else {
	      std::cout << "Legend Position not defined!" << std::endl;
	      std::exit(0);
	    }
	    
	    Legend->SetLineColor(kWhite);
	    Legend->SetFillColor(kWhite);
	    Legend->SetFillStyle(0);
	    Legend->SetTextFont(132);
	    Legend->SetTextSize(0.04);
	    Legend->SetBorderSize(0);
	    
	    for (int iDrawGroupInt=0;iDrawGroupInt<(int)DrawGroups[iDrawGroup].size();iDrawGroupInt++) {
	      vec_HistIntv_ByIntv[DrawGroups[iDrawGroup][iDrawGroupInt]][iHierarchy][iOscParam][jOscParam]->Draw("SAME CONT3");
	      Legend->AddEntry(vec_HistIntv_ByIntv[DrawGroups[iDrawGroup][iDrawGroupInt]][iHierarchy][iOscParam][jOscParam],IntervalCaption[DrawGroups[iDrawGroup][iDrawGroupInt]],"l");
	    }
	    
	    Legend->Draw("SAME");
	    Canvas->Print(PDFName);
	    Canvas->Clear();
	  }
	}
      }
    }
  }
  
  
  //--------------------------------------------------------------------------------------------------------
  TString OutFileName = "Contours_2D";
  if (RC) OutFileName += "_wRC";
  else OutFileName += "_woRC";
  OutFileName += Form("_BurnIn_%i", BurnIn);
  if (AdditionalNameInfo!="") {
    OutFileName += "_"+AdditionalNameInfo;
  }
  OutFileName += "_float";
  OutFileName += ".root";

  TFile* OutputFile = new TFile(OutFileName,"RECREATE");

  std::cout << "Writing output to file:" << OutFileName << std::endl;

  OutputFile->mkdir("Posteriors");
  OutputFile->cd("Posteriors");
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	if (jOscParam == iOscParam) continue;
	vec_HistPost[iHierarchy][iOscParam][jOscParam]->Write();
      }
    }
  }
  
  OutputFile->mkdir(IntervalTypeNames[IntervalType]+"Intervals");
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    TString DirName = IntervalTypeNames[IntervalType]+"Intervals/"+IntervalName[iInterval];
    OutputFile->mkdir(DirName);
    OutputFile->cd(DirName);
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  vec_HistIntv_ByIntv[iInterval][iHierarchy][iOscParam][jOscParam]->Write();
	}
      }
    }
  }
  
  OutputFile->Close();
  
  std::cout << "Finished" << std::endl;
}
  
  void FormatHistogram(TH2D* Hist, int Hierarchy, std::vector<TString> HierarchyTitleOptions, int xOscParam, int yOscParam, std::vector<TString> OscParamAxisTitles, bool RC, std::vector<TString> RCTitleOptions, TString IntervalAxisLabel, TString IntervalName) {
  TString HistTitle = "";

  if (RC) {
    HistTitle += RCTitleOptions[k_wRC];
  } else {
    HistTitle += RCTitleOptions[k_woRC];
  }
  HistTitle += ", ";

  HistTitle += HierarchyTitleOptions[Hierarchy];

  if (IntervalName!="") {
    HistTitle += ", ";
    HistTitle += IntervalName;
  }

  Hist->SetTitle(HistTitle);

  TString YAxisTitle = OscParamAxisTitles[yOscParam];
  Hist->GetYaxis()->SetTitle(YAxisTitle);
  Hist->GetYaxis()->SetTitleOffset(1.2);

  TString XAxisTitle = OscParamAxisTitles[xOscParam];
  Hist->GetXaxis()->SetTitle(XAxisTitle);  

  TString ZAxisTitle = IntervalAxisLabel;
  Hist->GetZaxis()->SetTitle(ZAxisTitle);

  Hist->SetLineWidth(1);
}

std::vector< std::vector< std::vector< std::vector<TH2D*> > > > SplitIntervalByLevel(std::vector< std::vector< std::vector<TH2D*> > > vec_HistPost, std::vector< std::vector< std::vector< std::vector<double> > > > vec_HistIntvLevels) {
  std::vector< std::vector< std::vector< std::vector<TH2D*> > > > returnVec(n_Intervals);
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    returnVec[iInterval].resize(n_Hierarchies);
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      returnVec[iInterval][iHierarchy].resize(n_OscParams);
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	returnVec[iInterval][iHierarchy][iOscParam].resize(n_OscParams);
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  returnVec[iInterval][iHierarchy][iOscParam][jOscParam] = (TH2D*)vec_HistPost[iHierarchy][iOscParam][jOscParam]->Clone();
	}
      }
    }
  }

  for (int iInterval=1;iInterval<n_Intervals;iInterval++) {
    for(int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int jOscParam=0;jOscParam<n_OscParams;jOscParam++) {
	  if (jOscParam == iOscParam) continue;
	  double Contour[1];
	  Contour[0] = vec_HistIntvLevels[iInterval][iHierarchy][iOscParam][jOscParam];
	  returnVec[iInterval][iHierarchy][iOscParam][jOscParam]->SetContour(1,Contour);
	}
      }
    }
  }

  return returnVec;
}


TH2D* ReturnBinning(TString HistName, int xOscParam, int yOscParam, int Hierarchy, bool RC) {
  /*
  int dcp_nBins = 80;
  double dcp_lEdge = -1.0*TMath::Pi();
  double dcp_hEdge = 1.0*TMath::Pi();

  int th13_nBins = 300;
  double th13_lEdge = 0.005;
  double th13_hEdge = 0.08;

  if (RC) {
    th13_nBins = 100;
    th13_lEdge = 0.018;
    th13_hEdge = 0.028;
  }

  int th23_nBins = 30;
  double th23_lEdge = 0.35;
  double th23_hEdge = 0.7;

  int dm32_nBins = 200;
  double dm32_lEdge = -3.015e-3;
  double dm32_hEdge = 2.985e-3;

  if (Hierarchy == k_NH) {
    dm32_nBins = 200;
    dm32_lEdge = 0.0022;
    dm32_hEdge = 0.0031;
  }

  if (Hierarchy == k_IH) {
    dm32_nBins = 200;
    dm32_lEdge = -0.0029;
    dm32_hEdge = -0.0020;
  }
  */

  std::vector<double> Binning = GetBinning(2, Hierarchy, RC, xOscParam, yOscParam);
  
  TH2D* Hist = new TH2D(HistName,"",(int)Binning[0],Binning[1],Binning[2],(int)Binning[3],Binning[4],Binning[5]);
  return Hist;
}
