// **************************************************************
//
//      ------ CrystalEyePrimaryGeneratorAction.hh  ------
//           Modified by Libo Wu (23 Sep 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyePrimaryGeneratorAction_h
#define CrystalEyePrimaryGeneratorAction_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"

class G4Event;
class G4ParticleGun;
class G4GeneralParticleSource;

class CrystalEyeDetectorConstruction;
class CrystalEyePrimaryGeneratorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyePrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction {
public:

  CrystalEyePrimaryGeneratorAction();
  ~CrystalEyePrimaryGeneratorAction();

public:
  void GeneratePrimaries(G4Event*);

  void SetRndmFlag(G4String val) {
    rndmFlag = val;
  }

  void SetSourceType(G4int val) {
    nSourceType = val;
  }

  void SetSpectrumType(G4int val) {
    nSpectrumType = val;
  }

  void SetVertexRadius(G4double val) {
    dVertexRadius = val;
  }

  void SetSourceGen(G4bool val) {
    sourceGun = val;
  }

private:
  const CrystalEyeDetectorConstruction *fDetector;
  CrystalEyePrimaryGeneratorMessenger *gunMessenger;

  G4ParticleGun *fParticleGun;
  G4GeneralParticleSource *fParticleSource;
  G4String rndmFlag; //flag for a random impact point
  G4int nSourceType;
  G4double dVertexRadius;
  G4int nSpectrumType;
  G4bool sourceGun; // false for GeneralParticleSource

//  G4SPSPosDistribution *fPosDist;
//  G4SPSAngDistribution *fAngDist;
//  G4SPSEneDistribution *fEngDist;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
