/*
DB: 19/01/2021
----------------------
PrePostFitComp.cpp

- Generates the Pre-/Post- fit parameter pull plots for Xsec and AtmFlux
- Requires output from PerformPostFitAnalysis
- Can generate these in Arithmetic, Gaussian or HPD analysis

Usage:
./PrePostFitComp posteriorfile X
or
./PrePostFitComp_incSingleExpFit posterior_1 posterior_2 posterior_3 X

Where posteriorfile is output of PerformPostFitAnalysis
and X = {0 [Arithmetic],1 [Gaussian fit],2 [HPD]}

Output:
Saves parameter pull plots to plots/AtmComp.pdf and plots/XsecComp.pdf

----------------------
*/

#include <TFile.h>
#include <TMatrixDSym.h>
#include <TMatrixD.h>
#include <TVectorT.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TLine.h>
#include <vector>
#include <iostream>
#include <TTree.h>

double AxisScaleUp = 1.1;
double AxisScaleLow = 0.9;
double BottomMargin = 0.25;

// if (ObjType==1) PreFit  - AtmSKDet
// if (ObjType==2) PreFit  - SKCalib
// if (ObjType==3) PreFit  - SKDetBeam
// if (ObjType==4) PostFit - AtmSKDet Arithmetic
// if (ObjType==5) PostFit - AtmSKDet Gaussian
// if (ObjType==6) PostFit - AtmSKDet HPD
// if (ObjType==7) PostFit - SKCalib Arithmetic
// if (ObjType==8) PostFit - SKCalib Gaussian
// if (ObjType==9) PostFit - SKCalib HPD
// if (ObjType==10) PostFit - SKDetBeam Arithmetic
// if (ObjType==11) PostFit - SKDetBeam Gaussian
// if (ObjType==12) PostFit - SKDetBeam HPD

// split the AtmSKDet systematics into sub-groups
// AtmSKDet: 0,1,2,3,4,5,6,7 --> 8 groups
int NAtmSKDetGroup = 8;
int NSKCalibGroup = 1;
int NSKDetBeamGroup = 1;
std::vector<int> AtmSKDetGroup ={0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7};
std::vector<int> SKCalibGroup ={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
std::vector<int> SKDetBeamGroup = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

class FitObj
{
public:
  FitObj(int ObjType_, int SplitGroup_, TString FileName);
  void AtmSKDetPreFit(TString FileName, int SplitGroup_);
  void SKCalibPreFit(TString FileName, int SplitGroup_);
  void SKDetBeamPreFit(TString FileName, int SplitGroup_);

  void PostFit(TString FileName, int FitParam, int FitType, int SplitGroup_);

  std::vector<double> CentralVals;
  std::vector<double> ErrorValsUpper;
  std::vector<double> ErrorValsLower;
  std::vector<std::string> ParamNames;

  int nParams;
  int ObjType;
  int SplitGroup;

  double maxY;
  double minY;
};

TString returnFancyString(std::string inStr);
void formatHistAsPreFit(TGraphAsymmErrors *Hist);
void formatHistAsPostFit(TGraphAsymmErrors *Hist);
std::vector<TGraphAsymmErrors *> FitResultsToHist(std::vector<FitObj *> FitObjs, TString HistTitle);
TCanvas *formatCanvas(std::vector<TGraphAsymmErrors *>, std::vector<FitObj *> FitObjs, TString CanvTitle);
TCanvas *formatCanvas_1Pad(std::vector<TGraphAsymmErrors *>, std::vector<FitObj *> FitObjs, TString CanvTitle, int GroupID);
//topology 
TString returnTopology(int topoIndex);
TString returnTitle(int GroupID);
//Prior errors
double FindUpperLimit(int GroupID, TH2D* Hist, double Prior);
double FindLowerLimit(int GroupID, TH2D* Hist, double Prior);

int main(int argc, char *argv[])
{

  if (argc != 4)
  {
    std::cout << "Usage: PrePostFitComp_detSysts posterior_jf X output_dir" << std::endl;
    std::cout << "\n"
              << std::endl;
    std::cout << "Where posteriorfile is output of PerformPostFitAnalysis" << std::endl;
    std::cout << "and X = {0 [Arithmetic],1 [Gaussian fit],2 [HPD]}" << std::endl;
    throw;
  }
  TString PosteriorFileName;
  
  int FitType;
  bool includeSingleFits;
  TString saveDir;
  
  includeSingleFits = false;
  PosteriorFileName = argv[1];
  FitType = atoi(argv[2]);
  saveDir = argv[3];
  
  //std::string AtmSKDetConfigName;
  //std::string SKCalibConfigName;
  //std::string SKDetBeamConfigName;

  //TString AtmSKDetCovName = "inputs/skatm/covariance_AtmSKDetSyst.root";
  //TString SKCalibConfigName = "configs/AtmosphericConfigs/SKCalibrationSyst.cfg";
  //TString SKDetBeamConfigName = "configs/AtmosphericConfigs/SKDetBeamSyst.cfg";
  std::string MaCh3Install = "/home/mojia/MaCh3_2023_detShiftSmear/MaCh3";
  TString AtmSKDetCovName = MaCh3Install+"/inputs/skatm/covariance_AtmSKDetSyst.root";
  TString SKCalibCovName = MaCh3Install+"/inputs/skatm/covariance_SKCalibrationSyst.root";
  TString SKDetBeamCovName = MaCh3Install+"/inputs/skatm/covariance_SKDetBeamSyst.root"; 

  if (!includeSingleFits){
    std::cout << "Posterior file:" << PosteriorFileName << std::endl;
  } 
  std::cout << "AtmSKDet Cov file:" << AtmSKDetCovName << std::endl;
  std::cout << "SKCalib Cov file:" << SKCalibCovName << std::endl;
  std::cout << "SKDetBeam Cov file:" << SKDetBeamCovName << std::endl;

  if (FitType == 0)
  {
    std::cout << "Using Arithmetic posterior results" << std::endl;
  }
  else if (FitType == 1)
  {
    std::cout << "Using Gaussian fit posterior results" << std::endl;
  }
  else if (FitType == 2)
  {
    std::cout << "Using HPD posterior results" << std::endl;
  }
  else
  {
    std::cout << "Invalid FitType. Quitting.." << std::endl;
    throw;
  }

  std::vector<FitObj *> AtmSKDetPreFit;
  AtmSKDetPreFit.resize(NAtmSKDetGroup);
  for (int igroup = 0; igroup < NAtmSKDetGroup; igroup++)
  {
    AtmSKDetPreFit[igroup] = new FitObj(1, igroup, AtmSKDetCovName);
    std::cout << "Found " << AtmSKDetPreFit[igroup]->nParams << " params in AtmSKDetPreFit Group" << igroup << std::endl;
  }

  std::vector<FitObj *> SKCalibPreFit;
  SKCalibPreFit.resize(NSKCalibGroup);
  for (int igroup = 0; igroup < NSKCalibGroup; igroup++)
  {
    SKCalibPreFit[igroup] = new FitObj(2, igroup, SKCalibCovName);
    std::cout << "Found " << SKCalibPreFit[igroup]->nParams << " params in SKCalibPreFit Group" << igroup << std::endl;
  }

  std::vector<FitObj *> SKDetBeamPreFit;
  SKDetBeamPreFit.resize(NSKDetBeamGroup);
  for (int igroup = 0; igroup < NSKDetBeamGroup; igroup++)
  {
    SKDetBeamPreFit[igroup] = new FitObj(3, igroup, SKDetBeamCovName);
    std::cout << "Found " << SKDetBeamPreFit[igroup]->nParams << " params in SKDetBeamPreFit Group" << igroup << std::endl;
  }

  std::vector<FitObj *> AtmSKDetPostFit;
  std::vector<FitObj *> SKCalibPostFit;
  std::vector<FitObj *> SKDetBeamPostFit;
  if (!includeSingleFits){
    AtmSKDetPostFit.resize(NAtmSKDetGroup);
    for (int igroup = 0; igroup < NAtmSKDetGroup; igroup++)
    {
      AtmSKDetPostFit[igroup] = new FitObj(4 + FitType, igroup, PosteriorFileName);
      std::cout << "Found " << AtmSKDetPostFit[igroup]->nParams << " params in AtmSKDetPostFit Group" << igroup << std::endl;
    }

    SKCalibPostFit.resize(NSKCalibGroup);
    for (int igroup = 0; igroup < NSKCalibGroup; igroup++)
    {
      SKCalibPostFit[igroup] = new FitObj(7 + FitType, igroup, PosteriorFileName);
      std::cout << "Found " << SKCalibPostFit[igroup]->nParams << " params in SKCalibPostFit Group" << igroup << std::endl;
    }

    SKDetBeamPostFit.resize(NSKDetBeamGroup);
    for (int igroup = 0; igroup < NSKDetBeamGroup; igroup++)
    {
      SKDetBeamPostFit[igroup] = new FitObj(10 + FitType, igroup, PosteriorFileName);
      std::cout << "Found " << SKDetBeamPostFit[igroup]->nParams << " params in SKDetBeamPostFit Group" << igroup << std::endl;
    }
  }   

  std::vector<std::vector<FitObj *>> AtmSKDetComp;
  AtmSKDetComp.resize(NAtmSKDetGroup);
  for (int igroup = 0; igroup < NAtmSKDetGroup; igroup++)
  {
    if(!includeSingleFits){
      AtmSKDetComp[igroup].resize(2);
      AtmSKDetComp[igroup][0] = AtmSKDetPreFit[igroup];
      AtmSKDetComp[igroup][1] = AtmSKDetPostFit[igroup];
    }  
  }

  std::vector<std::vector<FitObj *>> SKCalibComp;
  SKCalibComp.resize(NSKCalibGroup);
  for (int igroup = 0; igroup < NSKCalibGroup; igroup++)
  {
    if(!includeSingleFits){
      SKCalibComp[igroup].resize(2);
      SKCalibComp[igroup][0] = SKCalibPreFit[igroup];
      SKCalibComp[igroup][1] = SKCalibPostFit[igroup];
    }
  }

  std::vector<std::vector<FitObj *>> SKDetBeamComp;
  SKDetBeamComp.resize(NSKDetBeamGroup);
  for (int igroup = 0; igroup < NSKDetBeamGroup; igroup++)
  {
    if(!includeSingleFits){
      SKDetBeamComp[igroup].resize(2);
      SKDetBeamComp[igroup][0] = SKDetBeamPreFit[igroup];
      SKDetBeamComp[igroup][1] = SKDetBeamPostFit[igroup];
    }  
  }

  std::cout << "Histogramming results.." << std::endl;

  std::vector<std::vector<TGraphAsymmErrors *>> SKCalibComp_Hists;
  SKCalibComp_Hists.resize(NSKCalibGroup);
  for (int igroup = 0; igroup < NSKCalibGroup; igroup++)
  {
    TString groupname = Form("SKCalibComp_G%i", igroup);
    SKCalibComp_Hists[igroup] = FitResultsToHist(SKCalibComp[igroup], groupname);
    formatHistAsPreFit(SKCalibComp_Hists[igroup][0]);
    formatHistAsPostFit(SKCalibComp_Hists[igroup][1]);   
  }

  std::vector<std::vector<TGraphAsymmErrors *>> AtmSKDetComp_Hists;
  AtmSKDetComp_Hists.resize(NAtmSKDetGroup);
  for (int igroup = 0; igroup < NAtmSKDetGroup; igroup++)
  {
    TString groupname = Form("AtmSKDetComp_G%i", igroup);
    AtmSKDetComp_Hists[igroup] = FitResultsToHist(AtmSKDetComp[igroup], groupname);
    formatHistAsPreFit(AtmSKDetComp_Hists[igroup][0]);
    formatHistAsPostFit(AtmSKDetComp_Hists[igroup][1]);
  }

  std::vector<std::vector<TGraphAsymmErrors *>> SKDetBeamComp_Hists;
  SKDetBeamComp_Hists.resize(NSKDetBeamGroup);
  for (int igroup = 0; igroup < NSKDetBeamGroup; igroup++)
  {
    TString groupname = Form("SKDetBeamComp_G%i", igroup);
    SKDetBeamComp_Hists[igroup] = FitResultsToHist(SKDetBeamComp[igroup], groupname);
    formatHistAsPreFit(SKDetBeamComp_Hists[igroup][0]);
    formatHistAsPostFit(SKDetBeamComp_Hists[igroup][1]);    
  }

  std::cout << "Plotting histograms .." << std::endl;

  std::vector<TCanvas *> SKCalibComp_Canv;
  SKCalibComp_Canv.resize(NSKCalibGroup);
  for (int igroup = 0; igroup < NSKCalibGroup; igroup++)
  {
    TString canvasname = Form("SKCalibCanvas_G%i", igroup);
    SKCalibComp_Canv[igroup] = formatCanvas(SKCalibComp_Hists[igroup], SKCalibComp[igroup], canvasname);
    //TString printname = Form("./plots/SKCalibComp_G%i.pdf", igroup);
    //SKCalibComp_Canv[igroup]->Print(printname.Data());
    TString printname = Form("/plots/SKCalibComp_G%i.pdf", igroup);
    SKCalibComp_Canv[igroup]->Print(saveDir+printname);
  }

  std::vector<TCanvas *> AtmSKDetComp_Canv;
  AtmSKDetComp_Canv.resize(NAtmSKDetGroup);
  for (int igroup = 0; igroup < NAtmSKDetGroup; igroup++)
  {
    TString canvasname = Form("AtmSKDetCanvas_G%i", igroup);
    AtmSKDetComp_Canv[igroup] = formatCanvas_1Pad(AtmSKDetComp_Hists[igroup], AtmSKDetComp[igroup], canvasname, igroup);
    //TString printname = Form("./plots/AtmSKDetComp_G%i.pdf", igroup);
    //AtmSKDetComp_Canv[igroup]->Print(printname.Data());
    TString printname = Form("/plots/AtmSKDetComp_G%i.pdf", igroup);
    AtmSKDetComp_Canv[igroup]->Print(saveDir+printname);
  }

  std::vector<TCanvas *> SKDetBeamComp_Canv;
  SKDetBeamComp_Canv.resize(NSKDetBeamGroup);
  for (int igroup = 0; igroup < NSKDetBeamGroup; igroup++)
  {
    TString canvasname = Form("SKDetBeamCanvas_G%i", igroup);
    SKDetBeamComp_Canv[igroup] = formatCanvas(SKDetBeamComp_Hists[igroup], SKDetBeamComp[igroup], canvasname);
    //TString printname = Form("./plots/SKDetBeamComp_G%i.pdf", igroup);
    //SKDetBeamComp_Canv[igroup]->Print(printname.Data());
    TString printname = Form("/plots/SKDetBeamComp_G%i.pdf", igroup);
    SKDetBeamComp_Canv[igroup]->Print(saveDir+printname);
  }

  return 0;
}

void formatHistAsPreFit(TGraphAsymmErrors *Hist)
{
  Hist->SetFillColor(kRed);
  Hist->SetFillStyle(3003);
  Hist->SetMarkerStyle(7);
  Hist->SetMarkerColor(kRed);
  Hist->SetLineColor(kRed);
}

void formatHistAsPostFit(TGraphAsymmErrors *Hist)
{
  Hist->SetMarkerColor(kBlack);
  Hist->SetMarkerStyle(7);
  Hist->SetLineColor(kBlack);
}

TCanvas *formatCanvas(std::vector<TGraphAsymmErrors *> HistList, std::vector<FitObj *> FitObjs, TString CanvName)
{
  TCanvas *Canv = new TCanvas(CanvName.Data(), "");
  Canv->SetBottomMargin(BottomMargin);

  TH1D *Hist = new TH1D((CanvName + "_Axis").Data(), "", FitObjs[0]->nParams, 0, FitObjs[0]->nParams);

  for (int iBin = 0; iBin < HistList[0]->GetN(); iBin++)
  {
    Hist->GetXaxis()->SetBinLabel(iBin + 1, /*returnFancyString(FitObjs[0]->ParamNames[iBin]*/"");
  }
  //Hist->GetXaxis()->LabelsOption("v");
  //TString ytitle = "(x-#mu)/#sigma";
  TString ytitle = "Parameter variation";
  Hist->GetYaxis()->SetTitle(ytitle.Data());
  Hist->GetYaxis()->SetTitleOffset(0.8);
  Hist->GetYaxis()->SetTickLength(0.01);
  Hist->SetStats(false);

  double ymin = 999;
  double ymax = -999;
  for (int iobj = 0; iobj < HistList.size(); iobj++)
  {
    if (FitObjs[iobj]->minY < ymin)
      ymin = FitObjs[iobj]->minY;
    if (FitObjs[iobj]->maxY > ymax)
      ymax = FitObjs[iobj]->maxY;
  }
  if (ymin<0.) ymin = ymin*AxisScaleUp;
  else ymin = ymin*AxisScaleLow;
  if (ymax<0.) ymax = ymax*AxisScaleLow;
  else ymax = ymax*AxisScaleUp;
  Hist->SetMinimum(ymin);
  Hist->SetMaximum(ymax);

  // ~~~~~~~~~~~~~~~~~Ratio hists start~~~~~~~~~~~~~~~~~~~
  if (HistList.size() != 2 && HistList.size() != 4 && HistList.size() !=3)
  {
    std::cout << "Currently can only plot two or four set of fit results. Other options are not implemented!" << std::endl;
    throw;
  }

  TString ratio_ytitle = "(x-#mu_{Prior})/#sigma_{Prior}";
  std::vector<TGraphAsymmErrors *> Ratio_Asym(HistList.size());
  int Np = HistList[0]->GetN();

  double x[Np];
  double y[Np];
  double exl[Np];
  double exh[Np];
  double eyl[Np];
  double eyh[Np];

  double ymin_ratio = 999;
  double ymax_ratio = -999;
  for (int iObj = 0; iObj < HistList.size(); iObj++ ) {
    for (int ipt = 0; ipt < Np; ipt++) {
      x[ipt] = ipt + 0.5;
      exl[ipt] = exh[ipt] = 0.5;
      y[ipt] = ((HistList[iObj]->GetY())[ipt] - (HistList[0]->GetY())[ipt]) / (fabs(HistList[0]->GetErrorYhigh(ipt)));
      eyl[ipt] = (HistList[iObj]->GetErrorYhigh(ipt)) / (HistList[0]->GetErrorYhigh(ipt));
      eyh[ipt] = (HistList[iObj]->GetErrorYlow(ipt)) / (HistList[0]->GetErrorYlow(ipt));

      if ((y[ipt] + eyh[ipt]) > ymax_ratio)
      {
        ymax_ratio = y[ipt] + eyh[ipt];
      }
      if ((y[ipt] - eyl[ipt]) < ymin_ratio)
      {
        ymin_ratio = y[ipt] - eyl[ipt];
      }
    }

    Ratio_Asym[iObj] = new TGraphAsymmErrors(Np, x, y, exl, exh, eyl, eyh);
    Ratio_Asym[iObj]->SetName(CanvName + Form("_%i_Ratio", iObj));
    Ratio_Asym[iObj]->GetXaxis()->SetRangeUser(0, Np);
    Ratio_Asym[iObj]->SetMarkerStyle(7);
    if (iObj == 0){
      Ratio_Asym[iObj]->SetFillColor(kRed);
      Ratio_Asym[iObj]->SetFillStyle(3003);
      Ratio_Asym[iObj]->SetMarkerColor(kRed);
      Ratio_Asym[iObj]->SetLineColor(kRed);
    } else if (iObj == 1){
      Ratio_Asym[iObj]->SetMarkerColor(kBlack);
      Ratio_Asym[iObj]->SetLineColor(kBlack);
    } else if (iObj == 2){
      Ratio_Asym[iObj]->SetMarkerColor(kGreen);
      Ratio_Asym[iObj]->SetLineColor(kGreen);
      if(CanvName.BeginsWith("SKCalibCanvas")){
        Ratio_Asym[iObj]->SetMarkerColor(kBlue);
	Ratio_Asym[iObj]->SetLineColor(kBlue);
      }
    } else if (iObj == 3){
      Ratio_Asym[iObj]->SetMarkerColor(kBlue);
      Ratio_Asym[iObj]->SetLineColor(kBlue);
    }
  }

  TH1D *RatioHist = new TH1D((CanvName + "_Ratio").Data(), "", FitObjs[0]->nParams, 0, FitObjs[0]->nParams);
  for (int iBin = 0; iBin < HistList[0]->GetN(); iBin++)
  {
    RatioHist->GetXaxis()->SetBinLabel(iBin + 1, returnFancyString(FitObjs[0]->ParamNames[iBin]));
    //RatioHist->GetXaxis()->SetBinLabel(iBin + 1, FitObjs[0]->ParamNames[iBin]); 
  }
  RatioHist->GetXaxis()->LabelsOption("v");
  RatioHist->GetXaxis()->SetLabelSize(/*0.05*/0.04);
  RatioHist->GetYaxis()->SetTickLength(0.017);
  //RatioHist->GetYaxis()->SetTitleOffset(0);
  RatioHist->GetYaxis()->SetTitle(ratio_ytitle.Data());
  RatioHist->GetYaxis()->SetTitleOffset(0.8);
  //RatioHist->GetYaxis()->SetTitleSize(25);
  RatioHist->GetYaxis()->CenterTitle();
  RatioHist->SetStats(false);

  if (ymin_ratio<0.) ymin_ratio = ymin_ratio*AxisScaleUp;
  else ymin_ratio = ymin_ratio*AxisScaleLow;
  if (ymax_ratio<0.) ymax_ratio = ymax_ratio*AxisScaleLow;
  else ymax_ratio = ymax_ratio*AxisScaleUp;

  RatioHist->SetMinimum(ymin_ratio);
  RatioHist->SetMaximum(ymax_ratio);

  TLine *upperbound = new TLine(0.,1.0,Np,1.0);
  upperbound->SetLineColor(kRed);
  upperbound->SetLineStyle(2);
  TLine *lowerbound = new TLine(0.,-1.0,Np,-1.0);
  lowerbound->SetLineColor(kRed);
  lowerbound->SetLineStyle(2);
  // ~~~~~~~~~~~~~~~~~Ratio hists end~~~~~~~~~~~~~~~~~~~
  Canv->Divide(1,2);
  TVirtualPad* upperpad;
  TVirtualPad* lowerpad;

  upperpad = Canv->cd(1);
  upperpad->SetPad(0.0,0.5,1.0,1.0);
  upperpad->SetBottomMargin(0.);
  for (int iHist = 0; iHist < HistList.size(); iHist++)
  {
    if (iHist == 0)
    {
      Hist->Draw("AXIS");
      HistList[iHist]->Draw("P 2");
    }
    else
    {
      HistList[iHist]->Draw("SAME P");
    }
  }

  lowerpad = Canv->cd(2);
  lowerpad->SetPad(0.0,0.0,1.0,0.5);
  lowerpad->SetTopMargin(0.);
  lowerpad->SetBottomMargin(0.5);
  for (int iHist = 0; iHist < HistList.size(); iHist++)
  {
    if (iHist == 0)
    {
      RatioHist->Draw("AXIS");
      //Ratio_Asym[iHist]->Draw("P 2");
      upperbound->Draw("SAME");
      lowerbound->Draw("SAME");
    }
    else
    {
      Ratio_Asym[iHist]->Draw("SAME P");
    }
  }
  Canv->Update();

  return Canv;
}

TCanvas *formatCanvas_1Pad(std::vector<TGraphAsymmErrors *> HistList, std::vector<FitObj *> FitObjs, TString CanvName, int GroupID)
{
  TCanvas *Canv = new TCanvas(CanvName.Data(), "");
  Canv->SetBottomMargin(BottomMargin);

  TH1D *Hist = new TH1D((CanvName + "_Axis").Data(), "", FitObjs[0]->nParams, 0, FitObjs[0]->nParams);

  for (int iBin = 0; iBin < HistList[0]->GetN(); iBin++)
  {
    //Hist->GetXaxis()->SetBinLabel(iBin + 1, /*returnFancyString(FitObjs[0]->ParamNames[iBin]*/"");
    Hist->GetXaxis()->SetBinLabel(iBin + 1, returnFancyString(FitObjs[0]->ParamNames[iBin]));
  }
  Hist->GetXaxis()->LabelsOption("v");
  Hist->GetXaxis()->SetLabelSize(/*0.05*/0.04);
  //TString ytitle = "(x-#mu)/#sigma";
  TString ytitle = "Parameter variation";
  Hist->GetYaxis()->SetTitle(ytitle.Data());
  Hist->GetYaxis()->SetTitleOffset(0.95);
  Hist->GetYaxis()->SetTickLength(0.01);
  Hist->SetStats(false);
  //adjust Hist range
  double ymin = 999;
  double ymax = -999;
  for (int iobj = 1; iobj < HistList.size(); iobj++)  // ignore Prefit's max/min 
  {
    if (FitObjs[iobj]->minY < ymin)
      ymin = FitObjs[iobj]->minY;
    if (FitObjs[iobj]->maxY > ymax)
      ymax = FitObjs[iobj]->maxY;
  }
  // top axis for event topology
  TH1D* Hist_top = new TH1D((CanvName + "_topAxis").Data(), "", 7, 0, 7);
  for(int iBin=0; iBin < 7; iBin++){
    Hist_top->GetXaxis()->SetBinLabel(iBin + 1, returnTopology(iBin));
  } 
  Hist_top->GetXaxis()->SetLabelSize(/*0.05*/0.04);
  Hist_top->GetXaxis()->SetTickLength(0.07);
  Hist_top->GetYaxis()->SetLabelSize(0.);
  Hist_top->GetYaxis()->SetTickSize(0.);
  Hist_top->SetTitle(returnTitle(GroupID));
  Hist_top->SetTitleSize(0.02);;
  Hist_top->SetStats(false);
 
  // ~~~~~~~~~~~~~~~~~Prior line~~~~~~~~~~~~~~~~~~~
  int Np = HistList[0]->GetN();

  TLine* prior_line_1;
  TLine* prior_line_2;
  if (GroupID%2==0) {
   //smear   
    prior_line_1 = new TLine(0., 1.0, 12, 1.0);
    prior_line_1->SetLineColor(kRed);
    prior_line_1->SetLineStyle(2);

    prior_line_2 = new TLine(15, 1.0, Np, 1.0);
    prior_line_2->SetLineColor(kRed);
    prior_line_2->SetLineStyle(2);
  } else {
    //shift 
    prior_line_1 = new TLine(0., 0.0, 12, 0.0);
    prior_line_1->SetLineColor(kRed);
    prior_line_1->SetLineStyle(2);

    prior_line_2 = new TLine(15, 0.0, Np, 0.0);
    prior_line_2->SetLineColor(kRed);
    prior_line_2->SetLineStyle(2);
  }

  // ~~~~~~~~~~~~~~~~~Prior line end~~~~~~~~~~~~~~~~~~~
  // ~~~~~~~~~~~~~~~~~HybridPi0-constrained priors~~~~~
  std::string MaCh3Install = "/home/mojia/MaCh3_2023_detShiftSmear/MaCh3";
  TString AtmSKDetHybridPi0FileName = MaCh3Install+"/inputs/skatm/hmupi_hpi0_chi2_maps_14c_320_320_corr.root";
  TFile* hybridPi0File = new TFile(AtmSKDetHybridPi0FileName, "READ");
  //name of the selection parameter
  std::string selecParamName;
  if(GroupID == 0 || GroupID == 1) {
    selecParamName = "rcpar";
  }else if(GroupID == 2 || GroupID == 3){
    selecParamName = "emu";
  }else if(GroupID == 4 || GroupID == 5){
    selecParamName = "epi0";
  }else if(GroupID == 6 || GroupID == 7){
    selecParamName = "mupip";
  }else{
    std::cerr << "invalid GroupID for selection parameter!" << std::endl;
    std::cerr << "GroupID: " << GroupID << std::endl;
    throw;
  }
  std::string hybrid_prefix = "h_pi0_chi2_";
  std::vector<TString> chi2_hist_names(3); 
  for(int iEvis=0; iEvis < 3; iEvis++){
    //TString chi2HistName = hybrid_prefix+selecParamName+Form("%i", iEvis);
    chi2_hist_names[iEvis] = hybrid_prefix+selecParamName+Form("%i", iEvis);
  }
  // read the chi2 map and extract its minimum
  TH2D* chi2_hist_temp;
  double prior_values[3];
  for(int iEvis=0; iEvis<3; iEvis++){
    chi2_hist_temp = (TH2D*)hybridPi0File->Get(chi2_hist_names[iEvis]);
    int MinBin = chi2_hist_temp->GetMinimumBin();
    int X,Y,Z;
    chi2_hist_temp->GetBinXYZ(MinBin,X,Y,Z);

    if(GroupID%2==0){
      //smear
      prior_values[iEvis] = chi2_hist_temp->GetXaxis()->GetBinCenter(X);
    }else{
      //shift
      prior_values[iEvis] = chi2_hist_temp->GetYaxis()->GetBinCenter(Y);
    }
  }
  // read the chi2 map and extract the 1sigma error
  double upper_limits[3];
  double lower_limits[3];
  for(int iEvis=0; iEvis<3; iEvis++){
    chi2_hist_temp = (TH2D*)hybridPi0File->Get(chi2_hist_names[iEvis]);
    upper_limits[iEvis] = FindUpperLimit(GroupID, chi2_hist_temp, prior_values[iEvis]);
    lower_limits[iEvis] = FindLowerLimit(GroupID, chi2_hist_temp, prior_values[iEvis]); 
    if (prior_values[iEvis]+upper_limits[iEvis] > ymax) ymax = prior_values[iEvis]+upper_limits[iEvis];
    if (prior_values[iEvis]-lower_limits[iEvis] < ymin) ymin = prior_values[iEvis]-lower_limits[iEvis];  
  }

  hybridPi0File->Close();
  // create TGraph for hybridPi0 priors
  double pi0Param_x[3];
  double pi0Param_x_el[3];
  double pi0Param_x_eh[3];
  //hard-coded values for x
  pi0Param_x[0] = 12+0.5;
  pi0Param_x[1] = 13+0.5;
  pi0Param_x[2] = 14+0.5;
  pi0Param_x_el[0] = 0.5;
  pi0Param_x_el[1] = 0.5;
  pi0Param_x_el[2] = 0.5;
  pi0Param_x_eh[0] = 0.5;
  pi0Param_x_eh[1] = 0.5;
  pi0Param_x_eh[2] = 0.5;
  
  //TGraph* hybridPi0Prior = new TGraph(3, pi0Param_x, prior_values);
  //hybridPi0Prior->SetMarkerColor(kRed);
  //hybridPi0Prior->SetMarkerStyle(7);
  TGraphAsymmErrors* hybridPi0Prior = new TGraphAsymmErrors(3, pi0Param_x, prior_values, pi0Param_x_el, pi0Param_x_eh, lower_limits, upper_limits);
  formatHistAsPreFit(hybridPi0Prior);
  // ~~~~~~~~~~~~~~~~~HybridPi0-constrained priors end~
  // adjust Hist range
  if (ymin<0.) ymin = ymin*AxisScaleUp;
  else ymin = ymin*AxisScaleLow;
  if (ymax<0.) ymax = ymax*AxisScaleLow;
  else ymax = ymax*AxisScaleUp;
  Hist->SetMinimum(ymin);
  Hist->SetMaximum(ymax);

  //hard-code the Range of hist for smearing parameters
  if(GroupID%2 == 0) {
    Hist->SetMinimum(0.9);
    Hist->SetMaximum(1.15);
  }

  //~~~~~~~~~~~~~~~~~~Make plots~~~~~~~~~~~~~~~~~~~~~~~
  Canv->cd();
  TPad *pad1 = new TPad("pad1","",0,0,1,1);
  TPad *pad2 = new TPad("pad2","",0,0,1,1);
  pad2->SetFillColor(0);
  pad2->SetFillStyle(0);
  pad2->SetFrameFillStyle(0);
  //Pad1
  pad1->Draw();
  pad1->cd();
  pad1->SetTopMargin(0.14);
  pad1->SetBottomMargin(0.27);
  for (int iHist = 0; iHist < HistList.size(); iHist++)
  {
    if (iHist == 0)
    {
      Hist->Draw("AXIS");
      //HistList[iHist]->Draw("P 2");
      prior_line_1->Draw("SAME");
      prior_line_2->Draw("SAME");
      hybridPi0Prior->Draw("SAME P 2");
    }
    else
    {
      HistList[iHist]->Draw("SAME P");
    }
  }
 //
  //Pad2
  pad2->Draw();
  pad2->cd();
  pad2->SetTopMargin(0.14);
  pad2->SetBottomMargin(0.27);
  Hist_top->Draw("X+"); 
 
  //Canv->SetBottomMargin(0.4);
  Canv->Update();

  return Canv;
}

std::vector<TGraphAsymmErrors *> FitResultsToHist(std::vector<FitObj *> FitObjs, TString HistTitle)
{

  std::vector<TGraphAsymmErrors *> returnVec_Asym(FitObjs.size());

  bool MatchingNParams = true;
  if (FitObjs.size() > 1)
  {
    for (int i = 1; i < FitObjs.size(); i++)
    {
      if (FitObjs[i]->nParams != FitObjs[0]->nParams)
      {
        MatchingNParams = false;
      }
    }
  }

  int nParams = FitObjs[0]->nParams;

  if (!MatchingNParams)
  {
    std::cout << "Different elements of FitObjs have different nParams. Quitting.." << std::endl;
    throw;
  }

  double binContent;
  double binError;

  double minY = 1e9;
  double maxY = -1e9;

  double x[nParams];
  double y[nParams];
  double exl[nParams];
  double exh[nParams];
  double eyl[nParams];
  double eyh[nParams];

  for (int iObj = 0; iObj < FitObjs.size(); iObj++)
  {
    for (int iParam = 0; iParam < nParams; iParam++)
    {

      //===========
      x[iParam] = iParam + 0.5;
      y[iParam] = FitObjs[iObj]->CentralVals[iParam];
      exl[iParam] = 0.5;
      exh[iParam] = 0.5;
      eyl[iParam] = FitObjs[iObj]->ErrorValsLower[iParam];
      eyh[iParam] = FitObjs[iObj]->ErrorValsUpper[iParam];

      // std::cout << exl[iParam] << " " << x[iParam] << " " << exh[iParam] << " " << eyl[iParam] << " " << y[iParam] << " " << eyh[iParam] << std::endl;
      //===========

      if ((y[iParam] + eyh[iParam]) > maxY)
      {
        maxY = y[iParam] + eyh[iParam];
      }
      if ((y[iParam] - eyl[iParam]) < minY)
      {
        minY = y[iParam] - eyl[iParam];
      }
    }

    returnVec_Asym[iObj] = new TGraphAsymmErrors(nParams, x, y, exl, exh, eyl, eyh);
    returnVec_Asym[iObj]->SetName(HistTitle + Form("_%i_Asym", iObj));
    returnVec_Asym[iObj]->GetXaxis()->SetRangeUser(0, nParams);
  }

  for (int iHist = 0; iHist < returnVec_Asym.size(); iHist++)
  {
    FitObjs[iHist]->maxY = maxY /* AxisScaleUp*/;
    FitObjs[iHist]->minY = minY /* AxisScaleLow*/;
  }

  return returnVec_Asym;
}

//============================================================================================================
//============================================================================================================
//============================================================================================================

FitObj::FitObj(int ObjType_, int SplitGroup_, TString FileName)
{
  ObjType = ObjType_; // for now: 0-->AtmSKDet, 1-->SKCalib, 2-->SKDetBeam
  SplitGroup = SplitGroup_;

  if (ObjType == 1)
    AtmSKDetPreFit(FileName, SplitGroup);
  if (ObjType == 2)
    SKCalibPreFit(FileName, SplitGroup);
  if (ObjType == 3)
    SKDetBeamPreFit(FileName, SplitGroup);
  // AtmSKDet
  if (ObjType == 4)
    PostFit(FileName, 0, 0, SplitGroup);
  if (ObjType == 5)
    PostFit(FileName, 0, 1, SplitGroup);
  if (ObjType == 6)
    PostFit(FileName, 0, 2, SplitGroup);
  // SKCalib
  if (ObjType == 7)
    PostFit(FileName, 1, 0, SplitGroup);
  if (ObjType == 8)
    PostFit(FileName, 1, 1, SplitGroup);
  if (ObjType == 9)
    PostFit(FileName, 1, 2, SplitGroup);
  // SKDetBeam
  if (ObjType == 10)
    PostFit(FileName, 2, 0, SplitGroup);
  if (ObjType == 11)
    PostFit(FileName, 2, 1, SplitGroup);
  if (ObjType == 12)
    PostFit(FileName, 2, 2, SplitGroup);
}

void FitObj::AtmSKDetPreFit(TString CovFile, int SplitGroup)
{
  TFile *fcov = new TFile(CovFile, "open");
  if (fcov->IsZombie())
  {
    std::cerr << "Couldn't find AtmSKDet File " << fcov << std::endl;
    throw;
  }

  TMatrixDSym *cov = (TMatrixDSym *)(fcov->Get("AtmSKDetSyst"));

  int NParams;
  NParams = cov->GetNrows();

  int count = 0;
  int group_count = 0;   // NParams not equal to the size of AtmSKDetGroup; need to filter out parameters assigned to high Evis bin
  for (int icount = 0; icount < NParams; icount++)
  {
    if(icount == 6 || icount == 7) continue;
    if(icount == 14 || icount == 15) continue;
    if(icount == 22 || icount == 23) continue;
    if(icount == 30 || icount == 31) continue;
    if(icount == 38 || icount == 39) continue;
    if(icount == 46 || icount == 47) continue;
    if(icount == 54 || icount == 55) continue;
    if(icount == 62 || icount == 63) continue;
    if(icount == 70 || icount == 71) continue;
    if(icount == 78 || icount == 79) continue;
    if(icount == 86 || icount == 87) continue;
    if(icount == 94 || icount == 95) continue;
    if(icount == 102 || icount == 103) continue;
    if(icount == 110 || icount == 111) continue;
    if(icount == 118 || icount == 119) continue;
    if(icount == 126 || icount == 127) continue;
    if(icount == 134 || icount == 135) continue;
    if(icount == 142 || icount == 143) continue;
    if(icount == 150 || icount == 151) continue;
    if(icount == 158 || icount == 159) continue;
    if(icount == 166 || icount == 167) continue;
    if(icount == 174 || icount == 175) continue;
    if(icount == 182 || icount == 183) continue;
    if(icount == 190 || icount == 191) continue;
    if(icount == 198 || icount == 199) continue;
    if(icount == 206 || icount == 207) continue;
    if(icount == 214 || icount == 215) continue;
    if(icount == 222 || icount == 223) continue;
  
    if (AtmSKDetGroup[group_count] == SplitGroup)
      count++;

    group_count++;
  }
  if (count == 0)
  {
    std::cout << "Cannot find syst that is labeled with splitgroup " << SplitGroup << std::endl;
    throw;
  }
  nParams = count;
  CentralVals.resize(nParams);
  ErrorValsLower.resize(nParams);
  ErrorValsUpper.resize(nParams);
  ParamNames.resize(nParams);

  int index = 0;
  int group_i = 0; 
  for (int i = 0; i < NParams; i++)
  { 
    if(i == 6 || i == 7) continue;
    if(i == 14 || i == 15) continue;
    if(i == 22 || i == 23) continue;
    if(i == 30 || i == 31) continue;
    if(i == 38 || i == 39) continue;
    if(i == 46 || i == 47) continue;
    if(i == 54 || i == 55) continue;
    if(i == 62 || i == 63) continue;
    if(i == 70 || i == 71) continue;
    if(i == 78 || i == 79) continue;
    if(i == 86 || i == 87) continue;
    if(i == 94 || i == 95) continue;
    if(i == 102 || i == 103) continue;
    if(i == 110 || i == 111) continue;
    if(i == 118 || i == 119) continue;
    if(i == 126 || i == 127) continue;
    if(i == 134 || i == 135) continue;
    if(i == 142 || i == 143) continue;
    if(i == 150 || i == 151) continue;
    if(i == 158 || i == 159) continue;
    if(i == 166 || i == 167) continue;
    if(i == 174 || i == 175) continue;
    if(i == 182 || i == 183) continue;
    if(i == 190 || i == 191) continue;
    if(i == 198 || i == 199) continue;
    if(i == 206 || i == 207) continue;
    if(i == 214 || i == 215) continue;
    if(i == 222 || i == 223) continue;
 
    if (AtmSKDetGroup[group_i] == SplitGroup)
    {
      // Push back the name
      // JJ: use the boring name here
      TString TempString = Form("atmskdet_%i", i);
      ParamNames[index] = TempString;

      
      if( i%2 == 1 )CentralVals[index] = 0.;
      if( i%2 == 0 )CentralVals[index] = 1.;

      ErrorValsLower[index] = +1.;
      ErrorValsUpper[index] = +1.;

      index++;
    }
    group_i++;
  }

  fcov->Close();
  delete cov;

}

void FitObj::SKCalibPreFit(TString CovFile, int SplitGroup)
{
  //manager *fitMan = new manager(const_cast<char *>(ConfigName.Data()), false);
  //int NParams;
  //NParams = fitMan->GetNumSKCalibrationSyst();
  TFile *fcov = new TFile(CovFile, "open");
  if (fcov->IsZombie())
  {	        
    std::cerr << "Couldn't find SKCalib Cov File " << fcov << std::endl;
    throw;
  }
  TMatrixDSym *cov = (TMatrixDSym *)(fcov->Get("SKCalibrationSyst"));
  int NParams;
  NParams = cov->GetNrows(); 

  if (NParams != SKCalibGroup.size())
  {
    std::cout << "Input SKCalib cov size is not the same as splitgroup vec!" << std::endl;
    throw;
  }
  if (SplitGroup < 0 || SplitGroup > NSKCalibGroup)
  {
    std::cout << "Invalid SKCalib splitgroup!" << std::endl;
    throw;
  }

  int count = 0;
  for (int icount = 0; icount < NParams; icount++)
  {
    if (SKCalibGroup[icount] == SplitGroup)
      count++;
  }
  if (count == 0)
  {
    std::cout << "Cannot find syst that is labeled with splitgroup " << SplitGroup << std::endl;
    throw;
  }
  nParams = count;
  // ParamNames = fitMan->GetSKCalibSystNames();
  ParamNames.resize(nParams);
  CentralVals.resize(nParams);
  ErrorValsLower.resize(nParams);
  ErrorValsUpper.resize(nParams);

  int index = 0;
  for (int i = 0; i < NParams; i++)
  {
    if (SKCalibGroup[i] == SplitGroup)
    {
      // Push back the name
      // JJ: use the boring name here
      TString TempString = Form("skcalib_%i", i);
      ParamNames[index] = TempString;

      CentralVals[index] = 0.;
      ErrorValsLower[index] = +1.;
      ErrorValsUpper[index] = +1.;

      index++;
    }
  }

  //delete fitMan;
  fcov->Close();
  delete fcov;

}

void FitObj::SKDetBeamPreFit(TString CovFile, int SplitGroup)
{
  //manager *fitMan = new manager(const_cast<char *>(ConfigName.Data()), false);
  //TFile *fcov = new TFile((fitMan->GetSKDetBeamCovFileName()).c_str(), "READ");
  //TMatrixDSym *cov = (TMatrixDSym *)fcov->Get((fitMan->GetSKDetBeamCovMatrixName()).c_str());
  TFile* fcov = new TFile(CovFile, "READ");
  TMatrixDSym *cov = (TMatrixDSym *)(fcov->Get("SKDetBeamSyst"));

  int NParams;
  NParams = cov->GetNrows();
  if (NParams != SKDetBeamGroup.size())
  {
    std::cout << "Input SKDetBeam cov size is not the same as splitgroup vec!" << std::endl;
    throw;
  }
  if (SplitGroup < 0 || SplitGroup > NSKDetBeamGroup)
  {
    std::cout << "Invalid SKDetBeam splitgroup!" << std::endl;
    throw;
  }

  int count = 0;
  for (int icount = 0; icount < NParams; icount++)
  {
    if (SKDetBeamGroup[icount] == SplitGroup)
      count++;
  }
  if (count == 0)
  {
    std::cout << "Cannot find syst that is labeled with splitgroup " << SplitGroup << std::endl;
    throw;
  }
  nParams = count;
  CentralVals.resize(nParams);
  ErrorValsLower.resize(nParams);
  ErrorValsUpper.resize(nParams);
  ParamNames.resize(nParams);

  int index = 0;
  for (int i = 0; i < NParams; i++)
  {
    if (SKDetBeamGroup[i] == SplitGroup)
    {
      // Push back the name
      // JJ: use the boring name here
      TString TempString = Form("skdetbeam_%i", i);
      ParamNames[index] = TempString;

      
      CentralVals[index] = 0.;

      ErrorValsLower[index] = sqrt((*cov)(i, i));
      ErrorValsUpper[index] = sqrt((*cov)(i, i));

      index++;
    }
  }

  fcov->Close();
  delete cov;
  //delete fitMan;
}

void FitObj::PostFit(TString FileName, int FitParam, int FitType, int SplitGroup)
{
  TFile *inFile = new TFile(FileName, "READ");
  if (inFile->IsZombie())
  {
    std::cerr << "Couldn't find PostFit file " << inFile << std::endl;
    throw;
  }

  TString TreeName;
  if (FitParam == 0)
  {
    TreeName = "AtmSKDetPosteriors";
  }
  else if (FitParam == 1)
  {
    TreeName = "SKCalibPosteriors";
  }
  else if (FitParam == 2)
  {
    TreeName = "SKDetBeamPosteriors";
  }
  else
  {
    std::cout << "Unknown FitParam. Given:" << FitParam << std::endl;
    throw;
  }

  TString CentralString;
  TString ErrorString;
  TString ErrorLowerString;
  TString ErrorUpperString;

  if (FitType == 0)
  {
    CentralString = "ArithmeticCentral";
    ErrorString = "ArithmeticError";
  }
  else if (FitType == 1)
  {
    CentralString = "GaussCentral";
    ErrorString = "GaussError";
  }
  else if (FitType == 2)
  {
    CentralString = "HPDCentral";
    ErrorString = "HPDError";
    ErrorLowerString = "HPDError_Lower";
    ErrorUpperString = "HPDError_Upper";
  }
  else
  {
    std::cout << "Unknown FitType. Given:" << FitType << std::endl;
    throw;
  }

  std::string *ParamName = 0;
  double CentralVal;
  double ErrorVal;
  double ErrorLowerVal;
  double ErrorUpperVal;

  TTree *inTree = (TTree *)inFile->Get(TreeName.Data());
  if (!inTree)
  {
    std::cout << "Tree not found. Given:" << TreeName << std::endl;
    throw;
  }

  inTree->SetBranchAddress("ParamName", &ParamName);
  inTree->SetBranchAddress(CentralString.Data(), &CentralVal);
  inTree->SetBranchAddress(ErrorString.Data(), &ErrorVal);
  if (FitType == 2)
  {
    inTree->SetBranchAddress(ErrorLowerString.Data(), &ErrorLowerVal);
    inTree->SetBranchAddress(ErrorUpperString.Data(), &ErrorUpperVal);
  }

  int NParams;
  NParams = inTree->GetEntries();
  int count = 0;
  for (int icount = 0; icount < NParams; icount++)
  {
    if (FitParam == 0)
    {
      if (AtmSKDetGroup[icount] == SplitGroup)
        count++;
    }
    if (FitParam == 1)
    {
      if (SKCalibGroup[icount] == SplitGroup)
        count++;
    }
    if (FitParam == 2)
    {
      if (SKDetBeamGroup[icount] == SplitGroup)
        count++;
    }
  }
  if (count == 0)
  {
    std::cout << "Cannot find syst that is labeled with splitgroup " << SplitGroup << std::endl;
    throw;
  }
  nParams = count;

  ParamNames.resize(nParams);
  CentralVals.resize(nParams);
  ErrorValsLower.resize(nParams);
  ErrorValsUpper.resize(nParams);

  int index = 0;
  for (int iParam = 0; iParam < NParams; iParam++)
  {
    if (FitParam == 0)
    {
      if (AtmSKDetGroup[iParam] == SplitGroup)
      {
        inTree->GetEntry(iParam);
        ParamNames[index] = *ParamName;
        if (FitType == 2)
        {
          CentralVals[index] = CentralVal;
          ErrorValsLower[index] = ErrorLowerVal;
          ErrorValsUpper[index] = ErrorUpperVal;
        }
        else
        {
          CentralVals[index] = CentralVal;
          ErrorValsLower[index] = ErrorVal;
          ErrorValsUpper[index] = ErrorVal;
        }
        index++;
      }
    }
    if (FitParam == 1)
    {
      if (SKCalibGroup[iParam] == SplitGroup)
      {
        inTree->GetEntry(iParam);
        ParamNames[index] = *ParamName;
        if (FitType == 2)
        {
          CentralVals[index] = CentralVal;
          ErrorValsLower[index] = ErrorLowerVal;
          ErrorValsUpper[index] = ErrorUpperVal;
        }
        else
        {
          CentralVals[index] = CentralVal;
          ErrorValsLower[index] = ErrorVal;
          ErrorValsUpper[index] = ErrorVal;
        }
        index++;
      }
    }
    if (FitParam == 2)
    {
      if (SKDetBeamGroup[iParam] == SplitGroup)
      {
        inTree->GetEntry(iParam);
        ParamNames[index] = *ParamName;
        if (FitType == 2)
        {
          CentralVals[index] = CentralVal;
          ErrorValsLower[index] = ErrorLowerVal;
          ErrorValsUpper[index] = ErrorUpperVal;
        }
        else
        {
          CentralVals[index] = CentralVal;
          ErrorValsLower[index] = ErrorVal;
          ErrorValsUpper[index] = ErrorVal;
        }
        index++;
      }
    }
  }
}

TString returnFancyString(std::string inStr)
{
  TString returnString;

  //returnString = inStr;
  if (inStr == "atmskdet_0") returnString = "#alpha^{0}_{0,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_2") returnString = "#alpha^{0}_{0,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_4") returnString = "#alpha^{0}_{0,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_8") returnString = "#alpha^{0}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_10") returnString = "#alpha^{0}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_12") returnString = "#alpha^{0}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_16") returnString = "#alpha^{0}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_18") returnString = "#alpha^{0}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_20") returnString = "#alpha^{0}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_24") returnString = "#alpha^{0}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_26") returnString = "#alpha^{0}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_28") returnString = "#alpha^{0}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_32") returnString = "#alpha^{0}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_34") returnString = "#alpha^{0}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_36") returnString = "#alpha^{0}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_40") returnString = "#alpha^{0}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_42") returnString = "#alpha^{0}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_44") returnString = "#alpha^{0}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_48") returnString = "#alpha^{0}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_50") returnString = "#alpha^{0}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_52") returnString = "#alpha^{0}_{6,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_1") returnString = "#beta^{0}_{0,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_3") returnString = "#beta^{0}_{0,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_5") returnString = "#beta^{0}_{0,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_9") returnString = "#beta^{0}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_11") returnString = "#beta^{0}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_13") returnString = "#beta^{0}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_17") returnString = "#beta^{0}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_19") returnString = "#beta^{0}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_21") returnString = "#beta^{0}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_25") returnString = "#beta^{0}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_27") returnString = "#beta^{0}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_29") returnString = "#beta^{0}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_33") returnString = "#beta^{0}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_35") returnString = "#beta^{0}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_37") returnString = "#beta^{0}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_41") returnString = "#beta^{0}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_43") returnString = "#beta^{0}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_45") returnString = "#beta^{0}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_49") returnString = "#beta^{0}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_51") returnString = "#beta^{0}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_53") returnString = "#beta^{0}_{6,2}, 0.7 #leq Evis < 1.33";

  if (inStr == "atmskdet_56") returnString = "#alpha^{1}_{0,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_58") returnString = "#alpha^{1}_{0,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_60") returnString = "#alpha^{1}_{0,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_64") returnString = "#alpha^{1}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_66") returnString = "#alpha^{1}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_68") returnString = "#alpha^{1}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_72") returnString = "#alpha^{1}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_74") returnString = "#alpha^{1}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_76") returnString = "#alpha^{1}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_80") returnString = "#alpha^{1}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_82") returnString = "#alpha^{1}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_84") returnString = "#alpha^{1}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_88") returnString = "#alpha^{1}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_90") returnString = "#alpha^{1}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_92") returnString = "#alpha^{1}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_96") returnString = "#alpha^{1}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_98") returnString = "#alpha^{1}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_100") returnString = "#alpha^{1}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_104") returnString = "#alpha^{1}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_106") returnString = "#alpha^{1}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_108") returnString = "#alpha^{1}_{6,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_57") returnString = "#beta^{1}_{0,0},  0 #leq Evis < 0.3";
  if (inStr == "atmskdet_59") returnString = "#beta^{1}_{0,1},  0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_61") returnString = "#beta^{1}_{0,2},  0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_65") returnString = "#beta^{1}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_67") returnString = "#beta^{1}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_69") returnString = "#beta^{1}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_73") returnString = "#beta^{1}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_75") returnString = "#beta^{1}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_77") returnString = "#beta^{1}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_81") returnString = "#beta^{1}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_83") returnString = "#beta^{1}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_85") returnString = "#beta^{1}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_89") returnString = "#beta^{1}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_91") returnString = "#beta^{1}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_93") returnString = "#beta^{1}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_97") returnString = "#beta^{1}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_99") returnString = "#beta^{1}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_101") returnString = "#beta^{1}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_105") returnString = "#beta^{1}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_107") returnString = "#beta^{1}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_109") returnString = "#beta^{1}_{6,2}, 0.7 #leq Evis < 1.33";

  if (inStr == "atmskdet_112") returnString = "#alpha^{2}_{0,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_114") returnString = "#alpha^{2}_{0,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_116") returnString = "#alpha^{2}_{0,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_120") returnString = "#alpha^{2}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_122") returnString = "#alpha^{2}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_124") returnString = "#alpha^{2}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_128") returnString = "#alpha^{2}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_130") returnString = "#alpha^{2}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_132") returnString = "#alpha^{2}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_136") returnString = "#alpha^{2}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_138") returnString = "#alpha^{2}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_140") returnString = "#alpha^{2}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_144") returnString = "#alpha^{2}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_146") returnString = "#alpha^{2}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_148") returnString = "#alpha^{2}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_152") returnString = "#alpha^{2}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_154") returnString = "#alpha^{2}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_156") returnString = "#alpha^{2}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_160") returnString = "#alpha^{2}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_162") returnString = "#alpha^{2}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_164") returnString = "#alpha^{2}_{6,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_113") returnString = "#beta^{2}_{0,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_115") returnString = "#beta^{2}_{0,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_117") returnString = "#beta^{2}_{0,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_121") returnString = "#beta^{2}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_123") returnString = "#beta^{2}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_125") returnString = "#beta^{2}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_129") returnString = "#beta^{2}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_131") returnString = "#beta^{2}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_133") returnString = "#beta^{2}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_137") returnString = "#beta^{2}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_139") returnString = "#beta^{2}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_141") returnString = "#beta^{2}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_145") returnString = "#beta^{2}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_147") returnString = "#beta^{2}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_149") returnString = "#beta^{2}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_153") returnString = "#beta^{2}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_155") returnString = "#beta^{2}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_157") returnString = "#beta^{2}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_161") returnString = "#beta^{2}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_163") returnString = "#beta^{2}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_165") returnString = "#beta^{2}_{6,2}, 0.7 #leq Evis < 1.33";

  if (inStr == "atmskdet_168") returnString = "#alpha^{3}_{0,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_170") returnString = "#alpha^{3}_{0,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_172") returnString = "#alpha^{3}_{0,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_176") returnString = "#alpha^{3}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_178") returnString = "#alpha^{3}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_180") returnString = "#alpha^{3}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_184") returnString = "#alpha^{3}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_186") returnString = "#alpha^{3}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_188") returnString = "#alpha^{3}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_192") returnString = "#alpha^{3}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_194") returnString = "#alpha^{3}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_196") returnString = "#alpha^{3}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_200") returnString = "#alpha^{3}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_202") returnString = "#alpha^{3}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_204") returnString = "#alpha^{3}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_208") returnString = "#alpha^{3}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_210") returnString = "#alpha^{3}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_212") returnString = "#alpha^{3}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_216") returnString = "#alpha^{3}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_218") returnString = "#alpha^{3}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_220") returnString = "#alpha^{3}_{6,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_169") returnString = "#beta^{3}_{0,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_171") returnString = "#beta^{3}_{0,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_173") returnString = "#beta^{3}_{0,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_177") returnString = "#beta^{3}_{1,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_179") returnString = "#beta^{3}_{1,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_181") returnString = "#beta^{3}_{1,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_185") returnString = "#beta^{3}_{2,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_187") returnString = "#beta^{3}_{2,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_189") returnString = "#beta^{3}_{2,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_193") returnString = "#beta^{3}_{3,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_195") returnString = "#beta^{3}_{3,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_197") returnString = "#beta^{3}_{3,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_201") returnString = "#beta^{3}_{4,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_203") returnString = "#beta^{3}_{4,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_205") returnString = "#beta^{3}_{4,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_209") returnString = "#beta^{3}_{5,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_211") returnString = "#beta^{3}_{5,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_213") returnString = "#beta^{3}_{5,2}, 0.7 #leq Evis < 1.33";
  if (inStr == "atmskdet_217") returnString = "#beta^{3}_{6,0}, 0 #leq Evis < 0.3";
  if (inStr == "atmskdet_219") returnString = "#beta^{3}_{6,1}, 0.3 #leq Evis < 0.7";
  if (inStr == "atmskdet_221") returnString = "#beta^{3}_{6,2}, 0.7 #leq Evis < 1.33";

  if (inStr == "skdetbeam_0") returnString = "fiducial volume";
  if (inStr == "skdetbeam_1") returnString = "NC Gamma";
  if (inStr == "skdetbeam_2") returnString = "numu PID";
  if (inStr == "skdetbeam_3") returnString = "decay electron dial_1";
  if (inStr == "skdetbeam_4") returnString = "decay electron dial_2";
  if (inStr == "skdetbeam_5") returnString = "atm flux dial_1";
  if (inStr == "skdetbeam_6") returnString = "atm flux dial_2";
  if (inStr == "skdetbeam_7") returnString = "NC 1pi dial_1";
  if (inStr == "skdetbeam_8") returnString = "NC 1pi dial_2";
  if (inStr == "skdetbeam_9") returnString = "NC 1pi dial_3";
  if (inStr == "skdetbeam_10") returnString = "NC other dial_1";
  if (inStr == "skdetbeam_11") returnString = "NC other dial_2";
  if (inStr == "skdetbeam_12") returnString = "NC other dial_3";
  if (inStr == "skdetbeam_13") returnString = "DIF muon";
  if (inStr == "skdetbeam_14") returnString = "numu CC";
  if (inStr == "skdetbeam_15") returnString = "numu CC other";
  if (inStr == "skdetbeam_16") returnString = "numu nue dial";
  if (inStr == "skdetbeam_17") returnString = "numu NC";

  if (inStr == "skcalib_0") returnString = "FC Reduction";
  if (inStr == "skcalib_1") returnString = "PC Reduction";
  if (inStr == "skcalib_2") returnString = "FC PC Separation";
  if (inStr == "skcalib_3") returnString = "PC Stopping ThruGoing Separation Top";
  if (inStr == "skcalib_4") returnString = "PC Stopping ThruGoing Separation Barrel";
  if (inStr == "skcalib_5") returnString = "PC Stopping ThruGoing Separation Bottom";
  if (inStr == "skcalib_6") returnString = "Cosmic Ray Background";
  if (inStr == "skcalib_7") returnString = "Flasher Background";
  if (inStr == "skcalib_8") returnString = "Fiducial Volume";
  if (inStr == "skcalib_9") returnString = "Upmu Reduction";
  if (inStr == "skcalib_10") returnString = "Upmu Stopping ThruGoing Separation";
  if (inStr == "skcalib_11") returnString = "Upmu EnergyCut";
  if (inStr == "skcalib_12") returnString = "Upmu Path LengthCut";
  if (inStr == "skcalib_13") returnString = "Upmu Separation";
  if (inStr == "skcalib_14") returnString = "Upmu Stopping Background";
  if (inStr == "skcalib_15") returnString = "Upmu ThruGoing NonShowering Background";
  if (inStr == "skcalib_16") returnString = "Upmu ThruGoing Showering Background";
  if (inStr == "skcalib_17") returnString = "TwoRing Pi0 Norm";
  if (inStr == "skcalib_18") returnString = "Decay Electron Tagging";
  if (inStr == "skcalib_19") returnString = "Cosmic Muon Subtraction";
  if (inStr == "skcalib_20") returnString = "Up Down Asym Energy Scale";

  return returnString;
}

double FindUpperLimit(int GroupID, TH2D* Hist, double Prior){
  double chi2_min = Hist->GetMinimum(); 
  int N_bins_x = Hist->GetXaxis()->GetNbins();
  int N_bins_y = Hist->GetYaxis()->GetNbins();
  double max = -999.;
  if(GroupID%2 == 0){
    //smear  
    for(int iY=0; iY<N_bins_y; iY++){
      for(int iX=0; iX<N_bins_x; iX++){
        double binContent = Hist->GetBinContent(iX+1, iY+1);
        if ( (binContent-chi2_min) > 2.30 ) continue; 
      
        double x = Hist->GetXaxis()->GetBinCenter(iX+1);
        if (x > max){
          max = x;
        }
      }
    } 
  }else{
    //shift
    for(int iX=0; iX<N_bins_x; iX++){
      for(int iY=0; iY<N_bins_y; iY++){
        double binContent = Hist->GetBinContent(iX+1, iY+1);
        if ( (binContent-chi2_min) > 2.30 ) continue; 
      
        double y = Hist->GetYaxis()->GetBinCenter(iY+1);
        if (y > max){
          max = y;
        }
      }
    } 
  }  
  double Upper = max-Prior;
  
  return Upper;
} 

double FindLowerLimit(int GroupID, TH2D* Hist, double Prior){
  double chi2_min = Hist->GetMinimum(); 
  int N_bins_x = Hist->GetXaxis()->GetNbins();
  int N_bins_y = Hist->GetYaxis()->GetNbins();
  double min = 999.;
  if(GroupID%2 == 0){
    //smear  
    for(int iY=0; iY<N_bins_y; iY++){
      for(int iX=0; iX<N_bins_x; iX++){
        double binContent = Hist->GetBinContent(iX+1, iY+1);
        if ( (binContent-chi2_min) > 2.30 ) continue; 
      
        double x = Hist->GetXaxis()->GetBinCenter(iX+1);
        if (x < min){
          min = x;
        }
      }
    } 
  }else{
    //shift
    for(int iX=0; iX<N_bins_x; iX++){
      for(int iY=0; iY<N_bins_y; iY++){
        double binContent = Hist->GetBinContent(iX+1, iY+1);
        if ( (binContent-chi2_min) > 2.30 ) continue; 
      
        double y = Hist->GetYaxis()->GetBinCenter(iY+1);
        if (y < min){
          min = y;
        }
      }
    } 
  }  
  double Lower = Prior-min;
  
  return Lower;
}

TString returnTopology(int topoIndex){
  TString returnTopo;

  if (topoIndex==0) returnTopo = "1e";
  if (topoIndex==1) returnTopo = "1mu";
  if (topoIndex==2) returnTopo = "1e+other";
  if (topoIndex==3) returnTopo = "1mu+other";
  if (topoIndex==4) returnTopo = "1pi0";
  if (topoIndex==5) returnTopo = "1pip+p";
  if (topoIndex==6) returnTopo = "other";

  return returnTopo;
}

TString returnTitle(int GroupID){
  TString plot_title;

  if (GroupID==0) plot_title = "#alpha^{0}_{jk}, ring counting";
  if (GroupID==1) plot_title = "#beta^{0}_{jk}, ring counting";
  if (GroupID==2) plot_title = "#alpha^{1}_{jk}, e/#mu PID";
  if (GroupID==3) plot_title = "#beta^{1}_{jk}, e/#mu PID";
  if (GroupID==4) plot_title = "#alpha^{2}_{jk}, e/#pi^{0} PID";
  if (GroupID==5) plot_title = "#beta^{2}_{jk}, e/#pi^{0} PID";
  if (GroupID==6) plot_title = "#alpha^{3}_{jk}, #mu / #pi^{+} PID";
  if (GroupID==7) plot_title = "#beta^{3}_{jk}, #mu / #pi^{+} PID";

  return plot_title;
}
