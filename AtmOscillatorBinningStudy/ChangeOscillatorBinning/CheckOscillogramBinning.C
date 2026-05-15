#include "TH2D.h"
#include "TFile.h"

#include <iostream>
#include <cmath>
#include <vector>

void PrintBinEdges(std::vector<double> edges_array);
void CheckOscillogramBinning(){

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
  std::vector<double> coarse_x;
  std::vector<double> coarse_y;
  std::vector<double> fine_x;
  std::vector<double> fine_y;

  for(int i=0; i < coarse_x_bins+1; i++){
    double lower_edge = coarse_hist->GetXaxis()->GetBinLowEdge(i+1);
    coarse_x.push_back(lower_edge);
    std::cout << "Coarse x bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  } 
  for(int i=0; i < fine_x_bins+1; i++){
    double lower_edge = fine_hist->GetXaxis()->GetBinLowEdge(i+1);
    fine_x.push_back(lower_edge);
    std::cout << "Fine x bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  } 
  for(int i=0; i < coarse_y_bins+1; i++){
    double lower_edge = coarse_hist->GetYaxis()->GetBinLowEdge(i+1);
    coarse_y.push_back(lower_edge);
    std::cout << "Coarse y bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  } 
  for(int i=0; i < fine_y_bins+1; i++){
    double lower_edge = fine_hist->GetYaxis()->GetBinLowEdge(i+1);
    fine_y.push_back(lower_edge);
    std::cout << "Fine y bin: " << i+1 << " ; Lower Edge: " << lower_edge << std::endl;
  } 

  PrintBinEdges(coarse_x);
  PrintBinEdges(coarse_y);

  oscillator_template->Close();
  std::exit(0);
}

void PrintBinEdges(std::vector<double> edges_array){
  for (int i=0; i<edges_array.size();i++ ){
    std::cout << edges_array[i] << ", ";
  }
  std::cout<< std::endl;
}
