//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyePrimaryGeneratorMessenger_h
#define CrystalEyePrimaryGeneratorMessenger_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4UImessenger.hh"
#include "globals.hh"

class CrystalEyePrimaryGeneratorAction;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyePrimaryGeneratorMessenger: public G4UImessenger {
public:
  CrystalEyePrimaryGeneratorMessenger(CrystalEyePrimaryGeneratorAction*);
  ~CrystalEyePrimaryGeneratorMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  CrystalEyePrimaryGeneratorAction *CrystalEyeAction;
  G4UIcmdWithAString *RndmCmd;
  G4UIcmdWithAnInteger *SourceTypeCmd;
  G4UIcmdWithADoubleAndUnit *VertexRadiusCmd;
  G4UIcmdWithAnInteger *SpectrumTypeCmd;
  G4UIcmdWithABool *SourceGenCmd;

};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
