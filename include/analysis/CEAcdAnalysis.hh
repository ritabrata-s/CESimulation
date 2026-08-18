/********************************************************************************
 *
 * CEAcdAnalysis.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CEAcdAnalysis_h
#define CEAcdAnalysis_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "TVector3.h"

#include "analysis/CEAnalysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CEAcdAnalysis: public CEAnalysis {
public:
  CEAcdAnalysis();
  ~CEAcdAnalysis();

  void Init();

  void BeginOfEvent(Int_t evt);
  void EndOfEvent(Int_t evt);

  vector<Int_t> GetPixelIds() {
    return *fPixId;
  }

  vector<Float_t> GetEdeps() {
    return fSmearedEdep;
  }

  Float_t GetTotalEdep(Float_t &eUp, Float_t &eBot);

  Bool_t IsTriggerOK(Int_t nTC);

private:
  void FCheckEvent();
  void FClearEvent();

  Int_t fCurEvt = -1;
  Int_t fEvtId = -1;
  Int_t fNHits = 0;
  vector<Int_t> *fPixId = 0;
  vector<Float_t> *fEdep = 0;
  vector<Float_t> fSmearedEdep;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CEAcdAnalysis_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
