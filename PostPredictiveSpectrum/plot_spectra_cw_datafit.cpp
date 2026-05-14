#include <vector>
#include <iostream>
#include <TString.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TLegend.h>


TH2D* AddAdditionalBinToBottomOfHist(TH2D* Hist);
std::vector<TH1D*> GetExpectedSpectra(std::vector<TString> SampleNames, TString SpectraFileName);
std::vector<TH2D*> GetPredictiveSpectraDistribution(std::vector<TString> SampleNames, TString PredictiveFileName);
std::vector<TGraphAsymmErrors*> GetPredictiveSpectraInfo(std::vector<TString> SampleNames, TString PredictiveFileName);

void plot_spectra_cw_datafit(){

  //gStyle->SetPalette(kFruitPunch);
  gStyle->SetPalette(kLake);
  //kgStyle->SetPalette(kBird);
  TColor::InvertPalette();

  TString SpectraFileName = "../Atm_data.root";
  //TString SpectraFileName = "AsimovA_ExpectedSpectra.root";
  TString PredictiveFileName = "../output/MaCh3_PosteriorPredictiveSpectrum_SKOnly_N3000_Pred.root";
    //"MaCh3-Atmospherics-MCMC_FullDataFitVaryCorrFDDet_3000throws_Pred.root";//"MaCh3-Atmospherics-MCMC_FullDataFitVaryXsec_3000throws_Pred.root";
    //"MaCh3-Atmospherics-MCMC_FullData10KthrowsAgain_FullData10KthrowsAgain_Pred.root";
    //"MaCh3-Atmospherics-MCMC_FullDataFit_3000throws_Pred.root"; 
  //TString PredictiveFileName = "MaCh3-Atmospherics-MCMC_AsimovA_Pred.root";
    //"MaCh3-Atmospherics-MCMC_FullDataRCreweight_FullDataRC_3000throws_Pred.root";
    //"MaCh3-Atmospherics-MCMC_FullDataFit_3000throws_Pred.root";

  TObjArray* Token = PredictiveFileName.Tokenize("_");
  TString PredictiveName = (TString)(((TObjString*)(Token->At(1)))->String());
  
  TString OutputNameBase = "Predictive_"+PredictiveName+"_delme";
    
  std::vector<TString> SampleNames;

  SampleNames.push_back("SubGeV-elike-0dcy");
  SampleNames.push_back("SubGeV-elike-1dcy");
  SampleNames.push_back("SubGeV-mulike-0dcy");
  SampleNames.push_back("SubGeV-mulike-1dcy");
  SampleNames.push_back("SubGeV-mulike-2dcy");
  SampleNames.push_back("SubGeV-pi0like");
  SampleNames.push_back("MultiGeV-elike-nue");
  SampleNames.push_back("MultiGeV-elike-nuebar");
  SampleNames.push_back("MultiGeV-mulike");
  SampleNames.push_back("MultiRing-elike-nue");
  SampleNames.push_back("MultiRing-elike-nuebar");
  SampleNames.push_back("MultiRing-mulike");
  SampleNames.push_back("MultiRingOther-1");
  SampleNames.push_back("PCStop");
  SampleNames.push_back("PCThru");
  SampleNames.push_back("UpStop-mu");
  SampleNames.push_back("UpThruNonShower-mu");
  SampleNames.push_back("UpThruShower-mu");
  
  //SampleNames.push_back("FHC1Rmu-2020");
  //SampleNames.push_back("RHC1Rmu-2020");
  //SampleNames.push_back("FHC1Re-2020");
  //SampleNames.push_back("RHC1Re-2020");
  //SampleNames.push_back("FHCCC1pi-2020");

  //JJ: Data spectra related
  std::vector<TString> SampleNames_Data;
  
  SampleNames_Data.push_back("SubGeV-elike-0dcy");
  SampleNames_Data.push_back("SubGeV-elike-1dcy");
  SampleNames_Data.push_back("SubGeV-mulike-0dcy");
  SampleNames_Data.push_back("SubGeV-mulike-1dcy");
  SampleNames_Data.push_back("SubGeV-mulike-2dcy");
  SampleNames_Data.push_back("SubGeV-pi0like");
  SampleNames_Data.push_back("MultiGeV-elike-nue");
  SampleNames_Data.push_back("MultiGeV-elike-nuebar");
  SampleNames_Data.push_back("MultiGeV-mulike");
  SampleNames_Data.push_back("MultiRing-elike-nue");
  SampleNames_Data.push_back("MultiRing-elike-nuebar");
  SampleNames_Data.push_back("MultiRing-mulike");
  SampleNames_Data.push_back("MultiRingOther-1");
  SampleNames_Data.push_back("PCStop");
  SampleNames_Data.push_back("PCThru");
  SampleNames_Data.push_back("UpStop-mu");
  SampleNames_Data.push_back("UpThruNonShower-mu");
  SampleNames_Data.push_back("UpThruShower-mu");
  
  //SampleNames_Data.push_back("numu_erecTheta");
  //SampleNames_Data.push_back("numubar_erecTheta");
  //SampleNames_Data.push_back("nue_erecTheta");
  //SampleNames_Data.push_back("nuebar_erecTheta");
  //SampleNames_Data.push_back("nue_cc1pi_erecTheta");

  //for (int i = 0; i < SampleNames_Data.size(); ++i) SampleNames_Data[i] = "Spectra_"+SampleNames_Data[i];

  int nSamples = SampleNames.size();

  std::vector<TH1D*> ExpectedSpectra = GetExpectedSpectra(SampleNames_Data,SpectraFileName);
  std::vector<TH2D*> PredictiveSpectraDistribution = GetPredictiveSpectraDistribution(SampleNames,PredictiveFileName);
  std::vector<TGraphAsymmErrors*> PredictiveSpectraInfo = GetPredictiveSpectraInfo(SampleNames,PredictiveFileName);

  std::string pospred = PredictiveFileName.Data();
  TFile *fpospred = new TFile(pospred.c_str(), "open");

  for (int iSample=0;iSample<(2*nSamples);iSample++) {

    if (iSample==36) {
      ExpectedSpectra[iSample]->GetXaxis()->SetRangeUser(0.2,/*6.*/2.);
      PredictiveSpectraDistribution[iSample]->GetXaxis()->SetRangeUser(0.2,/*6.*/2.);
      PredictiveSpectraInfo[iSample]->GetXaxis()->SetRangeUser(0.2,/*6.*/2.);
    }
    
    if (iSample==38) {
      ExpectedSpectra[iSample]->GetXaxis()->SetRangeUser(0.2,/*6.*/2.);
      PredictiveSpectraDistribution[iSample]->GetXaxis()->SetRangeUser(0.2,/*6.*/2.);
      PredictiveSpectraInfo[iSample]->GetXaxis()->SetRangeUser(0.2,/*6.*/2.);
    }

    if (iSample==40) {
      ExpectedSpectra[iSample]->GetXaxis()->SetRangeUser(0.1,1.25);
      PredictiveSpectraDistribution[iSample]->GetXaxis()->SetRangeUser(0.1,1.25);
      PredictiveSpectraInfo[iSample]->GetXaxis()->SetRangeUser(0.1,1.25);
    }

    if (iSample==42) {
      ExpectedSpectra[iSample]->GetXaxis()->SetRangeUser(0.1,1.25);
      PredictiveSpectraDistribution[iSample]->GetXaxis()->SetRangeUser(0.1,1.25);
      PredictiveSpectraInfo[iSample]->GetXaxis()->SetRangeUser(0.1,1.25);
    }

    if (iSample==44) {
      ExpectedSpectra[iSample]->GetXaxis()->SetRangeUser(0.4,1.25);
      PredictiveSpectraDistribution[iSample]->GetXaxis()->SetRangeUser(0.4,1.25);
      PredictiveSpectraInfo[iSample]->GetXaxis()->SetRangeUser(0.4,1.25);
    }
  }

  if (ExpectedSpectra.size() != nSamples*2) {
    std::cout << "ExpectedSpectra vector of wrong size!" << std::endl;
    exit(-1);
  }

  if (PredictiveSpectraDistribution.size() != nSamples*2) {
    std::cout << "PredictiveSpectraDistribution vector of wrong size!" << std::endl;
    exit(-1);
  }

  if (PredictiveSpectraInfo.size() != nSamples*2) {
    std::cout << "PredictiveSpectraInfo vector of wrong size!" << std::endl;
    exit(-1);
  }

  // JJ: the directory to extract pp spectra is the same in all case
  std::vector<std::string> pp_sample_name;
  pp_sample_name.push_back("SubGeV-elike-0dcy");
  pp_sample_name.push_back("SubGeV-elike-1dcy");
  pp_sample_name.push_back("SubGeV-mulike-0dcy");
  pp_sample_name.push_back("SubGeV-mulike-1dcy");
  pp_sample_name.push_back("SubGeV-mulike-2dcy");
  pp_sample_name.push_back("SubGeV-pi0like");
  pp_sample_name.push_back("MultiGeV-elike-nue");
  pp_sample_name.push_back("MultiGeV-elike-nuebar");
  pp_sample_name.push_back("MultiGeV-mulike");
  pp_sample_name.push_back("MultiRing-elike-nue");
  pp_sample_name.push_back("MultiRing-elike-nuebar");
  pp_sample_name.push_back("MultiRing-mulike");
  pp_sample_name.push_back("MultiRingOther-1");
  pp_sample_name.push_back("PCStop");
  pp_sample_name.push_back("PCThru");
  pp_sample_name.push_back("UpStop-mu");
  pp_sample_name.push_back("UpThruNonShower-mu");
  pp_sample_name.push_back("UpThruShower-mu");
  //pp_sample_name.push_back("FHC1Rmu-2020");
  //pp_sample_name.push_back("RHC1Rmu-2020");
  //pp_sample_name.push_back("FHC1Re-2020");
  //pp_sample_name.push_back("RHC1Re-2020");
  //pp_sample_name.push_back("FHCCC1pi-2020");
  std::string pp_dir_prefix("Spectra");
  std::string pp_dir_postfix("Total");
  std::string pp_sample_hist_extralabel("_Total_Spectra");
  int Nsample = pp_sample_name.size();
  std::vector<TDirectory *> pp_dir;
  pp_dir.resize(Nsample);

  // Load up the posterior predictives
  for (int isample = 0; isample < Nsample; isample++) {
    pp_dir[isample] = (TDirectory *)fpospred->Get(((std::string)(pp_dir_prefix + "/" + pp_sample_name[isample] + "/" + pp_dir_postfix)).c_str());
    if (!(pp_dir[isample])) {
      std::cout << "ERROR: Cannot find TDirectory: " << (std::string)(pp_dir_prefix + "/" + pp_sample_name[isample] + "/" + pp_dir_postfix) << std::endl;
      throw;
    }
  }

  // Read in each of the throws
  int nsteps = 3000;//10000;
  if (pp_dir[0]->GetNkeys() < nsteps) {
    std::cerr << "You requested more throws than there are keys!" << std::endl;
    std::cerr << "There are " << pp_dir[0]->GetNkeys() << " keys in file " << fpospred->GetName() << std::endl;
    std::cerr << "You requested " << nsteps << std::endl;
    throw;
  }
  TCanvas *canv = new TCanvas("canv", "canv", 1200, 950);
  canv->SetRightMargin(canv->GetRightMargin()*1.7);
  canv->SetLeftMargin(canv->GetLeftMargin()*1.4);
  TString outputname = "predictive_woRC_SKOnly_NT3000.pdf";
  canv->Print(outputname+"[");

  const double sin2th13_central = 0.0218;
  const double sin2th13_error = 0.0007;

  TTree *partree = (TTree*)fpospred->Get("ParamTree");
  double sin2th13 = 0;
  partree->SetBranchStatus("*", false);
  partree->SetBranchStatus("sin2th_13", true);
  partree->SetBranchAddress("sin2th_13", &sin2th13);

  std::cout << "Number of draws: " << nsteps << std::endl;

  // Make the TH2D
  TH2D **test = new TH2D*[2*Nsample];
  TH1D **test1d = new TH1D*[2*Nsample];
  int counter = 0;
  for (int i = 0; i < (2*Nsample); ++i) {
    test1d[counter] = (TH1D*)ExpectedSpectra[i]->Clone(Form("test_1d_%i", i));
    test1d[counter]->Reset();
    test[counter] = (TH2D*)PredictiveSpectraDistribution[i]->Clone(Form("test_%i", i));
    test[counter]->Reset();
    counter++;
  }

  for (int step = 0; step < nsteps; step++) {

    partree->GetEntry(step);
    // Calculate the weight for this configuration
    double new_chi = (sin2th13 - sin2th13_central)/sin2th13_error;
    double weight = std::exp(-0.5*new_chi*new_chi);
    /*
    std::cout << sin2th13 << std::endl;
    std::cout << new_chi << std::endl;
    std::cout << weight << std::endl;
    */

    // Get the reactor weight
    for (int isample = 0; isample < Nsample; isample++) {
      TString hist_name = Form("%s%s%d", pp_sample_name[isample].c_str(), pp_sample_hist_extralabel.c_str(), step);
      TH2D *htemp = (TH2D *)pp_dir[isample]->Get(hist_name.Data());
      if (!htemp) {
        std::cout << "ERROR: Cannot find hist: " << hist_name << std::endl;
        throw;
      }
      TH1D *hprojx = htemp->ProjectionX();

      for (int i = 0; i < test[2*isample]->GetXaxis()->GetNbins(); ++i) {
        double bincenterx = hprojx->GetXaxis()->GetBinCenter(i+1);
	double contentx = hprojx->GetBinContent(i+1);
	test[2*isample]->Fill(bincenterx, contentx, weight);
      }
      TH1D *hprojy = htemp->ProjectionY();
      for (int i = 0; i < test[2*isample+1]->GetXaxis()->GetNbins(); ++i) {
	double bincentery = hprojy->GetXaxis()->GetBinCenter(i+1);
	double contenty = hprojy->GetBinContent(i+1);
	test[2*isample+1]->Fill(bincentery, contenty, weight);
      }
    }
  }
  
  for (int isample = 0; isample < 2*Nsample; ++isample) {
    for (int j = 0; j < test[isample]->GetXaxis()->GetNbins(); ++j) {
      double sum = 0;
      for (int k = 0; k < test[isample]->GetYaxis()->GetNbins(); ++k) {
        sum += test[isample]->GetBinContent(j+1, k+1);
      }
      for (int k = 0; k < test[isample]->GetYaxis()->GetNbins(); ++k) {
        test[isample]->SetBinContent(j+1, k+1, test[isample]->GetBinContent(j+1, k+1)/sum);
      }
    }
  }
  
  for (int isample = 0; isample < 2*Nsample; ++isample) {

    for (int j = 0; j < test[isample]->GetXaxis()->GetNbins(); ++j) {
      TH1D *temp1d = test[isample]->ProjectionY("_py", j+1, j+1, "e");
      double mean = temp1d->GetMean();
      double rms = temp1d->GetRMS();
      test1d[isample]->SetBinContent(j+1, mean);
      test1d[isample]->SetBinError(j+1, rms);
    }
    
  }

  TLegend* Legend = new TLegend(/*0.1, 0.91, 0.9, 0.94*/0.14,0.905,0.83,0.94);
  Legend->SetNColumns(3);
  Legend->SetTextSize(0.028);
  Legend->SetMargin(0.3);
  
  Legend->AddEntry(ExpectedSpectra[0], Form("#kern[-3]{ } %s ","Data"), "pel");
  Legend->AddEntry(PredictiveSpectraInfo[0], Form("#kern[-5]{ } %s ","Without RC"), "pe");
  Legend->AddEntry(test1d[0], Form("#kern[-3]{ } %s ","With RC"), "pel");
  //JJ: align the legend text
  Legend->SetColumnSeparation(0.25);
  Legend->SetTextAlign(22);
  //Legend->SetMargin(0.5);
  
  TString RootOutputName = OutputNameBase+".root";
  TFile* File = new TFile(RootOutputName,"RECREATE");

  for (int iSample=0;iSample<2*nSamples;iSample++) {

    test[iSample]->SetMaximum(0.25);
    test[iSample]->SetMinimum(0);
    test[iSample]->Draw("colz");

    test[iSample]->GetYaxis()->SetTitleOffset(test[iSample]->GetYaxis()->GetTitleOffset()*1.5);
    test[iSample]->GetZaxis()->SetTitleOffset(test[iSample]->GetZaxis()->GetTitleOffset()*1.3);

    test[iSample]->SetContour(255);
    test[iSample]->GetXaxis()->SetMaxDigits(6);
    test[iSample]->GetZaxis()->SetTitle("Effective number of throws");

    //JJ: test the axis title and label sizes
    test[iSample]->GetXaxis()->SetTitleSize(0.028);
    test[iSample]->GetXaxis()->SetLabelSize(0.024);
    test[iSample]->GetYaxis()->SetTitleSize(0.028);
    test[iSample]->GetYaxis()->SetLabelSize(0.024);
    test[iSample]->GetZaxis()->SetTitleSize(0.028);
    test[iSample]->GetZaxis()->SetLabelSize(0.024);
        
    //ExpectedSpectra[2*isample]->Draw("same");

    PredictiveSpectraInfo[iSample]->SetLineColor(kRed);
    PredictiveSpectraInfo[iSample]->SetLineWidth(6);
    PredictiveSpectraInfo[iSample]->SetLineStyle(1/*kDashed*/);

    //PredictiveSpectraDistribution[iSample]->GetYaxis()->SetRangeUser(0.,PredictiveSpectraDistribution[iSample]->GetYaxis()->GetBinLowEdge(PredictiveSpectraDistribution[iSample]->GetNbinsY()+1));

    TH2D *hist = test[iSample];

    //PredictiveSpectraDistribution[2*iSample]->GetZaxis()->SetTitle("Relative number of throws");

    if (hist->GetXaxis()->GetBinLowEdge(hist->GetXaxis()->GetNbins()+1) >= 90000) {
      canv->SetLogx(true);
      //hist->GetXaxis()->SetMoreLogLabels();
    } else {
      canv->SetLogx(false);
    }

    if (TString(hist->GetTitle()).Contains("-2020") && hist->GetXaxis()->GetBinLowEdge(hist->GetXaxis()->GetNbins()+1) >= 10 && TString(hist->GetXaxis()->GetTitle()).Contains("Neutrino Energy")) {
      hist->GetXaxis()->SetRange(hist->GetXaxis()->FindBin(0.2), hist->GetXaxis()->FindBin(2));
      //hist->RebinX(2);
      //ExpectedSpectra[iSample]->Rebin(2);
      //PredictiveSpectraInfo[iSample]->Rebin(2);
    }
    //hist->RebinY(2);
    //hist->Scale(1./2);

    ExpectedSpectra[iSample]->SetLineWidth(3);
    //ExpectedSpectra[iSample]->SetLineColor(kRed);
    //ExpectedSpectra[iSample]->SetMarkerStyle(kCircle);
    ExpectedSpectra[iSample]->SetMarkerColor(kBlack);

    //JJ: change the marker size
    ExpectedSpectra[iSample]->SetMarkerStyle(158);
    
    ExpectedSpectra[iSample]->SetLineColor(ExpectedSpectra[iSample]->GetMarkerColor());
    //ExpectedSpectra[iSample]->SetFillColor(kMagenta-9);
    //ExpectedSpectra[iSample]->SetFillStyle(3001);
    ExpectedSpectra[iSample]->SetFillStyle(0);
    //ExpectedSpectra[iSample]->Draw("SAME E2");
    ExpectedSpectra[iSample]->Draw("E SAME");

    //PredictiveSpectraInfo[iSample]->SetMarkerStyle(0);
    PredictiveSpectraInfo[iSample]->SetLineColor(kRed);
    //PredictiveSpectraInfo[iSample]->SetMarkerSize(0);
    //PredictiveSpectraInfo[iSample]->SetFillStyle(1001);
    PredictiveSpectraInfo[iSample]->SetFillStyle(1);
    //PredictiveSpectraInfo[iSample]->SetFillColorAlpha(kGreen, 0.5);
    PredictiveSpectraInfo[iSample]->Draw("E SAME");

    test1d[iSample]->Draw("same");
    test1d[iSample]->SetLineColor(kBlue);
    //JJ: test marker line style
    test1d[iSample]->SetLineWidth(2);
    
    Legend->Draw("SAME");

    canv->Print(outputname);
  }

  canv->Print(outputname+"]");
  File->Close();
}

//Get the 2D 'expected' spectra (So 'Data' for posterior predictive or 'PreBANFF spectra' for prior predictive) of each sample, project into X and Y axis (Ordered as such) and return them in a vector
std::vector<TH1D*> GetExpectedSpectra(std::vector<TString> SampleNames, TString SpectraFileName) {

  TFile* File = new TFile(SpectraFileName);
  if (!File || File->IsZombie()) {
    std::cout << "Did not find file:" << SpectraFileName << std::endl;
    exit(-1);
  }

  int nSamples = SampleNames.size();
  std::vector<TH1D*> HistVector(nSamples*2);

  for (int iSample=0;iSample<nSamples;iSample++) {
    //TH2D* Hist = (TH2D*)File->Get(TString("Spectra_")+SampleNames[iSample]); //JJ: Asimov spectra format
    TH2D* Hist = (TH2D*)File->Get(SampleNames[iSample]); //JJ: Data spectra format

    if (!Hist) {
      std::cout << "Did not find histogram:" << SampleNames[iSample] << " in File:" << SpectraFileName << std::endl;
      File->ls();
      exit(-1);
    }

    HistVector[2*iSample] = (TH1D*)(Hist->ProjectionX())->Clone();
    HistVector[2*iSample+1] = (TH1D*)(Hist->ProjectionY())->Clone();
  }

  return HistVector;
}

//Get the 2D predictive spectra distribution of each sample and return them in a vector
std::vector<TH2D*> GetPredictiveSpectraDistribution(std::vector<TString> SampleNames, TString PredictiveFileName) {

  TFile* File = new TFile(PredictiveFileName);
  if (!File || File->IsZombie()) {
    std::cout << "Did not find file:" << PredictiveFileName << std::endl;
    exit(-1);
  }

  int nSamples = SampleNames.size();
  std::vector<TH2D*> HistVector(nSamples*2);

  for (int iSample=0;iSample<nSamples;iSample++) {
    TH2D* Hist_X = (TH2D*)File->Get(TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_IntegralHist_X_BinByBin"));

    if (!Hist_X) {
      std::cout << "Did not find histogram:" << TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_IntegralHist_X_BinByBin") << " in File:" << PredictiveFileName << std::endl;
      exit(-1);
    }

    //HistVector[2*iSample] = (TH2D*)Hist_X->Clone();
    HistVector[2*iSample] = AddAdditionalBinToBottomOfHist(Hist_X);
    delete Hist_X;

    TH2D* Hist_Y = (TH2D*)File->Get(TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_IntegralHist_Y_BinByBin"));

    if (!Hist_Y) {
      std::cout << "Did not find histogram:" << TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_IntegralHist_Y_BinByBin") << " in File:" << PredictiveFileName << std::endl;
      exit(-1);
    }

    //HistVector[2*iSample+1] = (TH2D*)Hist_Y->Clone();
    HistVector[2*iSample+1] = AddAdditionalBinToBottomOfHist(Hist_Y);

    delete Hist_Y;
  }

  return HistVector;
}

//Get the quantised predictive information of each sample and return them in a vector                                                                                                                     
std::vector<TGraphAsymmErrors*> GetPredictiveSpectraInfo(std::vector<TString> SampleNames, TString PredictiveFileName) {

  TFile* File = new TFile(PredictiveFileName);
  if (!File || File->IsZombie()) {
    std::cout << "Did not find file:" << PredictiveFileName << std::endl;
    exit(-1);
  }

  int nSamples = SampleNames.size();
  std::vector<TGraphAsymmErrors*> GraphVector(nSamples*2);

  for (int iSample=0;iSample<nSamples;iSample++) {
    TGraphAsymmErrors* Graph_X = (TGraphAsymmErrors*)File->Get(TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_Asym_X"));

    if (!Graph_X) {
      std::cout << "Did not find histogram:" << TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_Asym_X") << " in File:" << PredictiveFileName << std::endl;
      exit(-1);
    }

    GraphVector[2*iSample] = (TGraphAsymmErrors*)Graph_X->Clone();

    TGraphAsymmErrors* Graph_Y = (TGraphAsymmErrors*)File->Get(TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_Asym_Y"));

    if (!Graph_Y) {
      std::cout << "Did not find histogram:" << TString("AnalysedHistogram/")+SampleNames[iSample]+TString("/Total/")+SampleNames[iSample]+TString("_Total_Asym_Y") << " in File:" << PredictiveFileName << std::endl;
      exit(-1);
    }

    GraphVector[2*iSample+1] = (TGraphAsymmErrors*)Graph_Y->Clone();
  }

  return GraphVector;
}

TH2D* AddAdditionalBinToBottomOfHist(TH2D* Hist) {

  std::vector<double> XBinEdges;
  std::vector<double> YBinEdges;

  for (int xBin=1;xBin<=Hist->GetNbinsX()+1;xBin++) {
    XBinEdges.push_back(Hist->GetXaxis()->GetBinLowEdge(xBin));
  }
  int nXBins = XBinEdges.size()-1;

  YBinEdges.push_back(0.);
  for (int yBin=1;yBin<=Hist->GetNbinsY()+1;yBin++) {
    YBinEdges.push_back(Hist->GetYaxis()->GetBinLowEdge(yBin));
  }
  int nYBins = YBinEdges.size()-1;

  TString NewHistName = Hist->GetName();
  Hist->SetName(NewHistName+"_Old");

  TString HistTitle = Hist->GetTitle();
  TString XAxisTitle = Hist->GetXaxis()->GetTitle();
  TString YAxisTitle = Hist->GetYaxis()->GetTitle();

  TH2D* NewHist = new TH2D(NewHistName,HistTitle+";"+XAxisTitle+";"+YAxisTitle,nXBins,XBinEdges.data(),nYBins,YBinEdges.data());
  NewHist->SetStats(false);

  for (int xBin=1;xBin<=Hist->GetNbinsX();xBin++) {
    for (int yBin=1;yBin<=Hist->GetNbinsY();yBin++) {
      NewHist->SetBinContent(xBin,yBin+1,Hist->GetBinContent(xBin,yBin));
    }
  }

  return NewHist;
}
