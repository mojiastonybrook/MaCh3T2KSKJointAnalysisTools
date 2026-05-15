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
----------------------
Mo: 2D posterior distribution plots for detector shifting/smearing parameters constrained by hybridPi0 Chi2 maps.
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

bool WriteChain = false;
int nBins = 70;
void Make2DPosterior(TFile* OutputFile, TChain* Chain, Int_t Burnin);
TString returnFancyString(std::string inStr);

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
 
  //============= - Generate 2D Posterior distribution plots for selected parameters
  Make2DPosterior(OutputFile,Chain,Burnin);

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

// *********************
// Make the post-fit covariance matrix in all dimensions
void Make2DPosterior(TFile* OutputFile, TChain* Chain, Int_t Burnin) {
  // *********************

  TObjArray* brlis = (TObjArray*)Chain->GetListOfBranches();
  int nbr = brlis->GetEntries();
  
  std::vector<TString> BranchNames;  
  TString bname;
  std::string bname_str;
  bool drawPosterior;

  int AtmSKDetIndex;
  TString FancyName;

  int counter = 0;

  for (int i = 0; i < nbr; i++) {
    drawPosterior = false;

    bname = brlis->At(i)->GetName();
    bname_str = bname.Data();
    FancyName = bname;

    if(bname.BeginsWith("xsec_")) {
      continue;
    }
    else if(bname.BeginsWith("atmflux_")) {
      continue;
    }
    else if(bname.BeginsWith("skd_joint_")) {
      continue;
    }
    else if(bname.BeginsWith("skcalib_")) {
      continue;
    }
    else if(bname.BeginsWith("atmpddet_")) {
      continue;
    }
    else if(bname.BeginsWith("SKDetBeam_")) {
      continue;
    }
    else if(bname.BeginsWith("AtmSKDet_")) {
      AtmSKDetIndex = atoi((bname_str.substr(bname_str.find("_")+1)).c_str());
      // select hybridPi0 Chi2 constrained smearing parameters first
      if(AtmSKDetIndex == 32 || AtmSKDetIndex == 34 || AtmSKDetIndex == 36 ||
         AtmSKDetIndex == 88 || AtmSKDetIndex == 90 || AtmSKDetIndex == 92 ||
         AtmSKDetIndex == 144 || AtmSKDetIndex == 146 || AtmSKDetIndex == 148 ||
         AtmSKDetIndex == 200 || AtmSKDetIndex == 202 || AtmSKDetIndex == 204){
	      
        drawPosterior = true;
      }
    }
    else if(bname.BeginsWith("corrfddet_")) {
      continue;
    }else{
      continue;
    }

    if (bname.Contains("prop")) {
      drawPosterior = false;
    }

    if (drawPosterior) {
      BranchNames.push_back(bname);
      counter += 1;
    }
  }

  int nDraw = BranchNames.size();  
  std::cout << "-----------------------------------" << std::endl;
  std::cout << "Ploting 2D Posteriors" << std::endl;
  std::cout << "-----------------------------------" << std::endl;

  OutputFile->mkdir("2DPosteriorPlots");
  OutputFile->cd("2DPosteriorPlots");

  TString bname_x;
  TString bname_y;
  std::string bname_x_str;
  int index_y;
  for (int iParam = 0; iParam < nDraw; iParam++) {
    
    if (iParam % (nDraw/3) == 0) {
      std::cout << "  " << iParam << "/" << nDraw << " (" << int((double(iParam)/double(nDraw)*100.0))+1 << "%)" << std::endl;
    }
    // find the corresponding branch name of the shifting parameters
    bname_x = BranchNames[iParam];
    bname_x_str = bname_x.Data();
    index_y = atoi((bname_x_str.substr(bname_x_str.find("_")+1)).c_str()) + 1;
    bname_y = Form("AtmSKDet_%i", index_y);
    // The draw which we want to perform
    TString DrawMe = bname_y+":"+bname_x;
    std::cout << "drawing " << DrawMe << std::endl;	
    // TH2F to hold the Correlation 
    TH2D *hPost2D = new TH2D(DrawMe, DrawMe, nBins, Chain->GetMinimum(bname_x), Chain->GetMaximum(bname_x), nBins, Chain->GetMinimum(bname_y), Chain->GetMaximum(bname_y));
	
    hPost2D->SetMinimum(0);
    hPost2D->GetXaxis()->SetTitle(bname_x);
    hPost2D->GetYaxis()->SetTitle(bname_y);
    hPost2D->GetZaxis()->SetTitle("Steps");
	
    // The draw command we want, i.e. draw param j vs param i
    Chain->Project(DrawMe, DrawMe, Form("step >= %i && LogL > 1000 && LogL < 1000000",Burnin));    

    hPost2D->Write();

    delete hPost2D;
  }
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

