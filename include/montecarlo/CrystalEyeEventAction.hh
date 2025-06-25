// **************************************************************
//
//      ------------ CrystalEyeEventAction  -----------
//           by Libo Wu (23 Sep 2021)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (09 Feb, 2024)
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeEventAction_h
#define CrystalEyeEventAction_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"

class CrystalEyeRunAction;
class CrystalEyeAnticoincidenceDataManager;
class CrystalEyeCalorimeterDataManager;
class CrystalEyeAnalysis;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeEventAction: public G4UserEventAction {
public:
  CrystalEyeEventAction(CrystalEyeRunAction *aRunAction);

  virtual ~CrystalEyeEventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  void AddStructureEdep(G4double edep) {
    fStruEdep += edep;
  }

  void AddGammaNumber() {
    fGammaNum++;
  }

  void SetFirstHitsVolume(G4int i) {
    firHitsVolume = i;
  }

  void SetInteraction(G4int i) {
    fInteraction = i;
  }

  G4int GetInteraction() {
    return fInteraction;
  }

  void SetEvtInFOV(G4int i) {
    fEvtInFOV = i;
  }

  int GetEvtInFOV() {
    return fEvtInFOV;
  }

  void SetFirstPos(G4ThreeVector xyz) {
    fFirstPos = xyz;
  }

  void GetPosFirstInteraction(G4ThreeVector xyz) {
    fIntPos = xyz;
  }

  G4int GetEventID() {
    return fEventID;
  }

private:
  G4int fEventID;
  G4double fStruEdep = 0;
  G4int fGammaNum = 0;
  G4int firHitsVolume = 0; //0/1: Strcuture/Senstive Detector
  G4int fInteraction = 0; //0/1: No/Yes Interact with Detector
  G4int fEvtInFOV = 0;
  G4ThreeVector fFirstPos;
  G4ThreeVector fIntPos;

  CrystalEyeRunAction *theRunAction;
  CrystalEyeCalorimeterDataManager *fDataManagerCal;
  CrystalEyeAnticoincidenceDataManager *fDataManagerAcd;
  CrystalEyeAnalysis *fAnalysis;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
