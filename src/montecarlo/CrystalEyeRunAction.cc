// **************************************************************
//
//      ------------ CrystalEyeEventAction  -----------
//           by Libo Wu (23 Sep 2021)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (09 Feb, 2024)
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeRunAction.hh"
#include "montecarlo/CrystalEyeAnalysis.hh"
#include "montecarlo/CrystalEyeAnticoincidenceDataManager.hh"
#include "montecarlo/CrystalEyeCalorimeterDataManager.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4VisExtent.hh"
#include "G4TessellatedSolid.hh"
#include "G4Run.hh"
#include "G4AutoDelete.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeRunAction::CrystalEyeRunAction() :
    fRunID(-1), fRootFileName(""), fRootFilePath("") {
  fDataManagerCal = new CrystalEyeCalorimeterDataManager();
  fDataManagerAcd = new CrystalEyeAnticoincidenceDataManager();
  G4AutoDelete::Register(fDataManagerCal);
  G4AutoDelete::Register(fDataManagerAcd);

  fAnalysis = CrystalEyeAnalysis::GetInstance();
}

//------------------------------------------------------------
CrystalEyeRunAction::~CrystalEyeRunAction() {
}

//------------------------------------------------------------
void CrystalEyeRunAction::BeginOfRunAction(const G4Run *aRun) {
  fRunID = aRun->GetRunID();
  fDataManagerAcd->BeginRun(aRun);
  fDataManagerCal->BeginRun(aRun);

  fAnalysis->SetRootFileName(fRootFileName);
  fAnalysis->SetRootFilePath(fRootFilePath);
  fAnalysis->BeginOfRun();

  if (IsMaster()) {
    G4cout << "### Run " << aRun->GetRunID() << " starts (master)." << G4endl;
//  if (G4VVisManager::GetConcreteInstance()) {
//    G4UImanager *UI = G4UImanager::GetUIpointer();
//    UI->ApplyCommand("/vis/scene/notifyHandlers");
//  }

    auto phyVolStore = G4PhysicalVolumeStore::GetInstance();
//    G4cout << phyVolStore->GetMap().size() << G4endl;
//    for (const auto& [key, value] : phyVolStore->GetMap())
//      std::cout << key << " = " << (value[0]->GetLogicalVolume()->GetSolid())->GetExtent().GetExtentCentre() << "\n";
//      std::cout << key << " = " << (value[0]->GetLogicalVolume()->GetSolid())->GetCubicVolume() << "\n";
//      std::cout << key << " = " << G4BestUnit(value[0]->GetLogicalVolume()->GetMass(), "Mass") << "\n";

  } else
    G4cout << "### Run " << aRun->GetRunID() << " starts (worker)." << G4endl;
}

//------------------------------------------------------------
void CrystalEyeRunAction::EndOfRunAction(const G4Run *aRun) {
  G4cout << "End of Run " << aRun->GetRunID() << G4endl;

  fAnalysis->EndOfRun();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
