// **************************************************************
//
//      ------ CrystalEyeAnalysis  ------
//           Modified by Libo Wu (06 May 2022)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (30 September 2024)
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeDetectorConstructionV2_h
#define CrystalEyeDetectorConstructionV2_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4VUserDetectorConstruction.hh"
#include "G4Cache.hh"

class G4Box;
class G4Tubs;
class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UniformMagField;
class G4Region;
class CrystalEyeDetectorMessenger;
class CrystalEyeAnticoincidenceSD;
class CrystalEyeCalorimeterSD;
class G4GlobalMagFieldMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeDetectorConstructionV2: public G4VUserDetectorConstruction {
public:

  CrystalEyeDetectorConstructionV2();
  ~CrystalEyeDetectorConstructionV2();

public:

  void SetNbOfLayers(G4int); // CAL material, lenght, thickness
  void SetNbOfPixel(G4int);

  void SetACDThickness(G4double); //ACD Thickness

  void SetMagField(G4double); // Magnetic Field

  G4VPhysicalVolume* Construct();
  void UpdateGeometry();
  void ConstructSDandField();

public:

  void PrintPayloadParameters();

  G4double GetWorldSizeZ() const {
    return WorldSizeZ;
  }

  G4double GetWorldSizeXY() const {
    return WorldSizeXY;
  }

  G4int GetNbOfLayers() const {
    return NbOfLayers;
  }

  G4int GetNbOfPixel() const {
    return NbOfPixel;
  }

  G4double GetBackgroundTimeU() const {
    return fBackgroundTimeU;
  }

  G4double GetBackgroundTimeD() const {
    return fBackgroundTimeD;
  }

  G4double GetThresholdTime() const {
    return fThresholdTime;
  }

  G4double GetACDThickness() const {
    return ACDThickness;
  }

private:
  std::string StructurePath;
  bool checkOverlaps;
  G4double fCadUnit;

  G4int NbOfLayers;
  G4int NbOfPixel;
  G4double ACDThickness;
  G4double fBackgroundTimeD;
  G4double fBackgroundTimeU;
  G4double fThresholdTime;
  G4bool fCheckOverlaps = false;

  G4Material *defaultMaterial = nullptr;
  G4Material *upPixelMaterial = nullptr;
  G4Material *dnPixelMaterial = nullptr;
  G4Material *ACDMaterial = nullptr;
  G4Material *alStrMaterial = nullptr;
  G4Material *wfStrMaterial = nullptr;
  G4Material *mWindF = nullptr;
  G4Material *mTung = nullptr;

  G4double WorldSizeXY = 0.;
  G4double WorldSizeZ = 0.;

  G4Box *solidWorld  = nullptr; // World
  G4LogicalVolume *logicWorld = nullptr;
  G4VPhysicalVolume *physiWorld = nullptr;

  G4VSolid *solidAlStructure = nullptr; // Tracker
  G4LogicalVolume *logicAlStructure = nullptr;
  G4VPhysicalVolume *physiAlStructure = nullptr;

  G4VSolid *solidUpStructure2 = nullptr; // Tracker
  G4LogicalVolume *logicUpStructure2 = nullptr;

  G4VSolid *uppix = nullptr; // Calorimeter
  G4LogicalVolume *logicUpPixel = nullptr;
  G4VPhysicalVolume *physiUpPixel = nullptr;

  G4VSolid *solidWfStructure = nullptr; // Top Anticoincidence
  G4LogicalVolume *logicWfStructure = nullptr;
  G4VPhysicalVolume *physiWfStructure = nullptr;

  G4VSolid *downpix = nullptr; // Lateral Anticoincidence
  G4LogicalVolume *logicDownPixel = nullptr;
  G4VPhysicalVolume *physiDownPixel = nullptr;

  G4VSolid *solidACD = nullptr;
  G4LogicalVolume *logicACD = nullptr;
  G4VPhysicalVolume *physiACD = nullptr;

//  G4Tubs *solidACD_Bottom = nullptr;
  G4VSolid *solidACD_Bottom = nullptr;
  G4LogicalVolume *logicACD_Bottom = nullptr;
  G4VPhysicalVolume *physiACD_Bottom = nullptr;

  // magnetic field messenger
  static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;

//  CrystalEyeDetectorMessenger *detectorMessenger;  //pointer to the Messenger

  G4Cache<CrystalEyeAnticoincidenceSD*> anticoincidenceSD;
  G4Cache<CrystalEyeCalorimeterSD*> calorimeterSD;

private:

  void DefineMaterials();
  G4VPhysicalVolume* ConstructPayload();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
