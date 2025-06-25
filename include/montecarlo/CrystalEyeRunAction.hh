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
#ifndef CrystalEyeRunAction_h
#define CrystalEyeRunAction_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class CrystalEyeCalorimeterDataManager;
class CrystalEyeAnticoincidenceDataManager;
class CrystalEyeAnalysis;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeRunAction: public G4UserRunAction {
public:
  CrystalEyeRunAction();
  ~CrystalEyeRunAction();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  void SetRootFileName(G4String val) {
    fRootFileName = val;
  }

  void SetRootFilePath(G4String val) {
    fRootFilePath = val;
  }

  CrystalEyeCalorimeterDataManager* GetCalDataManager() {
    return fDataManagerCal;
  }

  CrystalEyeAnticoincidenceDataManager* GetAcsDataManager() {
    return fDataManagerAcd;
  }

private:
  G4int fRunID;
  G4String fRootFilePath;
  G4String fRootFileName;

  CrystalEyeCalorimeterDataManager *fDataManagerCal;
  CrystalEyeAnticoincidenceDataManager *fDataManagerAcd;
  CrystalEyeAnalysis *fAnalysis;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
