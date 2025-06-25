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

  vector<Int_t> GetPixelIds(Int_t ent);
  vector<Float_t> GetEdeps(Int_t ent);
  Float_t GetTotalEdep(Int_t ent, Float_t &eUp, Float_t &eBot);

  Bool_t IsTriggerOK(Int_t ent, Int_t nTC);

private:
  void FCheckEvent();

  Int_t fNHits = 0;
  vector<Int_t> *fPixId = 0;
  vector<Float_t> *fEdep = 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CEAcdAnalysis_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
