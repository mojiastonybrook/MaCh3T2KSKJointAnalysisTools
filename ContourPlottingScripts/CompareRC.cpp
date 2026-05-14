double sigma[4] = { 0.00, 0.68, 0.954, 0.9973 };

std::vector<TH1D*> GetIntervals(TH1D *h) {

  TH1D *htemp = (TH1D*)h->Clone(Form("htemp_%s", h->GetName()));
  std::vector<TH1D*> hists;
  hists.push_back((TH1D*)htemp->Clone(Form("h1_%s", h->GetName())));
  hists.push_back((TH1D*)htemp->Clone(Form("h2_%s", h->GetName())));
  hists.push_back((TH1D*)htemp->Clone(Form("h3_%s", h->GetName())));

  // Reset
  for (int i = 0; i < 3; ++i) {
    hists[i]->Reset();
    hists[i]->SetLineWidth(h->GetLineWidth());
    hists[i]->SetLineStyle(3);
  }

  double integral = h->Integral();
  double sum = 0;

  while (sum/integral < sigma[3]) {

    sum += htemp->GetMaximum();
    int maxbin = htemp->GetMaximumBin();

    // Loop over the sigma levels
    for (int i = 1; i < 4; ++i) {
      if (sum/integral < sigma[i] && 
          sum/integral >= sigma[i-1]) {
        hists[i-1]->SetBinContent(maxbin, htemp->GetBinContent(maxbin));
      }
    }

    htemp->SetBinContent(maxbin, 0.);
  }

  delete htemp;

  return hists;
}

void RemoveErrors(TH1D *h) {
  std::cout << h->GetName() << ": " << h->GetXaxis()->GetNbins() << std::endl;
  for (int i = 0; i < h->GetXaxis()->GetNbins()+1; ++i) {
    h->SetBinError(i+1, 0);
  }
}

void CompareRC(TString distname="h_th13_BH") {
  //TFile *f = new TFile("fine_binning/Contours_1D_wRC_BurnIn_80000.root");
  //TFile *f2 = new TFile("fine_binning/Contours_1D_woRC_BurnIn_80000.root");
  TFile *f = new TFile("new_binning/t2kskJF_summitlast_beluga_p100_t4_summitlarge_reduced_float_reweighted_smeared_0.0000360_1D_wRC_BurnIn_80000_rebin.root");
  //TFile *f2 = new TFile("new_binning/t2kskJF_summitlast_beluga_p100_t4_summitlarge_reduced_float_reweighted_smeared_0.0000360_1D_woRC_BurnIn_80000_rebin.root");
  TFile *f2 = new TFile("xsec_reweight_prior/Contours_1D_wRC_BurnIn_80000_xsec_reweight_test.root");
  //TFile *f2 = new TFile("Contours_1D_wRC_BurnIn_80000_flatsindcp_test.root");

  TCanvas *canv = new TCanvas("canv", "canv", 1024, 1024);
  canv->SetLeftMargin(canv->GetLeftMargin()*1.4);
  canv->SetRightMargin(canv->GetRightMargin()*0.8);
  canv->SetBottomMargin(canv->GetBottomMargin()*0.7);
  TString pdfname = Form("xsecprior_wRC_comp_%s.pdf", distname.Data());
  canv->Print(pdfname+"[");

  TH1D *h = (TH1D*)f->Get(Form("Posteriors/%s", distname.Data()));
  h->SetName(Form("%s_wRC", h->GetName()));
  TH1D *h2 = (TH1D*)f2->Get(Form("Posteriors/%s", distname.Data()));
  h2->SetName(Form("%s_woRC", h2->GetName()));

  RemoveErrors(h);
  RemoveErrors(h2);

  if (!distname.Contains("dm32")) {
    h->Rebin(2);
    h2->Rebin(2);
  } else {
    h2->GetXaxis()->SetMaxDigits(1);
    h2->GetXaxis()->SetTitle("#Deltam^{2}_{32} (eV^{2})");
  }

  h->Scale(1./h->Integral());
  h2->Scale(1./h2->Integral());

  /*
  // Need to extend the axes of wRC, set content to zero
  double maxaxis = h2->GetXaxis()->GetBinUpEdge(h2->GetXaxis()->GetNbins()+1);
  double minaxis = h2->GetXaxis()->GetBinLowEdge(1);
  // Get bin width from original histogram
  double binwidth = h->GetXaxis()->GetBinWidth(1);
  int nbins = (maxaxis-minaxis)/binwidth+1;
  // Need to modify upper range for new histogram to make it match original histogram's bin width
  double newmax = binwidth*nbins;

  TH1D *hcopy = new TH1D("hcopy", "hcopy", nbins, minaxis, newmax);
  for (int i = 0; i < nbins+1; ++i) {
  double binlow = hcopy->GetXaxis()->GetBinLowEdge(i+1);
  if (binlow < h->GetXaxis()->GetBinLowEdge(1) || binlow > h->GetXaxis()->GetBinLowEdge(h->GetXaxis()->GetNbins()+1)) {
  hcopy->SetBinContent(i+1, 0);
  } else {
  hcopy->SetBinContent(i+1, h->GetBinContent(h->FindBin(hcopy->GetXaxis()->GetBinCenter(i+1))));
  }
  }
  */

  //h=hcopy; 

  //hcopy->Draw();
  //h->SetLineColor(kRed);
  //h->Draw("same");
  //hcopy->GetXaxis()->SetRange(hcopy->GetXaxis()->FindBin(0.015), hcopy->GetXaxis()->FindBin(0.03));
  //canv->Print(pdfname);

  //hcopy->GetYaxis()->SetRangeUser(1E-5, hcopy->GetMaximum());
  //canv->SetLogy();
  //canv->Print(pdfname);
  // Update the pointer
  //h = hcopy;

  TString title = h2->GetTitle();
  title.ReplaceAll("Without reactor constraint, ", "");
  h2->SetTitle(title);
  h2->Draw();
  h->Draw("same");


  int col1 = tolcols::kTBriBlue;
  int col2 = tolcols::kTBriYellow;

  h->SetLineColor(kBlack);
  h->SetLineWidth(1);
  h->SetLineStyle(kSolid);

  h2->SetLineColor(kBlack);
  h2->SetLineWidth(2);
  h2->SetLineStyle(kSolid);

  // Also draw the 1 and 2 sigmas
  std::vector<TH1D*> intervals = GetIntervals(h);
  intervals[0]->SetFillColorAlpha(col1, 0.7);
  intervals[1]->SetFillColorAlpha(col1, 0.5);
  intervals[2]->SetFillColorAlpha(col1, 0.3);

  std::vector<TH1D*> intervals2 = GetIntervals(h2);
  intervals2[0]->SetFillColorAlpha(col2, 0.9);
  intervals2[1]->SetFillColorAlpha(col2, 0.5);
  intervals2[2]->SetFillColorAlpha(col2, 0.3);

  for (int i = 0; i < 3; ++i) {
    intervals2[i]->SetLineStyle(7);
    intervals2[i]->Draw("same");
  }
  for (int i = 0; i < 3; ++i) {
    intervals[i]->Draw("same");
  }

  double maximum = h->GetMaximum() > h2->GetMaximum() ? h->GetMaximum() : h2->GetMaximum();
  maximum*=1.1;

  h2->GetYaxis()->SetRangeUser(0, maximum);

  h->Draw("same");
  h2->Draw("same");

  h2->GetYaxis()->SetTitleOffset(h2->GetYaxis()->GetTitleOffset()*1.2);
  h2->GetYaxis()->SetMaxDigits(1);

  TLegend *leg = new TLegend(0.6, 0.6, 0.92, 0.85);
  leg->SetNColumns(2);
  leg->SetTextSize(0.03);
  //leg->AddEntry((TObject*)NULL, "wRC", "");
  //leg->AddEntry((TObject*)NULL, "woRC", "");
  TH1D *intervals_copy = (TH1D*)intervals[0]->Clone();
  intervals_copy->SetFillColorAlpha(intervals_copy->GetFillColor(), 1);
  intervals_copy->SetLineWidth(h->GetLineWidth());
  intervals_copy->SetLineStyle(h->GetLineStyle());

  TH1D *intervals2_copy = (TH1D*)intervals2[0]->Clone();
  intervals2_copy->SetFillColorAlpha(intervals2_copy->GetFillColor(), 1);
  intervals2_copy->SetLineWidth(h2->GetLineWidth());
  intervals2_copy->SetLineStyle(h2->GetLineStyle());

  leg->AddEntry(intervals_copy, "No weight", "f");
  leg->AddEntry(intervals2_copy, "Weighted", "f");
  //leg->AddEntry(intervals_copy, "With RC", "f");
  //leg->AddEntry(intervals2_copy, "Without RC", "f");
  //leg->AddEntry(intervals_copy, "Flat in #delta_{CP}", "f");
  //leg->AddEntry(intervals2_copy, "Flat in sin#delta_{CP}", "f");

  leg->AddEntry(intervals[0], "1#sigma", "lf");
  leg->AddEntry(intervals2[0], "1#sigma", "lf");

  leg->AddEntry(intervals[1], "2#sigma", "lf");
  leg->AddEntry(intervals2[1], "2#sigma", "lf");

  leg->AddEntry(intervals[2], "3#sigma", "lf");
  leg->AddEntry(intervals2[2], "3#sigma", "lf");

  // Change legend position depending on plot
  if (distname.Contains("th13")) {
    leg->SetX1(0.25);
    leg->SetX2(0.95);
    leg->SetY1(0.6);
    leg->SetY2(0.85);
    h2->GetYaxis()->SetRangeUser(0, maximum*1.3);
    h2->GetXaxis()->SetMaxDigits(1);
  } else if (distname.Contains("dcp")) {
    leg->SetX1(0.5);
    leg->SetY1(0.5);
  } else if (distname.Contains("dm32")) {
    if (distname.Contains("_BH")) {
      leg->SetX1(0.3);
      leg->SetX2(0.95);
      leg->SetY1(0.3);
      leg->SetY2(0.8);
    } else {
      leg->SetX1(0.32);
      leg->SetX2(0.95);
      leg->SetY1(0.55);
      leg->SetY2(0.88);
      h2->GetYaxis()->SetRangeUser(0, maximum*1.2);
    }
  } else if (distname.Contains("th23")) {
    leg->SetX1(0.3);
    leg->SetX2(0.95);
    leg->SetY1(0.67);
    leg->SetY2(0.88);
    h2->GetYaxis()->SetRangeUser(0, maximum*1.3);
  }

  leg->Draw("same");

  gPad->RedrawAxis();
  canv->Print(pdfname);

  //canv->SetLogy();
  //h2->GetYaxis()->SetRangeUser(1E-5, maximum);
  //canv->Print(pdfname);

  canv->Print(pdfname+"]");
}
