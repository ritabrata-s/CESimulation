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
#include "montecarlo/CrystalEyeEventAction.hh"
#include "montecarlo/CrystalEyeRunAction.hh"
#include "montecarlo/CrystalEyeAnalysis.hh"
#include "montecarlo/CrystalEyeAnticoincidenceDataManager.hh"
#include "montecarlo/CrystalEyeCalorimeterDataManager.hh"

#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeEventAction::CrystalEyeEventAction(CrystalEyeRunAction *aRunAction) :
    fEventID(-1), theRunAction(aRunAction) {

  if (!theRunAction) {
    G4Exception("CrystalEyeEventAction::CrystalEyeEventAction()", "CRYE0001", FatalException,
        "Null pointer to Run Action!");
  }

  fDataManagerCal = theRunAction->GetCalDataManager();
  fDataManagerAcd = theRunAction->GetAcsDataManager();

  fAnalysis = CrystalEyeAnalysis::GetInstance();
}

//------------------------------------------------------------
CrystalEyeEventAction::~CrystalEyeEventAction() {
}

//------------------------------------------------------------
void CrystalEyeEventAction::BeginOfEventAction(const G4Event *evt) {
  fStruEdep = 0;
  fGammaNum = 0;
  firHitsVolume = 0;
  fInteraction = 0;
  fEvtInFOV = 0;
  fFirstPos = G4ThreeVector(-999, -999, -999);

  fEventID = evt->GetEventID();
  if (fEventID % 1000 == 0)
    G4cout << "EventID: " << fEventID << G4endl;

  fDataManagerAcd->BeginEvent(evt);
  fDataManagerCal->BeginEvent(evt);

  fAnalysis->BeginOfEvent();
}

//------------------------------------------------------------
void CrystalEyeEventAction::EndOfEventAction(const G4Event *evt) {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Save primary particle information
  G4ThreadLocal CrystalEyeAnalysis::PrimInfo priminfo;
  priminfo.Reset();

  priminfo.evtId = evt->GetEventID();

  if (evt->GetNumberOfPrimaryVertex() > 1) {
    G4Exception("CrystalEyeEventAction::EndOfEventAction()", "CRYE0001", FatalException,
        "Multiple vertices detected! Checkout...");
  }

  G4PrimaryVertex *pv = evt->GetPrimaryVertex(0);
  G4PrimaryParticle *pp = pv->GetPrimary(0);

  priminfo.pdg = pp->GetPDGcode();
  priminfo.position = pv->GetPosition();
  priminfo.direction = pp->GetMomentumDirection();
  priminfo.energy = pp->GetKineticEnergy();

  fAnalysis->SetPrimaryData(priminfo);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Save calorimeter data
  fDataManagerCal->FillEvent(evt);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Save anticoincidence data
  fDataManagerAcd->FillEvent(evt);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Save event data
  G4ThreadLocal CrystalEyeAnalysis::EventData evtdata;
  evtdata.Reset();

  evtdata.evtId = evt->GetEventID();
  evtdata.gammaN = fGammaNum;
  evtdata.tagFirstHit = firHitsVolume;
  evtdata.tagInteract = fInteraction;
  evtdata.tagEvtInFov = fEvtInFOV;
  evtdata.edepStruct = fStruEdep;
  evtdata.firstPos = fFirstPos;

  fAnalysis->SetEventData(evtdata);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fAnalysis->EndOfEvent(fEventID);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
