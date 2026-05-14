#include "TFile.h"
#include "TString.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPad.h"

#include <vector>

void read_file_hybridPi0_AtmSKDets_deltaChi2(){
  //name of selection parameter
  std::vector<std::string> selecParamName(4);
  selecParamName[0] = "rcpar"; 
  selecParamName[1] = "emu";
  selecParamName[2] = "epi0";
  selecParamName[3] = "mupip";
  // Chi2 map name prefix 
  std::string prefix = "h_pi0_chi2_";

  std::vector<TString> Posts2D;
  std::vector<TString> Chi2Maps;
  //std::vector<TString> 2DPosts;

  for(int iParam=0; iParam < selecParamName.size(); iParam++){
    for(int iEvis=0; iEvis < 3; iEvis++) {
      //name of Chi2 map
      TString Chi2Name = prefix+selecParamName[iParam]+Form("%i", iEvis);     
      std::cout << Chi2Name << std::endl;
      Chi2Maps.push_back(Chi2Name);
      //name of 2D Posterior
      int smearIndex = 32+iParam*56+iEvis*2;
      TString Post2DName = Form("AtmSKDet_%i:AtmSKDet_%i", smearIndex+1, smearIndex);
      std::cout << Post2DName << std::endl;
      Posts2D.push_back(Post2DName);
    }
  } 
  // directory to the root file of hybrid Pi0 Chi2 maps
  std::string AtmSKDetHybridPi0FileName = "/home/mojia/MaCh3_2023_detShiftSmear/MaCh3/inputs/skatm/hmupi_hpi0_chi2_maps_14c_320_320_corr.root"; 
  TFile* Chi2File = new TFile(AtmSKDetHybridPi0FileName.c_str(),"READ");
  if (!Chi2File || Chi2File->IsZombie()) {
    std::cerr << "Hybrid Pi0 Input File invalid!" << std::endl;
    std::cerr << "Given:" << AtmSKDetHybridPi0FileName << std::endl;
    throw;
  }
  // convert Chi2 map to LLH : LLH = - 0.5*Chi2
  // delta_chi2 = Chi2 - Chi2_Min
  std::vector<TH2D*> HybridPi0LLHs;
  for(int i=0; i<Chi2Maps.size(); i++){
    TH2D* chi2_map_temp = ((TH2D*)Chi2File->Get(Chi2Maps[i]))->Clone();
    TH2D* LLH_temp =((TH2D*)Chi2File->Get(Chi2Maps[i]))->Clone();
    
    int N_bins_x = chi2_map_temp->GetXaxis()->GetNbins();
    int N_bins_y = chi2_map_temp->GetYaxis()->GetNbins();
    double chi2_min = chi2_map_temp->GetMinimum();
    for(int iX=0; iX<N_bins_x; iX++){
      for(int iY=0; iY<N_bins_y; iY++){
        double chi2 = chi2_map_temp->GetBinContent(iX+1, iY+1);
        //LLH_temp->SetBinContent(iX+1, iY+1, (chi2-chi2_min));
	
	if ((chi2-chi2_min) < 11.83 ){
	  LLH_temp->SetBinContent(iX+1, iY+1, (chi2-chi2_min));
	}else{
	  LLH_temp->SetBinContent(iX+1, iY+1, 0.);
	}
      }
    } 
    HybridPi0LLHs.push_back(LLH_temp); 
  }
  
  // 2DPosteior file and directory
  TFile* jf_post_file = new TFile("JFDetShiftNSmear_MaCh3_Merge_2DPost_AtmSKDet_Pi0.root","read");
  TDirectory* tempDir = (TDirectory*)jf_post_file->Get("2DPosteriorPlots");

  std::vector<TH2D*> Post2DHists;
  for(int i=0; i<Posts2D.size(); i++){
    TH2D* temp = (TH2D*)tempDir->Get(Posts2D[i])->Clone();
    int N_bins_x = temp->GetXaxis()->GetNbins();
    int N_bins_y = temp->GetYaxis()->GetNbins();
    double MaxBinContent = temp->GetMaximum();
    temp->Smooth(1);
    for(int iX=0; iX<N_bins_x; iX++){
      for(int iY=0; iY<N_bins_y; iY++){
        double BinContent = temp->GetBinContent(iX+1, iY+1);
        if (BinContent == 0) BinContent = 1;
	BinContent = -2.0 * TMath::Log(BinContent/MaxBinContent);
        //temp->SetBinContent(iX+1, iY+1, BinContent);

        if (BinContent < 11.83){
	  temp->SetBinContent(iX+1, iY+1, BinContent);
	}else{
          temp->SetBinContent(iX+1, iY+1, 0.);	
	}
      }
    } 
    Post2DHists.push_back(temp);
  }
  // ploting
  gStyle->SetOptStat(0);
  
  TCanvas* C = new TCanvas("Delta Chi2 Hist2D","Comparison of prior and posterior", 800,400);
  TPad* pad1 = new TPad("pad1","",0, 0, 0.5, 1);
  TPad* pad2 = new TPad("pad2","",0.5, 0., 1., 1.);
  //C->SetGrid();

  //C->Divide(2);

  TH2D* LLH_map_temp;
  TH2D* Hist2D_temp;
  for(int i=0; i<Chi2Maps.size();i++){
    //find chi2 map from root file
    LLH_map_temp = HybridPi0LLHs[i];
    //find hist2D from file
    Hist2D_temp = Post2DHists[i]; 
    /*
    double x_max=LLH_map_temp->GetXaxis()->GetBinLowEdge(LLH_map_temp->GetNbinsX());
    double y_max=LLH_map_temp->GetYaxis()->GetBinLowEdge(LLH_map_temp->GetNbinsY());
    double x_min=LLH_map_temp->GetXaxis()->GetBinLowEdge(1);
    double y_min=LLH_map_temp->GetYaxis()->GetBinLowEdge(1);
    std::cout << "LLH_map: x_max " << x_max << "  x_min " << x_min<<std::endl;
    std::cout << "LLH_map: y_max " << y_max << "  y_min " << y_min<<std::endl;

    if(x_max < Hist2D_temp->GetXaxis()->GetBinLowEdge(Hist2D_temp->GetNbinsX())){
      x_max = Hist2D_temp->GetXaxis()->GetBinLowEdge(Hist2D_temp->GetNbinsX());
    }
    if(y_max < Hist2D_temp->GetYaxis()->GetBinLowEdge(Hist2D_temp->GetNbinsY())){
      y_max = Hist2D_temp->GetYaxis()->GetBinLowEdge(Hist2D_temp->GetNbinsY());
    }
    if(x_min > Hist2D_temp->GetXaxis()->GetBinLowEdge(1)){
      x_min = Hist2D_temp->GetXaxis()->GetBinLowEdge(1);
    }
    if(y_min > Hist2D_temp->GetYaxis()->GetBinLowEdge(1)){
      y_min = Hist2D_temp->GetYaxis()->GetBinLowEdge(1);
    }
    */
    double x_max=Hist2D_temp->GetXaxis()->GetBinLowEdge(LLH_map_temp->GetNbinsX());
    double y_max=Hist2D_temp->GetYaxis()->GetBinLowEdge(LLH_map_temp->GetNbinsY());
    double x_min=Hist2D_temp->GetXaxis()->GetBinLowEdge(1);
    double y_min=Hist2D_temp->GetYaxis()->GetBinLowEdge(1);
 
    std::cout << " x_max " << x_max << "  x_min " << x_min<<std::endl;
    std::cout << " y_max " << y_max << "  y_min " << y_min<<std::endl;

    //
    //C->cd(1);
    //C->SetGrid();
    //C->SetTickx();
    //C->SetTicky();
    //gPad->SetLeftMargin(0.15);
    //gPad->SetRightMargin(0.15);

    //LLH_map_temp->SetMaximum(0);
    //double range = LLH_map_temp->GetMaximum()-LLH_map_temp->GetMinimum();
    //LLH_map_temp->SetMinimum(LLH_map_temp->GetMaximum()-0.6*range);
    //LLH_map_temp->SetMinimum(0.);
    C->cd();
    pad1->Draw();
    pad1->SetLeftMargin(0.15);
    pad1->SetRightMargin(0.15);
    pad1->SetGrid();
    pad1->SetTickx();
    pad1->SetTicky();
    pad1->cd();

    LLH_map_temp->SetMaximum(11.83);
    LLH_map_temp->GetXaxis()->SetRangeUser(x_min,x_max);
    LLH_map_temp->GetYaxis()->SetRangeUser(y_min,y_max);
    LLH_map_temp->GetZaxis()->SetTitle("#Delta #chi^{2}");
    LLH_map_temp->GetZaxis()->SetLabelSize(0.03);
    LLH_map_temp->SetTitle("Prior");
    LLH_map_temp->Draw("COLZ");
 
    //C->cd(2);
    //C->SetGrid();
    //C->SetTickx();
    //C->SetTicky();
 
    //gPad->SetLeftMargin(0.15);
    //gPad->SetRightMargin(0.15);
    C->cd();
    pad2->Draw();
    pad2->SetLeftMargin(0.15);
    pad2->SetRightMargin(0.15);
    pad2->SetGrid();
    pad2->SetTickx();
    pad2->SetTicky();
    pad2->cd();

    Hist2D_temp->GetZaxis()->SetLabelSize(0.03);
    Hist2D_temp->GetZaxis()->SetTitleOffset(0.8);
    Hist2D_temp->GetXaxis()->SetTitle("#alpha");
    Hist2D_temp->GetYaxis()->SetTitle("#beta");
    Hist2D_temp->SetTitle("Posterior");
    //Hist2D_temp->SetMinimum(0.);
    Hist2D_temp->SetMaximum(11.83);
    Hist2D_temp->GetZaxis()->SetTitle("#Delta #chi^{2}");
    //Hist2D_temp->SetAxisRange(x_min,x_max,"X");
    //Hist2D_temp->SetAxisRange(y_min,y_max,"Y");
    Hist2D_temp->Draw("COLZ");
    //Hist2D_temp->GetXaxis()->SetRangeUser(x_min,x_max);
    //Hist2D_temp->GetYaxis()->SetRangeUser(y_min,y_max);
    y_min = Hist2D_temp->GetYaxis()->GetBinLowEdge(1);
    std::cout << "Hist2D y_min: " << y_min << std::endl;
    // print name
    TString print_name = Form("HybridPi0Comparison/HyrbidPi0_dChi2_Hist2D_Comp_%s.pdf",Chi2Maps[i].Data());
    C->Print(print_name);
  }

}
