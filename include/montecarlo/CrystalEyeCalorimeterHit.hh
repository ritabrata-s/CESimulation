// **************************************************************
//
//      ------------ CrystalEyeCalorimeterHit  ------
//           by Libo Wu (20 Sep 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeCalorimeterHit_h
#define CrystalEyeCalorimeterHit_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeCalorimeterHit: public G4VHit {
public:
  CrystalEyeCalorimeterHit();
  virtual ~CrystalEyeCalorimeterHit();
  CrystalEyeCalorimeterHit(const CrystalEyeCalorimeterHit&);

  const CrystalEyeCalorimeterHit& operator=(const CrystalEyeCalorimeterHit&);
  G4bool operator==(const CrystalEyeCalorimeterHit&) const;

  //  Memory allocation and de-allocation
  inline void* operator new(size_t);
  inline void operator delete(void*);

  //methods from base class
  virtual void Draw();
  virtual void Print();

private:
  G4double fEdep;
  G4double fEdepDelay;
  G4int fCrystalID;
  G4int fOrder;
  G4double fLength;
  G4int fnSteps;
  G4double fdE_dx;
  G4ThreeVector fPos;
  G4double fTime;

public:

  inline virtual void AddEnergy(G4double de) {
    fEdep += de;
  }

  inline virtual void AddEnergyDelay(G4double de) {
    fEdepDelay += de;
  }

  inline virtual void AddLength(G4double le) {
    fLength += le;
  }

  inline virtual void Add_dE_dx(G4double dedx) {
    fdE_dx += dedx;
  }

  inline virtual void AddnSteps() {
    fnSteps++;
  }

  inline virtual void SetCrystalID(G4int i) {
    fCrystalID = i;
  }

  inline virtual void SetCrystalOrder(G4int i) {
    fOrder = i;
  }

  inline virtual void SetPos(G4ThreeVector xyz) {
    fPos = xyz;
  }

  inline virtual void SetTime(G4double tim) {
    fTime = tim;
  }

  inline G4double GetEdep() {
    return fEdep;
  }

  inline G4double GetEdepDelay() {
    return fEdepDelay;
  }

  inline G4double GetLength() {
    return fLength;
  }

  inline G4double Get_de_dx() {
    return fdE_dx;
  }

  inline G4int GetnSteps() {
    return fnSteps;
  }

  inline G4int GetCrystalID() {
    return fCrystalID;
  }

  inline G4int GetCrystalOrder() {
    return fOrder;
  }

  inline G4ThreeVector GetPos() {
    return fPos;
  }

  inline G4double GetTime() {
    return fTime;
  }

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//using CrystalEyeCalorimeterHitsCollection = G4THitsCollection<CrystalEyeCalorimeterHit>;
typedef G4THitsCollection<CrystalEyeCalorimeterHit> CrystalEyeCalorimeterHitsCollection;
extern G4ThreadLocal G4Allocator<CrystalEyeCalorimeterHit>* CrystalEyeCalorimeterHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
inline void* CrystalEyeCalorimeterHit::operator new(size_t) {
  if (!CrystalEyeCalorimeterHitAllocator) {
    CrystalEyeCalorimeterHitAllocator = new G4Allocator<CrystalEyeCalorimeterHit>;
  }
  void *hit;
  hit = (void*) CrystalEyeCalorimeterHitAllocator->MallocSingle();
  return hit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
inline void CrystalEyeCalorimeterHit::operator delete(void *hit) {
  if (!CrystalEyeCalorimeterHitAllocator) {
    CrystalEyeCalorimeterHitAllocator = new G4Allocator<CrystalEyeCalorimeterHit>;
  }
  CrystalEyeCalorimeterHitAllocator->FreeSingle((CrystalEyeCalorimeterHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
