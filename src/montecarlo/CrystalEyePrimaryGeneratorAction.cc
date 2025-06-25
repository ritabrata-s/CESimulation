// **************************************************************
//
//      ------ CrystalEyePrimaryGeneratorAction.cc  ------
//           by Libo Wu (23 Sep 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyePrimaryGeneratorAction.hh"
#include "montecarlo/CrystalEyePrimaryGeneratorMessenger.hh"
#include "montecarlo/CrystalEyeAnalysis.hh"
#include "geometry/CrystalEyeDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Geantino.hh"
#include "G4IonTable.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyePrimaryGeneratorAction::CrystalEyePrimaryGeneratorAction() {
  fDetector =
      static_cast<const CrystalEyeDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  //---  Create a messenger for this class  ---
  gunMessenger = new CrystalEyePrimaryGeneratorMessenger(this);

  rndmFlag = "off";
  nSourceType = 0;
  nSpectrumType = 0;
  sourceGun = false;
  dVertexRadius = 18. * cm;

  G4int n_particle = 1;

  fParticleGun = new G4ParticleGun(n_particle);
  // default particle kinematic
  fParticleGun->SetParticleEnergy(0.0 * eV);
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));

  fParticleSource = new G4GeneralParticleSource();
}

//------------------------------------------------------------

CrystalEyePrimaryGeneratorAction::~CrystalEyePrimaryGeneratorAction() {
//  if (fParticleGun)
  delete fParticleGun;
//  if (fParticleSource)
  delete fParticleSource;
  delete gunMessenger;
}

//------------------------------------------------------------

void CrystalEyePrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
  //---  For Particle Gun  ---
  /*if (sourceGun) {
   G4double z0 = 0.5 * (fDetector->GetWorldSizeZ());
   G4double x0 = 0. * cm, y0 = 0. * cm;

   G4ThreeVector pos0;
   G4ThreeVector dir0;
   G4ThreeVector vertex0 = G4ThreeVector(x0, y0, z0);

   dir0 = G4ThreeVector(0., 0., -1.);

   G4double theta, phi, y, f;
   G4double theta0 = 0.;
   G4double phi0 = 0.;

   switch (nSourceType) {
   case 0:
   fParticleGun->SetParticlePosition(vertex0);
   fParticleGun->SetParticleMomentumDirection(dir0);
   break;

   case 1:
   // GS: Generate random position on the 4PIsphere to create a unif. distrib.
   // GS: on the sphere
   phi = G4UniformRand() * twopi;
   do {
   y = G4UniformRand() * 1.0;
   theta = G4UniformRand() * pi;
   f = std::sin(theta);
   } while (y > f);
   vertex0 = G4ThreeVector(1., 0., 0.);
   vertex0.setMag(dVertexRadius);
   vertex0.setTheta(theta);
   vertex0.setPhi(phi);
   fParticleGun->SetParticlePosition(vertex0);

   dir0 = G4ThreeVector(1., 0., 0.);
   do {
   phi = G4UniformRand() * twopi;
   do {
   y = G4UniformRand() * 1.0;
   theta = G4UniformRand() * pi;
   f = std::sin(theta);
   } while (y > f);
   dir0.setPhi(phi);
   dir0.setTheta(theta);
   } while (vertex0.dot(dir0) >= -0.7 * vertex0.mag());
   fParticleGun->SetParticleMomentumDirection((G4ParticleMomentum) dir0);
   break;

   case 2:
   // GS: Generate random position on the upper semi-sphere z>0 to create a unif. distrib.
   // GS: on a plane
   phi = G4UniformRand() * twopi;
   do {
   y = G4UniformRand() * 1.0;
   theta = G4UniformRand() * halfpi;
   f = std::sin(theta) * std::cos(theta);
   } while (y > f);
   vertex0 = G4ThreeVector(1., 0., 0.);

   G4double xy = fDetector->GetWorldSizeXY();
   G4double z = fDetector->GetWorldSizeZ();

   if (dVertexRadius > xy * 0.5) {
   G4cout << "vertexRadius too big " << G4endl;
   G4cout << "vertexRadius setted to " << xy * 0.45 << G4endl;
   dVertexRadius = xy * 0.45;
   }

   if (dVertexRadius > z * 0.5) {
   G4cout << "vertexRadius too high " << G4endl;
   G4cout << "vertexRadius setted to " << z * 0.45 << G4endl;
   dVertexRadius = z * 0.45;
   }

   vertex0.setMag(dVertexRadius);
   vertex0.setTheta(theta);
   vertex0.setPhi(phi);

   // GS: Get the user defined direction for the primaries and
   // GS: Rotate the random position according to the user defined direction for the particle

   dir0 = fParticleGun->GetParticleMomentumDirection();
   if (dir0.mag() > 0.001) {
   theta0 = dir0.theta();
   phi0 = dir0.phi();
   }

   if (theta0 != 0.) {
   G4ThreeVector rotationAxis(1., 0., 0.);
   rotationAxis.setPhi(phi0 + halfpi);
   vertex0.rotate(theta0 + pi, rotationAxis);
   }
   fParticleGun->SetParticlePosition(vertex0);
   break;
   }				//Source Type

   G4double pEnergy;
   switch (nSpectrumType) {
   case 0:
   fEngDist->SetEnergyDisType("Pow");
   fEngDist->SetAlpha(-1);
   fEngDist->SetEmin(10 * keV);
   fEngDist->SetEmax(100 * MeV);
   break;

   case 1:
   fEngDist->SetEnergyDisType("Cdg");
   fEngDist->SetEmin(10 * keV);
   fEngDist->SetEmax(100 * MeV);
   break;

   case 2:
   do {
   y = G4UniformRand() * 100000.0;
   pEnergy = G4UniformRand() * 10. * GeV;
   f = std::pow(pEnergy * (1 / GeV), -4.);
   } while (y > f);

   fParticleGun->SetParticleEnergy(pEnergy);
   break;

   case 3:
   break;
   }				//Spectrum Type

   fParticleGun->GeneratePrimaryVertex(anEvent);
   }
   //---  For Particle Source  ---
   else {
   fParticleSource->GeneratePrimaryVertex(anEvent);
   }*/

  //---  For Particle Gun  ---
  if (sourceGun) {
    char name[50];
    /*G4int nP = G4UniformRand() * 223;
     if (nP < 112)
     sprintf(name, "UpPixel%d", nP);
     //      sprintf(name, "UpPixel91");
     else
     sprintf(name, "DownPixel%d", (nP - 112));*/
//      sprintf(name, "DownPixel109");
//  G4cout << name << G4endl;
    G4bool verbose = true;
    G4PhysicalVolumeStore *volumestore;
//    G4VPhysicalVolume *pvol = volumestore->GetVolume(name, verbose);
    /*    G4VPhysicalVolume *pvol = volumestore->GetVolume("UpPixel91", verbose);
     G4VPhysicalVolume *pvol2 = volumestore->GetVolume("DownPixel109", verbose);
     G4LogicalVolume *lvol = pvol->GetLogicalVolume();
     G4LogicalVolume *lvol2 = pvol2->GetLogicalVolume();
     //    G4cout << G4BestUnit(lvol->GetMass(), "Mass") << G4endl;
     //    getchar();
     G4VSolid *solid = lvol->GetSolid();
     G4VSolid *solid2 = lvol2->GetSolid();
     */

    // ------For whole detector------
//    const G4int nbPix = 224;
//    const G4int nbPix = 2*fDetector->GetNbOfPixel();
//    G4VSolid *solid[nbPix];
//
//    for (int i = 0; i < nbPix; i++) {
//      if (i < 112)
//        sprintf(name, "UpPixel%d", i);
//      else
//        sprintf(name, "DownPixel%d", (i - 112));
//
//      solid[i] = volumestore->GetVolume(name, verbose)->GetLogicalVolume()->GetSolid();
//    }
    //------------------------------
    // ------For bottom crystals only------
//    const G4int nbPix = 10;
//    const G4int nbPix = 126; // V2R8
    const G4int nbPix = fDetector->GetNbOfPixel();
    G4VSolid *solid[nbPix];

    for (int i = 0; i < nbPix; i++) {
      sprintf(name, "DownPixel%d", i);

      solid[i] = volumestore->GetVolume(name, verbose)->GetLogicalVolume()->GetSolid();
    }

    dVertexRadius = 9 * cm;
    //------------------------------

    // ------For prototype------
//    const G4int nbPix = 6;
//    G4VSolid * solid[nbPix];
//    G4String pixNames[] = {"UpPixel90", "UpPixel91", "UpPixel109", "DownPixel107", "DownPixel109", "DownPixel110"};
//    G4int np(0);
//    for (auto nam: pixNames) {
//      solid[np++] = volumestore->GetVolume(nam, verbose)->GetLogicalVolume()->GetSolid();
//    }
    //------------------------------

    G4ThreeVector point;
    G4int maxtries = 100000, itry = 1;

    G4double xylo = -dVertexRadius; //all
    G4double xyhi = dVertexRadius; //all
//    G4double xylo = -5*cm;// prototype
//    G4double xyhi = 5*cm;// prototype
//    G4double zlo = 0.0 * cm;
    G4double zlo = -dVertexRadius;
    G4double zhi = dVertexRadius;

    G4bool inCrystal = false;
    do {
      point.set(xylo + G4UniformRand() * (xyhi - xylo), xylo + G4UniformRand() * (xyhi - xylo),
          zlo + G4UniformRand() * (zhi - zlo));

      for (int i = 0; i < nbPix; i++) {
        if (solid[i]->Inside(point)) {
          inCrystal = true;
          break;
        }
      }

    } while (!inCrystal && (++itry < maxtries));
//    } while (!(solid->Inside(point) || solid2->Inside(point)) && ++itry < maxtries);

    fParticleGun->SetParticlePosition(point);

    if (itry >= maxtries) {
      G4cerr << "Unable to find a point inside your volume!" << G4endl;

      fParticleGun->SetParticleDefinition(G4Geantino::Geantino());
      fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
      fParticleGun->SetParticleEnergy(1. * keV);
    } else {
//    if (fParticleGun->GetParticleDefinition() == G4Geantino::Geantino()) {
      G4int Z = 71, A = 176;
      G4double ionCharge = 0. * eplus;
      G4double excitEnergy = 0. * keV;

      G4ParticleDefinition *ion = G4IonTable::GetIonTable()->GetIon(Z, A, excitEnergy);
      fParticleGun->SetParticleDefinition(ion);
      fParticleGun->SetParticleCharge(ionCharge);
    }
    //create vertex
    //
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
//---  For Particle Source  ---
  else {
//    fParticleSource = new G4GeneralParticleSource();

    fParticleSource->GeneratePrimaryVertex(anEvent);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

