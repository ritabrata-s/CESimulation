// ------------------------------------------------------------
//
//      ------------ CrystalEyeCalorimeterDataManager.cc  ------
//           by Libo Wu (15 Feb 2022)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (09 Feb, 2024)
//
// ------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeCalorimeterDataManager.hh"
#include "montecarlo/CrystalEyeCalorimeterHit.hh"
#include "montecarlo/CrystalEyeAnalysis.hh"
#include "geometry/CrystalEyeDetectorConstruction.hh"

#include "G4SDManager.hh"
#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeCalorimeterDataManager::CrystalEyeCalorimeterDataManager() :
    CollectionID(-1) {
}

// ------------------------------------------------------------
CrystalEyeCalorimeterDataManager::~CrystalEyeCalorimeterDataManager() {
}

// ------------------------------------------------------------
void CrystalEyeCalorimeterDataManager::BeginRun(const G4Run *aRun) {
}

// ------------------------------------------------------------
void CrystalEyeCalorimeterDataManager::BeginEvent(const G4Event *aEvt) {
}

// ------------------------------------------------------------
void CrystalEyeCalorimeterDataManager::FillEvent(const G4Event *aEvt) {
  G4ThreadLocal CrystalEyeAnalysis::CalData
  caldata;
  caldata.Reset();

  caldata.evtId = aEvt->GetEventID();

  G4HCofThisEvent *HCE = aEvt->GetHCofThisEvent();
  CrystalEyeCalorimeterHitsCollection *CHC = 0;

  if (HCE) {
    G4SDManager *SDman = G4SDManager::GetSDMpointer();
    if (CollectionID == -1)
      CollectionID = SDman->GetCollectionID("CalorimeterCollection");

    CHC = (CrystalEyeCalorimeterHitsCollection*) (HCE->GetHC(CollectionID));
    if (CHC) {

      caldata.nHits = CHC->entries();

      for (int i = 0; i < CHC->entries(); i++) {
        caldata.crystalId.push_back((*CHC)[i]->GetCrystalID());
        G4double edep = (*CHC)[i]->GetEdep();
        caldata.edep.push_back(edep);
        G4double edepDel = (*CHC)[i]->GetEdepDelay();
        caldata.edepDel.push_back(edepDel);
        caldata.order.push_back((*CHC)[i]->GetCrystalOrder());
//        fNsteps = (*CHC)[i]->GetnSteps();
        caldata.posX.push_back(((*CHC)[i]->GetPos()).x());
        caldata.posY.push_back(((*CHC)[i]->GetPos()).y());
        caldata.posZ.push_back(((*CHC)[i]->GetPos()).z());
        caldata.tim.push_back((*CHC)[i]->GetTime());

        caldata.totEdep += edep;
        caldata.totEdepDel += edepDel;
      }
    }
  }

  CrystalEyeAnalysis *fanalysis = CrystalEyeAnalysis::GetInstance();
  fanalysis->SetDataCAL(caldata);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
