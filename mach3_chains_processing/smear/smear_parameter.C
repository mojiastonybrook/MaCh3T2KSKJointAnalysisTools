// Parameter smearing script, should work for both reduced/non-reduced chains,
// for any parameter you wish. This script will not overwrite your precious
// chains, it will make a new file with a "_smeared.root" postfix.
// USAGE:
//
// smear_parameter("chain.root") 
//   smear dm23 from a reduced chain with gaus(0, 3.92429e-5)
//
// updatePrior("chain.root", "parameter", central, error) 
//   smear "parameter" (from either reduced or full chain) with gaus(central, error)
//   WARNIG: setting central to non-zero will not only smear the parameter, bur
//   also bias it...
//
// TODO:
//  * More cerr if e.g. branch not loaded (wrong branch name given)
//  * Sort out some memory issues, I'm sure it is possible have only one TFile
//    and TTree at the time... Probably because of CloneTree()?

void smear_parameter(std::string fin_str, std::string param="dm23", double central = 0.0, double error = 1.446e-5){

  // First get the in/out name
  std::size_t pos;
  pos = fin_str.find(".root");
  // This will be your file output name
  std::string sout = fin_str.substr(0,pos) + "_smeared.root";

  // Parameter value
  //Double_t parameter;
  Float_t parameter;

  // Load the file in
  TFile *fin= new TFile(fin_str.c_str(),"READ");

  // Load the ttree
  TTree *treeold; 
  TTree *tree;
  if (fin->Get("osc_posteriors")){
    std::cout << "Loading a reduced chain (osc_posteriors)" << std::endl;
    treeold = (TTree*)fin->Get("osc_posteriors");
  }
  else if (fin->Get("posteriors")) {
    std::cout << "Loading a non-reduced chain (posteriors)" << std::endl;
    treeold = (TTree*)fin->Get("posteriors");
  }
  else {
    std::cerr << "Make sure you're loading a chain from MaCh3..." << std::endl;
    std::exit(1);
  }

  // Set the parameter branch, make a new Tfile and clone the old TTree
  treeold->SetBranchAddress(param.c_str(), &parameter);
  TFile *fout= new TFile(sout.c_str(),"recreate");
  tree = treeold->CloneTree(0);

  // Now let's iterate through the chain and smear
  Long64_t nentries = treeold->GetEntries();
  TRandom3 *gRandom = new TRandom3();
  std::cout << "Smearing..." << std::endl;
  for (Long64_t i = 0; i < nentries; ++i) {
    // Entry from the old chain
    treeold->GetEntry(i);

    // Smear it
    parameter = parameter + gRandom->Gaus(0, error);

    // Fill to the new chain
    tree->Fill();
  }

  // Save. The end.
  tree->AutoSave();
  fout->Close();
  std::cout << "Done!" << std::endl;
}
