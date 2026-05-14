double Binning_nBins_1D[n_OscParams] = {
  100,  //dcp
  150,  //th13
  150,  //th23
  400  //dm32
};

double Binning_nBins_2D[n_OscParams] = {
  60,  //dcp
  60,  //th13
  60,  //th23
  100  //dm32
};

double Binning_LBound[n_OscParams] = {  
  -1*TMath::Pi(), //dcp
  0.005,          //th13
  0.35,           //th23
  -3e-3           //dm32
};

double Binning_HBound[n_OscParams] = {
  TMath::Pi(), //dcp 
  0.05,        //th13
  0.65,        //th23
  3e-3         //dm32
};

//When including RC, change th13 range to use these values
double Binning_th13_wRC[2] = {
  0.018,
  0.026
};

//When NH only, change dm32 range to use these values
double Binning_dm32_NH[4] = {
  200,
  2.1e-3,
  2.9e-3,
  60     //2D Number of bins
};

//When IH only, change dm32 range to use these values
double Binning_dm32_IH[4] = {
  200,
  -2.9e-3,
  -2.1e-3,
  60      //2D Number of bins
};

std::vector<double> GetBinning(int Dimension, int Hierarchy, bool wRC, int XParam, int YParam) {

  std::vector<double> Binning;

  if (Dimension == 1) {
    double xnBins = Binning_nBins_1D[XParam];
    double xLBound = Binning_LBound[XParam];
    double xHBound = Binning_HBound[XParam];

    if ((XParam == k_th13) && wRC) {
      xLBound = Binning_th13_wRC[0];
      xHBound = Binning_th13_wRC[1];
    }

    if (XParam == k_dm32) {
      if (Hierarchy == k_NH) {
	xnBins = Binning_dm32_NH[0];
	xLBound = Binning_dm32_NH[1];
	xHBound = Binning_dm32_NH[2];
      } else if (Hierarchy == k_IH) {
	xnBins = Binning_dm32_IH[0];
	xLBound = Binning_dm32_IH[1];
	xHBound = Binning_dm32_IH[2];
      }
    }

    Binning.resize(3);
    Binning[0] = xnBins;
    Binning[1] = xLBound;
    Binning[2] = xHBound;
    
  } else if (Dimension == 2) {
    double xnBins = Binning_nBins_1D[XParam];
    double xLBound = Binning_LBound[XParam];
    double xHBound = Binning_HBound[XParam];

    double ynBins = Binning_nBins_1D[YParam];
    double yLBound = Binning_LBound[YParam];
    double yHBound = Binning_HBound[YParam];

    if ((XParam == k_th13) && wRC) {
      xLBound = Binning_th13_wRC[0];
      xHBound = Binning_th13_wRC[1];
    }

    if (XParam == k_dm32) {
      if (Hierarchy == k_NH) {
	xnBins = Binning_dm32_NH[3];
	xLBound = Binning_dm32_NH[1];
	xHBound = Binning_dm32_NH[2];
      } else if (Hierarchy == k_IH) {
	xnBins = Binning_dm32_IH[3];
	xLBound = Binning_dm32_IH[1];
	xHBound = Binning_dm32_IH[2];
      }
    }

    if ((YParam == k_th13) && wRC) {
      yLBound = Binning_th13_wRC[0];
      yHBound = Binning_th13_wRC[1];
    }

    if (YParam == k_dm32) {
      if (Hierarchy == k_NH) {
	ynBins = Binning_dm32_NH[3];
	yLBound = Binning_dm32_NH[1];
	yHBound = Binning_dm32_NH[2];
      } else if (Hierarchy == k_IH) {
	ynBins = Binning_dm32_IH[3];
	yLBound = Binning_dm32_IH[1];
	yHBound = Binning_dm32_IH[2];
      }
    }

    Binning.resize(6);
    Binning[0] = xnBins;
    Binning[1] = xLBound;
    Binning[2] = xHBound;
    Binning[3] = ynBins;
    Binning[4] = yLBound;
    Binning[5] = yHBound;

  }
  
  return Binning;
}
