/********************************************************************************
 *
 * CECalAnalysis.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CECalAnalysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CECalAnalysis::CECalAnalysis() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CECalAnalysis::~CECalAnalysis() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CECalAnalysis::Init() {
  CEAnalysis::Init();

  fTree = fStreamer->GetCalTree();

  fNEntries = (Int_t) fTree->GetEntries();
//  cout << "No. of entries = " << fNEntries << endl;

  if (!fTree) {
    printf("[CECalAnalysis::Init] Tree object for this analysis... \n");
    return;
  }

  fTree->SetBranchAddress("EventID", &fEvtId);
  fTree->SetBranchAddress("nHits_CAL", &fNHits);
  fTree->SetBranchAddress("PixelID_CAL[nHits]", &fPixId);
  fTree->SetBranchAddress("Edep_CAL[nHits]", &fEdep);
  fTree->SetBranchAddress("EdepDelay_CAL[nHits]", &fEdepDel);
  fTree->SetBranchAddress("EdepPosX_CAL[nHits]", &fPosX);
  fTree->SetBranchAddress("EdepPosY_CAL[nHits]", &fPosY);
  fTree->SetBranchAddress("EdepPosZ_CAL[nHits]", &fPosZ);
  fTree->SetBranchAddress("EdepTime_CAL[nHits]", &fTime);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CECalAnalysis::FCheckEvent() {
  Int_t nhits_p = fPixId->size();
  Int_t nhits_e = fEdep->size();
  Int_t nhits_d = fEdepDel->size();
  Int_t nhits_x = fPosX->size();
  Int_t nhits_y = fPosY->size();
  Int_t nhits_z = fPosZ->size();
  Int_t nhits_t = fTime->size();

  if ((nhits_p != nhits_e) || (nhits_p != nhits_d) || (nhits_p != nhits_x) || (nhits_p != nhits_y)
      || (nhits_p != nhits_z) || (nhits_p != nhits_t))
    printf("[CECalAnalysis::FCheckHits] Problem is number of hits in cal for event no. %d, please check the data... \n",
        fEntId);

  // Apply threshold >= 30 keV
  unsigned ii(0);
  for (auto v : (*fEdep)) {

//    if (((v < 30.e-3) && ((fEdepDel->at(ii)) < 30.e-3)) && ((fPixId->at(ii)) > 2 * NPIXEL)) {
    if ((((v + (fEdepDel->at(ii))) * 1.E3 < CALTHRESHOLD)) || ((fPixId->at(ii)) > 2 * fNPIXEL)) {
      fPixId->erase(fPixId->begin() + ii);
      fEdep->erase(fEdep->begin() + ii);
      fEdepDel->erase(fEdepDel->begin() + ii);
      fPosX->erase(fPosX->begin() + ii);
      fPosY->erase(fPosY->begin() + ii);
      fPosZ->erase(fPosZ->begin() + ii);
      fTime->erase(fTime->begin() + ii);
    } else
      ii++;
  }

  fNHits = fPixId->size();

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Int_t CECalAnalysis::GetNHits(Int_t ent) {
  FGetEntry(ent);
  if (ent != fEntId) {
    FCheckEvent();
  }

  return fNHits;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<Int_t> CECalAnalysis::GetPixelIds(Int_t ent) {
  FGetEntry(ent);
  if (ent != fEntId) {
    FCheckEvent();
  }

  return *fPixId;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<Float_t> CECalAnalysis::GetEdeps(Int_t ent) {
  FGetEntry(ent);
  if (ent != fEntId) {
    FCheckEvent();
  }

  // Convert to keV
  vector<Float_t> vEdep;
  int ii(0);
  for (auto v : (*fEdep)) {
    v = v * 1000 + fEdepDel->at(ii++) * 1000;

    vEdep.push_back(v);
  }

  return vEdep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<TVector3> CECalAnalysis::GetEdepPos(Int_t ent) {
  FGetEntry(ent);
  if (ent != fEntId) {
    FCheckEvent();
  }

  fEdepPos.clear();

  for (Int_t i = 0; i < fNHits; i++)
    fEdepPos.push_back(TVector3((*fPosX)[i], (*fPosY)[i], (*fPosZ)[i]));

  return fEdepPos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<Float_t> CECalAnalysis::GetEdepTimes(Int_t ent) {
  FGetEntry(ent);
  if (ent != fEntId) {
    FCheckEvent();
  }

  return *fTime;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Float_t CECalAnalysis::GetTotalEdep(Int_t ent, Float_t &eUp, Float_t &eBot) {
  Float_t totE(0.);
  auto edepV = GetEdeps(ent);
  auto pixV = GetPixelIds(ent);

  for (unsigned i = 0; i < pixV.size(); i++) {
    Int_t pid = pixV[i];
    Float_t edep = edepV[i];

//    if (edep < 20)
//      continue;

//    if (!((pid == 91) || (pid == 221)))
//      continue;

//    cout << pid << '\t' << edep << endl;
//    getchar();

    totE += edep;

    if (pid < fNPIXEL)
      eUp += edep;
    else
      eBot += edep;
  }

  return totE;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CECalAnalysis::GetSortedEdeps(Int_t ent, vector<Float_t> &edepV, vector<Int_t> &pixV) {
  edepV.clear();
  pixV.clear();
  edepV = GetEdeps(ent);
  pixV = GetPixelIds(ent);

  for (Int_t i = 0; i < pixV.size(); i++)
    for (Int_t j = i + 1; j < pixV.size(); j++) {
      if (edepV[i] < edepV[j]) {
        Float_t temp = edepV[i];
        edepV[i] = edepV[j];
        edepV[j] = temp;
        Int_t tid = pixV[i];
        pixV[i] = pixV[j];
        pixV[j] = tid;
      }
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Bool_t CECalAnalysis::IsTriggerOK(Int_t ent, Int_t nTC) {
  Float_t eCalUp(0.), eCalBot(0.), totEdepCal(0.);
  bool inNear = false;
  vector<Float_t> edepV;
  vector<Int_t> pixV;
  Int_t nHits(0);
  vector<Int_t> near;
  near.clear();
  Float_t edepN(0.);
  vector<pair<Int_t, Float_t>> nCont;

  switch (nTC) {
  case 1: // valid event: total edep in calo > 0.0 keV
    totEdepCal = this->GetTotalEdep(ent, eCalUp, eCalBot);
    if (totEdepCal > 0.0)
      return true;
    break;

  case 2: // valid event: total edep in up calo layer > total edep in bottom calo layer
    totEdepCal = this->GetTotalEdep(ent, eCalUp, eCalBot);
    if (eCalUp > eCalBot)
      return true;
    break;

  case 3:
    // valid event: at least one edep in the up calo layer;
    // for edep in multiple crystals max edep in up calo layer;
    // other edeps are only in the nearby crystals of the max edep
    this->GetSortedEdeps(ent, edepV, pixV);
    nHits = edepV.size();

    if (pixV[0] > fNPIXEL)
      return false;

    if (nHits < 2)
      return true;

    near = this->NearbyCrystals(pixV[0]);
    for (int hit = 1; hit < nHits; hit++) {
      inNear = false;
      for (auto nc : near)
        if (pixV[hit] == nc) {
          inNear = true;
          break;
        }

      if (!inNear) // if any of the hits not in the nearby of max edep exit the hit with inNear = false
        break;
    }

    if (inNear)
      return true;

    break;

  case 4:
    // valid event: at least one edep in the up calo layer;
    // for edep in multiple crystals max edep in up calo layer;
    // at least one of the other edeps are in the nearby crystals of the max edep;
    // edep in the nearby crystals are greater than half of total edep in calo

    totEdepCal = this->GetTotalEdep(ent, eCalUp, eCalBot);
    this->GetSortedEdeps(ent, edepV, pixV);
    nHits = pixV.size();

//    if ((nHits < 2) && (pixV[0] <= NPIXEL)) // if only one hit and that is in up layer
//      return true;

    if (pixV[0] > fNPIXEL)
      return false;

//    if (pixV[0] <= NPIXEL)
//      return true;

    if (nHits < 2)
      return true;

//    cout << "Evt. = " << ent << endl;

    nCont.push_back(make_pair(pixV[0], edepV[0]));
    pixV.erase(pixV.begin());

    while (pixV.size()) {
      for (auto ncnt : nCont) {
        inNear = false;
        near = this->NearbyCrystals(ncnt.first);
        for (auto nc : near)
          if (pixV[0] == nc) {
            nCont.push_back(make_pair(pixV[0], edepV[0]));
            inNear = true;
            break;
          }
        if (inNear)
          break;
      }
      pixV.erase(pixV.begin());
    }

    for (auto nc : nCont)
      edepN += nc.second;

//    if ((nCont.size() > 1))
    if ((nCont.size() > 1) && ((edepN / totEdepCal) > 0.5))
      return true;

    /*
     totEdepCal = this->GetTotalEdep(ent, eCalUp, eCalBot);
     this->GetSortedEdeps(ent, edepV, pixV);
     nHits = edepV.size();

     if (pixV[0] > NPIXEL)
     return false;

     if (nHits < 2)
     return true;

     near = this->NearbyCrystals(pixV[0]);
     for (int hit = 1; hit < nHits; hit++) {
     inNear = false;
     for (auto nc : near)
     if (pixV[hit] == nc) {
     edepN += edepV[hit];
     //          if (!inNear)
     //            inNear = true;
     break;
     }

     //      if (inNear) // if any of the hits in the nearby of max edep exit the hit with inNear = true
     //        break;
     }

     edepN += edepV[0];

     //    if (inNear)
     if ((edepN > edepV[0]) && ((edepN / totEdepCal) > 0.5))
     return true;
     */
    break;

  case 5:
    // valid event: at least one edep in the up calo layer;
    // for edep in multiple crystals at least one of the other edeps are in the nearby crystals of the up crystal
    totEdepCal = this->GetTotalEdep(ent, eCalUp, eCalBot);
    this->GetSortedEdeps(ent, edepV, pixV);
    nHits = edepV.size();

    if ((nHits < 2) && (pixV[0] <= fNPIXEL)) // if only one hit and that is in up layer
      return true;

    for (auto px : pixV) { // loop on all edep crystals
      if (px > fNPIXEL) // don't consider lower layer crystals
        continue;

      else { // consider only up crystals
        near = this->NearbyCrystals(px); // nearby crystals of this edep
        for (auto opx : pixV) {
          for (auto nc : near)
            if (opx == nc) {
              inNear = true;
              break;
            }
          if (inNear)
            break;
        }
        if (inNear)
          break;
      }
    }

    if (inNear)
      return true;

    break;

  default:
    cout << "[CECalAnalysis::IsTriggerOK] Trigger condition type does not match! (valid type: 1, 2, 3, 4, 5)" << endl;
    exit(1);

  }

  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
