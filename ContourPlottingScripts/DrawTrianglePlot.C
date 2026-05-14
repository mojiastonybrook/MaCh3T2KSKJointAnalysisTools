#include <vector>
#include <iostream>

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TString.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TColor.h"
#include "TGaxis.h"

enum OscParams{k_dcp, k_th13, k_th23, k_dm32, n_OscParams};
enum Hierarchy{k_BH, k_NH, k_IH, n_Hierarchies};
enum Intervals{k_0, k_1sig, k_90, k_2sig, k_99, k_3sig, k_4sig, n_Intervals};

//double dm32_LLimit = 0.0023;
//double dm32_HLimit = 0.0027;
double dm32_LLimit = 0.0015;
double dm32_HLimit = 0.004;
double sth23_LLimit = 0.39;
double sth23_HLimit = 0.61;

double sth13_woRC_LLimit = 0.010;
double sth13_woRC_HLimit = 0.040;
double sth13_wRC_LLimit = 0.018;
double sth13_wRC_HLimit = 0.025;

///   ______________
///  |     7       |
///  |   sth13     |
///  |___________________________
///  |     3       |     8      |
///  | sth13-sth23 |   sth23    |
///  |________________________________________
///  |     2       |     5      |     9      |
///  | sth13-dm32  | sth23-dm32 |   dm32     |
///  |____________________________________________________
///  |     1       |     4      |     6      |    10     |
///  | sth13-dcp   | sth23-dcp  |  dm32-dcp  |    dcp    |
///  |___________________________________________________|  

std::vector< std::vector<TH1D*> > Return1DContours(TString FileName, std::vector<TString> OscParamNames,  std::vector<TString> IntervalName, TString HierarchyName, std::vector<int> Contour1D_XIndex, std::vector<int> IntervalsToPlot, bool wRC);
std::vector< std::vector<TH2D*> > Return2DContours(TString FileName, std::vector<TString> OscParamNames,  std::vector<TString> IntervalName, TString HierarchyName, std::vector<int> Contour2D_XIndex, std::vector<int> Contour2D_YIndex, std::vector<int> IntervalsToPlot, bool wRC);
void makePrettyText(TPaveText* text_);

void DrawTrianglePlot(TString Contour1DFileName, TString Contour2DFileName, bool wRC) {

  TObjArray* Arr = Contour1DFileName.Tokenize("/");
  TString OutputName = ((((TObjString*)(Arr->At(Arr->GetEntries()-1)))->String()).ReplaceAll(TString(".root"),TString(""))+"_TrianglePlot.pdf");

  std::vector<int> IntervalsToPlot(3);
  IntervalsToPlot[0] = k_0;
  IntervalsToPlot[1] = k_1sig;
  IntervalsToPlot[2] = k_2sig;
  //IntervalsToPlot[3] = k_3sig;

  int HierarchyToPlot = k_BH;

  //==========================================================================================
  //Contour File Specifics...

  //Used in Histogram Name (Not Title)
  std::vector<TString> OscParamNames(n_OscParams);
  OscParamNames[k_dcp] = "dcp";
  OscParamNames[k_th13] = "th13";
  OscParamNames[k_th23] = "th23";
  OscParamNames[k_dm32] = "dm32";

  //Used in Histogram Title (Not Name) and Directory Name
  std::vector<TString> IntervalName(n_Intervals);
  IntervalName[k_0] = "All";
  IntervalName[k_1sig] = "1sig";
  IntervalName[k_90] = "90";
  IntervalName[k_2sig] = "2Sig";
  IntervalName[k_99] = "99";
  IntervalName[k_3sig] = "3Sig";
  IntervalName[k_4sig] = "4Sig";

  //Used in Histogram Name (Not Title)
  std::vector<TString> HierarchyNames(n_Hierarchies);
  HierarchyNames[k_BH] = "BH";
  HierarchyNames[k_NH] = "NH";
  HierarchyNames[k_IH] = "IH";

  //==========================================================================================
  //Triangle Plot Specifics...

  int n1DHistograms = n_OscParams;
  std::vector<int> Contour1D_XIndex(n1DHistograms);
  Contour1D_XIndex[0] = k_th13;
  Contour1D_XIndex[1] = k_th23;
  Contour1D_XIndex[2] = k_dm32;
  Contour1D_XIndex[3] = k_dcp;

  int n2DHistograms = 6; // (n_OscParams-1)! {Factorial}
  std::vector<int> Contour2D_XIndex(n2DHistograms);
  Contour2D_XIndex[0] = k_th13;
  Contour2D_XIndex[1] = k_th13;
  Contour2D_XIndex[2] = k_th13;
  Contour2D_XIndex[3] = k_th23;
  Contour2D_XIndex[4] = k_th23;
  Contour2D_XIndex[5] = k_dm32;

  std::vector<int> Contour2D_YIndex(n2DHistograms);
  Contour2D_YIndex[0] = k_dcp;
  Contour2D_YIndex[1] = k_dm32;
  Contour2D_YIndex[2] = k_th23;
  Contour2D_YIndex[3] = k_dcp;
  Contour2D_YIndex[4] = k_dm32;
  Contour2D_YIndex[5] = k_dcp;

  //==========================================================================================
  //Grab the histograms...

  std::vector< std::vector<TH1D*> > Contours1D = Return1DContours(Contour1DFileName,OscParamNames,IntervalName,HierarchyNames[HierarchyToPlot],Contour1D_XIndex,IntervalsToPlot,wRC);
  std::vector< std::vector<TH2D*> > Contours2D = Return2DContours(Contour2DFileName,OscParamNames,IntervalName,HierarchyNames[HierarchyToPlot],Contour2D_XIndex,Contour2D_YIndex,IntervalsToPlot,wRC);

  //==========================================================================================
  //Contour graphics
  /// Set better deep-sea-style palette
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  Int_t DeepSeaMod[NCont];

  //Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 }; // for normal palette 51
  Double_t stops[NRGBs] = { 0.00, 0.02, 0.20, 0.70, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.09, 0.18, 0.09, 0.00 };
  Double_t green[NRGBs] = { 0.01, 0.02, 0.39, 0.68, 0.97 };
  Double_t blue[NRGBs]  = { 0.17, 0.39, 0.62, 0.79, 0.97 };
  Int_t FI = TColor::CreateGradientColorTable(NRGBs,stops,red,green,blue,NCont);
  for (int i=0;i<NCont;i++) DeepSeaMod[i] = FI+i;
 
  /// Some cosmetics
  gStyle->SetPalette(NCont,DeepSeaMod);
  gStyle->SetOptStat(0);
  gStyle->SetTextFont(132);

  TGaxis::SetMaxDigits(3);

  //==========================================================================================
  //Generate Canvas and set Margins

  TCanvas* Canv = new TCanvas("Canv","",2000,2000);

  int n2DPads = Contours2D.size();
  std::vector<TPad*> vecPad2D(n2DPads);
  vecPad2D[0] = new TPad("Pad1","",0.1,0.1,0.3,0.3); //sth13-dcp Pad
  vecPad2D[1] = new TPad("Pad2","",0.1,0.3,0.3,0.5); //sth13-dm32 Pad
  vecPad2D[2] = new TPad("Pad3","",0.1,0.5,0.3,0.7); //sth13-sth23 Pad
  vecPad2D[3] = new TPad("Pad4","",0.3,0.1,0.5,0.3); //sth23-dcp Pad
  vecPad2D[4] = new TPad("Pad5","",0.3,0.3,0.5,0.5); //sth13-dm32 Pad
  vecPad2D[5] = new TPad("Pad6","",0.5,0.1,0.7,0.3); //dm32-dcp Pad

  for (int i2DPad=0;i2DPad<n2DPads;i2DPad++) {
    vecPad2D[i2DPad]->SetBottomMargin(0);
    vecPad2D[i2DPad]->SetTopMargin(0);
    vecPad2D[i2DPad]->SetLeftMargin(0);
    vecPad2D[i2DPad]->SetRightMargin(0);
  }

  int n1DPads = Contours1D.size();
  std::vector<TPad*> vecPad1D(n1DPads);
  vecPad1D[0] = new TPad("Pad6","",0.1,0.7,0.3,0.9); //sth13 Pad
  vecPad1D[1] = new TPad("Pad7","",0.3,0.5,0.5,0.7); //sth23 Pad
  vecPad1D[2] = new TPad("Pad8","",0.5,0.3,0.7,0.5); //dm32 Pad
  vecPad1D[3] = new TPad("Pad9","",0.7,0.1,0.9,0.3); //dcp Pad

  for (int i1DPad=0;i1DPad<n1DPads;i1DPad++) {
    vecPad1D[i1DPad]->SetBottomMargin(0);
    vecPad1D[i1DPad]->SetTopMargin(0);
    vecPad1D[i1DPad]->SetLeftMargin(0);
    vecPad1D[i1DPad]->SetRightMargin(0);
  }

  vecPad2D[0]->SetLeftMargin(0.14);
  vecPad2D[1]->SetLeftMargin(0.14);
  vecPad2D[2]->SetLeftMargin(0.14);
  vecPad1D[0]->SetLeftMargin(0.14);

  vecPad2D[0]->SetBottomMargin(0.14);
  vecPad2D[3]->SetBottomMargin(0.14);
  vecPad2D[5]->SetBottomMargin(0.14);
  vecPad1D[3]->SetBottomMargin(0.14);

  //==========================================================================================
  //Do Some Drawing

  for (int i2DPad=0;i2DPad<n2DPads;i2DPad++) {
    Canv->cd();
    vecPad2D[i2DPad]->Draw();
    vecPad2D[i2DPad]->cd();

    for (int iContour=0;iContour<(int)Contours2D[i2DPad].size();iContour++) {
      if (iContour == 0) {
	Contours2D[i2DPad][iContour]->Draw("COL");
      } else {
	Contours2D[i2DPad][iContour]->Draw("CONT3 SAME");
      }
    }
  }

  for (int i1DPad=0;i1DPad<n1DPads;i1DPad++) {
    Canv->cd();
    vecPad1D[i1DPad]->Draw();
    vecPad1D[i1DPad]->cd();
    
    for(int iContour=0;iContour<(int)Contours1D[i1DPad].size();iContour++) {
      int ContourToDraw = (int)Contours1D[i1DPad].size()-iContour-1;
      if (iContour == 0) {
	Contours1D[i1DPad][ContourToDraw]->Draw();
      } else {
	Contours1D[i1DPad][ContourToDraw]->Draw("SAME");
      }
    }
  }

  //==========================================================================================
  //Add Text

  Canv->cd();

  TPaveText *sth13_text_h = new TPaveText(0.15, 0.03, 0.25, 0.07);
  makePrettyText(sth13_text_h) ;
  //sth13_text_h -> AddText("sin^{2} #theta_{13} [#times 10^{-3}]") ;
  sth13_text_h -> AddText("sin^{2} #theta_{13}") ;
  sth13_text_h -> Draw();

  TPaveText *sth23_text_h = new TPaveText(0.35, 0.03, 0.45, 0.07);
  makePrettyText(sth23_text_h) ;
  sth23_text_h -> AddText("sin^{2} #theta_{23}") ;
  sth23_text_h -> Draw();

  TPaveText *dm32_text_h = new TPaveText(0.55, 0.03, 0.65, 0.07);
  makePrettyText(dm32_text_h) ;
  dm32_text_h -> AddText("#Delta m_{32}^{2} [#times 10^{-3} eV^{2}]") ;
  dm32_text_h -> Draw();

  TPaveText *dcp_text_h = new TPaveText(0.75, 0.03, 0.85, 0.07);
  makePrettyText(dcp_text_h) ;
  dcp_text_h -> AddText("#delta_{CP}") ;
  dcp_text_h -> Draw();

  TPaveText *dcp_text_v = new TPaveText(0.02, 0.15, 0.05, 0.25);
  makePrettyText(dcp_text_v) ;
  TText *dcp_text = dcp_text_v -> AddText("#delta_{CP}") ;
  dcp_text -> SetTextAngle(90);
  dcp_text_v -> Draw();

  TPaveText *dm32_text_v = new TPaveText(0.02, 0.35, 0.05, 0.45);
  makePrettyText(dm32_text_v) ;
  TText *dm32_text = dm32_text_v -> AddText("#Delta m_{32}^{2} [#times 10^{-3} eV^{2}]") ;
  dm32_text -> SetTextAngle(90);
  dm32_text_v -> Draw();

  TPaveText *sth23_text_v = new TPaveText(0.02, 0.55, 0.05, 0.65);
  makePrettyText(sth23_text_v) ;
  TText *sth23_text = sth23_text_v -> AddText("sin^{2} #theta_{23}") ;
  sth23_text -> SetTextAngle(90);
  sth23_text_v -> Draw();

  TPaveText *proba_text_v = new TPaveText(0.02, 0.75, 0.05, 0.85);
  makePrettyText(proba_text_v) ;
  TText *proba_text = proba_text_v -> AddText("Post. prob.") ;
  proba_text -> SetTextAngle(90);
  proba_text_v -> Draw();

  Canv->Print(OutputName);
}

std::vector< std::vector<TH1D*> > Return1DContours(TString FileName, std::vector<TString> OscParamNames,  std::vector<TString> IntervalName, TString HierarchyName, std::vector<int> Contour1D_XIndex, std::vector<int> IntervalsToPlot, bool wRC) {

  TFile* File = new TFile(FileName);
  if (!File || File->IsZombie()) {
    std::cout << "Did not find FileName:" << FileName << std::endl;
    exit(-1);
  }

  int nParamsToFind = Contour1D_XIndex.size();
  int nContoursPerParam = IntervalsToPlot.size();

  std::vector< std::vector<TH1D*> > ReturnVec(nParamsToFind);
  for (int iParam=0;iParam<nParamsToFind;iParam++) {
    ReturnVec[iParam].resize(nContoursPerParam);
  }

  for (int iParam=0;iParam<nParamsToFind;iParam++) {
    for (int iContour=0;iContour<nContoursPerParam;iContour++) {

      TString HistName = "CredibleIntervals/"+IntervalName[IntervalsToPlot[iContour]]+"/h_"+OscParamNames[Contour1D_XIndex[iParam]]+"_"+HierarchyName;
      if (IntervalsToPlot[iContour] == k_0) {
	HistName = "Posteriors/h_"+OscParamNames[Contour1D_XIndex[iParam]]+"_"+HierarchyName;
      }

      TH1D* Hist = (TH1D*)File->Get(HistName);
      if (!Hist) {
	std::cout << "Did not find histogram named " << HistName << " in file named " << FileName << std::endl;
	exit(-1);
      }

      ReturnVec[iParam][iContour] = (TH1D*)Hist->Clone();
      ReturnVec[iParam][iContour]->SetTitle("");
      ReturnVec[iParam][iContour]->GetXaxis()->SetTitle("");
      ReturnVec[iParam][iContour]->GetYaxis()->SetTitle("");

      ReturnVec[iParam][iContour]->GetYaxis()->SetLabelOffset(999);

      ReturnVec[iParam][iContour]->GetXaxis()->SetLabelSize(0.1);
      ReturnVec[iParam][iContour]->GetYaxis()->SetLabelSize(0.1);

      ReturnVec[iParam][iContour]->GetXaxis()->SetNdivisions(4);
      ReturnVec[iParam][iContour]->GetYaxis()->SetNdivisions(4);


      if (Contour1D_XIndex[iParam] == k_dm32) {
	ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(dm32_LLimit,dm32_HLimit);
      }
      /*
      if (Contour1D_XIndex[iParam] == k_th13) {
	if (wRC) {
	  ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(sth13_wRC_LLimit,sth13_wRC_HLimit);
	} else {
	  ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(sth13_woRC_LLimit,sth13_woRC_HLimit);
	}
      }
      if (Contour1D_XIndex[iParam] == k_th23) {
	ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(sth23_LLimit,sth23_HLimit);
      }
      */
    }
  }

  return ReturnVec;
}

std::vector< std::vector<TH2D*> > Return2DContours(TString FileName, std::vector<TString> OscParamNames,  std::vector<TString> IntervalName, TString HierarchyName, std::vector<int> Contour2D_XIndex, std::vector<int> Contour2D_YIndex, std::vector<int> IntervalsToPlot, bool wRC) {

  TFile* File = new TFile(FileName);
  if (!File || File->IsZombie()) {
    std::cout << "Did not find FileName:" << FileName << std::endl;
    exit(-1);
  }

  if (Contour2D_XIndex.size() != Contour2D_YIndex.size()) {
    std::cout << "Mismatch in the number of X and Y indices" << std::endl;
    exit(-1);
  }

  int nParamsToFind = Contour2D_XIndex.size();
  int nContoursPerParam = IntervalsToPlot.size();

  std::vector< std::vector<TH2D*> > ReturnVec(nParamsToFind);
  for (int iParam=0;iParam<nParamsToFind;iParam++) {
    ReturnVec[iParam].resize(nContoursPerParam);
  }

  for (int iParam=0;iParam<nParamsToFind;iParam++) {
    for (int iContour=0;iContour<nContoursPerParam;iContour++) {
      TString HistName = "CredibleIntervals/"+IntervalName[IntervalsToPlot[iContour]]+"/h_"+OscParamNames[Contour2D_XIndex[iParam]]+"_"+OscParamNames[Contour2D_YIndex[iParam]]+"_"+HierarchyName;

      TH2D* Hist = (TH2D*)File->Get(HistName);
      if (!Hist) {
	std::cout << "Did not find histogram named " << HistName << " in file named " << FileName << std::endl;
	exit(-1);
      }

      ReturnVec[iParam][iContour] = (TH2D*)Hist->Clone();

      ReturnVec[iParam][iContour]->SetTitle("");
      ReturnVec[iParam][iContour]->GetXaxis()->SetTitle("");
      ReturnVec[iParam][iContour]->GetYaxis()->SetTitle("");
      ReturnVec[iParam][iContour]->SetLineColor(kWhite);

      ReturnVec[iParam][iContour]->GetXaxis()->SetLabelSize(0.1);
      ReturnVec[iParam][iContour]->GetYaxis()->SetLabelSize(0.1);

      ReturnVec[iParam][iContour]->GetXaxis()->SetNdivisions(4);
      ReturnVec[iParam][iContour]->GetYaxis()->SetNdivisions(4);

      if (Contour2D_XIndex[iParam] == k_dm32) {
	ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(dm32_LLimit,dm32_HLimit);
      }
      if (Contour2D_YIndex[iParam] == k_dm32) {
	ReturnVec[iParam][iContour]->GetYaxis()->SetRangeUser(dm32_LLimit,dm32_HLimit);
      }
      /*
      if (Contour2D_XIndex[iParam] == k_th13) {
	if (wRC) {
	  ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(sth13_wRC_LLimit,sth13_wRC_HLimit);
	} else {
	  ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(sth13_woRC_LLimit,sth13_woRC_HLimit);
	}
      }
      if (Contour2D_YIndex[iParam] == k_th13) {
	if (wRC) {
	  ReturnVec[iParam][iContour]->GetYaxis()->SetRangeUser(sth13_wRC_LLimit,sth13_wRC_HLimit);
	} else {
	  ReturnVec[iParam][iContour]->GetYaxis()->SetRangeUser(sth13_woRC_LLimit,sth13_woRC_HLimit);
	}
      }
      if (Contour2D_XIndex[iParam] == k_th23) {
	ReturnVec[iParam][iContour]->GetXaxis()->SetRangeUser(sth23_LLimit,sth23_HLimit);
      }
      if (Contour2D_YIndex[iParam] == k_th23) {
	ReturnVec[iParam][iContour]->GetYaxis()->SetRangeUser(sth23_LLimit,sth23_HLimit);
      }
      */
    }
  }

  return ReturnVec;
}

void makePrettyText(TPaveText* Text){
  Text->SetTextFont(132) ;
  Text->SetTextSize(0.025) ;
  Text->SetBorderSize(0) ;
  Text->SetFillColor(0) ;
}
