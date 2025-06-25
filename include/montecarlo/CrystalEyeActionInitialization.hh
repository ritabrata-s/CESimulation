/*****************************************************************************
 *
 * CrystalEyeActionInitialization.hh
 *
 *****************************************************************************/

/// \file CrystalEyeActionInitialization.hh
/// \brief Definition of the CrystalEyeActionInitialization class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeActionInitialization_h
#define CrystalEyeActionInitialization_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4VUserActionInitialization.hh"

#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeActionInitialization: public G4VUserActionInitialization {
public:
  CrystalEyeActionInitialization(G4String fName = "filename", G4String fPath = "./");
  ~CrystalEyeActionInitialization();

  void Build() const;
  void BuildForMaster() const;

private:
  G4String fRootFilePath;
  G4String fRootFileName;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
