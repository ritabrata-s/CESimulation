/********************************************************************************
 *
 * CEPrimAnalysis.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CEPrimAnalysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEPrimAnalysis::CEPrimAnalysis() :
    fPosX(0.), fPosY(0.), fPosZ(0.), fDirX(0.), fDirY(0.), fDirZ(0.) {
  fDataPath = std::getenv("CRYSTALEYE_DATA");
  fDataPath += "Spec/";

  TString dataFile = fDataPath + "cosmicPhoton.txt";
  fGSpec[0] = new TGraph(dataFile.Data());
  dataFile = fDataPath + "albedoPhoton.txt";
  fGSpec[1] = new TGraph(dataFile.Data());
  dataFile = fDataPath + "cosmicProton.txt";
  fGSpec[2] = new TGraph(dataFile.Data());
  dataFile = fDataPath + "secondProton.txt";
  fGSpec[3] = new TGraph(dataFile.Data());
  dataFile = fDataPath + "albedoNeutron.txt";
  fGSpec[4] = new TGraph(dataFile.Data());
  dataFile = fDataPath + "secondElectron.txt";
  fGSpec[5] = new TGraph(dataFile.Data());
  dataFile = fDataPath + "secondPositron.txt";
  fGSpec[6] = new TGraph(dataFile.Data());

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEPrimAnalysis::~CEPrimAnalysis() {
  for (Int_t i = 0; i < 7; i++)
    delete fGSpec[i];

  if (fPSpec)
    delete fPSpec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEPrimAnalysis::Init() {
  CEAnalysis::Init();

  fTree = fStreamer->GetPrimTree();

  fNEntries = (Int_t) fTree->GetEntries();

  if (!fTree) {
    printf("[CEPrimAnalysis::Init] Tree object for this analysis... \n");
    return;
  }

  fTree->SetBranchAddress("EventID", &fEvtId);
  fTree->SetBranchAddress("Kinetic", &fPEng);
  fTree->SetBranchAddress("Pos_x", &fPosX);
  fTree->SetBranchAddress("Pos_y", &fPosY);
  fTree->SetBranchAddress("Pos_z", &fPosZ);
  fTree->SetBranchAddress("Dir_x", &fDirX);
  fTree->SetBranchAddress("Dir_y", &fDirY);
  fTree->SetBranchAddress("Dir_z", &fDirZ);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Float_t CEPrimAnalysis::GetEnergy(Int_t ent) {
  if (ent != fEntId)
    FGetEntry(ent);

  return fPEng * 1000.; // in keV
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TVector3 CEPrimAnalysis::GetVertexPos(Int_t ent) {
  if (ent != fEntId)
    FGetEntry(ent);

  return fPosVec = TVector3(fPosX, fPosY, fPosZ);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TVector3 CEPrimAnalysis::GetDirection(Int_t ent) {
  if (ent != fEntId)
    FGetEntry(ent);

  return fDirVec = TVector3(fDirX, fDirY, fDirZ);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FCosmicPhoton(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return fGSpec[0]->Eval(xx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FAlbedoPhoton(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return fGSpec[1]->Eval(xx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FCosmicProton(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return fGSpec[2]->Eval(xx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FSecondProton(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return fGSpec[3]->Eval(xx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FAlbedoNeutron(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return fGSpec[4]->Eval(xx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FSecondElectron(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return fGSpec[5]->Eval(xx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FSecondPositron(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return fGSpec[6]->Eval(xx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Double_t CEPrimAnalysis::FGRB(Double_t *x, Double_t *p) {
  Double_t xx = x[0];

  return p[0] * pow((xx / 100), p[2]) * exp(-(p[2] + 2) * xx / p[1]);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TF1* CEPrimAnalysis::GetPrimarySpec(TString type, Int_t dir, Float_t minE, Float_t maxE) {
  if (type.Contains("gamma")) {
    cout << "Normalization for particle: Gamma\n";

    if (dir == 1)
      fPSpec = new TF1("fCosmicPhoton", this, &CEPrimAnalysis::FCosmicPhoton, minE, maxE, 0);
    else if (dir == 2)
      fPSpec = new TF1("fAlbedoPhoton", this, &CEPrimAnalysis::FAlbedoPhoton, minE, maxE, 0);
    else {
      cout << "No function defined for particle distribution from all direction!\n";
      cout << "Please specify downward/upward particles...\n";
    }
  } else if (type.Contains("proton")) {
    cout << "Normalization for particle: Proton\n";

    if (dir == 0)
      fPSpec = new TF1("fSecondProton", this, &CEPrimAnalysis::FSecondProton, minE, maxE, 0);
    else if (dir == 1)
      fPSpec = new TF1("fCosmicProton", this, &CEPrimAnalysis::FCosmicProton, minE, maxE, 0);
    else {
      cout << "No function defined for particle distribution for upward protons!\n";
      cout << "Please specify downward (primary)/all (secondary) particles...\n";
    }
  } else if (type.Contains("neutron")) {
    cout << "Normalization for particle: Neutron\n";

    if (dir == 2)
      fPSpec = new TF1("fAlbedoNeutron", this, &CEPrimAnalysis::FAlbedoNeutron, minE, maxE, 0);
    else {
      cout << "No function defined for particle distribution for downward or all direction neutrons!\n";
      cout << "Please specify upward (albedo) particles...\n";
    }
  } else if (type.Contains("electron")) {
    cout << "Normalization for particle: Electron\n";

    if (!dir)
      fPSpec = new TF1("fSecondElectron", this, &CEPrimAnalysis::FSecondElectron, minE, maxE, 0);
    else {
      cout << "No function defined for particle distribution for downward/upward electrons!\n";
      cout << "Please specify particles from all direction...\n";
    }
  } else if (type.Contains("positron")) {
    cout << "Normalization for particle: Positron\n";

    if (!dir)
      fPSpec = new TF1("fSecondPositron", this, &CEPrimAnalysis::FSecondPositron, minE, maxE, 0);
    else {
      cout << "No function defined for particle distribution for downward/upward positrons!\n";
      cout << "Please specify particles from all direction...\n";
    }
  } else
    cout << "No function defined for the specified particle type " << type.Data() << endl;

  if (fPSpec)
    fPSpec->SetTitle(";Energy (keV); Counts cm^{-2} sr^{-2} kev^{-1} s^{-1}");

  return fPSpec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TF1* CEPrimAnalysis::GetGRBSpec(Float_t minE, Float_t maxE) {
  cout << "Normalization for GRB\n";

  fPSpec = new TF1("fGRBSpec", this, &CEPrimAnalysis::FGRB, minE, maxE, 3);
  fPSpec->SetParameters(0.0123671, 205.739, -0.513499);

  return fPSpec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
