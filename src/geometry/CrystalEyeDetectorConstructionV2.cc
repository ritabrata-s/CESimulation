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
#include "geometry/CrystalEyeDetectorConstructionV2.hh"
#include "geometry/CrystalEyeDetectorMessenger.hh"
#include "montecarlo/CrystalEyeAnticoincidenceSD.hh"
#include "montecarlo/CrystalEyeCalorimeterSD.hh"
#include "geometry/CrystalEyeSTLFromFile.hh"

#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4UImanager.hh"

#include "G4Transform3D.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"

#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cstring>

#include "G4RegionStore.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4ThreadLocal G4GlobalMagFieldMessenger *CrystalEyeDetectorConstructionV2::fMagFieldMessenger = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeDetectorConstructionV2::CrystalEyeDetectorConstructionV2() {

  StructurePath = std::getenv("CRYSTALEYE_DATA");
//  StructurePath += "CE_STL/";
//  StructurePath += "CE_STL_V2/";
  StructurePath += "CE_STL_V2R8/"; // V2R1, V2R2, V2R3, V2R4, V2R5, V2R6, V2R7, V2R8, V2R9
  checkOverlaps = false;
  fCadUnit = 0.1 * mm;

// Default parameter values of the payload
  NbOfPixel = 126; // V2, V2R1, V2R2, V2R4, V2R5, V2R6, V2R7, V2R8, V2R9
//  NbOfPixel = 96; // V2R3
  NbOfLayers = 2;
  ACDThickness = 0.5 * cm;

  fBackgroundTimeD = 30 * CLHEP::s;
  fBackgroundTimeU = 100 * 24 * 60 * 60 * CLHEP::s;
  fThresholdTime = 50 * CLHEP::us;

  G4cout << "  \n@@@@@" << "  ACDThickness = " << ACDThickness << "mm" << "  fBkgTimeUp = " << fBackgroundTimeU << "ns"
      << "  fBkgTimeDown = " << fBackgroundTimeD << "ns" << "  fThresholdTime = " << fThresholdTime << "ns"
      << "  @@@@@@@\n" << G4endl;

  calorimeterSD.Put(0);
  anticoincidenceSD.Put(0);

//  detectorMessenger = new CrystalEyeDetectorMessenger(this);
}

//------------------------------------------------------------

CrystalEyeDetectorConstructionV2::~CrystalEyeDetectorConstructionV2() {
//  delete detectorMessenger;
}

//------------------------------------------------------------

G4VPhysicalVolume* CrystalEyeDetectorConstructionV2::Construct() {
  DefineMaterials();
  return ConstructPayload();
}

//------------------------------------------------------------

void CrystalEyeDetectorConstructionV2::DefineMaterials() {
  G4NistManager *man = G4NistManager::Instance();

  G4String name, symbol;
  G4double a, z, density;

  G4int ncomponents;
  G4int natoms;
  G4double fractionmass;
  G4double temperature, pressure;

  //
  // define Elements
  //

  /*
   a= 55.845*g/mole;
   G4Element* Iron = new G4Element(name="Iron", symbol="Fe", z=26.,a);

   a = 126.904*g/mole;
   G4Element* I  = new G4Element(name="Iodine"  ,symbol="I" , z= 53., a);

   a = 132.905*g/mole;
   G4Element* Cs  = new G4Element(name="Cesium"  ,symbol="Cs" , z= 55., a);

   a = 207.19*g/mole;
   G4Element* Lead = new G4Element(name="Lead", symbol="Pb", z=82., a);
   */

  //a = 174.97*g/mole;
  //G4Element* Lu = new G4Element(name="Luthetium", symbol="Lu", z=71. , a);
  //a = 88.9059*g/mole;
  //G4Element* Y = new G4Element(name="Yttrium", symbol="Y", z=39. , a);
  //a = 28.09*g/mole;
  //G4Element* Silicon = new G4Element(name="Silicon", symbol="Si", z=14., a);
  //a = 15.99*g/mole;
  //G4Element* O  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);
  //a = 14.006*g/mole;
  //G4Element* N  = new G4Element(name="Nitrogen"  ,symbol="N" , z= 7., a);
  //a = 1.01*g/mole;
  //G4Element* H  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);
  //a = 12.01*g/mole;
  //G4Element* C  = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a);
  //a = 26.98*g/mole;
  //G4Element* Alumin = new G4Element(name="Aluminum"  ,symbol="A G4Element* Alumin = man->FindOrBuildElement("Al"); G4Element* Alumin = man->FindOrBuildElement("Al"); G4Element* Alumin = man->FindOrBuildElement("Al"); G4Element* Alumin = man->FindOrBuildElement("Al");l" , z= 13., a);
  G4Element *Lu = man->FindOrBuildElement("Lu");
  G4Element *Y = man->FindOrBuildElement("Y");
  G4Element *Si = man->FindOrBuildElement("Si");
  G4Element *O = man->FindOrBuildElement("O");
  G4Element *N = man->FindOrBuildElement("N");
  G4Element *H = man->FindOrBuildElement("H");
  G4Element *C = man->FindOrBuildElement("C");
  G4Element *Alumin = man->FindOrBuildElement("Al");
  G4Element *Gd = man->FindOrBuildElement("Gd");
  G4Element *Ga = man->FindOrBuildElement("Ga");
  G4Element *Cr = man->FindOrBuildElement("Cr");
  G4Element *Cu = man->FindOrBuildElement("Cu");
  G4Element *Fe = man->FindOrBuildElement("Fe");
  G4Element *Mg = man->FindOrBuildElement("Mg");
  G4Element *Mn = man->FindOrBuildElement("Mn");
  G4Element *Ti = man->FindOrBuildElement("Ti");
  G4Element *Zn = man->FindOrBuildElement("Zn");
  G4Element *W = man->FindOrBuildElement("W");
//  G4cout << Ga->GetName() << '\t'  << Ga->GetSymbol() << G4endl;
//  getchar();

  //
  // define simple materials: LYSO
  //

  G4Material *LSO = new G4Material("LSO", 7.4 * g / cm3, 3);
  LSO->AddElement(Lu, 2);
  LSO->AddElement(Si, 1);
  LSO->AddElement(O, 5);
  G4Material *YSO = new G4Material("YSO", 4.5 * g / cm3, 3);
  YSO->AddElement(Y, 2);
  YSO->AddElement(Si, 1);
  YSO->AddElement(O, 5);
  G4Material *lyso = new G4Material("LYSO", 7.25 * g / cm3, 2); // 7.1 * g / cm3
  lyso->AddMaterial(LSO, 95 * perCent); // 90
  lyso->AddMaterial(YSO, 5 * perCent); // 10

  //density = 7.3*g/cm3;
  //G4Material* lyso = new G4Material(name="Lu2SiO5", density, ncomponents=4);
  //lyso->AddElement(Lu, natoms=1.8);//????
  //lyso->AddElement(Y, fractionmass=0.2);
  //lyso->AddElement(Silicon, natoms=1.);
  //lyso->AddElement(O , natoms=5.);

  //
  // define simple materials: GAGG (Gd3Al2Ga3O12)
  //

  G4Material *gagg = new G4Material("GAGG", 6.6 * g / cm3, 4);
  gagg->AddElement(Gd, 3);
  gagg->AddElement(Alumin, 2);
  gagg->AddElement(Ga, 3);
  gagg->AddElement(O, 12);

  //
  // define a material from elements.   case 1: chemical molecule
  //

  density = 1.032 * g / cm3;
  G4Material *Sci = new G4Material(name = "Scintillator", density, ncomponents = 2);
  Sci->AddElement(C, natoms = 9);
  Sci->AddElement(H, natoms = 10);

  density = 1.01 * g / cm3;
  G4Material *N12 = new G4Material(name = "Nylon12", density, ncomponents = 4);
  N12->AddElement(C, natoms = 12);
  N12->AddElement(H, natoms = 23);
  N12->AddElement(N, natoms = 1);
  N12->AddElement(O, natoms = 1);

  //
  // define a material from elements.   case 2: mixture by fractional mass
  //

  density = 1.290 * mg / cm3;
  G4Material *Air = new G4Material(name = "Air", density, ncomponents = 2);
  Air->AddElement(N, fractionmass = 0.7);
  Air->AddElement(O, fractionmass = 0.3);

  // Aluminum
  density = 2.700 * g / cm3;
  G4Material *Al = new G4Material(name = "Aluminum", density, ncomponents = 9);
//  Al->AddElement(Alumin, fractionmass = 1.0);
  Al->AddElement(Alumin, fractionmass = 0.9715);
  Al->AddElement(Cr, fractionmass = 0.0025);
  Al->AddElement(Cu, fractionmass = 0.0010);
  Al->AddElement(Fe, fractionmass = 0.0050);
  Al->AddElement(Mg, fractionmass = 0.0060);
  Al->AddElement(Mn, fractionmass = 0.0040);
  Al->AddElement(Si, fractionmass = 0.0070);
  Al->AddElement(Ti, fractionmass = 0.0010);
  Al->AddElement(Zn, fractionmass = 0.0020);

  // Carbon fiber
  density = 1.75 * g / cm3;
  G4Material *CFiber = new G4Material(name = "CarbonFiber", density, ncomponents = 1);
  CFiber->AddElement(C, fractionmass = 1.0);

  //
  // examples of vacuum
  //

  density = universe_mean_density;    //from PhysicalConstants.h
  pressure = 3.e-18 * pascal;
  temperature = 2.73 * kelvin;
  G4Material *vacuum = new G4Material(name = "Galactic", z = 1., a = 1.01 * g / mole, density, kStateGas, temperature,
      pressure);

  density = 1.e-5 * g / cm3;
  pressure = 2.e-2 * bar;
  temperature = STP_Temperature;         //from PhysicalConstants.h
  G4Material *beam = new G4Material(name = "Beam", density, ncomponents = 1, kStateGas, temperature, pressure);
  beam->AddMaterial(Air, fractionmass = 1.);

  // WindForm
  density = 1.097 * g / cm3;
//  pressure = 2.e-2 * bar;
//  temperature = STP_Temperature;         //from PhysicalConstants.h
  G4Material *Windform = new G4Material(name = "WindForm", density, ncomponents = 2, kStateSolid);
  Windform->AddMaterial(CFiber, fractionmass = 0.1175);
  Windform->AddMaterial(N12, fractionmass = 0.8825);
//  mWindF = new G4Material(name = "WindForm", density, ncomponents = 2, kStateSolid);
//  mWindF->AddMaterial(CFiber, fractionmass = 0.1175);
//  mWindF->AddMaterial(N12, fractionmass = 0.8825);

  density = 8.96 * g / cm3;
  G4Material *mCu = new G4Material(name = "Copper", density, ncomponents = 1);
  mCu->AddElement(Cu, fractionmass = 1.);

  density = 19.3 * g / cm3;
  mTung = new G4Material(name = "Tungsten", density, ncomponents = 1);
  mTung->AddElement(W, fractionmass = 1.);

  //G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  //default materials of the payload

  defaultMaterial = vacuum;
  ACDMaterial = Sci;
  upPixelMaterial = gagg; //lyso; //
  dnPixelMaterial = lyso; //gagg; //
  alStrMaterial = Al;
  wfStrMaterial = Windform;
//  mWindF = Windform;
}

//------------------------------------------------------------

G4VPhysicalVolume* CrystalEyeDetectorConstructionV2::ConstructPayload() {
  // Some visualization styles
  // Old Version
  //G4Colour(0.3,0.8,0.1), G4Colour(0.2,0.3,0.8), G4Colour(0.8,0.2,0.3)

  G4VisAttributes *VisAtt_ACD = new G4VisAttributes();
  VisAtt_ACD->SetVisibility(true);
  VisAtt_ACD->SetForceSolid(TRUE);
  VisAtt_ACD->SetColour(G4Colour::Cyan());

  G4VisAttributes *VisAtt_BottomACD = new G4VisAttributes();
  VisAtt_BottomACD->SetVisibility(true);
  VisAtt_BottomACD->SetForceSolid(TRUE);
  VisAtt_BottomACD->SetColour(G4Colour::Green());

  G4VisAttributes *VisAtt_Up = new G4VisAttributes();
  VisAtt_Up->SetVisibility(true);
  VisAtt_Up->SetForceSolid(TRUE);
  VisAtt_Up->SetColour(G4Colour::White());

  G4VisAttributes *VisAtt_Down = new G4VisAttributes();
  VisAtt_Down->SetVisibility(true);
  VisAtt_Down->SetForceSolid(TRUE);
  VisAtt_Down->SetColour(G4Colour::Yellow());

  G4VisAttributes *VisAtt_Structure = new G4VisAttributes();
  VisAtt_Structure->SetVisibility(true);
  VisAtt_Structure->SetForceWireframe(TRUE);
//  VisAtt_Structure->SetForceSolid(TRUE);
  VisAtt_Structure->SetColour(G4Colour::Red());

  // Complete the Payload parameters definition
  G4ThreeVector offset;

  WorldSizeXY = 80 * cm;
  WorldSizeZ = 80 * cm;

  //
  // World
  //
  solidWorld = new G4Box("World", WorldSizeXY / 2, WorldSizeXY / 2, WorldSizeZ / 2);
  logicWorld = new G4LogicalVolume(solidWorld, defaultMaterial, "World");
  physiWorld = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), "World", logicWorld, 0, false, 0, checkOverlaps);

  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  //
  // AL STRUCTURE
  //
  // fascia modello nuovo con anti a piastrelle
  std::string file_name;
  char *tmp_char;
//  int cpN = 0;

  G4RotationMatrix *rotstr = new G4RotationMatrix();
  rotstr->rotateX(-90.0 * deg); // V2R7
  for (int nVol = 1; nVol <= 22; nVol++) {
    file_name.clear();
    file_name = StructurePath + "Alum_Parts/" + std::to_string(nVol) + ".stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stlup(tmp_char);
    stlup.Scale(mm);
    solidAlStructure = stlup.CreateG4Solid("Alum" + std::to_string(nVol));

//    if (nVol >= 12)
//      logicAlStructure = new G4LogicalVolume(solidAlStructure, dnPixelMaterial, "Alum" + std::to_string(nVol), 0, 0, 0);
//    else
    logicAlStructure = new G4LogicalVolume(solidAlStructure, alStrMaterial, "Alum" + std::to_string(nVol), 0, 0, 0);

    logicAlStructure->SetVisAttributes(VisAtt_Structure);

    physiAlStructure = new G4PVPlacement(rotstr, G4ThreeVector(), "Alum" + std::to_string(nVol), logicAlStructure,
        physiWorld, false, nVol - 1, checkOverlaps);
  }

  //
  // WINDFORM STRUCTURE
  //
  for (int nVol = 1; nVol <= 64; nVol++) {
    file_name.clear();
    file_name = StructurePath + "Windform_Parts/" + std::to_string(nVol) + ".stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stlup(tmp_char);
    stlup.Scale(mm);
    solidWfStructure = stlup.CreateG4Solid("Windform" + std::to_string(nVol));

//    if (nVol > 32 && nVol <= 64 /*&& (nVol % 2)*/)
//      logicWfStructure = new G4LogicalVolume(solidWfStructure, dnPixelMaterial, "Windform" + std::to_string(nVol), 0, 0, 0);
////      wfStrMaterial = mTung;
//    else
//      logicWfStructure = new G4LogicalVolume(solidWfStructure, wfStrMaterial, "Windform" + std::to_string(nVol), 0, 0, 0);
////      wfStrMaterial = mWindF;

    logicWfStructure = new G4LogicalVolume(solidWfStructure, wfStrMaterial, "Windform" + std::to_string(nVol), 0, 0, 0);
    logicWfStructure->SetVisAttributes(VisAtt_Structure);

    physiWfStructure = new G4PVPlacement(rotstr, G4ThreeVector(), "Windform" + std::to_string(nVol), logicWfStructure,
        physiWorld, false, nVol - 1, checkOverlaps);
  }

  //
  // ------------UP PIXEL------------
  //
  G4RotationMatrix *rot = new G4RotationMatrix();
  rot->rotateX(-90.0 * deg); // V2R7, V2R8, V2R9
//  rot->rotateX(-58.5 * deg); // V2, V2R1, V2R2, V2R4, V2R5, V2R6
//  rot->rotateY(150 * deg); // V2R3
  G4ThreeVector *tran = new G4ThreeVector();
  //
  for (int i = 0; i < NbOfPixel; i++) {
    file_name.clear();
    file_name = StructurePath + "EST/" + std::to_string(i + 1) + ".stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stlupp(tmp_char);
    stlupp.Scale(mm);
    uppix = stlupp.CreateG4Solid("UpPixel" + std::to_string(i));

    logicUpPixel = new G4LogicalVolume(uppix, upPixelMaterial, "UpPixel" + std::to_string(i), 0, 0, 0);
    logicUpPixel->SetVisAttributes(VisAtt_Up);

//    G4cout << G4BestUnit(logicUpPixel->GetMass(), "Mass") << G4endl;

    physiUpPixel = new G4PVPlacement(rot, *tran, "UpPixel" + std::to_string(i), logicUpPixel, physiWorld, false, i,
        checkOverlaps);
  }

  //
  // ------------DOWN PIXEL------------
  //
  for (int i = 0; i < NbOfPixel; i++) {

    file_name.clear();
    file_name = StructurePath + "INT/" + std::to_string(i + 1) + ".stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stldnp(tmp_char);
    stldnp.Scale(mm);
    downpix = stldnp.CreateG4Solid("DownPixel" + std::to_string(i));

    logicDownPixel = new G4LogicalVolume(downpix, dnPixelMaterial, "DownPixel" + std::to_string(i), 0, 0, 0);
    logicDownPixel->SetVisAttributes(VisAtt_Down);

    //    G4cout << G4BestUnit(logicDownPixel->GetMass(), "Mass") << G4endl;

    physiDownPixel = new G4PVPlacement(rot, *tran, "DownPixel" + std::to_string(i), logicDownPixel, physiWorld, false,
        NbOfPixel + i, checkOverlaps); // Please NOTE the CopyNo!!!
  }

//  downpix = new G4Sphere("DownPixel0", 5 * cm, 8 * cm, 0, 2 * pi, 0., pi / 2.);
//  logicDownPixel = new G4LogicalVolume(downpix, dnPixelMaterial, "DownPixel0", 0, 0, 0);
//  logicDownPixel->SetVisAttributes(VisAtt_Down);
//
//  physiDownPixel = new G4PVPlacement(0, *tran, "DownPixel0", logicDownPixel, physiWorld, false, NbOfPixel,
//      checkOverlaps); // Please NOTE the CopyNo!!!

  //
  // ---------  Anticoincidence (ACD)  -------
  //

  for (int i = 0; i < NbOfPixel; i++) {
    file_name.clear();
    file_name = StructurePath + "ACS/" + std::to_string(i + 1) + ".stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stlacd(tmp_char);
    stlacd.Scale(mm);
    solidACD = stlacd.CreateG4Solid("ACD" + std::to_string(i));

    logicACD = new G4LogicalVolume(solidACD, ACDMaterial, "ACD" + std::to_string(i), 0, 0, 0);
    logicACD->SetVisAttributes(VisAtt_ACD);

    physiACD = new G4PVPlacement(rot, *tran, "ACD" + std::to_string(i), logicACD, physiWorld, false, i, checkOverlaps);
  }

  // Bottom ACD
  G4Tubs *solidACD_Bottom = new G4Tubs("solidBottomACD", // name
      0. * cm,            // innerRadius
      16.7 * cm, // outerRadius (V2:16.26; V2R1:16.26; V2R2:14.76; V2R3:17.6; V2R4:18.57; V2R5:21.2; V2R6:19.1; V2R789:16.7)
      0.5 * cm,           // half length
      0. * deg,           // starting Phi
      360. * deg          // segment angle
          );
  G4LogicalVolume *logicACD_Bottom = new G4LogicalVolume(solidACD_Bottom, ACDMaterial,
      "ACD" + std::to_string(NbOfPixel), 0, 0, 0);
  logicACD_Bottom->SetVisAttributes(VisAtt_BottomACD);

  physiACD_Bottom = new G4PVPlacement(0, G4ThreeVector(0., 0., -0.5 * cm), "ACD" + std::to_string(NbOfPixel),
      logicACD_Bottom, physiWorld, false, NbOfPixel, checkOverlaps);

  /*
   G4RotationMatrix *rot1 = new G4RotationMatrix();
   rot1->rotateX(-90. * deg);
   for (int i = 0; i < 2; i++) {
   file_name.clear();
   file_name = StructurePath + "ACS_BOT/" + std::to_string(i + 1) + ".stl";
   tmp_char = const_cast<char*>(file_name.c_str());

   CrystalEyeSTLFromFile stlacdd(tmp_char);
   stlacdd.Scale(mm);
   solidACD_Bottom = stlacdd.CreateG4Solid("ACD" + std::to_string(NbOfPixel + i));

   logicACD_Bottom = new G4LogicalVolume(solidACD_Bottom, ACDMaterial, "ACD" + std::to_string(NbOfPixel + i), 0, 0, 0);
   logicACD_Bottom->SetVisAttributes(VisAtt_BottomACD);
   physiACD_Bottom = new G4PVPlacement(rot1, G4ThreeVector(), "ACD" + std::to_string(NbOfPixel), logicACD_Bottom,
   physiWorld, false, NbOfPixel, checkOverlaps);
   }
   */

  //
  //always return the physical World
  //
  return physiWorld;
}

//------------------------------------------------------------  

void CrystalEyeDetectorConstructionV2::ConstructSDandField() {
  //  -------  Set Sensitive Detectors: Anticoincidence --------

  if (anticoincidenceSD.Get() == 0) {
    CrystalEyeAnticoincidenceSD *SD = new CrystalEyeAnticoincidenceSD("AnticoincidenceSD");
    anticoincidenceSD.Put(SD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(anticoincidenceSD.Get());

  for (int i = 0; i < NbOfPixel; i++) {
    SetSensitiveDetector("ACD" + std::to_string(i), anticoincidenceSD.Get());
  }
  for (int i = 0; i < 1; i++) { // double structure at bot ACD: 2 else 1
    SetSensitiveDetector("ACD" + std::to_string(NbOfPixel + i), anticoincidenceSD.Get());
  }

  //  -------  Set Sensitive Detectors: Calorimeter --------
  if (calorimeterSD.Get() == 0) {
    CrystalEyeCalorimeterSD *SD = new CrystalEyeCalorimeterSD("CalorimeterSD");
    calorimeterSD.Put(SD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(calorimeterSD.Get());

  for (int i = 0; i < NbOfPixel; i++) {
    SetSensitiveDetector("DownPixel" + std::to_string(i), calorimeterSD.Get());
    SetSensitiveDetector("UpPixel" + std::to_string(i), calorimeterSD.Get());
  }
//  SetSensitiveDetector("DownPixel0", calorimeterSD.Get());

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);

  return;

}

//------------------------------------------------------------

void CrystalEyeDetectorConstructionV2::SetNbOfLayers(G4int val) {
  NbOfLayers = val;
}

//------------------------------------------------------------

void CrystalEyeDetectorConstructionV2::SetNbOfPixel(G4int val) {
  NbOfPixel = val;
}

//------------------------------------------------------------

void CrystalEyeDetectorConstructionV2::SetACDThickness(G4double val) {
  ACDThickness = val;
}

//------------------------------------------------------------

void CrystalEyeDetectorConstructionV2::SetMagField(G4double fieldValue) {
  // Just invoke manually the MT-safe command
  // /globalField/setValue
  // instantiated by the GlobalFieldMessenger
  std::stringstream sss;
  sss << "/globalField/setValue 0 0 " << fieldValue / tesla << " tesla";

  G4String command = sss.str();
  G4cout << "Going to execute: " << command << G4endl;

  G4UImanager *UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand(command);

}

//------------------------------------------------------------

void CrystalEyeDetectorConstructionV2::UpdateGeometry() {
  //  delete payloadSD;
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructPayload());
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  G4RegionStore::GetInstance()->UpdateMaterialList(physiWorld);

  G4RunManager::GetRunManager()->ReinitializeGeometry();

}

//------------------------------------------------------------

