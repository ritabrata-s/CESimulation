/********************************************************************************
 *
 * CEIOStreamer.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CEIOStreamer_h
#define CEIOStreamer_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "TString.h"
#include "TFile.h"
#include "TChain.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CEIOStreamer {
public:
  CEIOStreamer(TString inFile);
  ~CEIOStreamer();

  void SetInputFiles(TString val);

  void SetOutputFileName(TString val) {
    fOutputFileName = val;
  }

  TFile* GetOutputFile();

  TTree* GetPrimTree() {
    return (TTree*) fPrimChain;
  }

  TTree* GetTotalTree() {
    return (TTree*) fTotChain;
  }

  TTree* GetCalTree() {
    return (TTree*) fCalChain;
  }

  TTree* GetAcdTree() {
    return (TTree*) fAcdChain;
  }

private:
  TString fInputFileString = "";
  TString fOutputFileName = "";
  std::vector<TString> fInFileNames;

  TChain *fPrimChain = nullptr;
  TChain *fTotChain = nullptr;
  TChain *fCalChain = nullptr;
  TChain *fAcdChain = nullptr;
  TFile *fOutputFile = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CEIOStreamer_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
