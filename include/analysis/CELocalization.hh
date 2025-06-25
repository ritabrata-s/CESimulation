/********************************************************************************
 *
 * CELocalization.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 13 Dec, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CELocalization_h
#define CELocalization_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//#include<iostream>

//#include "Rtypes.h"
#include "TMath.h"
#include "TH2F.h"

#include "TVector3.h"

//#include "analysis/CEIOStreamer.hh"
#include "analysis/CEAnalysis.hh"

using namespace TMath;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
// Constants and definitions

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CELocalization {
public:
  CELocalization();
  ~CELocalization();

  void SetPixPosFile(TString fName);
  void SetEffAreaFile(TString fName);
  void SetTemPosInfoFile(TString fName);
  void SetTemDataFile(TString fName);
  void SetBkgFile(TString fName);

  // Single reconstruction
  TVector3 CalcRecDirection(TString fName, TString oFile = "");

  // Batch reconstruction
  void SetSamPosInfoFile(TString fName);
  vector<pair<TVector3, TVector3>> MultiRecDirection(TString dPath);

private:
//  TString fDataPath; // location of general detector information
//  TString fPixPosInfoFile; // filename of outer pixel positions
  Int_t fNPIX = 0; // number of pixels
  vector<TVector3> fVPixPos; // pixel position vector container
  TFile *fEffAreaFile = nullptr; // effective area file
  vector<pair<Float_t, Float_t>> fVTemPos; // template source position container
  vector<pair<TVector3, TH2F*>> fVTemPosHist; // template source positions and 2d energy deposition histograms
  vector<pair<TString, TVector3>> fVSamPos; // sample source filename and position vector container
  vector<pair<Float_t, TVector3> > fVProb; // container for calculated probability and direction
  TH1F *fHBkgTot = nullptr;
  TH1F *fHBkgPix = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CELocalization_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
