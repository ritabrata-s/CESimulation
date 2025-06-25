/*****************************************************************************
 *
 * CrystalEyeActionInitialization.cc
 *
 *****************************************************************************/
/// \file CrystalEyeActionInitialization.cc
/// \brief Implementation of the CrystalEyeActionInitialization class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeActionInitialization.hh"
#include "montecarlo/CrystalEyePrimaryGeneratorAction.hh"
#include "montecarlo/CrystalEyeRunAction.hh"
#include "montecarlo/CrystalEyeEventAction.hh"
#include "montecarlo/CrystalEyeTrackingAction.hh"
#include "montecarlo/CrystalEyeSteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CrystalEyeActionInitialization::CrystalEyeActionInitialization(G4String fName, G4String fPath) :
    G4VUserActionInitialization(), fRootFileName(fName), fRootFilePath(fPath) {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CrystalEyeActionInitialization::~CrystalEyeActionInitialization() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CrystalEyeActionInitialization::Build() const {
  CrystalEyeRunAction *runAction = new CrystalEyeRunAction();
  runAction->SetRootFileName(fRootFileName);
  runAction->SetRootFilePath(fRootFilePath);

  CrystalEyeEventAction *evtAction = new CrystalEyeEventAction(runAction);
  CrystalEyeTrackingAction *trkAction = new CrystalEyeTrackingAction(evtAction);
  CrystalEyeSteppingAction *stpAction = new CrystalEyeSteppingAction(evtAction);

  SetUserAction(new CrystalEyePrimaryGeneratorAction);
  SetUserAction(runAction);
  SetUserAction(evtAction);
  SetUserAction(trkAction);
  SetUserAction(stpAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CrystalEyeActionInitialization::BuildForMaster() const {
  CrystalEyeRunAction *runAction = new CrystalEyeRunAction();
  runAction->SetRootFileName(fRootFileName);
  runAction->SetRootFilePath(fRootFilePath);

  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
