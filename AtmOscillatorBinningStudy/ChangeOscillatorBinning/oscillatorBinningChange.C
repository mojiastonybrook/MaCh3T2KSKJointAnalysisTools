#include "TH2D.h"
#include "TFile.h"

#include <iostream>
#include <cmath>
#include <vector>

std::vector<double> ReturnFineBinningFromCoarseBinning(int FineCoarseRatio, std::vector<double> CoarseBinning);
std::vector<double> linspace(double Emin, double Emax, int nDiv);

void oscillatorBinningChange(){

  TFile* oscillator_template = new TFile("./OscillogramTemplate.root","READ");
  TH2D* coarse_hist = (TH2D*)oscillator_template->Get("OscillogramTemplate_Coarse");

  TString x_axis_name = coarse_hist->GetXaxis()->GetTitle();
  TString y_axis_name = coarse_hist->GetYaxis()->GetTitle();
 
  int coarse_x_bins = coarse_hist->GetXaxis()->GetNbins();
  int coarse_y_bins = coarse_hist->GetYaxis()->GetNbins();
  std::cout << "Original coarse template: X " << coarse_x_bins << " bins; Y " << coarse_y_bins << " bins"<< std::endl;

  TH2D* fine_hist = (TH2D*)oscillator_template->Get("OscillogramTemplate_Fine");
  int fine_x_bins = fine_hist->GetXaxis()->GetNbins();
  int fine_y_bins = fine_hist->GetYaxis()->GetNbins();
  std::cout << "Original fine template: X " << fine_x_bins << " bins; Y " << fine_y_bins << " bins"<< std::endl;
  //get the lower bin edges from the original coarse oscillogram;
  //number of bin edges =  bin numbers + 1
  std::vector<double> coarse_x_edges(coarse_x_bins+1);
  std::vector<double> coarse_y_edges(coarse_y_bins+1);
  for(int i=0; i < coarse_x_bins+1; i++){
    double lower_edge = coarse_hist->GetXaxis()->GetBinLowEdge(i+1);
    std::cout << "Coarse x bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
    coarse_x_edges[i]=lower_edge;
  } 
  for(int i=0; i < coarse_y_bins+1; i++){
    double lower_edge = coarse_hist->GetYaxis()->GetBinLowEdge(i+1);
    std::cout << "Coarse y bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
    coarse_y_edges[i]=lower_edge;
  }
  //select the edges after merging X adjacent bins
  std::vector<double> new_coarse_x_edges;
  std::vector<double> new_coarse_y_edges;
  int merging_adj_bins = 2;  // how many adjacent bins to be merged

  for(int i=0; i< coarse_x_edges.size()-1; i++){
    std::cout << i%merging_adj_bins <<" "<< coarse_x_edges[i] <<std::endl;
    if(i%merging_adj_bins == 0){
      new_coarse_x_edges.push_back(coarse_x_edges[i]); 
    }  
  }
  new_coarse_x_edges.push_back(coarse_x_edges[coarse_x_bins]);  // add the upper limit back

  for(int i=0; i< coarse_y_edges.size()-1; i++){
    if(i%merging_adj_bins == 0){
      new_coarse_y_edges.push_back(coarse_y_edges[i]);
    }  
  }
  new_coarse_y_edges.push_back(coarse_y_edges[coarse_y_bins]);  // add the upper limit back
  std::cout << "Make new coarse oscillogram with " << new_coarse_x_edges.size()-1 << " bins in x "
	    << "and " << new_coarse_y_edges.size()-1 << " bins in y. " << std::endl;
  for(int i=0; i< new_coarse_x_edges.size(); i++){
    std::cout <<"New x lower edges: " << new_coarse_x_edges[i] << std::endl;
  }
  for(int i=0; i< new_coarse_y_edges.size(); i++){
    std::cout <<"New y lower edges: " << new_coarse_y_edges[i] << std::endl;
  }
  //based on the new coarse oscillogram to produce the new fine oscillogram template
  int fine_per_coarse = 40;
  std::vector<double> new_fine_x_edges;
  std::vector<double> new_fine_y_edges;
  
  new_fine_x_edges = ReturnFineBinningFromCoarseBinning(fine_per_coarse, new_coarse_x_edges); 
  new_fine_y_edges = ReturnFineBinningFromCoarseBinning(fine_per_coarse, new_coarse_y_edges);

  std::cout << "Make new fine oscillogram with " << new_fine_x_edges.size()-1 << " bins in x "
	    << "and " << new_fine_y_edges.size()-1 << " bins in y. " << std::endl;
  for(int i=0; i< new_fine_x_edges.size(); i++){
    std::cout <<"New fine x lower edges: " << new_fine_x_edges[i] << std::endl;
  }
  for(int i=0; i< new_fine_y_edges.size(); i++){
    std::cout <<"New fine y lower edges: " << new_fine_y_edges[i] << std::endl;
  }
  std::cout<<"Making histograms... "<< std::endl;
  //make histograms with the new binning edges
  double* h_new_coarse_x_edges = &new_coarse_x_edges[0];
  double* h_new_coarse_y_edges = &new_coarse_y_edges[0];
  double* h_new_fine_x_edges = &new_fine_x_edges[0];
  double* h_new_fine_y_edges = &new_fine_y_edges[0];

  TH2D* new_coarse_hist = new TH2D("hCoarse",Form(";%s;%s",x_axis_name.Data(),y_axis_name.Data()),
		                   new_coarse_x_edges.size()-1,h_new_coarse_x_edges,
				   new_coarse_y_edges.size()-1,h_new_coarse_y_edges);

  TH2D* new_fine_hist = new TH2D("hFine",Form(";%s;%s",x_axis_name.Data(),y_axis_name.Data()),
		                   new_fine_x_edges.size()-1,h_new_fine_x_edges,
				   new_fine_y_edges.size()-1,h_new_fine_y_edges);
  std::cout<<"DONE."<< std::endl;
  Int_t new_x_bins_coarse = new_coarse_hist->GetXaxis()->GetNbins();
  Int_t new_y_bins_coarse = new_coarse_hist->GetYaxis()->GetNbins();
  Int_t new_x_bins_fine = new_fine_hist->GetXaxis()->GetNbins();
  Int_t new_y_bins_fine = new_fine_hist->GetYaxis()->GetNbins();
  for(int i=0; i < new_x_bins_coarse+1; i++){
    Double_t lower_edge = new_coarse_hist->GetXaxis()->GetBinLowEdge(i+1);
    std::cout << "New coarse x bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  }
  for(int i=0; i < new_x_bins_fine+1; i++){
    Double_t lower_edge = new_fine_hist->GetXaxis()->GetBinLowEdge(i+1);
    std::cout << "New fine x bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  }

  for(int i=0; i < new_y_bins_coarse+1; i++){
    Double_t lower_edge = new_coarse_hist->GetYaxis()->GetBinLowEdge(i+1);
    std::cout << "New coarse y bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  }
  for(int i=0; i < new_y_bins_fine+1; i++){
    Double_t lower_edge = new_fine_hist->GetYaxis()->GetBinLowEdge(i+1);
    std::cout << "New fine y bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  }
  std::cout << "Create new oscillogram template with " << new_x_bins_coarse << " and " << new_y_bins_coarse << " for OscillogramTemplate_Coarse" << std::endl;
  std::cout << "Create new oscillogram template with " << new_x_bins_fine << " and " << new_y_bins_fine << " for OscillogramTemplate_Fine" << std::endl;

  /*
  Int_t new_x_bins_coarse = coarse_x_bins;
  Int_t new_y_bins_coarse = coarse_y_bins;

  Int_t new_x_bins_fine = new_x_bins_coarse * 4;
  Int_t new_y_bins_fine = new_y_bins_coarse * 4;

  TH2D* new_coarse_hist = new TH2D("OscillogramTemplate_Coarse","",new_x_bins_coarse,x_lower_limit,x_upper_limit,new_y_bins_coarse,y_lower_limit,y_upper_limit);
  TH2D* new_fine_hist = new TH2D("OscillogramTemplate_Fine","",new_x_bins_fine,x_lower_limit,x_upper_limit,new_y_bins_fine,y_lower_limit,y_upper_limit);
  
  new_coarse_hist->GetXaxis()->SetTitle(x_axis_name.Data());
  new_coarse_hist->GetYaxis()->SetTitle(y_axis_name.Data());
  new_fine_hist->GetXaxis()->SetTitle(x_axis_name.Data());
  new_fine_hist->GetYaxis()->SetTitle(y_axis_name.Data());
  */
  /*
  // make the copies of the two histograms with new objects 
  TH2D* new_coarse_hist = (TH2D*)coarse_hist->Clone();
  TH2D* new_fine_hist = (TH2D*)fine_hist->Clone();
  std::cout << "Copy done." << std::endl;
  //oscillator_template->Close();
  //Rebinning
  new_coarse_hist->Rebin2D();  // merges two bins along the xaxis and yaxis in one
  new_fine_hist->Rebin2D();
  std::cout << "Rebinning done." << std::endl;
  */

  TString new_template_name = Form("OscillogramTemplate_coarse_%ix%i_fine_%ix%i.root", new_x_bins_coarse, new_y_bins_coarse,new_x_bins_fine,new_y_bins_fine);
  std::cout << "New template: " << new_template_name << std::endl;
  
  TFile* new_template = new TFile(new_template_name.Data(),"RECREATE");
  new_coarse_hist->Write("OscillogramTemplate_Coarse");
  new_fine_hist->Write("OscillogramTemplate_Fine");

  new_template->Close();
  oscillator_template->Close();
  std::exit(0);
}

std::vector<double> ReturnFineBinningFromCoarseBinning(int FineCoarseRatio, std::vector<double> CoarseBinning){
  std::vector<double> ReturnVec;
  int nCoarse = (int)CoarseBinning.size()-1;   // number of coarse binnings
  for (int iCoarseBin=0;iCoarseBin<nCoarse;iCoarseBin++) {
    std::vector<double> tmpVec = linspace(CoarseBinning[iCoarseBin],CoarseBinning[iCoarseBin+1],FineCoarseRatio);

    for (int iFineBin=0;iFineBin<FineCoarseRatio;iFineBin++) {
      ReturnVec.push_back(tmpVec[iFineBin]);
    }
  }
  ReturnVec.push_back(CoarseBinning[nCoarse]);  //retrive the upperlimit

  return ReturnVec;
}

std::vector<double> linspace(double Emin, double Emax, int nDiv){
  if (nDiv==0) {
    std::cout << "Requested linear spacing distribution with 0 divisions" << std::endl;
    throw;
  }

  std::vector<double> linpoints(nDiv+1, 0.0);

  double step_lin = (Emax - Emin)/double(nDiv);
  double EE = Emin;		
  for (int i=0; i<nDiv; i++) {
    if (fabs(EE)<1e-6) {EE = 0.;}
			          
    linpoints[i] = EE;				      
    EE += step_lin;
  }

  linpoints[nDiv] = Emax;
  return linpoints;
}
