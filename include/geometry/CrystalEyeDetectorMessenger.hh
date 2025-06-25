/*****************************************************************************
 *
 * CrystalEyeDetectorMessenger
 *
 *****************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeDetectorMessenger_h
#define CrystalEyeDetectorMessenger_h 1

#include "G4UImessenger.hh"

class CrystalEyeDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class CrystalEyeDetectorMessenger: public G4UImessenger {
public:
  CrystalEyeDetectorMessenger(CrystalEyeDetectorConstruction*);
  ~CrystalEyeDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  CrystalEyeDetectorConstruction *CrystalEyeDetector;

  G4UIdirectory *CrystalEyedetDir;

  // Silicon Tile
  G4UIcmdWithADoubleAndUnit *UpPixelThickCmd;
  G4UIcmdWithADoubleAndUnit *DownPixelThickCmd;
  G4UIcmdWithAnInteger *NbLayersCmd;
  G4UIcmdWithADoubleAndUnit *ACDThickCmd;

  // Tracker
  G4UIcmdWithAnInteger *NbPixelCmd;

  // Total
  G4UIcmdWithADoubleAndUnit *MagFieldCmd;
  G4UIcmdWithoutParameter *UpdateCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
