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

  void BeginOfEvent(Int_t evt);
  void EndOfEvent(Int_t evt);

  Int_t GetEvtId() {
    return fEvtId;
  }

  Int_t GetNHits() {
    return fNHits;
  }

  vector<Int_t> GetPixelIds() {
    return *fPixId;
  }

  vector<Float_t> GetEdeps() {
    return fSmearedEdep;
  }

  vector<Float_t> GetEdepTimes() {
    return *fTime;
  }

  vector<TVector3> GetEdepPos();
  Float_t GetTotalEdep(Float_t &eUp, Float_t &eBot);
  void GetSortedEdeps(vector<Float_t> &edepV, vector<Int_t> &pixV);

  Bool_t IsTriggerOK(Int_t nTC);

private:
  void FCheckEvent();
  void FClearEvent();

  Int_t fCurEvt = -1;
  Int_t fEvtId = -1;
  Int_t fNHits = 0;
  vector<Int_t> *fPixId = 0;
  vector<Float_t> *fEdep = 0;
  vector<Float_t> *fEdepDel = 0;
  vector<Float_t> *fPosX = 0;
  vector<Float_t> *fPosY = 0;
  vector<Float_t> *fPosZ = 0;
  vector<Float_t> *fTime = 0;
  vector<TVector3> fEdepPos;
  vector<Float_t> fSmearedEdep;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CECalAnalysis_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
