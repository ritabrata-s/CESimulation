// **************************************************************
//
//      ------ CrystalEyeSteppingAction  ------
//           by Libo Wu (12 Oct 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeSteppingAction.hh"
#include "montecarlo/CrystalEyeEventAction.hh"
#include "montecarlo/CrystalEyeAnalysis.hh"
#include "geometry/CrystalEyeDetectorConstruction.hh"

#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeSteppingAction::CrystalEyeSteppingAction(CrystalEyeEventAction *evt) :
    G4UserSteppingAction() {
  eventAct = evt;

  fevtid = -1;
}
//------------------------------------------------------------
CrystalEyeSteppingAction::~CrystalEyeSteppingAction() {
}

//------------------------------------------------------------
void CrystalEyeSteppingAction::UserSteppingAction(const G4Step *step) {
  G4int fID = eventAct->GetEventID();

  G4double fedep = step->GetTotalEnergyDeposit();
  G4int ftrackid = step->GetTrack()->GetTrackID();
  G4int fstepid = step->GetTrack()->GetCurrentStepNumber();
  //G4int fparentid= step->GetTrack()->GetParentID();
  //G4int fpdg = step->GetTrack()->GetDefinition()->GetPDGEncoding();

  G4StepPoint *fpreStepPoint = step->GetPreStepPoint();
  G4StepPoint *fpostStepPoint = step->GetPostStepPoint();
  G4LogicalVolume *fpreVolum = fpreStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  G4String fpreVolumName = fpreVolum->GetName();
  G4String fparticleName = step->GetTrack()->GetDefinition()->GetParticleName();
  G4ThreeVector fposition = fpreStepPoint->GetPosition();

  G4Track *aTrack = step->GetTrack();
  G4ThreeVector startPosition = aTrack->GetVertexPosition();

  if (fpreVolumName.find("Structure") != std::string::npos) {
    eventAct->AddStructureEdep(fedep);
  }
  //G4cout << "All: " << fposition.x() << " " <<fposition.y() << " " << fposition.z() << G4endl;
  //if(ftrackid==1 && fstepid==2){//Judge the first hits;
  if (ftrackid == 1 && eventAct->GetEvtInFOV() == 0) {
    if (fpreVolumName.find("World") == std::string::npos) {
      //G4LogicalVolume* fpostVolum = fpostStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
      //G4String fpostVolumName = fpostVolum->GetName();
      //G4cout
      //	<<"######  PriVolume: "<<fpreVolumName.c_str()
      //	<<"  PostVolume: "<<fpostVolumName.c_str()
      //	<<"  ######"<<G4endl;

      //G4cout
      //	<<"  EventID = "<<fID
      //	<<"  trackID = "<<ftrackid
      //	<<"  stepID = "<<fstepid
      //	<<G4endl;

      eventAct->SetEvtInFOV(1);
      //	eventAct->SetFirstPos(fposition);
      //	if( fpreVolumName.find("ACD")!=std::string::npos){
      if (startPosition.z() > 0) {
        //   G4cout << "startPosition: " << startPosition.z() << G4endl;
        //   G4cout << "Inside: " << fposition.z() << G4endl;
        eventAct->SetFirstPos(fposition);
        //eventAct->GetPosFirstInteraction(fposition);
      }
      //    if (fID % 100 == 0) {
      //  	cout << fposition.x() << endl;
      //   }
      //Judge the first hit Volume; 0:suppoting Sturcture 1:Senstive Detector
      if (fpreVolumName.find("Structure") == std::string::npos) {
        eventAct->SetFirstHitsVolume(1);
      }
    }
  }

  if (ftrackid != 1 && eventAct->GetInteraction() == 0) {
    if (fpreVolumName.find("World") == std::string::npos) {
      eventAct->SetInteraction(1);
    }
  }

  G4double stepTime = fpreStepPoint->GetGlobalTime();
  //if(stepTime>30.)
  //	G4cout
  //		<<"  stepTime = "<<stepTime
  //		<<"  stepTime = "<<stepTime/CLHEP::us
  //		<<"  stepTime = "<<stepTime/CLHEP::s
  //		<<"  stepEdep = "<<fedep
  //		<<G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
