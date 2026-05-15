/*
DB: 19/01/2021
----------------------
PerformPostFitAnalysis

- Generates the posterior plots with Arithmetic/Gaussian/HPD analysis
- Generates the Covariance/Correlation matrices

Usage:
./PerformPostFitAnalysis.cpp outputFile burnin inputFile [Additional input files]

inputFile is the posterior chain from running the MCMC. It doesn't have to be 'hadd-ed' together as this exec can handle multiple chains.
Burnin is an integer value at which any step with stepvalue below that number is neglected

Output:
PDF with covariance systematic parameter posterior analysis where Name is given in config+"_1D.pdf"
ROOT file with posterior information, correlation matrices and other
----------------------

Mo: mulitple chains does not work with root5; run the script with one 'hadd-ed' input file that merges all the chains  
*/

#include <TH1D.h>
#include <TH2D.h>
#include <TError.h>
#include <TFile.h>
#include <TF1.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TVectorD.h>
#include <TLine.h>
#include <TLegend.h>
#include <TMatrixDSym.h>
#include <TMatrix.h>
#include <iomanip>

bool printPosteriors = true;

bool printArith = true;
bool printGaussian = true;
bool printHPD = true;

bool drawXsec = true;       // true
bool drawAtmFlux = false;    // true
bool drawT2KDet = false;     // false
bool drawSKCalib = false;    // true
bool drawATMPDDet = false;   // true
bool drawSKDetBeam = false;  // true
bool drawAtmSKDet = false;
bool drawCorrFDDet = false;  //Mo: corrfddet systs in jf2023; true
bool drawOsc = true; //Mo: oscillation params; true 

bool WriteChain = false;
int nBins = 70;

bool isOA2020 = false;

std::vector<std::string> XsecNames;
double ConvertPar(TString name);

void MakeCovariance(TFile* OutputFile, TChain* Chain, Int_t Burnin);
void GetArithmetic(TH1D * const hpost, double &mean, double &error);
void GetGaussian(TH1D *& hpost, TF1 *& gauss, double &central, double &error);
void GetHPD(TH1D * const post, double &central, double &error, double &error_pos, double &error_neg);
void GetXsecName(std::string XsecCov);
TString returnFancyString(std::string inStr);
void DrawPosteriors(TFile* OutputFile, TChain* Chain, int Burnin);

int main(int argc, char *argv[]) {
  gErrorIgnoreLevel = kFatal;

  if (argc<4) {
    std::cout << "Usage: PerformPostFitAnalysis.cpp outputFile burnin inputFile [Additional input files]" << std::endl;
    throw;
  }

  TString OutputFileName = argv[1];
  TString InputFileName = argv[3];

  int Burnin = atoi(argv[2]);
  std::cout << "Burnin:" << Burnin << std::endl;

  TChain* Chain = new TChain("posteriors","");
  for (int i=3;i<argc;i++) {
    std::cout << "Adding file:" << argv[i] << std::endl;
    Chain->Add(argv[i]);
  }

  TFile* OutputFile = new TFile(OutputFileName,"RECREATE");

  //============= - Grab settings from first input file
  TFile* InputFile = new TFile(InputFileName, "READ");

  /*
  TTree* Settings = (TTree*)InputFile->Get("Settings");
  Settings->SetDirectory(0);

  std::string *XsecInput = 0;
  Settings->SetBranchAddress("XsecCov",&XsecInput);
  Settings->GetEntry(0);

  std::string XsecCov = *XsecInput;
  std::cout << "Xsec found:" << XsecCov << std::endl;
  GetXsecName(XsecCov);
  */

  InputFile->Close();

  //============= - Generate posteriors and analyse
  //DrawPosteriors(OutputFile,Chain,Burnin);
  
  //============= - Generate covariance and correlation matrices
  MakeCovariance(OutputFile,Chain,Burnin);

  //============= - Write Settings tree
  OutputFile->cd();
  //Settings->Write();

  //============= - Write Burnin to file
  TVectorD BurninVec(1);
  BurninVec[0] = Burnin;
  BurninVec.Write("Burnin");

  //============= -  Write chain to file
  if (WriteChain) Chain->Write();

  OutputFile->Close();

}

void DrawPosteriors(TFile* OutputFile, TChain* Chain, Int_t Burnin) {


  std::string PDFOutputName = (std::string(OutputFile->GetName()).substr(0, std::string(OutputFile->GetName()).find(".root")))+"_1D.pdf";
  TCanvas* Canv;
  if (printPosteriors) {
    Canv = new TCanvas("PostCanv","");
    Canv->Print((PDFOutputName+"[").c_str());
  }

  int XsecIndex;

  TObjArray* brlis = (TObjArray*)Chain->GetListOfBranches();
  int nbr = brlis->GetEntries();

  bool drawPosterior;
  
  OutputFile->cd();
  TTree* TotalOutputTree = new TTree("TotalPosteriors","");
  TTree* XsecOutputTree = new TTree("XsecPosteriors","");
  TTree* AtmFluxOutputTree = new TTree("AtmFluxPosteriors","");
  TTree* T2KDetOutputTree = new TTree("T2KDetPosteriors","");
  TTree* SKCalibOutputTree = new TTree("SKCalibPosteriors","");
  TTree* ATMPDDetOutputTree = new TTree("ATMPDDetPosteriors","");
  TTree* SKDetBeamOutputTree = new TTree("SKDetBeamPosteriors","");
  TTree* AtmSKDetOutputTree = new TTree("AtmSKDetPosteriors","");
  TTree* CorrFDDetOutputTree = new TTree("CorrFDDetPosteriors","");  // corrfddet systs in jf2023
  TTree* OscOutputTree = new TTree("OscPosteriors","");  // oscillation params 

  TString bname;
  std::string FormattedBname;

  TH1D* hPost_Arith;
  TH1D* hPost_Gauss;
  TH1D* hPost_HPD;
  TH1D* hPost;

  double Arith_Central;
  double Arith_Error;

  double Gauss_Central;
  double Gauss_Error;

  double HPD_Central;
  double HPD_Error;
  double HPD_Error_Upper;
  double HPD_Error_Lower;

  int ParamType;

  TotalOutputTree->Branch("ParamName",&FormattedBname);
  TotalOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  TotalOutputTree->Branch("ArithmeticError",&Arith_Error);
  TotalOutputTree->Branch("GaussCentral",&Gauss_Central);
  TotalOutputTree->Branch("GaussError",&Gauss_Error);
  TotalOutputTree->Branch("HPDCentral",&HPD_Central);
  TotalOutputTree->Branch("HPDError",&HPD_Error);
  TotalOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  TotalOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  XsecOutputTree->Branch("ParamName",&FormattedBname);
  XsecOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  XsecOutputTree->Branch("ArithmeticError",&Arith_Error);
  XsecOutputTree->Branch("GaussCentral",&Gauss_Central);
  XsecOutputTree->Branch("GaussError",&Gauss_Error);
  XsecOutputTree->Branch("HPDCentral",&HPD_Central);
  XsecOutputTree->Branch("HPDError",&HPD_Error);
  XsecOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  XsecOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  AtmFluxOutputTree->Branch("ParamName",&FormattedBname);
  AtmFluxOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  AtmFluxOutputTree->Branch("ArithmeticError",&Arith_Error);
  AtmFluxOutputTree->Branch("GaussCentral",&Gauss_Central);
  AtmFluxOutputTree->Branch("GaussError",&Gauss_Error);
  AtmFluxOutputTree->Branch("HPDCentral",&HPD_Central);
  AtmFluxOutputTree->Branch("HPDError",&HPD_Error);
  AtmFluxOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  AtmFluxOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  T2KDetOutputTree->Branch("ParamName",&FormattedBname);
  T2KDetOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  T2KDetOutputTree->Branch("ArithmeticError",&Arith_Error);
  T2KDetOutputTree->Branch("GaussCentral",&Gauss_Central);
  T2KDetOutputTree->Branch("GaussError",&Gauss_Error);
  T2KDetOutputTree->Branch("HPDCentral",&HPD_Central);
  T2KDetOutputTree->Branch("HPDError",&HPD_Error);
  T2KDetOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  T2KDetOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  SKCalibOutputTree->Branch("ParamName",&FormattedBname);
  SKCalibOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  SKCalibOutputTree->Branch("ArithmeticError",&Arith_Error);
  SKCalibOutputTree->Branch("GaussCentral",&Gauss_Central);
  SKCalibOutputTree->Branch("GaussError",&Gauss_Error);
  SKCalibOutputTree->Branch("HPDCentral",&HPD_Central);
  SKCalibOutputTree->Branch("HPDError",&HPD_Error);
  SKCalibOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  SKCalibOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  ATMPDDetOutputTree->Branch("ParamName",&FormattedBname);
  ATMPDDetOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  ATMPDDetOutputTree->Branch("ArithmeticError",&Arith_Error);
  ATMPDDetOutputTree->Branch("GaussCentral",&Gauss_Central);
  ATMPDDetOutputTree->Branch("GaussError",&Gauss_Error);
  ATMPDDetOutputTree->Branch("HPDCentral",&HPD_Central);
  ATMPDDetOutputTree->Branch("HPDError",&HPD_Error);
  ATMPDDetOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  ATMPDDetOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  SKDetBeamOutputTree->Branch("ParamName",&FormattedBname);
  SKDetBeamOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  SKDetBeamOutputTree->Branch("ArithmeticError",&Arith_Error);
  SKDetBeamOutputTree->Branch("GaussCentral",&Gauss_Central);
  SKDetBeamOutputTree->Branch("GaussError",&Gauss_Error);
  SKDetBeamOutputTree->Branch("HPDCentral",&HPD_Central);
  SKDetBeamOutputTree->Branch("HPDError",&HPD_Error);
  SKDetBeamOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  SKDetBeamOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  AtmSKDetOutputTree->Branch("ParamName",&FormattedBname);
  AtmSKDetOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  AtmSKDetOutputTree->Branch("ArithmeticError",&Arith_Error);
  AtmSKDetOutputTree->Branch("GaussCentral",&Gauss_Central);
  AtmSKDetOutputTree->Branch("GaussError",&Gauss_Error);
  AtmSKDetOutputTree->Branch("HPDCentral",&HPD_Central);
  AtmSKDetOutputTree->Branch("HPDError",&HPD_Error);
  AtmSKDetOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  AtmSKDetOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);
  // Mo: corrfddet syts
  CorrFDDetOutputTree->Branch("ParamName",&FormattedBname);
  CorrFDDetOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  CorrFDDetOutputTree->Branch("ArithmeticError",&Arith_Error);
  CorrFDDetOutputTree->Branch("GaussCentral",&Gauss_Central);
  CorrFDDetOutputTree->Branch("GaussError",&Gauss_Error);
  CorrFDDetOutputTree->Branch("HPDCentral",&HPD_Central);
  CorrFDDetOutputTree->Branch("HPDError",&HPD_Error);
  CorrFDDetOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  CorrFDDetOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);
  // Mo: oscillation params
  OscOutputTree->Branch("ParamName",&FormattedBname);
  OscOutputTree->Branch("ArithmeticCentral",&Arith_Central);
  OscOutputTree->Branch("ArithmeticError",&Arith_Error);
  OscOutputTree->Branch("GaussCentral",&Gauss_Central);
  OscOutputTree->Branch("GaussError",&Gauss_Error);
  OscOutputTree->Branch("HPDCentral",&HPD_Central);
  OscOutputTree->Branch("HPDError",&HPD_Error);
  OscOutputTree->Branch("HPDError_Lower",&HPD_Error_Lower);
  OscOutputTree->Branch("HPDError_Upper",&HPD_Error_Upper);

  OutputFile->mkdir("DrawnPosteriors");
  OutputFile->cd("DrawnPosteriors");

  TF1 *Gauss_TF1 = new TF1("gauss","[0]/sqrt(2.0*3.14159)/[2]*TMath::Exp(-0.5*pow(x-[1],2)/[2]/[2])",-5,5);
  
  std::cout << "-----------------------------------" << std::endl;
  std::cout << "Calculating Posterior Errors" << std::endl;
  std::cout << "-----------------------------------" << std::endl;

  std::cout << "PDFOutputName:" << PDFOutputName << std::endl;

  std::cout << std::setw(35) << "Parameter" << " | " << std::setw(14) << "Arith Cent." << " | " << std::setw(14) << "Arith Error" << " | " << std::setw(14) << "Gauss Cent." << " | " << std::setw(14) << "Gauss Error" << " | " << std::setw(14) << "HPD Cent." << " | " << std::setw(14) << "HPD Error" << " | " << std::setw(14) << "HPD Error Low" << " | " << std::setw(14) << "HPD Error Up" << std::endl;
  std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;

  for (int i = 0; i < nbr; i++) {
    drawPosterior = false;
    
    bname = brlis->At(i)->GetName();    
    FormattedBname = bname;

    if((bname.BeginsWith("xsec_") || bname.BeginsWith("b_")) && drawXsec) {
      ParamType = 0;
      /*
      XsecIndex = atoi((FormattedBname.substr(FormattedBname.find("_")+1)).c_str());
      FormattedBname = XsecNames[XsecIndex];
      */
      drawPosterior = true;
    }
    else if(bname.BeginsWith("atmflux_") && drawAtmFlux) {
      ParamType = 1;
      drawPosterior = true;
    }
    else if(bname.BeginsWith("skcalib_") && drawSKCalib) {
      ParamType = 2;
      drawPosterior = true;
    }
    else if(bname.BeginsWith("atmpddet_") && drawATMPDDet) {
      ParamType = 3;
      drawPosterior = true;
    }
    else if(bname.BeginsWith("SKDetBeam_") && drawSKDetBeam) {
      ParamType = 4;
      drawPosterior = true;
    }
    else if(bname.BeginsWith("AtmSKDet_") && drawAtmSKDet) {
      ParamType = 5;
      drawPosterior = true;
    }
    else if(bname.BeginsWith("skd_joint_") && drawT2KDet) {
      ParamType = 6;
      drawPosterior = true;
    }
    else if(bname.BeginsWith("corrfddet_") && drawCorrFDDet ){
      ParamType = 7;
      drawPosterior = true;
    }
    else if((bname.BeginsWith("sin2th_") || bname.BeginsWith("delm2_") || bname.BeginsWith("delta_")) && drawOsc ){
      ParamType = 8;
      drawPosterior = true;
    }

    if (bname.Contains("prop")) {
      drawPosterior = false;
    }

    if (drawPosterior) {

      hPost_Arith = new TH1D(bname,returnFancyString(FormattedBname)+";"+returnFancyString(FormattedBname),nBins,Chain->GetMinimum(bname),Chain->GetMaximum(bname));
     
      double Nom = ConvertPar(bname);
      Chain->Project(bname,bname,Form("step >= %i",Burnin));
      hPost_Arith->Smooth();

      hPost_Arith->SetName(bname+"_Arith");
      hPost_Gauss = (TH1D*)hPost_Arith->Clone(bname+"_Gauss");
      hPost_HPD = (TH1D*)hPost_Arith->Clone(bname+"_HPD");

      GetArithmetic(hPost_Arith,Arith_Central,Arith_Error);
      GetGaussian(hPost_Gauss,Gauss_TF1,Gauss_Central,Gauss_Error);
      GetHPD(hPost_HPD,HPD_Central,HPD_Error,HPD_Error_Lower,HPD_Error_Upper);

      hPost = (TH1D*)hPost_Arith->Clone(FormattedBname.c_str());
      hPost->Write();

      if (isOA2020) {
	Arith_Central *= Nom;
	Arith_Error *= Nom;
	Gauss_Central *= Nom;
	Gauss_Error *= Nom;
	HPD_Central *= Nom;
	HPD_Error *= Nom;
	HPD_Error_Lower *= Nom;
	HPD_Error_Upper *= Nom;
      }

      std::cout << std::setw(35) << FormattedBname << " | " << std::setw(14) << Arith_Central << " | " << std::setw(14) << Arith_Error << " | " << std::setw(14) << Gauss_Central << " | " << std::setw(14) << Gauss_Error << " | " << std::setw(14) << HPD_Central << " | " << std::setw(14) << HPD_Error << " | " << std::setw(14) << HPD_Error_Lower << " | " << std::setw(14) << HPD_Error_Upper << std::endl;

      if (ParamType==0) {
	XsecOutputTree->Fill();
      }
      else if (ParamType==1) {
	AtmFluxOutputTree->Fill();
      }
      else if (ParamType==2) {
	SKCalibOutputTree->Fill();
      }
      else if (ParamType==3) {
	ATMPDDetOutputTree->Fill();
      }
      else if (ParamType==4) {
	SKDetBeamOutputTree->Fill();
      }
      else if (ParamType==5) {
	AtmSKDetOutputTree->Fill();
      } 
      else if (ParamType==6) {
	T2KDetOutputTree->Fill();
      } 
      else if (ParamType==7){
        CorrFDDetOutputTree->Fill();
      }
      else if (ParamType==8){
        OscOutputTree->Fill();
      }
      else {
	std::cout << "Unknown ParamType. Quitting.." << std::endl;
	throw;
      }
      TotalOutputTree->Fill();

      if (printPosteriors) {
	hPost_Arith->SetMaximum(1.4*hPost_Arith->GetMaximum());
	hPost_Gauss->SetMaximum(1.4*hPost_Gauss->GetMaximum());
	hPost_HPD->SetMaximum(1.4*hPost_HPD->GetMaximum());

	hPost_Arith->SetLineColor(kBlue);
	hPost_Gauss->SetLineColor(kRed);
	hPost_HPD->SetLineColor(kGreen);

	TLegend *Legend = new TLegend(0.65, 0.6, 0.89, 0.89);
	Legend->SetTextSize(0.02);
	Legend->AddEntry(hPost_Arith, Form("#splitline{Arithmetic}{#mu = %.2f, #sigma = %.2f}", Arith_Central, Arith_Error), "l");
	Legend->AddEntry(hPost_Gauss, Form("#splitline{Gaussian}{#mu = %.2f, #sigma = %.2f}", Gauss_Central, Gauss_Error), "l");
	Legend->AddEntry(hPost_HPD, Form("#splitline{HPD}{#mu = %.2f, #sigma = %.2f (+%.2f-%.2f)}", HPD_Central, HPD_Error, HPD_Error_Upper, HPD_Error_Lower), "l");
	Legend->SetLineColor(0);
	Legend->SetLineStyle(0);
	Legend->SetFillColor(0);
	Legend->SetFillStyle(0);
	Legend->SetBorderSize(0);

	hPost_Arith->SetStats(false);
	hPost_Arith->Draw();
	if (printArith) {
	  TLine* ArithLineErrorLow = new TLine(Arith_Central-Arith_Error,0,Arith_Central-Arith_Error,hPost_Arith->GetMaximum());
	  TLine* ArithLineCentral = new TLine(Arith_Central,0,Arith_Central,hPost_Arith->GetMaximum());
	  TLine* ArithLineErrorHigh = new TLine(Arith_Central+Arith_Error,0,Arith_Central+Arith_Error,hPost_Arith->GetMaximum());
	  TLine* ArithLineHorizontal = new TLine(Arith_Central-Arith_Error,hPost_Arith->GetMaximum()*(5./8.),Arith_Central+Arith_Error,hPost_Arith->GetMaximum()*(5./8.));
	  ArithLineErrorLow->SetLineColor(kBlue);
	  ArithLineCentral->SetLineColor(kBlue);
	  ArithLineErrorHigh->SetLineColor(kBlue);
	  ArithLineHorizontal->SetLineColor(kBlue);
	  ArithLineErrorLow->SetLineStyle(7);
	  ArithLineErrorHigh->SetLineStyle(7);
	  ArithLineHorizontal->SetLineStyle(7);
	  ArithLineErrorLow->Draw("SAME");
	  ArithLineCentral->Draw("SAME");
	  ArithLineErrorHigh->Draw("SAME");
	  ArithLineHorizontal->Draw("SAME");
	}
	if (printGaussian) {
	  Gauss_TF1->Draw("Same");
	  TLine* GaussLineErrorLow = new TLine(Gauss_Central-Gauss_Error,0,Gauss_Central-Gauss_Error,hPost_Gauss->GetMaximum());
	  TLine* GaussLineCentral = new TLine(Gauss_Central,0,Gauss_Central,hPost_Gauss->GetMaximum());
	  TLine* GaussLineErrorHigh = new TLine(Gauss_Central+Gauss_Error,0,Gauss_Central+Gauss_Error,hPost_Gauss->GetMaximum());
	  TLine* GaussLineHorizontal = new TLine(Gauss_Central-Gauss_Error,hPost_Gauss->GetMaximum()*(6./8.),Gauss_Central+Gauss_Error,hPost_Gauss->GetMaximum()*(6./8.));
	  GaussLineErrorLow->SetLineColor(kRed);
	  GaussLineCentral->SetLineColor(kRed);
	  GaussLineErrorHigh->SetLineColor(kRed);
	  GaussLineHorizontal->SetLineColor(kRed);
	  GaussLineErrorLow->SetLineStyle(7);
	  GaussLineErrorHigh->SetLineStyle(7);
	  GaussLineHorizontal->SetLineStyle(7);
	  GaussLineErrorLow->Draw("SAME");
	  GaussLineCentral->Draw("SAME");
	  GaussLineErrorHigh->Draw("SAME");
	  GaussLineHorizontal->Draw("SAME");
	}
	if (printHPD) {
	  TLine* HPDLineErrorLow = new TLine(HPD_Central-HPD_Error_Lower,0,HPD_Central-HPD_Error_Lower,hPost_HPD->GetMaximum());
	  TLine* HPDLineCentral = new TLine(HPD_Central,0,HPD_Central,hPost_HPD->GetMaximum());
	  TLine* HPDLineErrorHigh = new TLine(HPD_Central+HPD_Error_Upper,0,HPD_Central+HPD_Error_Upper,hPost_HPD->GetMaximum());
	  TLine* HPDLineHorizontal = new TLine(HPD_Central-HPD_Error_Lower,hPost_HPD->GetMaximum()*(7./8.),HPD_Central+HPD_Error_Upper,hPost_HPD->GetMaximum()*(7./8.));
	  HPDLineErrorLow->SetLineColor(kGreen);
	  HPDLineCentral->SetLineColor(kGreen);
	  HPDLineErrorHigh->SetLineColor(kGreen);
	  HPDLineHorizontal->SetLineColor(kGreen);
	  HPDLineErrorLow->SetLineStyle(7);
	  HPDLineErrorHigh->SetLineStyle(7);
	  HPDLineHorizontal->SetLineStyle(7);
	  HPDLineErrorLow->Draw("SAME");
	  HPDLineCentral->Draw("SAME");
	  HPDLineErrorHigh->Draw("SAME");
	  HPDLineHorizontal->Draw("SAME");
	}
	
	Legend->Draw("SAME");

	Canv->Print(PDFOutputName.c_str());
      }
    }

  }

  OutputFile->cd();
  TotalOutputTree->Write();
  XsecOutputTree->Write();  
  AtmFluxOutputTree->Write();  
  T2KDetOutputTree->Write();  
  SKCalibOutputTree->Write();
  ATMPDDetOutputTree->Write();
  SKDetBeamOutputTree->Write();
  AtmSKDetOutputTree->Write();
  CorrFDDetOutputTree->Write();
  OscOutputTree->Write();

  if (printPosteriors) {
    Canv->Print((PDFOutputName+"]").c_str());
  }

  return;
}

// Get the highest posterior density from a TH1D
void GetHPD(TH1D * const hpost, double &central, double &error, double &error_pos, double &error_neg) {

  // Get the bin which has the largest posterior density
  int MaxBin = hpost->GetMaximumBin();
  // And it's value
  double peakval = hpost->GetBinCenter(MaxBin);


  // The total integral of the posterior
  double integral = hpost->Integral();


  // Keep count of how much area we're covering
  double sum = 0.0;


  // Counter for current bin
  int CurrBin = MaxBin;
  while (sum/integral < 0.6827/2.0 && CurrBin < hpost->GetNbinsX()+1) {
    sum += hpost->GetBinContent(CurrBin);
    CurrBin++;
  }
  double sigma_p = fabs(hpost->GetBinCenter(MaxBin)-hpost->GetBinCenter(CurrBin));
  // Reset the sum
  sum = 0.0;


  // Reset the bin counter
  CurrBin = MaxBin;
  // Counter for current bin
  while (sum/integral < 0.6827/2.0 && CurrBin >= 0) {
    sum += hpost->GetBinContent(CurrBin);
    CurrBin--;
  }
  double sigma_m = fabs(hpost->GetBinCenter(CurrBin)-hpost->GetBinCenter(MaxBin));


  // Now do the double sided HPD
  sum = 0.0;
  int LowBin = MaxBin-1;
  int HighBin = MaxBin+1;
  double LowCon = 0.0;
  double HighCon = 0.0;
  while (sum/integral < 0.6827 && LowBin >= 0 && HighBin < hpost->GetNbinsX()+1) {


    // Get the slice
    LowCon = hpost->GetBinContent(LowBin);
    HighCon = hpost->GetBinContent(HighBin);


    // If we're on the last slice and the lower contour is larger than the upper
    if ((sum+LowCon+HighCon)/integral > 0.6827 && LowCon > HighCon) {
      sum += LowCon;
      break;
      // If we're on the last slice and the upper contour is larger than the lower
    } else if ((sum+LowCon+HighCon)/integral > 0.6827 && HighCon >= LowCon) {
      sum += HighCon;
      break;
    } else {
      sum += LowCon + HighCon;
    }


    LowBin--;
    HighBin++;
  }


  double sigma_hpd = 0.0;
  if (LowCon > HighCon) {
    sigma_hpd = fabs(hpost->GetBinCenter(LowBin)-hpost->GetBinCenter(MaxBin));
  } else {
    sigma_hpd = fabs(hpost->GetBinCenter(HighBin)-hpost->GetBinCenter(MaxBin));
  }


  central = peakval;
  error = sigma_hpd;
  error_pos = sigma_p;
  error_neg = sigma_m;


}


// **************************
// Get the mean and RMS of a 1D posterior
void GetArithmetic(TH1D * const hpost, double &mean, double &error) {
  // **************************
  mean = hpost->GetMean();
  error = hpost->GetRMS();
}


// **************************
// Get Gaussian characteristics
void GetGaussian(TH1D *& hpost, TF1 *& gauss, double &central, double &error) {
  // **************************


  double mean = hpost->GetMean();
  double err = hpost->GetRMS();
  double peakval = hpost->GetBinCenter(hpost->GetMaximumBin());


  // Set the range for the Gaussian fit
  gauss->SetRange(mean - 1.5*err , mean + 1.5*err);
  // Set the starting parameters close to RMS and peaks of the histograms
  gauss->SetParameters(hpost->GetMaximum()*err*sqrt(2*3.14), peakval, err);


  // Perform the fit
  hpost->Fit(gauss->GetName(),"Rq");
  hpost->SetStats(0);


  central = gauss->GetParameter(1);
  error = gauss->GetParameter(2);


}

// **************************
// Just a converter from xsec_i to a real parameter name
// Use this when the conversion wasn't written to file :(
void GetXsecName(std::string XsecCov) {
  // **************************

  // Open the input covariance file to get the pre-fit error
  TFile* covFile = new TFile(XsecCov.c_str(), "OPEN");
  covFile->cd();

  // Get the array of cross-section parameter names
  TObjArray *xsec_param_names = (TObjArray*)(covFile->Get("xsec_param_names"));
  XsecNames.resize(xsec_param_names->GetEntries());

  for (int i=0;i<xsec_param_names->GetEntries();i++) {
    XsecNames[i] = std::string(((TObjString*)xsec_param_names->At(i))->GetString());
  }

  covFile->Close();

}

// *********************
// Make the post-fit covariance matrix in all dimensions
void MakeCovariance(TFile* OutputFile, TChain* Chain, Int_t Burnin) {
  // *********************

  TObjArray* brlis = (TObjArray*)Chain->GetListOfBranches();
  int nbr = brlis->GetEntries();
  
  std::vector<TString> BranchNames;  
  std::vector<TString> FancyBranchNames;
  TString bname;
  std::string bname_str;
  bool drawPosterior;

  int XsecIndex;
  TString FancyName;

  int nXsecParams = 0;
  int nAtmFluxParams = 0;
  int nT2KDetParams = 0;
  int nSKCalibParams = 0;
  int nATMPDDetParams = 0;
  int nSKDetBeamParams = 0;
  int nAtmSKDetParams = 0;
  int nCorrFDDetParams = 0;
  int nOscParams = 0;

  int XsecMinBin = -1;
  int AtmFluxMinBin = -1;
  int T2KDetMinBin = -1;
  int SKCalibMinBin = -1;
  int ATMPDDetMinBin = -1;
  int SKDetBeamMinBin = -1;
  int AtmSKDetMinBin = -1;
  int CorrFDDetMinBin = -1;
  int OscMinBin = -1;

  int XsecMaxBin = -1;
  int AtmFluxMaxBin = -1;
  int T2KDetMaxBin = -1;
  int SKCalibMaxBin = -1;
  int ATMPDDetMaxBin = -1;
  int SKDetBeamMaxBin = -1;
  int AtmSKDetMaxBin = -1;
  int CorrFDDetMaxBin = -1;
  int OscMaxBin = -1;

  int counter = 0;

  for (int i = 0; i < nbr; i++) {
    drawPosterior = false;

    bname = brlis->At(i)->GetName();
    bname_str = bname.Data();
    FancyName = bname;

    if(bname.BeginsWith("xsec_") && drawXsec) {
      if (XsecMinBin==-1) {
	XsecMinBin = counter;
      }
      XsecMaxBin = counter;

      XsecIndex = atoi((bname_str.substr(bname_str.find("_")+1)).c_str());
      //skip beam flux 
      if (XsecIndex > 88) continue;
      //FancyName = returnFancyString(XsecNames[XsecIndex]);
      drawPosterior = true;
      nXsecParams += 1;
    }
    else if(bname.BeginsWith("atmflux_") && drawAtmFlux) {
      if (AtmFluxMinBin==-1) {
	AtmFluxMinBin = counter;
      }
      AtmFluxMaxBin = counter;

      drawPosterior = true;
      nAtmFluxParams += 1;
    }
    else if(bname.BeginsWith("skd_joint_") && drawT2KDet) {
      if (T2KDetMinBin==-1) {
	T2KDetMinBin = counter;
      }
      T2KDetMaxBin = counter;

      drawPosterior = true;
      nT2KDetParams += 1;
    }
    else if(bname.BeginsWith("skcalib_") && drawSKCalib) {
      if (SKCalibMinBin==-1) {
	SKCalibMinBin = counter;
      }
      SKCalibMaxBin = counter;

      drawPosterior = true;
      nSKCalibParams += 1;
    }
    else if(bname.BeginsWith("atmpddet_") && drawATMPDDet) {
      if (ATMPDDetMinBin==-1) {
	ATMPDDetMinBin = counter;
      }
      ATMPDDetMaxBin = counter;

      drawPosterior = true;
      nATMPDDetParams += 1;
    }
    else if(bname.BeginsWith("SKDetBeam_") && drawSKDetBeam) {
      if (SKDetBeamMinBin==-1) {
	SKDetBeamMinBin = counter;
      }
      SKDetBeamMaxBin = counter;

      drawPosterior = true;
      nSKDetBeamParams += 1;
    }
    else if(bname.BeginsWith("AtmSKDet_") && drawAtmSKDet) {
      if (AtmSKDetMinBin==-1) {
	AtmSKDetMinBin = counter;
      }
      AtmSKDetMaxBin = counter;

      drawPosterior = true;
      AtmSKDetMinBin += 1;
    }
    else if(bname.BeginsWith("corrfddet_") && drawCorrFDDet) {
      if (CorrFDDetMinBin==-1) {
        CorrFDDetMinBin = counter;
      }
      CorrFDDetMaxBin = counter;

      drawPosterior = true;
      nCorrFDDetParams += 1;
    }else if((bname.BeginsWith("sin2th_") || bname.BeginsWith("delm2_") || bname.BeginsWith("delta_")) && drawOsc ){
     if (OscMinBin==-1){
       OscMinBin = counter;
     }
     OscMaxBin = counter;

     drawPosterior = true;
     nOscParams += 1;
    }

    if (bname.Contains("prop")) {
      drawPosterior = false;
    }

    if (drawPosterior) {
      BranchNames.push_back(bname);
      FancyBranchNames.push_back(FancyName);
      counter += 1;
    }
  }

  int nDraw = BranchNames.size();  
  TMatrixDSym* Covariance = new TMatrixDSym(nDraw);
  TMatrixDSym* Correlation = new TMatrixDSym(nDraw);

  std::cout << "-----------------------------------" << std::endl;
  std::cout << "Calculating Covariance/Correlations" << std::endl;
  std::cout << "-----------------------------------" << std::endl;

  OutputFile->mkdir("CorrelationPlots");
  OutputFile->cd("CorrelationPlots");

  for (int iParam = 0; iParam < nDraw; iParam++) {
    
    if (iParam % (nDraw/5) == 0) {
      std::cout << "  " << iParam << "/" << nDraw << " (" << int((double(iParam)/double(nDraw)*100.0))+1 << "%)" << std::endl;
    }
 
    // Loop over the other parameters to get the correlations
    for (int jParam = 0; jParam <= iParam; jParam++) {

      if (jParam == iParam) {
	TH1D* hPost1D = new TH1D(BranchNames[iParam],BranchNames[iParam],nBins,Chain->GetMinimum(BranchNames[iParam]),Chain->GetMaximum(BranchNames[iParam]));

	Chain->Project(BranchNames[iParam],BranchNames[iParam],Form("step >= %i",Burnin));
	hPost1D->Smooth();	

	double RMS = hPost1D->GetRMS();

	(*Covariance)(jParam,iParam) = RMS*RMS;
	(*Correlation)(iParam,jParam) = 1.0;
      }
      else {
 	
	// The draw which we want to perform
	TString DrawMe = BranchNames[jParam]+":"+BranchNames[iParam];
        std::cout << "drawing " << DrawMe << std::endl;	
	// TH2F to hold the Correlation 
	TH2D *hPost2D = new TH2D(DrawMe, DrawMe, nBins, Chain->GetMinimum(BranchNames[iParam]), Chain->GetMaximum(BranchNames[iParam]), nBins, Chain->GetMinimum(BranchNames[jParam]), Chain->GetMaximum(BranchNames[jParam]));
	
	hPost2D->SetMinimum(0);
	hPost2D->GetXaxis()->SetTitle(BranchNames[iParam]);
	hPost2D->GetYaxis()->SetTitle(BranchNames[jParam]);
	hPost2D->GetZaxis()->SetTitle("Steps");
	
	// The draw command we want, i.e. draw param j vs param i
	Chain->Project(DrawMe, DrawMe, Form("step >= %i",Burnin));
	
	// Get the Covariance for these two parameters
	(*Covariance)(iParam,jParam) = hPost2D->GetCovariance();
	(*Covariance)(jParam,iParam) = (*Covariance)(iParam,jParam);
	
	(*Correlation)(iParam,jParam) = hPost2D->GetCorrelationFactor();
	(*Correlation)(jParam,iParam) = (*Correlation)(iParam,jParam);

	hPost2D->GetXaxis()->SetTitle(FancyBranchNames[iParam]);
	hPost2D->GetYaxis()->SetTitle(FancyBranchNames[jParam]);
	hPost2D->Write();

	delete hPost2D;
      }
    } // End j loop
  } // End i loop

  OutputFile->cd();
  OutputFile->mkdir("Matrices");
  OutputFile->cd("Matrices");

  if (XsecMinBin != -1) {
    TMatrixD XsecCov(Covariance->GetSub(XsecMinBin,XsecMaxBin-1,XsecMinBin,XsecMaxBin-1));
    XsecCov.Write("Xsec_Covariance");

    TMatrixD XsecCor(Correlation->GetSub(XsecMinBin,XsecMaxBin-1,XsecMinBin,XsecMaxBin-1));
    XsecCor.Write("Xsec_Correlation");
  }

  if (AtmFluxMinBin != -1) {
    TMatrixDSym AtmFluxCov(Covariance->GetSub(AtmFluxMinBin,AtmFluxMaxBin-1,AtmFluxMinBin,AtmFluxMaxBin-1));
    AtmFluxCov.Write("AtmFlux_Covariance");

    TMatrixDSym AtmFluxCor(Correlation->GetSub(AtmFluxMinBin,AtmFluxMaxBin-1,AtmFluxMinBin,AtmFluxMaxBin-1));
    AtmFluxCor.Write("AtmFlux_Correlation");
  }

  if (T2KDetMinBin != -1) {
    TMatrixDSym T2KDetCov(Covariance->GetSub(T2KDetMinBin,T2KDetMaxBin-1,T2KDetMinBin,T2KDetMaxBin-1));
    T2KDetCov.Write("T2KDet_Covariance");

    TMatrixDSym T2KDetCor(Correlation->GetSub(T2KDetMinBin,T2KDetMaxBin-1,T2KDetMinBin,T2KDetMaxBin-1));
    T2KDetCor.Write("T2KDet_Correlation");
  }

  if (SKCalibMinBin != -1) {
    TMatrixDSym SKCalibCov(Covariance->GetSub(SKCalibMinBin,SKCalibMaxBin-1,SKCalibMinBin,SKCalibMaxBin-1));
    SKCalibCov.Write("SKCalib_Covariance");

    TMatrixDSym SKCalibCor(Correlation->GetSub(SKCalibMinBin,SKCalibMaxBin-1,SKCalibMinBin,SKCalibMaxBin-1));
    SKCalibCor.Write("SKCalib_Correlation");
  }

  if (ATMPDDetMinBin != -1) {
    TMatrixDSym ATMPDDetCov(Covariance->GetSub(ATMPDDetMinBin,ATMPDDetMaxBin-1,ATMPDDetMinBin,ATMPDDetMaxBin-1));
    ATMPDDetCov.Write("ATMPDDet_Covariance");

    TMatrixDSym ATMPDDetCor(Correlation->GetSub(ATMPDDetMinBin,ATMPDDetMaxBin-1,ATMPDDetMinBin,ATMPDDetMaxBin-1));
    ATMPDDetCor.Write("ATMPDDet_Correlation");
  }

  if (SKDetBeamMinBin != -1) {
    TMatrixDSym SKDetBeamCov(Covariance->GetSub(SKDetBeamMinBin,SKDetBeamMaxBin-1,SKDetBeamMinBin,SKDetBeamMaxBin-1));
    SKDetBeamCov.Write("SKDetBeam_Covariance");

    TMatrixDSym SKDetBeamCor(Correlation->GetSub(SKDetBeamMinBin,SKDetBeamMaxBin-1,SKDetBeamMinBin,SKDetBeamMaxBin-1));
    SKDetBeamCor.Write("SKDetBeam_Correlation");
  }

  if (AtmSKDetMinBin != -1) {
    TMatrixDSym AtmSKDetCov(Covariance->GetSub(AtmSKDetMinBin,AtmSKDetMaxBin-1,AtmSKDetMinBin,AtmSKDetMaxBin-1));
    AtmSKDetCov.Write("AtmSKDet_Covariance");

    TMatrixDSym AtmSKDetCor(Correlation->GetSub(AtmSKDetMinBin,AtmSKDetMaxBin-1,AtmSKDetMinBin,AtmSKDetMaxBin-1));
    AtmSKDetCor.Write("AtmSKDet_Correlation");
  }

  if (CorrFDDetMinBin != -1) {
    TMatrixDSym CorrFDDetCov(Covariance->GetSub(CorrFDDetMinBin,CorrFDDetMaxBin-1,CorrFDDetMinBin,CorrFDDetMaxBin-1));
    CorrFDDetCov.Write("CorrFDDet_Covariance");

    TMatrixDSym CorrFDDetCor(Correlation->GetSub(CorrFDDetMinBin,CorrFDDetMaxBin-1,CorrFDDetMinBin,CorrFDDetMaxBin-1));
    CorrFDDetCor.Write("CorrFDDet_Correlation");
  }

  if (OscMinBin != -1) {
    TMatrixDSym OscCov(Covariance->GetSub(OscMinBin,OscMaxBin-1,OscMinBin,OscMaxBin-1));
    OscCov.Write("Osc_Covariance");

    TMatrixDSym OscCor(Correlation->GetSub(OscMinBin,OscMaxBin-1,OscMinBin,OscMaxBin-1));
    OscCov.Write("Osc_Correlation");
  }

  TH2D* CovarianceTH2D = new TH2D(*Covariance);
  TH2D* CorrelationTH2D = new TH2D(*Correlation);
  
  CovarianceTH2D->SetStats(false);
  CorrelationTH2D->SetStats(false);

  if (XsecMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(XsecMinBin+1+int(nXsecParams/2),"Xsec");
  if (XsecMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(XsecMinBin+1+int(nXsecParams/2),"Xsec");
  if (XsecMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(XsecMinBin+1+int(nXsecParams/2),"Xsec");
  if (XsecMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(XsecMinBin+1+int(nXsecParams/2),"Xsec");

  if (AtmFluxMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(AtmFluxMinBin+1+int(nAtmFluxParams/2),"AtmFlux");
  if (AtmFluxMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(AtmFluxMinBin+1+int(nAtmFluxParams/2),"AtmFlux");
  if (AtmFluxMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(AtmFluxMinBin+1+int(nAtmFluxParams/2),"AtmFlux");
  if (AtmFluxMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(AtmFluxMinBin+1+int(nAtmFluxParams/2),"AtmFlux");

  if (T2KDetMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(T2KDetMinBin+1+int(nT2KDetParams/2),"T2KDet");
  if (T2KDetMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(T2KDetMinBin+1+int(nT2KDetParams/2),"T2KDet");
  if (T2KDetMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(T2KDetMinBin+1+int(nT2KDetParams/2),"T2KDet");
  if (T2KDetMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(T2KDetMinBin+1+int(nT2KDetParams/2),"T2KDet");

  if (SKCalibMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(SKCalibMinBin+1+int(nSKCalibParams/2),"SKCalib");
  if (SKCalibMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(SKCalibMinBin+1+int(nSKCalibParams/2),"SKCalib");
  if (SKCalibMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(SKCalibMinBin+1+int(nSKCalibParams/2),"SKCalib");
  if (SKCalibMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(SKCalibMinBin+1+int(nSKCalibParams/2),"SKCalib");

  if (ATMPDDetMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(ATMPDDetMinBin+1+int(nATMPDDetParams/2),"ATMPDDet");
  if (ATMPDDetMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(ATMPDDetMinBin+1+int(nATMPDDetParams/2),"ATMPDDet");
  if (ATMPDDetMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(ATMPDDetMinBin+1+int(nATMPDDetParams/2),"ATMPDDet");
  if (ATMPDDetMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(ATMPDDetMinBin+1+int(nATMPDDetParams/2),"ATMPDDet");

  if (SKDetBeamMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(SKDetBeamMinBin+1+int(nSKDetBeamParams/2),"SKDetBeam");
  if (SKDetBeamMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(SKDetBeamMinBin+1+int(nSKDetBeamParams/2),"SKDetBeam");
  if (SKDetBeamMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(SKDetBeamMinBin+1+int(nSKDetBeamParams/2),"SKDetBeam");
  if (SKDetBeamMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(SKDetBeamMinBin+1+int(nSKDetBeamParams/2),"SKDetBeam");

  if (AtmSKDetMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(AtmSKDetMinBin+1+int(nAtmSKDetParams/2),"AtmSKDet");
  if (AtmSKDetMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(AtmSKDetMinBin+1+int(nAtmSKDetParams/2),"AtmSKDet");
  if (AtmSKDetMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(AtmSKDetMinBin+1+int(nAtmSKDetParams/2),"AtmSKDet");
  if (AtmSKDetMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(AtmSKDetMinBin+1+int(nAtmSKDetParams/2),"AtmSKDet");

  if (CorrFDDetMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(CorrFDDetMinBin+1+int(nCorrFDDetParams/2),"CorrFDDet");
  if (CorrFDDetMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(CorrFDDetMinBin+1+int(nCorrFDDetParams/2),"CorrFDDet");
  if (CorrFDDetMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(CorrFDDetMinBin+1+int(nCorrFDDetParams/2),"CorrFDDet");
  if (CorrFDDetMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(CorrFDDetMinBin+1+int(nCorrFDDetParams/2),"CorrFDDet");

  if (OscMinBin!=-1) CovarianceTH2D->GetXaxis()->SetBinLabel(OscMinBin+1+int(nOscParams/2),"Osc");
  if (OscMinBin!=-1) CorrelationTH2D->GetXaxis()->SetBinLabel(OscMinBin+1+int(nOscParams/2),"Osc");
  if (OscMinBin!=-1) CovarianceTH2D->GetYaxis()->SetBinLabel(OscMinBin+1+int(nOscParams/2),"Osc");
  if (OscMinBin!=-1) CorrelationTH2D->GetYaxis()->SetBinLabel(OscMinBin+1+int(nOscParams/2),"Osc");

  std::cout << "AtmFlux:" << AtmFluxMinBin << " " << AtmFluxMaxBin << std::endl;
  std::cout << "AtmFlux:" << T2KDetMinBin << " " << T2KDetMaxBin << std::endl;
  std::cout << "Xsec:" << XsecMinBin << " " << XsecMaxBin << std::endl;
  std::cout << "SKCalib:" << SKCalibMinBin << " " << SKCalibMaxBin << std::endl;
  std::cout << "ATMPDDet:" << ATMPDDetMinBin << " " << ATMPDDetMaxBin << std::endl;
  std::cout << "SKDetBeam:" << SKDetBeamMinBin << " " << SKDetBeamMaxBin << std::endl;
  std::cout << "AtmSKDet:" << AtmSKDetMinBin << " " << AtmSKDetMaxBin << std::endl;
  std::cout << "CorrFDDet:" << CorrFDDetMinBin << " " << CorrFDDetMaxBin << std::endl;
  std::cout << "Osc:" << OscMinBin << " " << OscMaxBin << std::endl;

  TLine* VerticalLine1 = new TLine(AtmFluxMinBin,0,AtmFluxMinBin,nDraw);
  TLine* VerticalLine2 = new TLine(XsecMinBin,0,XsecMinBin,nDraw);
  TLine* VerticalLine3 = new TLine(SKCalibMinBin,0,SKCalibMinBin,nDraw);
  TLine* VerticalLine4 = new TLine(ATMPDDetMinBin,0,ATMPDDetMinBin,nDraw);
  TLine* VerticalLine5 = new TLine(SKDetBeamMinBin,0,SKDetBeamMinBin,nDraw);
  TLine* VerticalLine6 = new TLine(AtmSKDetMinBin,0,AtmSKDetMinBin,nDraw);
  TLine* VerticalLine7 = new TLine(T2KDetMinBin,0,T2KDetMinBin,nDraw);
  TLine* VerticalLine8 = new TLine(CorrFDDetMinBin,0,CorrFDDetMinBin,nDraw);
  TLine* VerticalLine9 = new TLine(OscMinBin,0,OscMinBin,nDraw);

  TLine* HorizontalLine1 = new TLine(0,AtmFluxMinBin,nDraw,AtmFluxMinBin);
  TLine* HorizontalLine2 = new TLine(0,XsecMinBin,nDraw,XsecMinBin);
  TLine* HorizontalLine3 = new TLine(0,SKCalibMinBin,nDraw,SKCalibMinBin);
  TLine* HorizontalLine4 = new TLine(0,ATMPDDetMinBin,nDraw,ATMPDDetMinBin);
  TLine* HorizontalLine5 = new TLine(0,SKDetBeamMinBin,nDraw,SKDetBeamMinBin);
  TLine* HorizontalLine6 = new TLine(0,AtmSKDetMinBin,nDraw,AtmSKDetMinBin);
  TLine* HorizontalLine7 = new TLine(0,T2KDetMinBin,nDraw,T2KDetMinBin);
  TLine* HorizontalLine8 = new TLine(0,CorrFDDetMinBin,nDraw,CorrFDDetMinBin);
  TLine* HorizontalLine9 = new TLine(0,OscMinBin,nDraw,OscMinBin);

  TCanvas* CovarianceCanv = new TCanvas("CovarianceCanv","");
  CovarianceTH2D->Draw("COLZ");

  if (AtmFluxMinBin!=-1) VerticalLine1->Draw("Same");
  if (XsecMinBin!=-1) VerticalLine2->Draw("Same");
  if (SKCalibMinBin!=-1) VerticalLine3->Draw("Same");
  if (ATMPDDetMinBin!=-1) VerticalLine4->Draw("Same");
  if (SKDetBeamMinBin!=-1) VerticalLine5->Draw("Same");
  if (AtmSKDetMinBin!=-1) VerticalLine6->Draw("Same");
  if (T2KDetMinBin!=-1) VerticalLine7->Draw("Same");
  if (CorrFDDetMinBin!=-1) VerticalLine8->Draw("Same");
  if (OscMinBin!=-1) VerticalLine9->Draw("Same");

  if (AtmFluxMinBin!=-1) HorizontalLine1->Draw("SAME");
  if (XsecMinBin!=-1) HorizontalLine2->Draw("SAME");
  if (SKCalibMinBin!=-1) HorizontalLine3->Draw("SAME");
  if (ATMPDDetMinBin!=-1) HorizontalLine4->Draw("SAME");
  if (SKDetBeamMinBin!=-1) HorizontalLine5->Draw("SAME");
  if (AtmSKDetMinBin!=-1) HorizontalLine6->Draw("SAME");
  if (T2KDetMinBin!=-1) HorizontalLine7->Draw("SAME");
  if (CorrFDDetMinBin!=-1) HorizontalLine8->Draw("SAME");
  if (OscMinBin!=-1) HorizontalLine9->Draw("SAME");

  TCanvas* CorrelationCanv = new TCanvas("CorrelationCanv","");
  CorrelationTH2D->Draw("COLZ");

  if (AtmFluxMinBin!=-1) VerticalLine1->Draw("Same");
  if (XsecMinBin!=-1) VerticalLine2->Draw("Same");
  if (SKCalibMinBin!=-1) VerticalLine3->Draw("Same");
  if (ATMPDDetMinBin!=-1) VerticalLine4->Draw("Same");
  if (SKDetBeamMinBin!=-1) VerticalLine5->Draw("Same");
  if (AtmSKDetMinBin!=-1) VerticalLine6->Draw("Same");
  if (T2KDetMinBin!=-1) VerticalLine7->Draw("Same");
  if (CorrFDDetMinBin!=-1) VerticalLine8->Draw("Same");
  if (OscMinBin!=-1) VerticalLine9->Draw("Same");

  if (AtmFluxMinBin!=-1) HorizontalLine1->Draw("SAME");
  if (XsecMinBin!=-1) HorizontalLine2->Draw("SAME");
  if (SKCalibMinBin!=-1) HorizontalLine3->Draw("SAME");
  if (ATMPDDetMinBin!=-1) HorizontalLine4->Draw("SAME");
  if (SKDetBeamMinBin!=-1) HorizontalLine5->Draw("SAME");
  if (AtmSKDetMinBin!=-1) HorizontalLine6->Draw("SAME");
  if (T2KDetMinBin!=-1) HorizontalLine7->Draw("SAME");
  if (CorrFDDetMinBin!=-1) HorizontalLine8->Draw("SAME");
  if (OscMinBin!=-1) HorizontalLine9->Draw("SAME");

  Covariance->Write("Covariance");
  Correlation->Write("Correlation");

  OutputFile->cd();

  CovarianceCanv->Write("CovarianceCanvas");
  CorrelationCanv->Write("CorrelationCanvas");

}

TString returnFancyString(std::string inStr) {
  return inStr;
  
  TString returnString;
  if (inStr=="atmflux_1") {returnString = "Abs Norm (E_{#nu}<1 GeV)";}
  if (inStr=="atmflux_2") {returnString = "Abs Norm (E_{#nu}>1 GeV)";}
  if (inStr=="atmflux_3") {returnString = "#nu/#bar{#nu} (E_{#nu}<1 GeV)";}
  if (inStr=="atmflux_4") {returnString = "#nu/#bar{#nu} (1<E_{#nu}<10 GeV)";}
  if (inStr=="atmflux_5") {returnString = "#nu/#bar{#nu} (E_{#nu}>10 GeV)";}
  if (inStr=="atmflux_6") {returnString = "#bar{#nu_{e}}/#nu_{e} (E_{#nu}<1 GeV)";}
  if (inStr=="atmflux_7") {returnString = "#bar{#nu_{e}}/#nu_{e} (1<E_{#nu}<10 GeV)";}
  if (inStr=="atmflux_8") {returnString = "#bar{#nu_{e}}/#nu_{e} (E_{#nu}>10 GeV)";}
  if (inStr=="atmflux_9") {returnString = "#bar{#nu_{#mu}}/#nu_{#mu} (E_{#nu}<1 GeV)";}
  if (inStr=="atmflux_10") {returnString = "#bar{#nu_{#mu}}/#nu_{#mu} (1<E_{#nu}<10 GeV)";}
  if (inStr=="atmflux_11") {returnString = "#bar{#nu_{#mu}}/#nu_{#mu} (E_{#nu}>10 GeV)";}
  if (inStr=="atmflux_12") {returnString = "Up/Down Ratio";}
  if (inStr=="atmflux_13") {returnString = "Horizontal/Vertical Ratio";}
  if (inStr=="atmflux_14") {returnString = "k/#pi Ratio";}
  if (inStr=="atmflux_15") {returnString = "#nu Path Length";}
  if (inStr=="atmflux_16") {returnString = "Solar Activity";}
  if (inStr=="MAQE") {returnString = "MAQE";}
  if (inStr=="2p2h_norm_nu") {returnString = "2p2h norm #nu";}
  if (inStr=="2p2h_norm_nubar") {returnString = "2p2h norm #bar{#nu}";}
  if (inStr=="2p2h_normCtoO") {returnString = "2p2h norm CtoO";}
  if (inStr=="2p2h_shape_C") {returnString = "2p2h shape C";}
  if (inStr=="2p2h_shape_O") {returnString = "2p2h shape O";}
  if (inStr=="2p2h_Edep_lowEnu") {returnString = "2p2h Edep lowE_{#nu}";}
  if (inStr=="2p2h_Edep_highEnu") {returnString = "2p2h Edep highE_{#nu}";}
  if (inStr=="2p2h_Edep_lowEnubar") {returnString = "2p2h Edep lowE_{#bar{#nu}}";}
  if (inStr=="2p2h_Edep_highEnubar") {returnString = "2p2h Edep highE_{#bar{#nu}}";}
  if (inStr=="Q2_norm_0") {returnString = "Q^{2}_{  0}";}
  if (inStr=="Q2_norm_1") {returnString = "Q^{2}_{  1}";}
  if (inStr=="Q2_norm_2") {returnString = "Q^{2}_{  2}";}
  if (inStr=="Q2_norm_3") {returnString = "Q^{2}_{  3}";}
  if (inStr=="Q2_norm_4") {returnString = "Q^{2}_{  4}";}
  if (inStr=="Q2_norm_5") {returnString = "Q^{2}_{  5}";}
  if (inStr=="Q2_norm_6") {returnString = "Q^{2}_{  6}";}
  if (inStr=="Q2_norm_7") {returnString = "Q^{2}_{  7}";}
  if (inStr=="EB_dial_C_nu") {returnString = "#nu EB C";}
  if (inStr=="EB_dial_C_nubar") {returnString = "#bar{#nu} EB C";}
  if (inStr=="EB_dial_O_nu") {returnString = "#nu EB O";}
  if (inStr=="EB_dial_O_nubar") {returnString = "#bar{#nu} EB O";}
  if (inStr=="CA5") {returnString = "CA5";}
  if (inStr=="MARES") {returnString = "MARES";}
  if (inStr=="ISO_BKG_LowPPi") {returnString = "I_{1/2} Low P_{#pi}";}
  if (inStr=="ISO_BKG") {returnString = "I_{1/2}";}
  if (inStr=="CC_norm_nu") {returnString = "#nu CC norm";}
  if (inStr=="CC_norm_nubar") {returnString = "#nubar CC norm";}
  if (inStr=="nue_numu") {returnString = "#nu_{e}/#nu_{#mu}";}
  if (inStr=="nuebar_numubar") {returnString = "#bar{#nu_{e}}/#bar{#nu_{#mu}}";}
  if (inStr=="CC_BY_DIS") {returnString = "CC DIS";}
  if (inStr=="CC_BY_MPi") {returnString = "CC MPi";}
  if (inStr=="CC_AGKY_Mult") {returnString = "CC AKGY";}
  if (inStr=="CC_Misc") {returnString = "CC Misc";}
  if (inStr=="CC_DIS_MultPi_Norm_Nu") {returnString = "#nu CC DIS norm";}
  if (inStr=="CC_DIS_MultPi_Norm_Nubar") {returnString = "#bar{#nu} CC DIS norm";}
  if (inStr=="CC_Coh_C") {returnString = "CC Coh. C";}
  if (inStr=="CC_Coh_O") {returnString = "CC Coh. O";}
  if (inStr=="NC_Coh") {returnString = "NC Coh.";}
  if (inStr=="NC_1gamma") {returnString = "NC 1#gamma";}
  if (inStr=="NC_other_near") {returnString = "NC Other (Near)";}
  if (inStr=="NC_other_far") {returnString = "NC Other (Far)";}
  if (inStr=="FEFQE") {returnString = "FEFQE";}
  if (inStr=="FEFQEH") {returnString = "FEFQEH";}
  if (inStr=="FEFINEL") {returnString = "FEFINEL";}
  if (inStr=="FEFABS") {returnString = "FEFABS";}
  if (inStr=="FEFCX") {returnString = "FEFCX";}
  return returnString;
}

double ConvertPar(TString name) {
  double nom = 1;

  if      (name == "xsec_0") nom = 1.21;
  else if (name == "xsec_22") nom = 1.01;
  else if (name == "xsec_23") nom = 0.95;
  else if (name == "xsec_24") nom = 1.30;
  else if (name == "xsec_25") nom = 1.30;
  else if (name == "xsec_42") nom = 1.069;
  else if (name == "xsec_43") nom = 1.824;
  else if (name == "xsec_44") nom = 1.002;
  else if (name == "xsec_45") nom = 1.404;
  else if (name == "xsec_46") nom = 0.697;

  return nom;
}
