//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyePhysicsListMessenger_h
#define CrystalEyePhysicsListMessenger_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "globals.hh"
#include "G4UImessenger.hh"

class CrystalEyePhysicsList;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class CrystalEyePhysicsListMessenger: public G4UImessenger {
public:

  CrystalEyePhysicsListMessenger(CrystalEyePhysicsList*);
  ~CrystalEyePhysicsListMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  CrystalEyePhysicsList *pPhysicsList;

  G4UIdirectory *physDir;
  G4UIcmdWithADoubleAndUnit *gammaCutCmd;
  G4UIcmdWithADoubleAndUnit *electCutCmd;
  G4UIcmdWithADoubleAndUnit *protoCutCmd;
  G4UIcmdWithADoubleAndUnit *allCutCmd;
  G4UIcmdWithAString *pListCmd;
  G4UIcmdWithAString *packageListCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
