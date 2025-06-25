/********************************************************************************
 *
 * CEPrimAnalysis.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CEPrimAnalysis_h
#define CEPrimAnalysis_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "TVector3.h"
#include "TF1.h"
#include "TGraph.h"

#include "analysis/CEAnalysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CEPrimAnalysis: public CEAnalysis {
public:
  CEPrimAnalysis();
  ~CEPrimAnalysis();

  void Init();

  Float_t GetEnergy(Int_t ent);
  TVector3 GetVertexPos(Int_t ent);
  TVector3 GetDirection(Int_t ent);
  TF1* GetPrimarySpec(TString type, Int_t dir, Float_t minE, Float_t maxE);
  TF1* GetGRBSpec(Float_t minE, Float_t maxE);

  Double_t FCosmicPhoton(Double_t *x, Double_t *p);
  Double_t FAlbedoPhoton(Double_t *x, Double_t *p);
  Double_t FCosmicProton(Double_t *x, Double_t *p);
  Double_t FSecondProton(Double_t *x, Double_t *p);
  Double_t FAlbedoNeutron(Double_t *x, Double_t *p);
  Double_t FSecondElectron(Double_t *x, Double_t *p);
  Double_t FSecondPositron(Double_t *x, Double_t *p);
  Double_t FGRB(Double_t *x, Double_t *p);

private:
  Float_t fPEng = 0.;
  Float_t fPosX, fPosY, fPosZ, fDirX, fDirY, fDirZ;
  TVector3 fPosVec;
  TVector3 fDirVec;
  TString fDataPath;
  TF1 *fPSpec = nullptr;
  TGraph *fGSpec[7];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CEPrimAnalysis_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
