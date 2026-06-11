/********************************************************************************
 *
 * CEAcdAnalysis.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CEAcdAnalysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEAcdAnalysis::CEAcdAnalysis() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEAcdAnalysis::~CEAcdAnalysis() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAcdAnalysis::Init() {
  CEAnalysis::Init();

  fTree = fStreamer->GetAcdTree();

  fNEntries = (Int_t) fTree->GetEntries();
//  cout << "No. of entries = " << fNEntries << endl;

  if (!fTree) {
    printf("[CEAcdAnalysis::Init] Tree object for this analysis... \n");
    return;
  }

  fTree->SetBranchAddress("EventID", &fEvtId);
  fTree->SetBranchAddress("nHits_ACD", &fNHits);
  fTree->SetBranchAddress("PixelID_ACD[nHits]", &fPixId);
  fTree->SetBranchAddress("Edep_ACD[nHits]", &fEdep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAcdAnalysis::FCheckEvent() {
  Int_t nhits_p = fPixId->size();
  Int_t nhits_e = fEdep->size();

  if ((nhits_p != nhits_e))
    printf("[CEAcdAnalysis::FCheckHits] Problem is number of hits in acd for event no. %d, please check the data... \n",
        fEntId);

  // Apply threshold >= 7 keV
  unsigned ii(0);
  for (auto v : (*fEdep)) {

    if ((v * 1.E3 < ACDTHRESHOLD) || ((fPixId->at(ii)) > 2 * fNPIXEL)) {
      fPixId->erase(fPixId->begin() + ii);
      fEdep->erase(fEdep->begin() + ii);
    } else
      ii++;
  }

  fNHits = fPixId->size();

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<Int_t> CEAcdAnalysis::GetPixelIds(Int_t ent) {
  if (ent != fEntId) {
    FGetEntry(ent);
    FCheckEvent();
  }

  return *fPixId;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<Float_t> CEAcdAnalysis::GetEdeps(Int_t ent) {
  if (ent != fEntId) {
    FGetEntry(ent);
    FCheckEvent();
  }

  // Convert to keV
  vector<Float_t> vEdep;
  for (auto v : (*fEdep)) {
    v = v * 1000;

    vEdep.push_back(v);
  }

  return vEdep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Float_t CEAcdAnalysis::GetTotalEdep(Int_t ent, Float_t &eUp, Float_t &eBot) {
  Float_t totE(0.);
  auto edepV = GetEdeps(ent);
  auto pixV = GetPixelIds(ent);

  for (Int_t i = 0; i < pixV.size(); i++) {
    Int_t pid = pixV[i];
    Float_t edep = edepV[i];
    totE += edep;

    if (pid < fNPIXEL)
      eUp += edep;
    else
      eBot += edep;
  }

  return totE;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Bool_t CEAcdAnalysis::IsTriggerOK(Int_t ent, Int_t nTC) {
  Float_t eAcdUp(0.), eAcdBot(0.), totEdepAcd(0.);
  totEdepAcd = this->GetTotalEdep(ent, eAcdUp, eAcdBot);

  switch (nTC) {
  case 1: // valid event: edep up ACD < 200 keV
    if (eAcdUp < ACDUPEDEPCUT)
      return true;
    break;

  case 2: // valid event: edep bottom ACD = 0.0
    if (eAcdBot <= ACDBOTEDEPCUT)
      return true;
    break;

  case 3: // valid event: edep up ACD = 0.0
    if (eAcdUp <= 0.0)
      return true;
    break;

  default:
    cout << "[CEAcdAnalysis::IsTriggerOK] Trigger condition type does not match! (valid type: 1, 2, 3)" << endl;
    exit(1);

  }

  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
