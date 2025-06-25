// **************************************************************
//
//      ------ CrystalEyeTrackingAction  ------
//           by Libo Wu (23 Oct 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeTrackingAction_h
#define CrystalEyeTrackingAction_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4UserTrackingAction.hh"
#include "globals.hh"

class CrystalEyeEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeTrackingAction: public G4UserTrackingAction {

public:
  CrystalEyeTrackingAction(CrystalEyeEventAction *evt);
  ~CrystalEyeTrackingAction();

  virtual void PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);

private:
  CrystalEyeEventAction *fEventAct;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
