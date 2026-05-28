#include "TFile.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TDirectory.h"
#include "TString.h"

#include <iostream>
#include <vector>

void compare_SystLLHScan_files(){
  // make canvas
  TCanvas* canvas = new TCanvas("c0","c0",0,0,1024,1024);
  canvas->SetGrid();
  canvas->SetTickx();
  canvas->SetTicky();
  canvas->SetBottomMargin(0.15);
  canvas->SetTopMargin(0.1);
  canvas->SetRightMargin(0.15);
  canvas->SetLeftMargin(0.15);
  // make pads
  canvas->Draw();
  /*
  TPad *p1 = new TPad("p1","p1",0.,0.25,1.,1.0);
  p1->SetBottomMargin(0.);
  p1->SetGrid();
  p1->Draw();
  TPad* p2 = new TPad("p2","p2",0.,0.,1.,0.25);
  p2->SetTopMargin(0.);
  p2->SetGrid();
  p2->Draw();
  */
  //std::string inputfile_name_base = inputfile_name.substr(0,inputfile_name.find(".root"));
  //TString canvasname = inputfile_name_base+"_dm23_scan.pdf[";
  TString canvasname = "SystLLHScan_comparison_dm23_scan_CoarseCosZ.pdf[";
  canvas->Print(canvasname);
  canvasname.ReplaceAll("[","");

  std::vector<TString> fileNames;
  fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/mach3_jobs/SystLLHScan/OscillatorSmearing/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_SystLLHScan.root");
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/mach3_jobs/SystLLHScan/OscillatorSmearing/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse31x26_fine310x260_SystLLHScan.root");
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/mach3_jobs/SystLLHScan/OscillatorSmearing/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse31x26_fine610x520_SystLLHScan.root");
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/mach3_jobs/SystLLHScan/OscillatorSmearing/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse122x104_fine610x520_SystLLHScan.root");
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse31x26_fine1240x1040/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse31x26_fine1240x1040_SystLLHScan.root"); 
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/mach3_jobs/SystLLHScan/OscillatorSmearing/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse61x52_fine305x260_SystLLHScan.root");
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/mach3_jobs/SystLLHScan/OscillatorSmearing/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse61x52_fine1220x1040_SystLLHScan.root");
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse34x15_fine68x60/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse34x15_fine68x60_SystLLHScan.root"); 
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse34x15_fine340x150/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse34x15_fine340x150_SystLLHScan.root"); 
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse34x15_fine680x600/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse34x15_fine680x600_SystLLHScan.root"); 
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse31x52_fine610x520/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse31x52_fine610x520_SystLLHScan.root"); 
  //fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse122x52_fine610x520/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse122x52_fine610x520_SystLLHScan.root"); 
  fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse61x26_fine610x520/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse61x26_fine610x520_SystLLHScan.root"); 
  fileNames.push_back("/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange/SystLLHScan_Submit/coarse61x104_fine610x520/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse61x104_fine610x520_SystLLHScan.root"); 
 
  //fileNames.push_back("./Asym_off/MaCh3-Atmospherics-SystLLHScan_SKOnly_OscOnly_HPD_SystLLHScan.root");
  //fileNames.push_back("./EScale_off/MaCh3-Atmospherics-SystLLHScan_SKOnly_OscOnly_HPD_SystLLHScan.root");
  //fileNames.push_back("./EB_off/MaCh3-Atmospherics-SystLLHScan_SKOnly_OscOnly_HPD_SystLLHScan.root");
 
  int nFiles = fileNames.size(); 
  
  std::vector<int> lineColors(nFiles);
  lineColors[0] = kBlack;
  lineColors[1] = kRed;
  lineColors[2] = kBlue;
  //lineColors[3] = kGreen;
  //lineColors[4] = kMagenta;
  //lineColors[5] = kViolet;

  std::vector<TString> lend_entries(nFiles);
  lend_entries[0] = "MaCh3 Default";
  lend_entries[1] = "coarse 61x26";
  lend_entries[2] = "coarse 61x104";
  //lend_entries[3] = "coarse 34x15 fine 680x600";
  //lend_entries[4] = "E Scale Atm/Beam at nominals";
  //lend_entries[5] = "EB at nominals";
  // Sample_LLH
  // read
  std::vector<TH1D*> Hist_1D_Vec;
  for (int iFile=0; iFile<nFiles; iFile++){
    TFile* inputfile = new TFile(fileNames[iFile],"READ");
    TDirectory* tempDir = (TDirectory*)inputfile->Get("Sample_LLH");
    int key_size = tempDir->GetListOfKeys()->GetSize();
 
    for (int i=0; i<key_size; i++){
      std::string plot_name = tempDir->GetListOfKeys()->At(i)->GetName();
      if (plot_name.find("delm2_23")==std::string::npos) continue;
      std::cout << plot_name << std::endl;
      TH1D* temp_hist = ((TH1D*)tempDir->Get(plot_name.c_str()))->Clone();

      temp_hist->SetLineColor(lineColors[iFile]);
      temp_hist->SetLineWidth(1.5);
      temp_hist->GetXaxis()->SetLabelSize(0.02);
      temp_hist->GetYaxis()->SetLabelSize(0.02);
      temp_hist->SetTitleOffset(1.1,"y");
      std::cout << temp_hist->GetMaximum() << std::endl;
      Hist_1D_Vec.push_back(temp_hist);
    } 
  }
  //residual
  /*
  std::vector<TH1D*> Hist_1D_res_Vec;
  TH1D* base = (TH1D*) Hist_1D_Vec[0]->Clone();
  for (int iFile=1; iFile<nFiles; iFile++){
    TH1D* res = (TH1D*)Hist_1D_Vec[iFile]->Clone();
    res->Add(base,-1.0);
    res->Divide(base);
    Hist_1D_res_Vec.push_back(res);
  }
  */
  // plot
  double Max = -1e8;
  double Min = 1e8;

  std::cout << Hist_1D_Vec[0]->GetMaximum() <<std::endl;;	
  for (int iFile=0; iFile<nFiles;iFile++){
    if (Hist_1D_Vec[iFile]->GetMaximum() > Max) Max = Hist_1D_Vec[iFile]->GetMaximum();
    if (Hist_1D_Vec[iFile]->GetMinimum() < Min) Min = Hist_1D_Vec[iFile]->GetMinimum();
  }
  canvas->cd();
  //p1->cd();
  TLegend* lend = new TLegend(0.15,0.75,0.4,0.9);  
  for (int iFile=0; iFile<nFiles; iFile++){
    Hist_1D_Vec[iFile]->SetMaximum(Max*1.02);
    Hist_1D_Vec[iFile]->SetMinimum(Min*0.98);
	  
    if (iFile==0){
      Hist_1D_Vec[iFile]->Draw("COLZ");
      lend->AddEntry(Hist_1D_Vec[iFile],lend_entries[iFile],"l"); 
    } else{
      Hist_1D_Vec[iFile]->Draw("SAME");
      lend->AddEntry(Hist_1D_Vec[iFile],lend_entries[iFile],"l");
    }
  }
  lend->Draw("SAME");
  //residual part
  /*
  p2->cd();
  double Max = -1e8;
  double Min = 1e8;
  for (int iPlot=0; iPlot<Hist_1D_res_Vec.size(); iPlot++){
    if (Hist_1D_res_Vec[iPlot]->GetMaximum() > Max) Max = Hist_1D_res_Vec[iPlot]->GetMaximum();
    if (Hist_1D_res_Vec[iPlot]->GetMinimum() < Min) Min = Hist_1D_res_Vec[iPlot]->GetMinimum();
  }
  for (int iPlot=0; iPlot<Hist_1D_res_Vec.size(); iPlot++){
    Hist_1D_res_Vec[iPlot]->SetMaximum(Max);
    Hist_1D_res_Vec[iPlot]->SetMinimum(Min);
    if (iPlot==0){
      Hist_1D_res_Vec[iPlot]->SetTitle(";#Delta m^{2}_{23};Residual");
      Hist_1D_res_Vec[iPlot]->SetTitleOffset(1.1,"y");
      Hist_1D_res_Vec[iPlot]->Draw("COLZ");
    } else{
      Hist_1D_res_Vec[iPlot]->Draw("SAME");
    }
  }
  */
  canvas->Print(canvasname);
  //canvas->Print(canvasname+"]"); 
  //std::exit(-1);
  // Total_LLH ////////////////////////////////////////////////////////////// 
  /*
  std::cout << "Total_LLH: " << std::endl;
  Hist_1D_Vec.clear();
  for (int iFile=0; iFile<nFiles; iFile++){
    TFile* inputfile = new TFile(fileNames[iFile]);
    TDirectory* tempDir = (TDirectory*)inputfile->Get("Total_LLH");
    int key_size = tempDir->GetListOfKeys()->GetSize();
 
    for (int i=0; i<key_size; i++){
      std::string plot_name = tempDir->GetListOfKeys()->At(i)->GetName();
      if (plot_name.find("delm2_23")==std::string::npos) continue;
      std::cout << plot_name << std::endl;
      TH1D* temp_hist = ((TH1D*)tempDir->Get(plot_name.c_str()))->Clone();

      temp_hist->SetLineColor(lineColors[iFile]);
      temp_hist->SetLineWidth(1.8);
      temp_hist->GetXaxis()->SetLabelSize(0.02);
      temp_hist->GetYaxis()->SetLabelSize(0.02);
      temp_hist->SetTitleOffset(1.1,"y");
      std::cout << temp_hist->GetMaximum() << std::endl;
      Hist_1D_Vec.push_back(temp_hist);
    } 
  }
  //residual
  Hist_1D_res_Vec.clear();
  TH1D* base = (TH1D*) Hist_1D_Vec[0]->Clone();
  for (int iFile=1; iFile<nFiles; iFile++){
    TH1D* res = (TH1D*)Hist_1D_Vec[iFile]->Clone();
    res->Add(base,-1.0);
    res->Divide(base);
    Hist_1D_res_Vec.push_back(res);
  }
  // plot
  double Max = -1e8;
  double Min = 1e8;

  std::cout << Hist_1D_Vec[0]->GetMaximum() <<std::endl;;	
  for (int iFile=0; iFile<nFiles;iFile++){
    if (Hist_1D_Vec[iFile]->GetMaximum() > Max) Max = Hist_1D_Vec[iFile]->GetMaximum();
    if (Hist_1D_Vec[iFile]->GetMinimum() < Min) Min = Hist_1D_Vec[iFile]->GetMinimum();
  }
  canvas->cd();
  p1->cd();
  TLegend* lend = new TLegend(0.1,0.7,0.5,0.9);  
  for (int iFile=0; iFile<nFiles; iFile++){
    Hist_1D_Vec[iFile]->SetMaximum(Max+10);
    Hist_1D_Vec[iFile]->SetMinimum(Min);
	  
    if (iFile==0){
      Hist_1D_Vec[iFile]->Draw("COLZ");
      lend->AddEntry(Hist_1D_Vec[iFile],lend_entries[iFile],"l"); 
    } else{
      Hist_1D_Vec[iFile]->Draw("SAME");
      lend->AddEntry(Hist_1D_Vec[iFile],lend_entries[iFile],"l");
    }
  }
  lend->Draw("SAME");
  //residual part
  p2->cd();
  double Max = -1e8;
  double Min = 1e8;
  for (int iPlot=0; iPlot<Hist_1D_res_Vec.size(); iPlot++){
    if (Hist_1D_res_Vec[iPlot]->GetMaximum() > Max) Max = Hist_1D_res_Vec[iPlot]->GetMaximum();
    if (Hist_1D_res_Vec[iPlot]->GetMinimum() < Min) Min = Hist_1D_res_Vec[iPlot]->GetMinimum();
  }
  for (int iPlot=0; iPlot<Hist_1D_res_Vec.size(); iPlot++){
    Hist_1D_res_Vec[iPlot]->SetMaximum(Max);
    Hist_1D_res_Vec[iPlot]->SetMinimum(Min);
    if (iPlot==0){
      Hist_1D_res_Vec[iPlot]->SetTitle(";#Delta m^{2}_{23};Residual");
      Hist_1D_res_Vec[iPlot]->SetTitleOffset(1.1,"y");
      Hist_1D_res_Vec[iPlot]->Draw("COLZ");
    } else{
      Hist_1D_res_Vec[iPlot]->Draw("SAME");
    }
  }
  canvas->Print(canvasname);
  //canvas->Print(canvasname+"]");
  //std::exit(-1);
  */
  // Samples /////////////////////////////////////////////////////////////
  // get sample names
  /*
  std::vector<TString> sample_names;
  TFile* inputfile1 = new TFile(fileNames[0]);
  TDirectory* tempDir = (TDirectory*)inputfile1->Get("Samples");
  int subdir_size = tempDir->GetListOfKeys()->GetSize();
  for (int i=0; i<subdir_size; i++){
    std::string subdir_name = tempDir->GetListOfKeys()->At(i)->GetName();
    if (subdir_name.find("ND") != std::string::npos) continue;
    std::cout << subdir_name << std::endl;
    TString sample = subdir_name.c_str();
    sample_names.push_back(sample);
  }	  
  // read
  for (iSample=0; iSample<sample_names.size();iSample++){
    Hist_1D_Vec.clear();
    for (int iFile=0; iFile<nFiles; iFile++){
      TFile* inputfile = new TFile(fileNames[iFile]);
      TDirectory* tempDir = (TDirectory*)inputfile->Get("Samples");
        
      TDirectory* temp_subDir = (TDirectory*)tempDir->Get(sample_names[iSample]);
      int key_size = temp_subDir->GetListOfKeys()->GetSize();
      for (int j=0; j<key_size; j++) {
        std::string plot_name = temp_subDir->GetListOfKeys()->At(j)->GetName();
        if (plot_name.find("delm2_23")==std::string::npos) continue;
        std::cout << plot_name << std::endl;
        TH1D* temp_hist = (TH1D*)temp_subDir->Get(plot_name.c_str());
     
        TString title = sample_names[iSample]+": delm2_23";
        temp_hist->SetTitle(title); 
        temp_hist->SetLineColor(lineColors[iFile]);
        temp_hist->SetLineWidth(1.8);
        temp_hist->GetXaxis()->SetLabelSize(0.02);
        temp_hist->GetYaxis()->SetLabelSize(0.02);
        temp_hist->SetTitleOffset(1.1,"y");
        std::cout << temp_hist->GetMaximum() << std::endl;
        Hist_1D_Vec.push_back(temp_hist);
      } 
    }
    Hist_1D_res_Vec.clear();
    TH1D* base = (TH1D*) Hist_1D_Vec[0]->Clone();
    for (int iFile=1; iFile<nFiles; iFile++){
      TH1D* res = (TH1D*)Hist_1D_Vec[iFile]->Clone();
      res->Add(base,-1.0);
      res->Divide(base);
      Hist_1D_res_Vec.push_back(res);
    }

  // plot
    double Max = -1e8;
    double Min = 1e8;

    std::cout << Hist_1D_Vec[0]->GetMaximum() <<std::endl;;	
    for (int iFile=0; iFile<nFiles;iFile++){
      if (Hist_1D_Vec[iFile]->GetMaximum() > Max) Max = Hist_1D_Vec[iFile]->GetMaximum();
      if (Hist_1D_Vec[iFile]->GetMinimum() < Min) Min = Hist_1D_Vec[iFile]->GetMinimum();
    }
    canvas->cd();
    p1->cd();
    TLegend* lend = new TLegend(0.1,0.7,0.5,0.9);  
    for (int iFile=0; iFile<nFiles; iFile++){
      Hist_1D_Vec[iFile]->SetMaximum(Max*1.02);
      Hist_1D_Vec[iFile]->SetMinimum(Min);
	  
      if (iFile==0){
        Hist_1D_Vec[iFile]->Draw("COLZ");
        lend->AddEntry(Hist_1D_Vec[iFile],lend_entries[iFile],"l"); 
      } else{
        Hist_1D_Vec[iFile]->Draw("SAME");
        lend->AddEntry(Hist_1D_Vec[iFile],lend_entries[iFile],"l");
      }
    }
    lend->Draw("SAME");
  //residual part
    p2->cd();
    double Max = -1e8;
    double Min = 1e8;
    for (int iPlot=0; iPlot<Hist_1D_res_Vec.size(); iPlot++){
      if (Hist_1D_res_Vec[iPlot]->GetMaximum() > Max) Max = Hist_1D_res_Vec[iPlot]->GetMaximum();
      if (Hist_1D_res_Vec[iPlot]->GetMinimum() < Min) Min = Hist_1D_res_Vec[iPlot]->GetMinimum();
    }
    for (int iPlot=0; iPlot<Hist_1D_res_Vec.size(); iPlot++){
      Hist_1D_res_Vec[iPlot]->SetMaximum(Max);
      Hist_1D_res_Vec[iPlot]->SetMinimum(Min);
      if (iPlot==0){
        Hist_1D_res_Vec[iPlot]->SetTitle(";#Delta m^{2}_{23};Residual");
        Hist_1D_res_Vec[iPlot]->SetTitleOffset(1.1,"y");
        Hist_1D_res_Vec[iPlot]->Draw("COLZ");
      } else{
        Hist_1D_res_Vec[iPlot]->Draw("SAME");
      }
    }
    canvas->Print(canvasname);
  }
  */
  canvas->Print(canvasname+"]");
  std::exit(0);
}
