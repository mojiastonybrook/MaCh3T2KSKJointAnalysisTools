#include "paul_tol_colors.hpp"

TCanvas *canv = NULL;
TString canvname;
TFile *f = NULL;
int FillColor = 0;
bool smooth = true;

// 1,2,3 sigma or 68, 90, 99%
int type = 0;

void RemoveErrors(TH2D *h) {
  for (int i = 0; i < h->GetXaxis()->GetNbins(); ++i) {
    h->SetBinError(i+1, 0);
  }
}

void Draw(TString plotname) {
  std::cout << plotname << std::endl;

  TH2D *h = (TH2D*)f->Get(Form("Posteriors/%s", plotname.Data()));
  h->SetFillColorAlpha(kRed+1, 0.9);
  h->SetLineWidth(0);
  RemoveErrors(h);
  h->Draw("colz");

  TH2D *h2 = (TH2D*)f->Get(Form("CredibleIntervals/1sig/%s", plotname.Data()));
  RemoveErrors(h2);
  h2->Draw("cont3 same");

  TH2D *h3 = (TH2D*)f->Get(Form("CredibleIntervals/2Sig/%s", plotname.Data()));
  if (type != 0) h3 = (TH2D*)f->Get(Form("CredibleIntervals/90/%s", plotname.Data()));
  RemoveErrors(h3);
  h3->Draw("cont3 same");

  TH2D *h4 = (TH2D*)f->Get(Form("CredibleIntervals/3Sig/%s", plotname.Data()));
  if (type != 0) h4 = (TH2D*)f->Get(Form("CredibleIntervals/99/%s", plotname.Data()));
  RemoveErrors(h4);
  h4->Draw("cont3 same");

  if (smooth) {
    h->Smooth();
    h2->Smooth();
    h3->Smooth();
    h4->Smooth();
  }

  h3->SetLineStyle(kDashed);
  h4->SetLineStyle(kDotted);

  h->SetLineColor(kBlack);
  h2->SetLineColor(kBlack);
  h3->SetLineColor(kBlack);
  h4->SetLineColor(kBlack);

  h->SetLineWidth(2);
  h2->SetLineWidth(2);
  h3->SetLineWidth(2);
  h4->SetLineWidth(2);

  h->Scale(1/h->Integral());
  h2->Scale(1/h->Integral());
  h3->Scale(1/h->Integral());
  h4->Scale(1/h->Integral());
  h->SetMinimum(-1E-10);

  h->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset()*1.35);
  h->GetZaxis()->SetTitleOffset(h->GetZaxis()->GetTitleOffset()*1.2);

  h->GetYaxis()->SetMaxDigits(1);
  h->GetZaxis()->SetMaxDigits(1);

  // Check axis
  TString xtitle = h->GetXaxis()->GetTitle();
  TString ytitle = h->GetYaxis()->GetTitle();
  if (xtitle.Contains("#Delta")) xtitle = "#Deltam^{2}_{23} (eV^{2})";
  else if (ytitle.Contains("#Delta")) ytitle = "#Deltam^{2}_{23} (eV^{2})";
  h->GetXaxis()->SetTitle(xtitle);
  h->GetYaxis()->SetTitle(ytitle);

  // Finally make the legend
  TLegend *leg = new TLegend(0, 0, 1, 1);
  leg->SetNColumns(4);
  leg->SetTextSize(0.04);
  leg->SetFillStyle(0);
  leg->SetLineColorAlpha(0,0);
  leg->SetHeader("Credible intervals, flat prior on #delta_{CP}");
  leg->AddEntry(h, "Prob.", "f");
  if (type == 0) {
    leg->AddEntry(h2, "1#sigma", "l");
    leg->AddEntry(h3, "2#sigma", "l");
    leg->AddEntry(h4, "3#sigma", "l");
  } else {
    leg->AddEntry(h2, "68%", "f");
    leg->AddEntry(h3, "90%", "f");
    leg->AddEntry(h4, "99%", "f");
  }

  if (plotname.Contains("h_dcp_th13")) {
    leg->SetX1(0.2);
    leg->SetX2(0.8);
    leg->SetY1(0.14);
    leg->SetY2(0.28);
  } else if (plotname.Contains("h_dcp_th23")) {
    leg->SetX1(0.2);
    leg->SetX2(0.8);
    leg->SetY1(0.14);
    leg->SetY2(0.28);
  } else if (plotname.Contains("h_dcp_dm32")) {
    if (plotname.Contains("_BH")) {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.4);
      leg->SetY2(0.6);
    } else {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.7);
      leg->SetY2(0.9);
    }
  } else if (plotname.Contains("h_th13_dcp")) {
    TString header = leg->GetHeader();
    TString start= header(0, header.Index(","));
    TString end = header(header.Index(",")+2, header.Length());
    leg->SetHeader(Form("#splitline{%s}{%s}", start.Data(), end.Data()));
    leg->SetNColumns(1);
    leg->SetX1(0.18);
    leg->SetX2(0.4);
    leg->SetY1(0.32);
    leg->SetY2(0.72);
    h->GetXaxis()->SetNdivisions(404);
  } else if (plotname.Contains("h_th13_th23")) {
    leg->SetX1(0.2);
    leg->SetX2(0.8);
    leg->SetY1(0.14);
    leg->SetY2(0.28);
    h->GetXaxis()->SetNdivisions(404);
  } else if (plotname.Contains("h_th13_dm32")) {
    if (plotname.Contains("_BH")) {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.4);
      leg->SetY2(0.6);
    } else {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.75);
      leg->SetY2(0.9);
    }
    h->GetXaxis()->SetNdivisions(404);
  } else if (plotname.Contains("h_th23_dcp")) {
    leg->SetX1(0.2);
    leg->SetX2(0.8);
    leg->SetY1(0.6);
    leg->SetY2(0.8);
  } else if (plotname.Contains("h_th23_dm32")) {
    if (plotname.Contains("_BH")) {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.4);
      leg->SetY2(0.6);
    } else {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.7);
      leg->SetY2(0.9);
    }
  } else if (plotname.Contains("h_dm32_dcp")) {
    if (plotname.Contains("_BH")) {
      leg->SetX1(0.18);
      leg->SetX2(0.7);
      leg->SetY1(0.63);
      leg->SetY2(0.85);
    } else {
      // Change the header
      TString header = leg->GetHeader();
      TString start= header(0, header.Index(","));
      TString end = header(header.Index(",")+2, header.Length());
      leg->SetHeader(Form("#splitline{%s}{%s}", start.Data(), end.Data()));
      leg->SetNColumns(1);
      leg->SetX1(0.19);
      leg->SetX2(0.4);
      leg->SetY1(0.4);
      leg->SetY2(0.77);
    }
    h->GetXaxis()->SetNdivisions(404);
  } else if (plotname.Contains("h_dm32_th13")) {
    if (plotname.Contains("_BH")) {
      TString header = leg->GetHeader();
      TString start= header(0, header.Index(","));
      TString end = header(header.Index(",")+2, header.Length());
      leg->SetHeader(Form("#splitline{%s}{%s}", start.Data(), end.Data()));
      leg->SetNColumns(1);
      leg->SetX1(0.28);
      leg->SetX2(0.6);
      leg->SetY1(0.3);
      leg->SetY2(0.7);
    } else {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.15);
      leg->SetY2(0.30);
    }
    h->GetXaxis()->SetNdivisions(404);
  } else if (plotname.Contains("h_dm32_th23")) {
    if (plotname.Contains("_BH")) {
      TString header = leg->GetHeader();
      TString start= header(0, header.Index(","));
      TString end = header(header.Index(",")+2, header.Length());
      leg->SetHeader(Form("#splitline{%s}{%s}", start.Data(), end.Data()));
      leg->SetNColumns(1);
      leg->SetX1(0.28);
      leg->SetX2(0.6);
      leg->SetY1(0.3);
      leg->SetY2(0.7);
      h->GetXaxis()->SetNdivisions(508);
    } else {
      leg->SetX1(0.2);
      leg->SetX2(0.8);
      leg->SetY1(0.15);
      leg->SetY2(0.30);
      h->GetXaxis()->SetNdivisions(404);
    }
  }

  gPad->RedrawAxis();
  leg->Draw("same");

  canv->Print(canvname);
}

void Draw2D(TString filename, TString dist) {

  f = new TFile(filename);

  canv = new TCanvas("canv", "canv", 1024, 1024);

  canv->SetLeftMargin(canv->GetLeftMargin()*1.6);
  canv->SetRightMargin(canv->GetRightMargin()*1.8);
  canv->SetBottomMargin(canv->GetBottomMargin()*0.8);

  canvname = f->GetName();
  canvname.ReplaceAll(".root", dist);
  canvname += Form("%o_smooth", smooth);
  if (type == 0) canvname += "_sigmas";
  else canvname += "_689099";
  canvname += ".pdf";
  canv->Print(canvname+"[");

  tolcols::init();
  
  gStyle->SetOptStat(0);
  gStyle->SetPadTickX(0);
  gStyle->SetPadTickY(0);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetPalette(kSunset);
  TColor::InvertPalette();

  Draw(dist);

  canv->Print(canvname+"]");
}

