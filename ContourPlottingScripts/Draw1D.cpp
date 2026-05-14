#include "paul_tol_colors.hpp"

TCanvas *canv = NULL;
TString canvname;
TFile *f = NULL;
int FillColor = 0;

// 1,2,3 sigma or 68, 90, 99%
int type = 0;

void RemoveErrors(TH1D *h) {
  for (int i = 0; i < h->GetXaxis()->GetNbins(); ++i) {
    h->SetBinError(i+1, 0);
  }
}

void Draw(TString plotname) {

  TH1D *h = (TH1D*)f->Get(Form("Posteriors/%s", plotname.Data()));
  RemoveErrors(h);
  h->SetFillStyle(0);
  h->Draw();

  TH1D *h2 = (TH1D*)f->Get(Form("CredibleIntervals/1sig/%s", plotname.Data()));
  RemoveErrors(h2);
  h2->SetFillStyle(1001);
  h2->Draw("same");

  TH1D *h3 = (TH1D*)f->Get(Form("CredibleIntervals/2Sig/%s", plotname.Data()));
  if (type != 0) h3 = (TH1D*)f->Get(Form("CredibleIntervals/90/%s", plotname.Data()));
  RemoveErrors(h3);
  h3->SetFillStyle(1001);
  h3->Draw("same");

  TH1D *h4 = (TH1D*)f->Get(Form("CredibleIntervals/3Sig/%s", plotname.Data()));
  if (type != 0) h4 = (TH1D*)f->Get(Form("CredibleIntervals/99/%s", plotname.Data()));
  RemoveErrors(h4);
  h4->SetFillStyle(1001);
  h4->Draw("same");

  h->SetFillColorAlpha(FillColor, 1.0);
  h2->SetFillColorAlpha(FillColor, 0.9);
  h3->SetFillColorAlpha(FillColor, 0.6);
  h4->SetFillColorAlpha(FillColor, 0.3);

  h->SetLineColor(h->GetFillColor());
  h2->SetLineColor(h->GetFillColor());
  h3->SetLineColor(h->GetFillColor());
  h4->SetLineColor(h->GetFillColor());

  h->SetLineWidth(1);
  h2->SetLineWidth(1);
  h3->SetLineWidth(1);
  h4->SetLineWidth(1);

  h->Scale(1/h->Integral());
  h2->Scale(1/h->Integral());
  h3->Scale(1/h->Integral());
  h4->Scale(1/h->Integral());

  h->GetYaxis()->SetMaxDigits(1);
  h->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset()*1.2);

  TLegend *leg = new TLegend(0.55, 0.50, 0.95, 0.88);
  leg->SetFillStyle(0);
  leg->SetLineColorAlpha(0,0);
  //leg->SetHeader("#splitline{Credible intervals}{flat prior on sin#delta_{CP}}");
  leg->SetHeader("#splitline{Credible intervals}{flat prior on #delta_{CP}}");
  if (type == 0) {
    leg->AddEntry(h2, "1#sigma", "f");
    leg->AddEntry(h3, "2#sigma", "f");
    leg->AddEntry(h4, "3#sigma", "f");
    leg->AddEntry(h,      ">3#sigma", "f");
  } else {
    leg->AddEntry(h2, "68%", "f");
    leg->AddEntry(h3, "90%", "f");
    leg->AddEntry(h4, "99%", "f");
    leg->AddEntry(h,      ">99%", "f");
  }
  leg->SetTextSize(0.04);
  leg->SetNColumns(2);
  leg->SetTextAlign(10*1+2);

  TString title = h->GetTitle();
  if (title.Contains("Both")) {
    title.ReplaceAll("Both", "both");
    h->SetTitle(title);
  }

  if (plotname.Contains("dcp")) {
    leg->SetX1(0.52);
    leg->SetX2(0.9);
    leg->SetY1(0.5);
    leg->SetY2(0.85);
  } else if (plotname.Contains("dm3")) {
    h->GetXaxis()->SetTitle("#Deltam^{2}_{23} (eV^{2})");
    h->GetXaxis()->SetMaxDigits(1);
    if (plotname.Contains("_BH")) {
      leg->SetX1(0.18);
      leg->SetX2(0.7);
    } else {
      leg->SetX1(0.58);
    }
  } else if (plotname.Contains("th13")) {
    h->GetXaxis()->SetMaxDigits(1);
    leg->SetNColumns(1);
    leg->SetX1(0.17);
    leg->SetX2(0.4);
    leg->SetY1(0.5);
    leg->SetY2(0.87);
  } else if (plotname.Contains("th23")) {
    leg->SetNColumns(1);
    leg->SetX1(0.18);
    leg->SetX2(0.4);
    leg->SetY1(0.5);
    leg->SetY2(0.87);
    h->SetMaximum(h->GetMaximum()*1.2);
    /*
    leg->SetX1(0.57);
    leg->SetX2(0.85);
    leg->SetY1(0.55);
    */
  }
  leg->Draw("same");

  gPad->RedrawAxis();
  canv->Print(canvname);
}

void Draw1D(TString filename, TString dist) {

  f = new TFile(filename);

  canv = new TCanvas("canv", "canv", 1024, 1024);
  canvname = f->GetName();
  canvname.ReplaceAll(".root", dist);
  if (type == 0) canvname += "_sigmas";
  else canvname += "_689099";
  canvname += ".pdf";
  canv->Print(canvname+"[");

  canv->SetLeftMargin(canv->GetLeftMargin()*1.4);
  canv->SetRightMargin(canv->GetRightMargin());
  canv->SetBottomMargin(canv->GetBottomMargin()*0.7);

  tolcols::init();

  if      (dist.Contains("_NH")) FillColor = tolcols::kTLigOrange;
  else if (dist.Contains("_BH")) FillColor = tolcols::kTLigOlive;
  else if (dist.Contains("_IH")) FillColor = tolcols::kTLigLightBlue;

  Draw(dist);

  canv->Print(canvname+"]");
}

