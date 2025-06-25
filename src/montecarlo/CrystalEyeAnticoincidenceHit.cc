// **************************************************************
//
//      ------------ CrystalEyeAnticoincidenceHit  ------
//           by Libo Wu (20 Sep 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeAnticoincidenceHit.hh"

#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4ThreadLocal G4Allocator<CrystalEyeAnticoincidenceHit> *CrystalEyeAnticoincidenceHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeAnticoincidenceHit::CrystalEyeAnticoincidenceHit() {
  fEdep = 0.;
  fEdepDelay = 0.;
  fAcdID = 0;
  fOrder = 0;
  pos = G4ThreeVector(0., 0., 0.);
}

//---------------------------------------------
CrystalEyeAnticoincidenceHit::~CrystalEyeAnticoincidenceHit() {
}

//---------------------------------------------
CrystalEyeAnticoincidenceHit::CrystalEyeAnticoincidenceHit(const CrystalEyeAnticoincidenceHit &right) :
    G4VHit() {
  fEdep = right.fEdep;
  fEdepDelay = right.fEdepDelay;
  fAcdID = right.fAcdID;
  fOrder = right.fOrder;
  pos = right.pos;
}

//---------------------------------------------
const CrystalEyeAnticoincidenceHit& CrystalEyeAnticoincidenceHit::operator=(const CrystalEyeAnticoincidenceHit &right) {
  fEdep = right.fEdep;
  fEdepDelay = right.fEdepDelay;
  fAcdID = right.fAcdID;
  fOrder = right.fOrder;
  pos = right.pos;

  return *this;
}

//---------------------------------------------
G4bool CrystalEyeAnticoincidenceHit::operator==(const CrystalEyeAnticoincidenceHit &right) const {
  G4bool b_edep = (fEdep == right.fEdep);
  G4bool b_edep_delay = (fEdepDelay == right.fEdepDelay);
  G4bool b_acdid = (fAcdID == right.fAcdID);
  G4bool b_order = (fOrder == right.fOrder);
  G4bool b_pos = (pos == right.pos);

  return b_edep && b_edep_delay && b_acdid && b_order && b_pos;
}

//---------------------------------------------
void CrystalEyeAnticoincidenceHit::Draw() {
  ;
}

//---------------------------------------------
void CrystalEyeAnticoincidenceHit::Print() {
  G4cout << "Edep: " << std::setw(8) << G4BestUnit(fEdep, "Energy") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
