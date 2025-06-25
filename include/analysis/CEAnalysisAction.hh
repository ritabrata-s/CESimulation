/********************************************************************************
 *
 * CEAnalysisAction.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 17 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CEAnalysisAction_h
#define CEAnalysisAction_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"

#include "analysis/CEPrimAnalysis.hh"
#include "analysis/CECalAnalysis.hh"
#include "analysis/CEAcdAnalysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CEAnalysisAction {
public:
  CEAnalysisAction();
  ~CEAnalysisAction();

  void SetInputFileName(TString val) {
    fInFileName = val;
  }

  void SetOutputFileName(TString val) {
    fOutFileName = val;
  }

  void SetGeomVersion(string val) {
    fGeomVer = val;
  }

  void SetTrigConfig(string val) {
    fTrigConf = val;
  }

  void BeginOfAnalysis();
  void EndOfAnalysis();

  void SetPrimEngBin(Int_t nB, Float_t minE, Float_t maxE);
  void SetDepEngBin(Int_t nB, Float_t minE, Float_t maxE);

  void AnaGamma(bool down);
  void AnalyzeBkg(TString type, Int_t dir); // type: particle type [gamma, proton, neutron, electron, positron]; dir: 0 (all), 1 (downward), 2 (upward)
//  void AnaIntrinsic(Float_t activityRate = 748774.21); // activityRate in #/s; sigTW in ns
  void AnaIntrinsic(bool bothLyr = true); // activityRate in #/s; sigTW in ns
  void AnalyzeGRB();
  void AnalyzeFiducialGRB();

  void CalcEffArea();

private:
  TString fInFileName = "";
  TString fOutFileName = "";
  TFile *fOutfile = nullptr;

  CEIOStreamer *fStreamer = nullptr;
  CEPrimAnalysis *fPrimAna = nullptr;
  CECalAnalysis *fCalAna = nullptr;
  CEAcdAnalysis *fAcdAna = nullptr;

  Int_t fNEntPrim = 0;
  Int_t fNEntCal = 0;
  Int_t fNEntAcd = 0;

  Int_t fNUpgoing, fNDngoing; // number of up/down going particles
  // number of particles cancelled for different trigger conditions
  // (1: total cal deposited energy > 0; 2: eng dep in top acds < 200 keV); 3: bottom acd eng dep = 0; 4: eng dep in cal up pixel > down pixel
  Int_t fNTrigCut1, fNTrigCut2, fNTrigCut3, fNTrigCut4, fNTrigCut5, fNTrigCut6, fNTrigCut7;

  TF1 *fPrimFunc = nullptr;
  Int_t fPNBin = 100;
  Float_t fPMinE = 10.;
  Float_t fPMaxE = 1.E5;
  vector<Float_t> fPEngB;
  Int_t fDNBin = 100;
  Float_t fDMinE = 10.;
  Float_t fDMaxE = 1.E5;
  vector<Float_t> fDEngB;

  string fGeomVer = "V1R0";
  string fTrigConf = "BT";
  Int_t fNPIXEL = 0;
  Float_t fDETRADIUS = 0;
  Float_t fSigTW = 0;
  Float_t fActivityRate = 0;
  Float_t fSrcRad = 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CEAnalysisAction_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
