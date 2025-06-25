// ------------------------------------------------------------
//
//      ------------ CrystalEyeCalorimeterDataManager.hh  ------
//           by Libo Wu (15 Feb 2022)
//           Email: libo.wu@gssi.it
//
// ------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeCalorimeterDataManager_h
#define CrystalEyeCalorimeterDataManager_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "globals.hh"

class G4Run;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeCalorimeterDataManager {
public:
  CrystalEyeCalorimeterDataManager();
  ~CrystalEyeCalorimeterDataManager();

public:
  void BeginRun(const G4Run *aRun);
  void BeginEvent(const G4Event *aEvt);
  void FillEvent(const G4Event *aEvt); //EndEvent

private:
  G4int CollectionID;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
