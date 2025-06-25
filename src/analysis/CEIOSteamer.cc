/********************************************************************************
 *
 * CEIOStreamer.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CEIOStreamer.hh"
#include "TObjString.h"
#include<iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEIOStreamer::CEIOStreamer(TString inFile) :
    fInputFileString(inFile) {
  SetInputFiles(fInputFileString);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEIOStreamer::~CEIOStreamer() {
  delete fPrimChain;
  delete fTotChain;
  delete fCalChain;
  delete fAcdChain;
  fOutputFile->Close();
  delete fOutputFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEIOStreamer::SetInputFiles(TString val) {
  fInputFileString = val;
  fInFileNames.clear();

  // Get the filename from the string
  TObjArray *tx = fInputFileString.Tokenize("+");
  for (Int_t i = 0; i < tx->GetEntries(); i++)
    fInFileNames.push_back(((TObjString*) (tx->At(i)))->String());

  fPrimChain = new TChain("PRIMARY");
  fTotChain = new TChain("TOTAL");
  fCalChain = new TChain("CAL");
  fAcdChain = new TChain("ACD");

  for (auto str : fInFileNames) {
    fPrimChain->Add(str.Data());
    fTotChain->Add(str.Data());
    fCalChain->Add(str.Data());
    fAcdChain->Add(str.Data());
  }

  if (!fPrimChain || !fTotChain || !fCalChain || !fAcdChain) {
    printf("[CEIOStreamer::CEIOStreamer] Could not open the data chain for %s ..... \n\n", fInputFileString.Data());
    return;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TFile* CEIOStreamer::GetOutputFile() {
  // Check for the .root extension
  if (fOutputFileName.First(".root") < 0)
    fOutputFileName.Append(".root");

  if (!fOutputFile)
    fOutputFile = new TFile(fOutputFileName.Data(), "RECREATE");

  if (!fOutputFile || fOutputFile->IsZombie()) {
    printf("[CEIOStreamer::CEIOStreamer] Could not open the output file %s ..... \n\n", fOutputFile->GetName());
  }

  return fOutputFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
