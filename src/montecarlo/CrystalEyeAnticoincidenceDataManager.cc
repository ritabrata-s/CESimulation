// ------------------------------------------------------------
//
//      ------------ CrystalEyeAnticoincidenceDataManager.cc  ------
//           by Libo Wu (15 Feb 2022)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (09 Feb, 2024)
//
// ------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeAnticoincidenceDataManager.hh"
#include "montecarlo/CrystalEyeAnticoincidenceHit.hh"
#include "montecarlo/CrystalEyeAnalysis.hh"
#include "geometry/CrystalEyeDetectorConstruction.hh"

#include "G4SDManager.hh"
#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeAnticoincidenceDataManager::CrystalEyeAnticoincidenceDataManager() :
    CollectionID(-1) {
}

// ------------------------------------------------------------
CrystalEyeAnticoincidenceDataManager::~CrystalEyeAnticoincidenceDataManager() {
}

// ------------------------------------------------------------
void CrystalEyeAnticoincidenceDataManager::BeginRun(const G4Run *aRun) {
}

// ------------------------------------------------------------
void CrystalEyeAnticoincidenceDataManager::BeginEvent(const G4Event *aEvt) {
}

// ------------------------------------------------------------
void CrystalEyeAnticoincidenceDataManager::FillEvent(const G4Event *aEvt) {
  G4ThreadLocal CrystalEyeAnalysis::AcdData
  acddata;
  acddata.Reset();

  acddata.evtId = aEvt->GetEventID();

  G4HCofThisEvent *HCE = aEvt->GetHCofThisEvent();
  CrystalEyeAnticoincidenceHitsCollection *AHC = 0;

  if (HCE) {
    G4SDManager *SDman = G4SDManager::GetSDMpointer();
    if (CollectionID == -1)
      CollectionID = SDman->GetCollectionID("AnticoincidenceCollection");

    AHC = (CrystalEyeAnticoincidenceHitsCollection*) (HCE->GetHC(CollectionID));
    if (AHC) {

      acddata.nHits = AHC->entries();

      for (int i = 0; i < AHC->entries(); i++) {
        acddata.crystalId.push_back((*AHC)[i]->GetACDID());
        G4double edep = (*AHC)[i]->GetEdep();
        acddata.edep.push_back(edep);
        G4double edepDel = (*AHC)[i]->GetEdepDelay();
        acddata.edepDel.push_back(edepDel);
        acddata.order.push_back((*AHC)[i]->GetACDOrder());

        acddata.totEdep += edep;
        acddata.totEdepDel += edepDel;
      }
    }
  }

  CrystalEyeAnalysis *fanalysis = CrystalEyeAnalysis::GetInstance();
  fanalysis->SetDataACD(acddata);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
