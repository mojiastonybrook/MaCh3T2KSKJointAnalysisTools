#include <cmath>
void hackBayesFactors() {
  //TFile *f = new TFile("../t2kskJF_summitlast_beluga_p100_t4_summitlarge_reduced_float_reweighted_smeared_0.0000360.root");
  TFile *f = new TFile("/home/clarence/work/mach3/skt2kjoint/burn_in_checks_asimov/T2KSK_JointFit_Asimov_Reduced_Floats_reweighted_smeared_0.0000360.root");
  TTree *t = (TTree*)f->Get("osc_posteriors");

  int burnin = 80E3;

  /*
  int total = t->GetEntries(Form("step > %i", burnin));
  int number = t->GetEntries(Form("dcp>0 && dcp<3.14 && step > %i", burnin));
  int numbersin = t->GetEntries(Form("sin(dcp) >= 0 && step > %i", burnin));

  std::cout << double(number)/double(total) << " " << double(numbersin)/double(total) << std::endl;
  */

  t->SetBranchStatus("*", false);

  t->SetBranchStatus("dcp", true);
  t->SetBranchStatus("step", true);
  t->SetBranchStatus("RCreweight", true);

  float dcp, rc;
  int step;
  t->SetBranchAddress("dcp", &dcp);
  t->SetBranchAddress("step", &step);
  t->SetBranchAddress("RCreweight", &rc);

  int total = 0;
  double total_weighted = 0;

  double nocpv = 0;
  double nocpv_weighted = 0;

  double sindcp_nocpv = 0;
  double sindcp_nocpv_weighted = 0;

  int nEntries = t->GetEntries();
  //int nEntries = 10000000;
  for (int i = 0; i < nEntries; ++i) {
    t->GetEntry(i);
    if (i % 1000000 == 0) {
      std::cout << i << "/" << nEntries << " (" << double(i)/nEntries * 100. << "%)" << std::endl;
    }
    if (step < burnin) continue;

    // Increase the totals
    total++;
    total_weighted += rc;

    if (dcp > 0 && dcp < M_PI) {
      nocpv++;
      nocpv_weighted += rc;
    }

    if (sin(dcp) > 0) {
      sindcp_nocpv++;
      sindcp_nocpv_weighted += rc;
    }
  }

  std::cout << std::setw(40) << "total, no RC: " << total << std::endl;
  std::cout << std::setw(40) << "total, RC: " <<  total_weighted << std::endl;
  std::cout << std::setw(40) << "*****" << std::endl;

  std::cout << std::setw(40) << "dcp > 0 && dcp < pi, no RC: " <<  nocpv << std::endl;
  std::cout << std::setw(40) << "dcp > 0 && dcp < pi, RC: " <<  nocpv_weighted << std::endl;

  std::cout << std::setw(40) << "sin(dcp) > 0, no RC: " <<  sindcp_nocpv << std::endl;
  std::cout << std::setw(40) << "sin(dcp) > 0, RC: " <<  sindcp_nocpv_weighted << std::endl;

  std::cout << std::setw(40) << "******" << std::endl;
  std::cout << std::setw(40) << "dcp > 0 && dcp < pi, no RC prob: " <<  nocpv/total << std::endl;
  std::cout << std::setw(40) << "dcp > 0 && dcp < pi, RC prob: " <<  nocpv_weighted/total_weighted << std::endl;

  std::cout << std::setw(40) << "sin(dcp) > 0, no RC prob: " <<  sindcp_nocpv/total << std::endl;
  std::cout << std::setw(40) << "sin(dcp) > 0, RC prob: " <<  sindcp_nocpv_weighted/total_weighted << std::endl;
}
