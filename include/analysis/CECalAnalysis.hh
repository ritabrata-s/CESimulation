/********************************************************************************
 *
 * CECalAnalysis.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CECalAnalysis_h
#define CECalAnalysis_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "TVector3.h"

#include "analysis/CEAnalysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CECalAnalysis: public CEAnalysis {
public:
  CECalAnalysis();
  ~CECalAnalysis();

  void Init();

  Int_t GetNHits(Int_t ent);
  vector<Int_t> GetPixelIds(Int_t ent);
  vector<Float_t> GetEdeps(Int_t ent);
  vector<TVector3> GetEdepPos(Int_t ent);
  vector<Float_t> GetEdepTimes(Int_t ent);
  Float_t GetTotalEdep(Int_t ent, Float_t &eUp, Float_t &eBot);
  void GetSortedEdeps(Int_t ent, vector<Float_t> &edepV, vector<Int_t> &pixV);

  Bool_t IsTriggerOK(Int_t ent, Int_t nTC);

private:
  void FCheckEvent();

  Int_t fNHits = 0;
  vector<Int_t> *fPixId = 0;
  vector<Float_t> *fEdep = 0;
  vector<Float_t> *fEdepDel = 0;
  vector<Float_t> *fPosX = 0;
  vector<Float_t> *fPosY = 0;
  vector<Float_t> *fPosZ = 0;
  vector<Float_t> *fTime = 0;
  vector<TVector3> fEdepPos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CECalAnalysis_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
