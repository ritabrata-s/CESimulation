// **************************************************************
//
//      ------------ CrystalEyeCalorimeterHit  ------
//           by Libo Wu (20 Sep 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeCalorimeterHit.hh"

#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4ThreadLocal G4Allocator<CrystalEyeCalorimeterHit> *CrystalEyeCalorimeterHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeCalorimeterHit::CrystalEyeCalorimeterHit() {
  fEdep = 0.;
  fEdepDelay = 0.;
  fCrystalID = 0;
  fOrder = -1;
  fLength = 0.;
  fnSteps = 0;
  fdE_dx = 0.;
  fPos = G4ThreeVector(0., 0., 0.);
  fTime = 0.;
}

//---------------------------------------------
CrystalEyeCalorimeterHit::~CrystalEyeCalorimeterHit() {
}

//---------------------------------------------
CrystalEyeCalorimeterHit::CrystalEyeCalorimeterHit(const CrystalEyeCalorimeterHit &right) :
    G4VHit() {
  fEdep = right.fEdep;
  fEdepDelay = right.fEdepDelay;
  fCrystalID = right.fCrystalID;
  fOrder = right.fOrder;
  fLength = right.fLength;
  fnSteps = right.fnSteps;
  fdE_dx = right.fdE_dx;
  fPos = right.fPos;
  fTime = right.fTime;
}

//---------------------------------------------
const CrystalEyeCalorimeterHit& CrystalEyeCalorimeterHit::operator=(const CrystalEyeCalorimeterHit &right) {
  fEdep = right.fEdep;
  fEdepDelay = right.fEdepDelay;
  fCrystalID = right.fCrystalID;
  fOrder = right.fOrder;
  fLength = right.fLength;
  fnSteps = right.fnSteps;
  fdE_dx = right.fdE_dx;
  fPos = right.fPos;
  fTime = right.fTime;

  return *this;
}

//---------------------------------------------
G4bool CrystalEyeCalorimeterHit::operator==(const CrystalEyeCalorimeterHit &right) const {
  G4bool b_edep = (fEdep == right.fEdep);
  G4bool b_edepDelay = (fEdepDelay == right.fEdepDelay);
  G4bool b_crystalid = (fCrystalID == right.fCrystalID);
  G4bool b_order = (fOrder == right.fOrder);
  G4bool b_lenght = (fLength == right.fLength);
  G4bool b_nsteps = (fnSteps == right.fnSteps);
  G4bool b_dedx = (fdE_dx == right.fdE_dx);
  G4bool b_pos = (fPos == right.fPos);
  G4bool b_time = (fTime == right.fTime);

  return (b_edep && b_edepDelay && b_crystalid && b_order && b_lenght && b_nsteps && b_dedx && b_pos && b_time);
}

//---------------------------------------------
void CrystalEyeCalorimeterHit::Draw() {
  ;
}

//---------------------------------------------
void CrystalEyeCalorimeterHit::Print() {
  G4cout << "Edep: " << std::setw(8) << G4BestUnit(fEdep, "Energy") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
