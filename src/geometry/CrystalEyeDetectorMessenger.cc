//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "geometry/CrystalEyeDetectorMessenger.hh"
#include "geometry/CrystalEyeDetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeDetectorMessenger::CrystalEyeDetectorMessenger(CrystalEyeDetectorConstruction *CrystalEyeDet) :
    CrystalEyeDetector(CrystalEyeDet)

{
  CrystalEyedetDir = new G4UIdirectory("/payload/");
  CrystalEyedetDir->SetGuidance("CrystalEye payload control.");

  // Up Pixel thickness command

  UpPixelThickCmd = new G4UIcmdWithADoubleAndUnit("/payload/setUpPixThick", this);
  UpPixelThickCmd->SetGuidance("Set Thickness of the Up Pixel");
  UpPixelThickCmd->SetParameterName("Size", false);
  UpPixelThickCmd->SetRange("Size>=0.");
  UpPixelThickCmd->SetUnitCategory("Length");
  UpPixelThickCmd->AvailableForStates(G4State_Idle);

  // Down Pixel thickness command

  DownPixelThickCmd = new G4UIcmdWithADoubleAndUnit("/payload/setDownPixThick", this);
  DownPixelThickCmd->SetGuidance("Set Thickness of the Down Pixel");
  DownPixelThickCmd->SetParameterName("Size", false);
  DownPixelThickCmd->SetRange("Size>=0.");
  DownPixelThickCmd->SetUnitCategory("Length");
  DownPixelThickCmd->AvailableForStates(G4State_Idle);

  // Up Pixel thickness command

  ACDThickCmd = new G4UIcmdWithADoubleAndUnit("/payload/setACDThick", this);
  ACDThickCmd->SetGuidance("Set Thickness of the ACD");
  ACDThickCmd->SetParameterName("Size", false);
  ACDThickCmd->SetRange("Size>=0.");
  ACDThickCmd->SetUnitCategory("Length");
  ACDThickCmd->AvailableForStates(G4State_Idle);

  // number of  layers

  NbLayersCmd = new G4UIcmdWithAnInteger("/payload/setNbOfLayers", this);
  NbLayersCmd->SetGuidance("Set number of Layers.");
  NbLayersCmd->SetParameterName("NbLayers", false);
  NbLayersCmd->SetRange("NbLayers>0 && NbLayers<30");
  NbLayersCmd->AvailableForStates(G4State_Idle);

  //    Number of pixel

  NbPixelCmd = new G4UIcmdWithAnInteger("/payload/setNbOfPixel", this);
  NbPixelCmd->SetGuidance("Set number of Pixel.");
  NbPixelCmd->SetParameterName("NbPixel", false);
  NbPixelCmd->SetRange("NbPixel>0 && Pixel<224");
  NbPixelCmd->AvailableForStates(G4State_Idle);

  // update Payload

  UpdateCmd = new G4UIcmdWithoutParameter("/payload/update", this);
  UpdateCmd->SetGuidance("Update payload geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_Idle);

  // magnetic field

  MagFieldCmd = new G4UIcmdWithADoubleAndUnit("/payload/setField", this);
  MagFieldCmd->SetGuidance("Define magnetic field.");
  MagFieldCmd->SetGuidance("Magnetic field will be in Z direction.");
  MagFieldCmd->SetParameterName("Bz", false);
  MagFieldCmd->SetUnitCategory("Magnetic flux density");
  MagFieldCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

CrystalEyeDetectorMessenger::~CrystalEyeDetectorMessenger() {

  delete UpPixelThickCmd;
  delete DownPixelThickCmd;
  delete ACDThickCmd;
  delete NbLayersCmd;
  delete NbPixelCmd;
  delete UpdateCmd;
  delete MagFieldCmd;
  delete CrystalEyedetDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void CrystalEyeDetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {

  //  if( command == UpPixelThickCmd )
  //    { CrystalEyeDetector->SetUpPixelThickness(UpPixelThickCmd->GetNewDoubleValue(newValue));}

  //  if( command == DownPixelThickCmd )
  //      { CrystalEyeDetector->SetDownPixelThickness(DownPixelThickCmd->GetNewDoubleValue(newValue));}

  if (command == ACDThickCmd) {
    CrystalEyeDetector->SetACDThickness(ACDThickCmd->GetNewDoubleValue(newValue));
  }

  if (command == NbLayersCmd) {
    CrystalEyeDetector->SetNbOfLayers(NbLayersCmd->GetNewIntValue(newValue));
  }

  if (command == NbPixelCmd) {
    CrystalEyeDetector->SetNbOfPixel(NbPixelCmd->GetNewIntValue(newValue));
  }

  if (command == UpdateCmd) {
    CrystalEyeDetector->UpdateGeometry();
  }

  if (command == MagFieldCmd) {
    CrystalEyeDetector->SetMagField(MagFieldCmd->GetNewDoubleValue(newValue));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

