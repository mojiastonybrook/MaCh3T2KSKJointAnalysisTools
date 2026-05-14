#include <vector>
#include <iostream>
#include <iomanip>

#include "TString.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TMath.h"
#include "TStyle.h"
#include "TGaxis.h"

enum RCReweight{k_woRC, k_wRC, n_RCrw};
enum OscParams{k_dcp, k_th13, k_th23, k_dm32, n_OscParams};
enum Hierarchy{k_BH, k_NH, k_IH, n_Hierarchies};
enum IntervalType{k_CredibleInterval, k_ConfidenceInterval, n_IntervalTypes};
enum Intervals{k_0, k_1sig, k_90, k_2sig, k_99, k_3sig, k_4sig, n_Intervals};
enum LegendPos{kLegendTopLeft, kLegendTopMiddle, kLegendTopRight};

#include "Binning.h"
//#include "Binning_Atm.h"  //special binning setting for atmospheric samples only fits
//#include "Binning_Atm_wideBin.h"

TH1D* SpecifyBinning(int Param, int Hierarchy, bool wRC);
std::vector< std::vector< std::vector<TH1D*> > > MultiplyIntervalByPosterior(std::vector< std::vector< std::vector<TH1D*> > > vec_HistIntv, std::vector< std::vector<TH1D*> > vec_HistPost);
std::vector< std::vector< std::vector<TH1D*> > > SplitIntervalByLevel(std::vector< std::vector<TH1D*> > vec_HistIntv, std::vector<int> IntervalIndex);
void FormatHistogram(TH1D* Hist, int Hierarchy, std::vector<TString> HierarchyTitleOptions, int OscParam, std::vector<TString> OscParamAxisTitles, bool RC, std::vector<TString> RCTitleOptions, TString IntervalAxisLabel, TString IntervalName="");
void CalculateInterval(TH1D* Hist);
void ScaleHistogram(TH1D* Hist, double YAxisVal);

void MakeContours1D(TString FileName, 
    int BurnIn=0, 
    bool RC=false, 
    int IntervalType=k_CredibleInterval, 
    bool Draw=false, 
    TString AdditionalNameInfo="") {
  //--------------------------------------------------------------------------------------------------------

  //Used in Histogram Y Axis Caption
  std::vector<TString> IntervalTypeAxisNames(n_IntervalTypes);
  IntervalTypeAxisNames[k_CredibleInterval] = "Posterior probability";
  IntervalTypeAxisNames[k_ConfidenceInterval] = "#Delta #chi^{2}";

  //Used in Histogram Name (Not Title)
  std::vector<TString> IntervalTypeNames(n_IntervalTypes);
  IntervalTypeNames[k_CredibleInterval] = "Credible";
  IntervalTypeNames[k_ConfidenceInterval] = "Confidence";

  //Used in Histogram Name (Not Title) and PDF Name
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
  OscParamBranchNames[k_dm32] = "dm23";//"dm23_smeared";

  //Used in Histogram Axis Titles
  std::vector<TString> OscParamAxisTitles(n_OscParams);
  OscParamAxisTitles[k_dcp] = "#delta_{CP}";
  OscParamAxisTitles[k_th13] = "sin^{2} #theta_{13}";
  OscParamAxisTitles[k_th23] = "sin^{2} #theta_{23}";
  OscParamAxisTitles[k_dm32] = "#Delta m^{2}_{23}";

  //Used in Histogram Name (Not Title) and PDF Name
  std::vector<TString> HierarchyNames(n_Hierarchies);
  HierarchyNames[k_BH] = "BH";
  HierarchyNames[k_NH] = "NH";
  HierarchyNames[k_IH] = "IH";

  //Used in Histogram Title (Not Name)
  std::vector<TString> HierarchyTitleOptions(n_Hierarchies);
  HierarchyTitleOptions[k_BH] = "Both orderings";
  HierarchyTitleOptions[k_NH] = "Normal ordering";
  HierarchyTitleOptions[k_IH] = "Inverted ordering";

  //Used in Histogram Title (Not Name)
  std::vector<TString> RCReweightTitleOptions(n_RCrw);
  RCReweightTitleOptions[k_woRC] = "Without reactor constraint";
  RCReweightTitleOptions[k_wRC] = "With reactor constraint";

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
  ConfidenceInterval[k_1sig] = 1.0;
  ConfidenceInterval[k_90] = 2.71;
  ConfidenceInterval[k_2sig] = 4.0;
  ConfidenceInterval[k_99] = 6.63;
  ConfidenceInterval[k_3sig] = 9.0;
  ConfidenceInterval[k_4sig] = 16.0;

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

  //Histogram Fill Colour
  std::vector<int> IntervalColor(n_Intervals);
  IntervalColor[k_0] = 0;
  IntervalColor[k_1sig] = 12;
  IntervalColor[k_90] = 13;
  IntervalColor[k_2sig] = 14;
  IntervalColor[k_99] = 15;
  IntervalColor[k_3sig] = 16;
  IntervalColor[k_4sig] = 17;

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
  std::vector< std::vector<int> > LegendPosition(n_Hierarchies);
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    LegendPosition[iHierarchy].resize(n_OscParams);
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      LegendPosition[iHierarchy][iOscParam] = kLegendTopRight;
    }
  }

  if (IntervalType == k_CredibleInterval) {
    LegendPosition[k_BH][k_th23] = kLegendTopLeft;
    LegendPosition[k_NH][k_th23] = kLegendTopLeft;
    LegendPosition[k_IH][k_th23] = kLegendTopLeft;
    
    LegendPosition[k_BH][k_dm32] = kLegendTopMiddle;
    LegendPosition[k_NH][k_dm32] = kLegendTopLeft;
  }

  if (IntervalType == k_ConfidenceInterval) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	LegendPosition[iHierarchy][iOscParam] = kLegendTopMiddle;
      }
    }
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

  //std::vector<TH1D*> vec_HistBinning = SpecifyBinning(RC);

  std::vector< std::vector<TH1D*> > vec_HistPost(n_Hierarchies);
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    vec_HistPost[iHierarchy].resize(n_OscParams);
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {

      //vec_HistPost[iHierarchy][iOscParam] = (TH1D*)vec_HistBinning[iOscParam]->Clone();
      vec_HistPost[iHierarchy][iOscParam] = (TH1D*)SpecifyBinning(iOscParam,iHierarchy,RC)->Clone();

      /*
      if (iOscParam == k_dm32) {
	if (iHierarchy == k_NH) {
	  vec_HistPost[iHierarchy][iOscParam]->GetXaxis()->SetRangeUser(0.002,0.003);
	}
	if (iHierarchy == k_IH) {
	  vec_HistPost[iHierarchy][iOscParam]->GetXaxis()->SetRangeUser(-0.003,-0.002);
	}
      }
      */
    }
  }

  //--------------------------------------------------------------------------------------------------------
  std::cout << "Filling histograms from TTree, using burnin = " << BurnIn << std::endl;

  int nEntries = Tree->GetEntries();
  std::cout << "Number of steps: " << nEntries << std::endl;

  std::vector<float> OscParams(n_OscParams);
  float b_RCw = 1.;
  float b_xs_66 = 1.;
  float b_xs_67 = 1.;
  float b_xs_24 = 1.;
  int b_Step;

  for (int iOscParam = 0; iOscParam < n_OscParams; iOscParam++) {
    Tree->SetBranchAddress(OscParamBranchNames[iOscParam], &OscParams[iOscParam]);
  }

  Tree->SetBranchAddress("step", &b_Step);
  //JJ: for now we don't need them
  /*
  Tree->SetBranchAddress("xsec_66_w", &b_xs_66);
  Tree->SetBranchAddress("xsec_67_w", &b_xs_67);
  Tree->SetBranchAddress("xsec_24_w", &b_xs_24);
  */
  if (RC) {
    std::cout << "Using RC reweight" << std::endl;
    Tree->SetBranchAddress("RCreweight", &b_RCw);
  }
  //JJ: a hard-coded cut on LogL<1000
  float LogL = 0.;
  Tree->SetBranchStatus("LogL",true);
  Tree->SetBranchAddress("LogL",&LogL);
  
  for (int iEntry=0;iEntry<nEntries;iEntry++) {

    Tree->GetEntry(iEntry);

    // Cut out the burn in
    if (b_Step < BurnIn) continue;
    //JJ: a hard-coded cut on LogL<1000
    if (LogL < 1000.) continue;
    //MJ: hard-coded cut on LogL>100000000
    if (LogL > 100000000) continue;

    if (iEntry % 1000000 == 0) {
      std::cout << iEntry << "/" << nEntries << " (" << double(iEntry)/double(nEntries)*100. << "%)" << std::endl;
    }
    
    double totWeight = b_RCw*b_xs_66*b_xs_67*b_xs_24;
    //std::cout << totWeight << std::endl;
    // Be cheeky and calculate a second weight here, prior flat in sindCP
    //double dcpvalue = OscParams[k_dcp]; 
    //std::cout << "***" << std::endl;
    //std::cout << "before: " << totWeight << std::endl;
    //totWeight = totWeight * TMath::Abs((TMath::Cos(dcpvalue)));
    //std::cout << "after: " << totWeight << std::endl;
    
    vec_HistPost[k_BH][k_dcp]->Fill(OscParams[k_dcp],totWeight);
    vec_HistPost[k_BH][k_th13]->Fill(OscParams[k_th13],totWeight);
    vec_HistPost[k_BH][k_th23]->Fill(OscParams[k_th23],totWeight);
    vec_HistPost[k_BH][k_dm32]->Fill(OscParams[k_dm32],totWeight);
    
    if (OscParams[k_dm32] >= 0) {
      vec_HistPost[k_NH][k_dcp]->Fill(OscParams[k_dcp],totWeight);
      vec_HistPost[k_NH][k_th13]->Fill(OscParams[k_th13],totWeight);
      vec_HistPost[k_NH][k_th23]->Fill(OscParams[k_th23],totWeight);
      vec_HistPost[k_NH][k_dm32]->Fill(OscParams[k_dm32],totWeight);
    } else {
      vec_HistPost[k_IH][k_dcp]->Fill(OscParams[k_dcp],totWeight);
      vec_HistPost[k_IH][k_th13]->Fill(OscParams[k_th13],totWeight);
      vec_HistPost[k_IH][k_th23]->Fill(OscParams[k_th23],totWeight);
      vec_HistPost[k_IH][k_dm32]->Fill(OscParams[k_dm32],totWeight);
    }
  }

  /*
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      vec_HistPost[iHierarchy][iOscParam]->Smooth(1);
    }
  }
  */

  //--------------------------------------------------------------------------------------------------------
  if (IntervalType == k_ConfidenceInterval) {
    std::cout << "Calculating confidence posterior" << std::endl;;
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	double MaxBinContent = vec_HistPost[iHierarchy][iOscParam]->GetMaximum();
	for (int xBin=1;xBin<=vec_HistPost[iHierarchy][iOscParam]->GetNbinsX();xBin++) {
	  double BinContent = vec_HistPost[iHierarchy][iOscParam]->GetBinContent(xBin);
	  if (BinContent == 0) BinContent = 1;
	  BinContent = -2.0 * TMath::Log(BinContent/MaxBinContent);
	  vec_HistPost[iHierarchy][iOscParam]->SetBinContent(xBin,BinContent);
	}
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  if (IntervalType == k_CredibleInterval) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	vec_HistPost[iHierarchy][iOscParam]->Scale(1.0/vec_HistPost[iHierarchy][iOscParam]->Integral());
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  std::vector< std::vector<TH1D*> > vec_HistIntv(n_Hierarchies);
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    vec_HistIntv[iHierarchy].resize(n_OscParams);
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      vec_HistIntv[iHierarchy][iOscParam] = (TH1D*)vec_HistPost[iHierarchy][iOscParam]->Clone();
      for (int xBin=1;xBin<=vec_HistIntv[iHierarchy][iOscParam]->GetNbinsX();xBin++) {
	vec_HistIntv[iHierarchy][iOscParam]->SetBinContent(xBin,0.);
      }
    }
  }
   
  //--------------------------------------------------------------------------------------------------------
  std::cout << "Calculating intervals from histograms" << std::endl;

  if (IntervalType == k_CredibleInterval) {
    double Integral, Sum = 0.;
    int MaxBin = 0;
    TH1D* Hist;
    
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	
	Hist = (TH1D*)vec_HistPost[iHierarchy][iOscParam]->Clone(); 
	Integral = Hist->Integral();
	Sum = 0.;
	MaxBin = 0;
	
	while((Sum/Integral) < CredibleInterval[n_Intervals-1]) {
	  
	  Sum += Hist->GetMaximum();
	  MaxBin = Hist->GetMaximumBin();
	  
	  for (int iCredibleInterval=1;iCredibleInterval<n_Intervals;iCredibleInterval++) {
	    if ((Sum/Integral >= CredibleInterval[iCredibleInterval-1]) && (Sum/Integral < CredibleInterval[iCredibleInterval])) {
	      vec_HistIntv[iHierarchy][iOscParam]->SetBinContent(MaxBin, IntervalIndex[iCredibleInterval]);
	      Hist->SetBinContent(MaxBin,0.);
	    }
	  }
	  
	}
	
      }
    }
  } else if (IntervalType == k_ConfidenceInterval) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int xBin=1;xBin<=vec_HistPost[iHierarchy][iOscParam]->GetNbinsX();xBin++) {
	  vec_HistIntv[iHierarchy][iOscParam]->SetBinContent(xBin,IntervalIndex[n_Intervals-1]);
	}
	double MinChi2 = vec_HistPost[iHierarchy][iOscParam]->GetMinimum();
	for (int xBin=1;xBin<=vec_HistPost[iHierarchy][iOscParam]->GetNbinsX();xBin++) {
	  for (int iConfidenceInterval=1;iConfidenceInterval<n_Intervals;iConfidenceInterval++) {
	    if (vec_HistPost[iHierarchy][iOscParam]->GetBinContent(xBin) < (MinChi2+ConfidenceInterval[iConfidenceInterval])) {
	      if (vec_HistIntv[iHierarchy][iOscParam]->GetBinContent(xBin) < IntervalIndex[iConfidenceInterval]) {
		vec_HistIntv[iHierarchy][iOscParam]->SetBinContent(xBin,IntervalIndex[iConfidenceInterval]);	
	      }
	    }
	  }
	}
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  std::cout << "Splitting histograms by interval level" << "\n\n\n" << std::endl;

  std::vector< std::vector< std::vector<TH1D*> > > vec_HistIntv_ByIntv = SplitIntervalByLevel(vec_HistIntv,IntervalIndex);

  for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
    std::cout << "Oscillation Parameter:" << OscParamNames[iOscParam] << std::endl;
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      std::cout << "\tHierarchy:" << HierarchyNames[iHierarchy] << std::endl;
      for (int iInterval=1;iInterval<n_Intervals;iInterval++) {
	std::cout << "\t\tInterval:" << IntervalName[iInterval] << std::endl;;
	CalculateInterval(vec_HistIntv_ByIntv[iInterval][iHierarchy][iOscParam]);
      }
      std::cout << "\n" << std::endl;
    }
    std::cout << "\n\n\n" << std::endl;
  }

  std::vector< std::vector< std::vector<TH1D*> > > vec_HistIntv_ByIntv_Post = MultiplyIntervalByPosterior(vec_HistIntv_ByIntv,vec_HistPost);

  //--------------------------------------------------------------------------------------------------------
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      FormatHistogram(vec_HistPost[iHierarchy][iOscParam],iHierarchy,HierarchyTitleOptions,iOscParam,OscParamAxisTitles,RC,RCReweightTitleOptions,IntervalTypeAxisNames[IntervalType]);

      vec_HistPost[iHierarchy][iOscParam]->SetLineColor(kBlack);
      vec_HistPost[iHierarchy][iOscParam]->SetFillColor(IntervalColor[k_0]);

      if (IntervalType == k_ConfidenceInterval) {
	ScaleHistogram(vec_HistPost[iHierarchy][iOscParam],ConfidenceInterval[n_Intervals-1]);      
      }

    }
  }

  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
        FormatHistogram(vec_HistIntv_ByIntv_Post[iInterval][iHierarchy][iOscParam],iHierarchy,HierarchyTitleOptions,iOscParam,OscParamAxisTitles,RC,RCReweightTitleOptions,IntervalTypeAxisNames[IntervalType]);

	vec_HistIntv_ByIntv_Post[iInterval][iHierarchy][iOscParam]->SetLineColor(kBlack);
	vec_HistIntv_ByIntv_Post[iInterval][iHierarchy][iOscParam]->SetFillColor(IntervalColor[iInterval]);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  if (Draw) {
    std::cout << "Drawing posteriors" << std::endl;

    TCanvas* Canvas = new TCanvas();
    TString PDFName;

    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {

	for (int iDrawGroup=0;iDrawGroup<(int)DrawGroups.size();iDrawGroup++) {
	  PDFName = "Contours_1D_"+OscParamNames[iOscParam]+"_"+HierarchyNames[iHierarchy]+"_"+Form("%i", iDrawGroup);
	  if (AdditionalNameInfo!="") {
	    PDFName += "_"+AdditionalNameInfo;
	  }
	  PDFName += ".pdf";

	  vec_HistPost[iHierarchy][iOscParam]->Draw();

	  TLegend* Legend;
	  if (LegendPosition[iHierarchy][iOscParam] == kLegendTopRight) {
	    Legend = new TLegend(0.5,0.7,0.89,0.89);
	  } else if (LegendPosition[iHierarchy][iOscParam] == kLegendTopMiddle) {
	    Legend = new TLegend(0.3,0.7,0.69,0.89);
	  } else if (LegendPosition[iHierarchy][iOscParam] == kLegendTopLeft) {
	    Legend = new TLegend(0.12,0.7,0.45,0.89);
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
	    vec_HistIntv_ByIntv_Post[DrawGroups[iDrawGroup][iDrawGroupInt]][iHierarchy][iOscParam]->Draw("SAME");
	    Legend->AddEntry(vec_HistIntv_ByIntv_Post[DrawGroups[iDrawGroup][iDrawGroupInt]][iHierarchy][iOscParam],IntervalCaption[DrawGroups[iDrawGroup][iDrawGroupInt]],"f");
	  }
	  
	  Legend->Draw("SAME");
	  Canvas->Print(PDFName);
	}
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  TString HistName;

  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      HistName = "h_"+OscParamNames[iOscParam]+"_"+HierarchyNames[iHierarchy];
      vec_HistPost[iHierarchy][iOscParam]->SetName(HistName);
      
      for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
	vec_HistIntv_ByIntv_Post[iInterval][iHierarchy][iOscParam]->SetName(HistName);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------------
  TString OutFileName = "Contours_1D";
  if (RC) OutFileName += "_wRC";
  else OutFileName += "_woRC";
  OutFileName += Form("_BurnIn_%i", BurnIn);
  if (AdditionalNameInfo!="") {
    OutFileName += "_"+AdditionalNameInfo;
  }
  OutFileName += "_float";
  OutFileName += ".root";

  TFile* OutputFile = new TFile(OutFileName, "RECREATE");

  std::cout << "Writing output to file:" << OutFileName << std::endl;

  OutputFile->mkdir("Posteriors");

  OutputFile->cd("Posteriors");
  for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
    for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
      vec_HistPost[iHierarchy][iOscParam]->Write();
    }
  }

  OutputFile->mkdir(IntervalTypeNames[IntervalType]+"Intervals");

  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    TString DirName = IntervalTypeNames[IntervalType]+"Intervals/"+IntervalName[iInterval];
    OutputFile->mkdir(DirName);
    OutputFile->cd(DirName);
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	vec_HistIntv_ByIntv_Post[iInterval][iHierarchy][iOscParam]->Write();
      }
    }
  }

  OutputFile->Close();

  std::cout << "Finished" << std::endl;
}

TH1D* SpecifyBinning(int Param, int Hierarchy, bool wRC) {

  std::vector<double> Binning = GetBinning(1, Hierarchy, wRC, Param, -1);
  TString HistName;
  if (Param == k_dcp) HistName = Form("h_dcp_%i",Hierarchy);
  if (Param == k_th13) HistName = Form("h_th13_%i",Hierarchy);
  if (Param == k_th23) HistName = Form("h_th23_%i",Hierarchy);
  if (Param == k_dm32) HistName = Form("h_dm32_%i",Hierarchy);

  TH1D* Hist = new TH1D(HistName,"",(int)Binning[0],Binning[1],Binning[2]);

  return Hist;
}

std::vector< std::vector< std::vector<TH1D*> > > MultiplyIntervalByPosterior(std::vector< std::vector< std::vector<TH1D*> > > vec_HistIntv, std::vector< std::vector<TH1D*> > vec_HistPost) {
  std::vector< std::vector< std::vector<TH1D*> > > returnVec(n_Intervals);
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    returnVec[iInterval].resize(n_Hierarchies);
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      returnVec[iInterval][iHierarchy].resize(n_OscParams);
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
        returnVec[iInterval][iHierarchy][iOscParam] = (TH1D*)vec_HistIntv[iInterval][iHierarchy][iOscParam]->Clone();

	for (int xBin=1;xBin<=returnVec[iInterval][iHierarchy][iOscParam]->GetNbinsX();xBin++) {
	  //returnVec[iInterval][iHierarchy][iOscParam]->SetBinContent(xBin,vec_HistPost[iHierarchy][iOscParam]->GetBinContent(xBin)*vec_HistIntv[iInterval][iHierarchy][iOscParam]->GetBinContent(xBin));
	  if (vec_HistIntv[iInterval][iHierarchy][iOscParam]->GetBinContent(xBin) != 0.) {
	    returnVec[iInterval][iHierarchy][iOscParam]->SetBinContent(xBin,vec_HistPost[iHierarchy][iOscParam]->GetBinContent(xBin));
	  }
	}
      }
    }
  }

  return returnVec;
}

std::vector< std::vector< std::vector<TH1D*> > > SplitIntervalByLevel(std::vector< std::vector<TH1D*> > vec_HistIntv, std::vector<int> IntervalIndex) {
  std::vector< std::vector< std::vector<TH1D*> > > returnVec(n_Intervals);
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    returnVec[iInterval].resize(n_Hierarchies);
    for (int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      returnVec[iInterval][iHierarchy].resize(n_OscParams);
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	returnVec[iInterval][iHierarchy][iOscParam] = (TH1D*)vec_HistIntv[iHierarchy][iOscParam]->Clone();
      }
    }
  }
  
  for (int iInterval=1;iInterval<n_Intervals;iInterval++) {
    for(int iHierarchy=0;iHierarchy<n_Hierarchies;iHierarchy++) {
      for (int iOscParam=0;iOscParam<n_OscParams;iOscParam++) {
	for (int xBin=1;xBin<=returnVec[iInterval][iHierarchy][iOscParam]->GetNbinsX();xBin++) {
	  if (returnVec[iInterval][iHierarchy][iOscParam]->GetBinContent(xBin) < IntervalIndex[iInterval]) {
	    returnVec[iInterval][iHierarchy][iOscParam]->SetBinContent(xBin,0);
	  } else {
	    returnVec[iInterval][iHierarchy][iOscParam]->SetBinContent(xBin,1);
	  }
	}
      }
    }
  }

  return returnVec;
}

void FormatHistogram(TH1D* Hist, int Hierarchy, std::vector<TString> HierarchyTitleOptions, int OscParam, std::vector<TString> OscParamAxisTitles, bool RC, std::vector<TString> RCTitleOptions, TString IntervalAxisLabel, TString IntervalName) {
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

  TString YAxisTitle = IntervalAxisLabel;
  Hist->GetYaxis()->SetTitle(YAxisTitle);
  Hist->GetYaxis()->SetTitleOffset(1.2);

  TString XAxisTitle = OscParamAxisTitles[OscParam];
  Hist->GetXaxis()->SetTitle(XAxisTitle);
}

void CalculateInterval(TH1D* Hist) {
  std::vector< std::vector<double> > Intervals;

  bool InInterval = false;
  double LeftEdge = -1e8;
  double RightEdge = -1e8;

  for (int xBin=1;xBin<=Hist->GetNbinsX();xBin++) {
    if (Hist->GetBinContent(xBin) == 1.0) {
      if (!InInterval) {
	LeftEdge = Hist->GetXaxis()->GetBinLowEdge(xBin);
      }
      InInterval = true;
    } else {
      if (InInterval) {
	RightEdge = Hist->GetXaxis()->GetBinLowEdge(xBin);

	std::vector<double> Vec(2);
	Vec[0] = LeftEdge;
	Vec[1] = RightEdge;
	Intervals.push_back(Vec);
	
	LeftEdge = -1e8;
	RightEdge = -1e8;
      }
      InInterval = false;	
    }

  }

  if (InInterval) {
    RightEdge = Hist->GetXaxis()->GetBinUpEdge(Hist->GetNbinsX());

    std::vector<double> Vec(2);
    Vec[0] = LeftEdge;
    Vec[1] = RightEdge;
    Intervals.push_back(Vec);
  }

  for (int iInterval=0;iInterval<(int)Intervals.size();iInterval++) {
    std::cout << "\t\t\tInterval " << iInterval << " | " << std::setw(10) << Intervals[iInterval][0] << " | " << std::setw(10) << Intervals[iInterval][1] << " | " << std::setw(10) << Intervals[iInterval][1]-Intervals[iInterval][0] << std::endl;
  }
}

void ScaleHistogram(TH1D* Hist, double YAxisVal) {
  double Lower = Hist->GetXaxis()->GetBinLowEdge(1);
  for (int xBin=1;xBin<=Hist->GetNbinsX();xBin++) {
    if (Hist->GetBinContent(xBin) > YAxisVal) {
      Lower = Hist->GetXaxis()->GetBinLowEdge(xBin);
    } else {
      break;
    }
  }

  double Upper = Hist->GetXaxis()->GetBinLowEdge(Hist->GetNbinsX());
  for (int xBin=Hist->GetNbinsX();xBin<=Hist->GetNbinsX();xBin--) {
    if (Hist->GetBinContent(xBin) > YAxisVal) {
      Upper = Hist->GetXaxis()->GetBinLowEdge(xBin);
    } else {
      break;
    }
  }

  Hist->GetXaxis()->SetRangeUser(Lower,Upper);
  Hist->GetYaxis()->SetRangeUser(0,YAxisVal);
}
