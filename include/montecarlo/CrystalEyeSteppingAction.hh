// **************************************************************
//
//      ------ CrystaliEyeSteppingAction  ------
//           by Libo Wu (12 Oct 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeSteppingAction_h
#define CrystalEyeSteppingAction_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class CrystalEyeDetectorConstruction;
class CrystalEyeEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeSteppingAction: public G4UserSteppingAction {
public:
  CrystalEyeSteppingAction(CrystalEyeEventAction *evt);
  ~CrystalEyeSteppingAction();

  void UserSteppingAction(const G4Step*);

private:
  CrystalEyeEventAction *eventAct;

  G4int fevtid;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
