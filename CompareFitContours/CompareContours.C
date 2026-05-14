#include <vector>
#include <iostream>

#include "TString.h"
#include "TObjString.h"
#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TCollection.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

enum IntervalType{k_CredibleInterval,k_ConfidenceInterval,k_Posterior,n_IntervalTypes};
enum Dimension{k_1D=1,k_2D=2,n_Dimensions};
enum OscParams{k_dcp, k_th13, k_th23, k_dm32, n_OscParams};
enum Hierarchy{k_BH, k_NH, k_IH, n_Hierarchies};
enum Intervals{k_0, k_1sig, k_90, k_2sig, k_99, k_3sig, k_4sig, n_Intervals};
enum LegendPos{kLegendTopLeft,kLegendTopMiddle,kLegendTopRight,kLegendMiddleRight};

std::vector<TString> ReturnStoredHistograms(TFile* File, int Dimension, std::vector<TString> IntervalTypeNames, int IntervalType);
void CheckHistogramNames(std::vector<TString> HistNames_File1, std::vector<TString> HistNames_File2);
void CompareContours(int Dimension, int IntervalTypeToPlot, std::vector<TString> FileNames, std::vector<TString> FileLegendEntries, TString AdditionalNameInfo) {
  //--------------------------------------------------------------------------------------------------------

  int MaxNFiles = 3;
  std::vector<int> FileColors(MaxNFiles);
  FileColors[0] = kBlack;
  FileColors[1] = kRed;
  FileColors[2] = kBlue;

  //Used in Histogram Name
  std::vector<TString> IntervalTypeNames(n_IntervalTypes);
  IntervalTypeNames[k_CredibleInterval] = "CredibleIntervals";
  IntervalTypeNames[k_ConfidenceInterval] = "ConfidenceIntervals";
  IntervalTypeNames[k_Posterior] = "Posteriors";

  //Used in Histogram Name (Not Title)
  std::vector<TString> IntervalTypeCaptions(n_IntervalTypes);
  IntervalTypeCaptions[k_CredibleInterval] = "Credible";
  IntervalTypeCaptions[k_ConfidenceInterval] = "Confidence";
  IntervalTypeCaptions[k_Posterior] = "Posterior";

  //Used in Directory Name
  std::vector<TString> IntervalName(n_Intervals);
  IntervalName[k_0] = "All";
  IntervalName[k_1sig] = "1sig";
  IntervalName[k_90] = "90";
  IntervalName[k_2sig] = "2Sig";
  IntervalName[k_99] = "99";
  IntervalName[k_3sig] = "3Sig";
  IntervalName[k_4sig] = "4Sig";

  //Histogram Line Style
  std::vector<int> IntervalStyle(n_Intervals);
  gStyle->SetLineStyleString(11,"2 4");
  IntervalStyle[k_0] = 1;
  IntervalStyle[k_1sig] = 1;
  IntervalStyle[k_90] = 3;//2;//3;
  IntervalStyle[k_2sig] = 3;//2;//3;
  IntervalStyle[k_99] = 2;//11;//2;
  IntervalStyle[k_3sig] = 2;//11;//2;
  IntervalStyle[k_4sig] = 2;

  //Legend Entries
  std::vector<TString> IntervalCaption(n_Intervals);
  IntervalCaption[k_0] = "Posterior";
  IntervalCaption[k_1sig] = "68% "+IntervalTypeCaptions[IntervalTypeToPlot]+" Interval";
  IntervalCaption[k_90] = "90% "+IntervalTypeCaptions[IntervalTypeToPlot]+" Interval";
  IntervalCaption[k_2sig] = "2#sigma "+IntervalTypeCaptions[IntervalTypeToPlot]+" Interval";
  IntervalCaption[k_99] = "99% "+IntervalTypeCaptions[IntervalTypeToPlot]+" Interval";
  IntervalCaption[k_3sig] = "3#sigma "+IntervalTypeCaptions[IntervalTypeToPlot]+" Interval";
  IntervalCaption[k_4sig] = "4#sigma "+IntervalTypeCaptions[IntervalTypeToPlot]+" Interval";

  //Which contours do you want to overlay per canvas
  std::vector< std::vector<int> > DrawGroups(2);
  
  std::vector<int> DrawGroup_1(3);
  DrawGroup_1[2] = k_1sig;
  DrawGroup_1[1] = k_90;
  DrawGroup_1[0] = k_99;
  
  /*
  std::vector<int> DrawGroup_1(2);
  DrawGroup_1[1] = k_1sig;
  DrawGroup_1[0] = k_90;
  */
  DrawGroups[0] = DrawGroup_1;
  
  std::vector<int> DrawGroup_2(3);
  DrawGroup_2[2] = k_1sig;
  DrawGroup_2[1] = k_2sig;
  DrawGroup_2[0] = k_3sig;
  
  /*
  std::vector<int> DrawGroup_2(2);
  DrawGroup_2[1] = k_1sig;
  DrawGroup_2[0] = k_2sig;
  */
  DrawGroups[1] = DrawGroup_2;
  
  //--------------------------------------------------------------------------------------------------------
  bool ValidDimension = false;
  for (int iDimension=0;iDimension<n_Dimensions;iDimension++) {
    if (Dimension == iDimension) {
      ValidDimension = true;
      break;
    }
  }
  if (!ValidDimension) {
    std::cout << "Invalid Dimesion asked for:" << Dimension << std::endl;
    std::exit(-1);
  }

  int nFiles = FileNames.size();
  if (nFiles != (int)FileLegendEntries.size()) {
    std::cout << "Number of file names given:" << FileNames.size() << std::endl;
    std::cout << "Number of Legend Entries given:" << FileLegendEntries.size() << std::endl;
    std::exit(-1);
  }
  if (nFiles > MaxNFiles) {
    std::cout << "Too many files specified. Increase 'MaxNFiles' and 'FileColors' to include " << nFiles << " elements" << std::endl;
    std::exit(-1);
  }

  std::vector<TFile*> Files(nFiles);
  for (int iFile=0;iFile<nFiles;iFile++) {
    Files[iFile] = new TFile(FileNames[iFile]);

    if (!Files[iFile] || Files[iFile]->IsZombie()) {
      std::cout << "File name:" << FileNames[iFile] << " not found!" << std::endl;
      std::exit(-1);
    }

    std::cout << "Found File " << iFile << " : " << FileNames[iFile] << std::endl;
  }

  std::cout << "Comparing " << nFiles << " files" << std::endl;
  std::cout << "Comparing " << Dimension << "D Contours" << std::endl;  

  std::vector< std::vector<TString> > HistNamesPerFile(nFiles);
  for (int iFile=0;iFile<nFiles;iFile++) {
    HistNamesPerFile[iFile] = ReturnStoredHistograms(Files[iFile],Dimension,IntervalTypeNames,IntervalTypeToPlot);
  }
  for (int iFile=1;iFile<nFiles;iFile++) {
    CheckHistogramNames(HistNamesPerFile[0],HistNamesPerFile[iFile]);
  }

  int nHists = HistNamesPerFile[0].size();
  std::cout << "Each file contains: " << nHists << " histograms" << std::endl;

  //--------------------------------------------------------------------------------------------------------

  std::vector<int> HistType(nHists);
  for (int iHist=0;iHist<nHists;iHist++) {
    for (int iHistType=0;iHistType<n_IntervalTypes;iHistType++) {
      if (HistNamesPerFile[0][iHist].Contains(IntervalTypeNames[iHistType])) {
	HistType[iHist] = iHistType;
      }
    }
  }

  int DummyVal = -2;
  std::vector<int> IntervalType(nHists,DummyVal);
  for (int iHist=0;iHist<nHists;iHist++) {
    if (HistType[iHist] == k_Posterior) {
      IntervalType[iHist] = -1;
    } else {
      for (int iIntervalType=0;iIntervalType<n_Intervals;iIntervalType++) {
	if (HistNamesPerFile[0][iHist].Contains(IntervalName[iIntervalType])) {
	  IntervalType[iHist] = iIntervalType;
	}
      }
    }
  }
  for (int iHist=0;iHist<nHists;iHist++) {
    if (IntervalType[iHist] == DummyVal) {
      std::cout << "Interval type of Hist:" << HistNamesPerFile[0][iHist] << " was not found correctly" << std::endl;
      std::exit(-1);
    }
  }

  std::vector<TString> ContourType(nHists);
  for (int iHist=0;iHist<nHists;iHist++) {
    TString HistName = HistNamesPerFile[0][iHist];
    TObjArray* HistName_Tok = HistName.Tokenize("/");
    TString ContourName = (TString)(((TObjString *)(HistName_Tok->At(HistName_Tok->GetEntries()-1)))->String());
    ContourName = ContourName.Remove(0,2);
    ContourType[iHist] = ContourName;
  }

  std::vector<TString> UniqueContourType;
  for (int iContour=0;iContour<nHists;iContour++) {
    bool foundContourName = false;
    for (int iUniqueContour=0;iUniqueContour<(int)UniqueContourType.size();iUniqueContour++) {
      if (ContourType[iContour] == UniqueContourType[iUniqueContour]) {
	foundContourName = true;
	break;
      }
    }
    if (!foundContourName) {
      UniqueContourType.push_back(ContourType[iContour]);
    }
  }

  int nUniqueContourTypes = UniqueContourType.size();
  std::cout << "Found " << nUniqueContourTypes << " unique contour types" << std::endl;

  if (nUniqueContourTypes*(n_Intervals+1) != nHists) {
    std::cout << "One of the ";
    for (int iHistType=0;iHistType<n_IntervalTypes;iHistType++) {
      std::cout << IntervalTypeNames[iHistType] << ", ";
    }
    std::cout << " directories has a different number of histograms than the others" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------

  std::vector<TH1D*> LegendHistograms_Files(nFiles);
  for (int iFile=0;iFile<nFiles;iFile++) {
    LegendHistograms_Files[iFile] = new TH1D(Form("LegendHistograms_Files_%i",iFile),"",1,0,1);
    LegendHistograms_Files[iFile]->SetLineColor(FileColors[iFile]);
  }

  std::vector<TH1D*> LegendHistograms_Intervals(n_Intervals);
  for (int iInterval=0;iInterval<n_Intervals;iInterval++) {
    LegendHistograms_Intervals[iInterval] = new TH1D(Form("LegendHistograms_Interval_%i",iInterval),"",1,0,1);
    LegendHistograms_Intervals[iInterval]->SetLineStyle(IntervalStyle[iInterval]);
    LegendHistograms_Intervals[iInterval]->SetLineWidth(2);
  }

  TCanvas* Canvas = new TCanvas("Canvas","");
  TH1D* Hist_1D_Posterior;
  TH1D* Hist_1D_Posterior_tmp;
  TH1D* Hist_1D_Interval;
  TH1D* Hist_1D_Interval_tmp;
  std::vector<TH1D*> Hist_1D_Vec;
  std::vector<double> Hist_1D_Vec_Integral;
  TH2D* Hist_2D;

  for (int iUniqueContour=0;iUniqueContour<nUniqueContourTypes;iUniqueContour++) { 
    for (int iDrawGroup=0;iDrawGroup<(int)DrawGroups.size();iDrawGroup++) {

	TString PDFName = "ContourComparison_";
	if (Dimension == k_1D) {
	  PDFName += "1D_";
	} else {
	  PDFName += "2D_";
	}
	PDFName += UniqueContourType[iUniqueContour]+"_"+Form("%i",iDrawGroup);
	if (AdditionalNameInfo!="") {
	  PDFName += "_"+AdditionalNameInfo;
	} 
	PDFName += ".pdf";

      TLegend* Legend = new TLegend(0.5,0.7,0.89,0.89);

      Legend->SetLineColor(kWhite);
      Legend->SetFillColor(kWhite);
      Legend->SetFillStyle(0);
      Legend->SetTextFont(132);
      Legend->SetTextSize(0.04);
      Legend->SetBorderSize(0);

      for (int iFile=0;iFile<nFiles;iFile++) {
	Legend->AddEntry(LegendHistograms_Files[iFile],FileLegendEntries[iFile],"l");
      }
      for (int iDrawGroupInt=0;iDrawGroupInt<(int)DrawGroups[iDrawGroup].size();iDrawGroupInt++) {
	Legend->AddEntry(LegendHistograms_Intervals[DrawGroups[iDrawGroup][iDrawGroupInt]],IntervalCaption[DrawGroups[iDrawGroup][iDrawGroupInt]],"l");
      }

      Hist_1D_Vec.clear();
      Hist_1D_Vec_Integral.clear();
      Hist_1D_Vec_Integral.resize(nFiles);

      //If 1D, grab all hists from file so we can TH1D->SetMaximum()
      if (Dimension == k_1D) {
	for (int iFile=0;iFile<nFiles;iFile++) {
	  
	  TString HistName = IntervalTypeNames[k_Posterior]+"/h_"+UniqueContourType[iUniqueContour];
	  Hist_1D_Posterior_tmp = (TH1D*)Files[iFile]->Get(HistName);
	  if (!Hist_1D_Posterior_tmp) {
	    std::cout << "Histogram name " << HistName << " not found in File:" << FileNames[iFile] << std::endl;
	    std::exit(-1);
	  }
	  //JJ: Normailize the hists from files
	  //Hist_1D_Posterior->Scale(1.0/Hist_1D_Posterior->Integral("width"));
	  Hist_1D_Posterior = (TH1D*)Hist_1D_Posterior_tmp->Clone();
	  Hist_1D_Posterior->SetLineColor(FileColors[iFile]);
	  Hist_1D_Posterior->SetLineWidth(2);
	  Hist_1D_Posterior->SetFillStyle(0);
	  Hist_1D_Vec_Integral[iFile] = Hist_1D_Posterior->Integral("width");
	  Hist_1D_Posterior->Scale(1.0/Hist_1D_Vec_Integral[iFile]);
	  Hist_1D_Vec.push_back(Hist_1D_Posterior);
	}
            
	double Max = -1e8;
	double Min = 1e8;
	
	for (int iFile=0;iFile<nFiles;iFile++) { 
	  if (Hist_1D_Vec[iFile]->GetMaximum() > Max) Max = Hist_1D_Vec[iFile]->GetMaximum();
	  if (Hist_1D_Vec[iFile]->GetMinimum() < Min) Min = Hist_1D_Vec[iFile]->GetMinimum();
	}
	
	for (int iFile=0;iFile<nFiles;iFile++) {
	  Hist_1D_Vec[iFile]->SetMaximum(Max*1.1);
	  Hist_1D_Vec[iFile]->SetMinimum(Min);
	  
	  if (iFile==0) {
	    Hist_1D_Vec[iFile]->Draw("HIST");
	  } else {
	    Hist_1D_Vec[iFile]->Draw("HIST SAME");
	  }
	}
      }
    
      for (int iFile=0;iFile<nFiles;iFile++) {
	//JJ: grab the posterior hists to calculate the true integral values for normalization
	//TString HistName = IntervalTypeNames[k_Posterior]+"/h_"+UniqueContourType[iUniqueContour];
	//Hist_1D_Posterior_tmp = (TH1D*)Files[iFile]->Get(HistName);
	//Hist_1D_Posterior = (TH1D*)Hist_1D_Posterior_tmp->Clone();
	//if (!Hist_1D_Posterior) {
	//  std::cout << "Histogram name " << HistName << " not found in File:" << FileNames[iFile] << std::endl;
	//  std::exit(-1);
	//}
	for (int iDrawGroupInt=0;iDrawGroupInt<(int)DrawGroups[iDrawGroup].size();iDrawGroupInt++) {
	  TString HistName = IntervalTypeNames[IntervalTypeToPlot]+"/"+IntervalName[DrawGroups[iDrawGroup][iDrawGroupInt]]+"/h_"+UniqueContourType[iUniqueContour];
	  
	  if (Dimension == k_1D) {
	    Hist_1D_Interval_tmp = (TH1D*)Files[iFile]->Get(HistName);
	    if (!Hist_1D_Interval_tmp) {
	      std::cout << "Histogram name " << HistName << " not found in File:" << FileNames[iFile] << std::endl;
	      std::exit(-1);
	    }
	    //JJ: Normalize the hists from files
	    //Hist_1D_Interval->Scale(1.0/Hist_1D_Posterior->Integral("width"));
	    Hist_1D_Interval = (TH1D*)Hist_1D_Interval_tmp->Clone();
	    Hist_1D_Interval->SetLineColor(FileColors[iFile]);
	    Hist_1D_Interval->SetLineStyle(IntervalStyle[DrawGroups[iDrawGroup][iDrawGroupInt]]);
	    Hist_1D_Interval->SetLineWidth(1);
	    Hist_1D_Interval->SetFillStyle(0);
	    Hist_1D_Interval->Scale(1.0/Hist_1D_Vec_Integral[iFile]);
	    Hist_1D_Interval->SetMinimum(0);	    
	    //Hist_1D_Interval->Draw("P SAME");
	    Hist_1D_Interval->Draw("HIST SAME");
	  }
	  if (Dimension == k_2D) {
	    Hist_2D = (TH2D*)Files[iFile]->Get(HistName);
	    if (!Hist_2D) {
	      std::cout << "Histogram name " << HistName << " not found in File:" << FileNames[iFile] << std::endl;
	      std::exit(-1);
	    }
	    //Hist_2D->Rebin2D();
	    Hist_2D->SetLineColor(FileColors[iFile]);
	    //if (iDrawGroupInt == 2) Hist_2D->SetLineStyleString(3,"4 16");
	    Hist_2D->SetLineStyle(IntervalStyle[DrawGroups[iDrawGroup][iDrawGroupInt]]);
	    //if (iDrawGroupInt == 2) Hist_2D->SetLineWidth(3);
	    //std::cout << "Name: " << Hist_2D->GetName() << " from file number " << iFile << " | LineStyle is :" << IntervalStyle[DrawGroups[iDrawGroup][iDrawGroupInt]] << std::endl;
 
	    if ((iFile==0)&&(iDrawGroupInt==0)) {
	      Hist_2D->Draw("CONT3");
	    } else {
	      Hist_2D->Draw("SAME CONT3");
	    }
	    //Hist_2D->SetLineStyle(IntervalStyle[DrawGroups[iDrawGroup][iDrawGroupInt]]);
	  }
	}
      }    
      Legend->Draw("SAME");
      Canvas->Print(PDFName);

    }
  }

}



std::vector<TString> ReturnHistogramNamesFromDirectory(TDirectory* Directory, int Dimension) {
  std::vector<TString> HistNames;

  TIter DirIter(Directory->GetListOfKeys());
  TString HistName;
  TKey* Key;
  TClass* Class;

  for (int iKey=0;iKey<Directory->GetNkeys();iKey++) {
    Key = (TKey*)DirIter();
    Class = Class->GetClass(Key->GetClassName());
    HistName = Key->GetName();
    std::cout << "JJDebug: " << HistName <<std::endl;

    if ((Dimension == k_1D)&&(Class->InheritsFrom("TH1D"))) {
      HistNames.push_back(HistName);
      std::cout << "JJDebug: blabla" <<std::endl;
    }
    if ((Dimension == k_2D)&&(Class->InheritsFrom("TH2D"))) {
      HistNames.push_back(HistName);
    }
  }

  return HistNames;
}

std::vector<TString> ReturnStoredHistograms(TFile* File, int Dimension, std::vector<TString> IntervalTypeNames, int IntervalType) {
  std::vector<TString> ReturnVec;

  std::vector<TString> DirectoryNames;
  TDirectory* Directory;
  
  //Get Posteriors Directory
  Directory = (TDirectory*)File->Get(IntervalTypeNames[k_Posterior]);
  if (!Directory) {
    std::cout << IntervalTypeNames[k_Posterior] << " directory not found!" << std::endl;
    std::exit(-1);
  }
  DirectoryNames.push_back(IntervalTypeNames[k_Posterior]);

  //Get SubDirectories of 'Intervals' Directory
  Directory = (TDirectory*)File->Get(IntervalTypeNames[IntervalType]);
  if (!Directory) {
    std::cout << IntervalTypeNames[IntervalType] << " directory not found!" << std::endl;
    std::exit(-1);
  }

  TIter DirIter(Directory->GetListOfKeys());
  TString DirName;
  TKey* Key;
  TClass* Class;

  for (int iKey=0;iKey<Directory->GetNkeys();iKey++) {
    Key = (TKey*)DirIter();
    Class = Class->GetClass(Key->GetClassName());
    DirName = Key->GetName();

    if (Class->InheritsFrom("TDirectory")) {
      DirectoryNames.push_back(IntervalTypeNames[IntervalType]+"/"+DirName);
    }
  }

  for (int iDir=0;iDir<(int)DirectoryNames.size();iDir++) {
    Directory = (TDirectory*)File->Get(DirectoryNames[iDir]);
    if (!Directory) {
      std::cout << "Did not find directory:" << DirectoryNames[iDir] << std::endl;
      std::exit(-1);
    }

    std::vector<TString> HistNameFromDirectory = ReturnHistogramNamesFromDirectory(Directory,Dimension);
    std::cout << "JJDebug: " << HistNameFromDirectory.size() << std::endl;
    for (int iHistName=0;iHistName<(int)HistNameFromDirectory.size();iHistName++) {
      TString HistName = DirectoryNames[iDir]+"/"+HistNameFromDirectory[iHistName];
      std::cout << "JJDebug: " << DirectoryNames[iDir] << ";" << HistNameFromDirectory[iHistName] << ";" << HistName << std::endl;
      ReturnVec.push_back(HistName);
    }
  }

  return ReturnVec;
}

void CheckHistogramNames(std::vector<TString> HistNames_File1, std::vector<TString> HistNames_File2) {
  if (HistNames_File1.size() != HistNames_File2.size()) {
    std::cout << "Different number of histograms found!" << std::endl;
    std::exit(-1);
  }

  for (int iHistName=0;iHistName<(int)HistNames_File1.size();iHistName++) {
    bool foundName = false;
    for (int jHistName=0;jHistName<(int)HistNames_File2.size();jHistName++) {
      if (HistNames_File1[iHistName] == HistNames_File2[jHistName]) {
	foundName = true;
	break;
      }
    }
    if (!foundName) {
      std::cout << "Found a name not contained in both files:" << HistNames_File1[iHistName] << std::endl;
      std::exit(-1);
    }
  }
}

void CompareContours(int Dimension, int IntervalType, TString FileName1, TString LegendEntry1, TString FileName2, TString LegendEntry2, TString AdditionalNameInfo="") {
  std::vector<TString> FileNames(2);
  FileNames[0] = FileName1;
  FileNames[1] = FileName2;

  std::vector<TString> LegendEntries(2);
  LegendEntries[0] = LegendEntry1;
  LegendEntries[1] = LegendEntry2;

  CompareContours(Dimension,IntervalType,FileNames,LegendEntries,AdditionalNameInfo);
}

void CompareContours(int Dimension, int IntervalType, TString FileName1, TString LegendEntry1, TString FileName2, TString LegendEntry2, TString FileName3, TString LegendEntry3, TString AdditionalNameInfo="") {
  std::vector<TString> FileNames(3);
  FileNames[0] = FileName1;
  FileNames[1] = FileName2;
  FileNames[2] = FileName3;

  std::vector<TString> LegendEntries(3);
  LegendEntries[0] = LegendEntry1;
  LegendEntries[1] = LegendEntry2;
  LegendEntries[2] = LegendEntry3;

  CompareContours(Dimension,IntervalType,FileNames,LegendEntries,AdditionalNameInfo);
}
