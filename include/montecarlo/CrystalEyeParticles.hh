//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyePARTICLES_HH
#define CrystalEyePARTICLES_HH 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "globals.hh"
#include "G4VPhysicsConstructor.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeParticles: public G4VPhysicsConstructor {
public:

  CrystalEyeParticles(const G4String &name = "particles");

  virtual ~CrystalEyeParticles();

  virtual void ConstructParticle();

  // This method is dummy
  virtual void ConstructProcess() {
  }
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

