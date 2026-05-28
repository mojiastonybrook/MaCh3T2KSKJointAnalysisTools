#include "TFile.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TDirectory.h"

#include <iostream>

void read_SystLLHScan_file(std::string inputfile_name){
  // make canvas
  TCanvas* canvas = new TCanvas("c0","c0",0,0,1024,1024);
  canvas->SetGrid();
  canvas->SetTickx();
  canvas->SetTicky();
  canvas->SetBottomMargin(0.1);
  canvas->SetTopMargin(0.1);
  canvas->SetRightMargin(0.15);
  canvas->SetLeftMargin(0.15);

  std::string inputfile_name_base = inputfile_name.substr(0,inputfile_name.find(".root"));
  TString canvasname = inputfile_name_base+"_dm23_scan.pdf[";
  canvas->Print(canvasname);
  canvasname.ReplaceAll("[","");

  TFile* inputfile = new TFile(inputfile_name.c_str(),"READ");
  // Sample_LLH
  TDirectory* tempDir = (TDirectory*)inputfile->Get("Sample_LLH");
  int key_size = tempDir->GetListOfKeys()->GetSize();
  for (int i=0; i<key_size; i++){
    std::string plot_name = tempDir->GetListOfKeys()->At(i)->GetName();
    if (plot_name.find("delm2_23")==std::string::npos) continue;
    std::cout << plot_name << std::endl;
    TH1D* temp_hist = (TH1D*)tempDir->Get(plot_name.c_str());
    
    temp_hist->GetXaxis()->SetLabelSize(0.02);
    temp_hist->GetYaxis()->SetLabelSize(0.02);
    temp_hist->SetTitleOffset(1.1,"y");

    canvas->cd();
    temp_hist->Draw("COLZ");
    canvas->Print(canvasname);
  }
  // Total_LLH
  std::cout << "Total_LLH: " << std::endl;
  TDirectory* tempDir = (TDirectory*)inputfile->Get("Total_LLH");
  int key_size = tempDir->GetListOfKeys()->GetSize();
  for (int i=0; i<key_size; i++){
    std::string plot_name = tempDir->GetListOfKeys()->At(i)->GetName();
    if (plot_name.find("delm2_23")==std::string::npos) continue;
    std::cout << plot_name << std::endl;
    TH1D* temp_hist = (TH1D*)tempDir->Get(plot_name.c_str());

    temp_hist->GetXaxis()->SetLabelSize(0.02);
    temp_hist->GetYaxis()->SetLabelSize(0.02);
    temp_hist->SetTitleOffset(1.8,"y");

    canvas->cd();
    temp_hist->Draw("COLZ");
    canvas->Print(canvasname);
  }
  // Samples
  TDirectory* tempDir = (TDirectory*)inputfile->Get("Samples");
  int subdir_size = tempDir->GetListOfKeys()->GetSize();
  for (int i=0; i<subdir_size; i++){
    std::string subdir_name = tempDir->GetListOfKeys()->At(i)->GetName();
    if (subdir_name.find("ND") != std::string::npos) continue;
    std::cout << subdir_name << std::endl;

    TDirectory* temp_subDir = (TDirectory*)tempDir->Get(subdir_name.c_str());
    int key_size = temp_subDir->GetListOfKeys()->GetSize();
    for (int j=0; j<key_size; j++) {
      std::string plot_name = temp_subDir->GetListOfKeys()->At(j)->GetName();
      if (plot_name.find("delm2_23")==std::string::npos) continue;
      std::cout << plot_name << std::endl;
      TH1D* temp_hist = (TH1D*)temp_subDir->Get(plot_name.c_str());
      
      std::string title = subdir_name+": delm2_23";
      temp_hist->SetTitle(title.c_str());
      temp_hist->GetXaxis()->SetLabelSize(0.02);
      temp_hist->GetYaxis()->SetLabelSize(0.02);

      canvas->cd();
      temp_hist->Draw("COLZ");
      canvas->Print(canvasname);
    }
  }

  canvas->Print(canvasname+"]");
  std::exit(0);
}
