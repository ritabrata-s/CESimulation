// **************************************************************
//
//      ------ CrystalEyeAnalysis  ------
//           Modified by Libo Wu (06 May 2022)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (09 Feb 2024)
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "geometry/CrystalEyeDetectorConstruction.hh"
#include "geometry/CrystalEyeDetectorMessenger.hh"
#include "montecarlo/CrystalEyeAnticoincidenceSD.hh"
#include "montecarlo/CrystalEyeCalorimeterSD.hh"
#include "geometry/CrystalEyeSTLFromFile.hh"

#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
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
G4ThreadLocal G4GlobalMagFieldMessenger *CrystalEyeDetectorConstruction::fMagFieldMessenger = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeDetectorConstruction::CrystalEyeDetectorConstruction() :
    solidWorld(0), logicWorld(0), physiWorld(0), solidUpStructure(0), logicUpStructure(0), physiUpStructure(0), solidUpStructure2(
        0), uppix(0), logicUpPixel(0), physiUpPixel(0), solidDownStructure(0), logicDownStructure(0), physiDownStructure(
        0), downpix(0), logicDownPixel(0), physiDownPixel(0), solidACD(0), logicACD(0), physiACD(0) {

  StructurePath = std::getenv("CRYSTALEYE_DATA");
  StructurePath += "CE_STL/";
  checkOverlaps = false;
  fCadUnit = 0.1 * mm;

  // Default parameter values of the payload
  NbOfPixel = 112;
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

  detectorMessenger = new CrystalEyeDetectorMessenger(this);
}

//------------------------------------------------------------

CrystalEyeDetectorConstruction::~CrystalEyeDetectorConstruction() {
  delete detectorMessenger;
}

//------------------------------------------------------------

G4VPhysicalVolume* CrystalEyeDetectorConstruction::Construct() {
  DefineMaterials();
  return ConstructPayload();
}

//------------------------------------------------------------

void CrystalEyeDetectorConstruction::DefineMaterials() {
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
  G4Element *Cu = man->FindOrBuildElement("Cu");
  G4Element *W = man->FindOrBuildElement("W");
//  G4cout << W->GetName() << '\t'  << W->GetSymbol() << G4endl;
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

  //
  // define a material from elements.   case 2: mixture by fractional mass
  //

  density = 1.290 * mg / cm3;
  G4Material *Air = new G4Material(name = "Air", density, ncomponents = 2);
  Air->AddElement(N, fractionmass = 0.7);
  Air->AddElement(O, fractionmass = 0.3);

  density = 2.700 * g / cm3;
  G4Material *Al = new G4Material(name = "Aluminum", density, ncomponents = 1);
  Al->AddElement(Alumin, fractionmass = 1.);

  density = 8.96 * g / cm3;
  G4Material *mCu = new G4Material(name = "Copper", density, ncomponents = 1);
  mCu->AddElement(Cu, fractionmass = 1.);

  density = 19.3 * g / cm3;
  G4Material *mW = new G4Material(name = "Tungsten", density, ncomponents = 1);
  mW->AddElement(W, fractionmass = 1.);

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
  //G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  //default materials of the payload

  defaultMaterial = vacuum;
  ACDMaterial = Sci;
  upPixelMaterial = gagg; //lyso; //
  dnPixelMaterial = gagg;
  structureMaterial = Al;
  shieldMaterial1 = mCu;
  shieldMaterial2 = mW;
}

//------------------------------------------------------------

G4VPhysicalVolume* CrystalEyeDetectorConstruction::ConstructPayload() {
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
  VisAtt_Structure->SetColour(G4Colour::Red());

  G4VisAttributes *VisAtt_Shield_Cu = new G4VisAttributes();
  VisAtt_Shield_Cu->SetVisibility(true);
  VisAtt_Shield_Cu->SetForceSolid(TRUE);
  VisAtt_Shield_Cu->SetColour(G4Colour::Brown());

  G4VisAttributes *VisAtt_Shield_W = new G4VisAttributes();
  VisAtt_Shield_W->SetVisibility(true);
  VisAtt_Shield_W->SetForceSolid(TRUE);
  VisAtt_Shield_W->SetColour(G4Colour::Gray());

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
  // UP STRUCTURE
  //
  // fascia modello nuovo con anti a piastrelle
  std::string file_name;
  char *tmp_char;

  file_name.clear();
  file_name = StructurePath + "Pixel_Up/Semisfera_esterna.stl";
  tmp_char = const_cast<char*>(file_name.c_str());

  CrystalEyeSTLFromFile stlup(tmp_char);
  stlup.Scale(mm);
  solidUpStructure = stlup.CreateG4Solid("UpStructure");

  logicUpStructure = new G4LogicalVolume(solidUpStructure, structureMaterial, "UpStructure", 0, 0, 0);
  logicUpStructure->SetVisAttributes(VisAtt_Structure);

  physiUpStructure = new G4PVPlacement(0, G4ThreeVector(), "UpStructure", logicUpStructure, physiWorld, false, 0,
      checkOverlaps);

  //
  // ------------UP PIXEL------------
  //
  for (int i = 0; i < NbOfPixel; i++) {
    file_name.clear();
    file_name = StructurePath + "Pixel_Up/" + std::to_string(i) + ".stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stlupp(tmp_char);
    stlupp.Scale(mm);
    uppix = stlupp.CreateG4Solid("UpPixel" + std::to_string(i));

    logicUpPixel = new G4LogicalVolume(uppix, upPixelMaterial, "UpPixel" + std::to_string(i), 0, 0, 0);
    logicUpPixel->SetVisAttributes(VisAtt_Up);

//    G4cout << G4BestUnit(logicUpPixel->GetMass(), "Mass") << G4endl;

    physiUpPixel = new G4PVPlacement(0, G4ThreeVector(), "UpPixel" + std::to_string(i), logicUpPixel, physiWorld, false,
        i, checkOverlaps);

    /*
     G4ThreeVector xyz = physiUpPixel->GetTranslation();
     G4cout<<" ===== PixelID =  "<<i
     <<"  X = "<<xyz.x()
     <<"  Y = "<<xyz.y()
     <<"  Z = "<<xyz.z()
     <<"    ======"
     <<G4endl;
     */
  }

  //
  // ------------DOWN STRUCTURE ------------
  //

  file_name.clear();
  file_name = StructurePath + "Pixel_Down/07-FASCIA_INT.stl";
  tmp_char = const_cast<char*>(file_name.c_str());

  CrystalEyeSTLFromFile stldn(tmp_char);
  stldn.Scale(mm);
  solidDownStructure = stldn.CreateG4Solid("DownStructure");

  logicDownStructure = new G4LogicalVolume(solidDownStructure, structureMaterial, "DownStructure", 0, 0, 0);
  logicDownStructure->SetVisAttributes(VisAtt_Structure);
  physiDownStructure = new G4PVPlacement(0, G4ThreeVector(), "DownStructure", logicDownStructure, physiWorld, false, 0,
      checkOverlaps);

  //
  // ------------DOWN PIXEL------------
  //
  for (int i = 0; i < NbOfPixel; i++) {

    file_name.clear();
    file_name = StructurePath + "Pixel_Down/06-CRISTALLI_INT(" + std::to_string(i) + ").stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stldnp(tmp_char);
    stldnp.Scale(mm);
    downpix = stldnp.CreateG4Solid("DownPixel" + std::to_string(i));

    logicDownPixel = new G4LogicalVolume(downpix, dnPixelMaterial, "DownPixel" + std::to_string(i), 0, 0, 0);
    logicDownPixel->SetVisAttributes(VisAtt_Down);

//    G4cout << G4BestUnit(logicDownPixel->GetMass(), "Mass") << G4endl;

    physiDownPixel = new G4PVPlacement(0, G4ThreeVector(), "DownPixel" + std::to_string(i), logicDownPixel, physiWorld,
        false, NbOfPixel + i, checkOverlaps);	// Please NOTE the CopyNo!!!
  }

  //
  // ---------  Anticoincidence (ACD)  -------
  //

  for (int i = 0; i < NbOfPixel; i++) {
    file_name.clear();
    file_name = StructurePath + "Veto_Up/" + std::to_string(i) + ".stl";
    tmp_char = const_cast<char*>(file_name.c_str());

    CrystalEyeSTLFromFile stlacd(tmp_char);
    stlacd.Scale(mm);
    solidACD = stlacd.CreateG4Solid("ACD" + std::to_string(i));

    logicACD = new G4LogicalVolume(solidACD, ACDMaterial, "ACD" + std::to_string(i), 0, 0, 0);
    logicACD->SetVisAttributes(VisAtt_ACD);
    physiACD = new G4PVPlacement(0, G4ThreeVector(), "ACD" + std::to_string(i), logicACD, physiWorld, false, i,
        checkOverlaps);
  }

  /**/
  G4double botACDTh = 1.0 * cm;

  solidACD_Bottom = new G4Tubs("solidBottomACD", // name
      0. * cm,            // innerRadius
      14.5 * cm,          // outerRadius
      botACDTh / 2.,           // half length
      0. * deg,           // starting Phi
      360. * deg          // segment angle
          );
  logicACD_Bottom = new G4LogicalVolume(solidACD_Bottom, ACDMaterial, "ACD" + std::to_string(NbOfPixel), 0, 0, 0);
  logicACD_Bottom->SetVisAttributes(VisAtt_BottomACD);
  physiACD_Bottom = new G4PVPlacement(0, G4ThreeVector(0., 0., -botACDTh / 2.), "ACD" + std::to_string(NbOfPixel),
      logicACD_Bottom, physiWorld, false, NbOfPixel, checkOverlaps);
  /**/

  /*
   G4RotationMatrix *rot = new G4RotationMatrix();
   rot->rotateX(-90. * deg);
   G4VSolid *botACD[2];
   for (int i = 0; i < 2; i++) {
   file_name.clear();
   file_name = StructurePath + "Veto_Down/" + std::to_string(i) + ".stl";
   tmp_char = const_cast<char*>(file_name.c_str());

   CrystalEyeSTLFromFile stlacdd(tmp_char);
   stlacdd.Scale(mm);
   solidACD_Bottom = stlacdd.CreateG4Solid("ACD" + std::to_string(NbOfPixel + i));

   logicACD_Bottom = new G4LogicalVolume(solidACD_Bottom, ACDMaterial, "ACD" + std::to_string(NbOfPixel + i), 0, 0, 0);
   logicACD_Bottom->SetVisAttributes(VisAtt_BottomACD);
   physiACD_Bottom = new G4PVPlacement(rot, G4ThreeVector(), "ACD" + std::to_string(NbOfPixel), logicACD_Bottom,
   physiWorld, false, NbOfPixel, checkOverlaps);
   }
   */
  //
  // ---------  Passive shielding  -------
  //
  G4double cuShieldTh = 0.3 * cm;
//  G4double inRad = (14.5 + 25.5)* cm;
  G4double inRad = 14.5 * cm;
//  G4double outRad = 26.0 * cm;
//  G4double conHgt = 6.0 * cm;
  G4double extent = 7.0 * cm;
  G4double conHgt = 3.5 * cm;
//  G4double conAng = atan(conHgt / (outRad - inRad));
  G4double conAng = asin(conHgt / extent);
  G4double outRad = inRad + (extent * cos(conAng));

  solidShield_CuIn = new G4Cons("solidShield_CuIn", // name
//  solidShield_CuIn = new G4Tubs("solidShield_CuIn", // name
      0. * cm,            // innerRadius
      inRad,          // outerRadius
      0. * cm,            // innerRadius
      inRad + cuShieldTh / sin(conAng),          // outerRadius
      cuShieldTh / 2.,           // half length
      0. * deg,           // starting Phi
      360. * deg          // segment angle
          );

  logicShield_CuIn = new G4LogicalVolume(solidShield_CuIn, shieldMaterial1, "Shield_CuIn", 0, 0, 0);
  logicShield_CuIn->SetVisAttributes(VisAtt_Shield_Cu);
//  physiShield_CuIn = new G4PVPlacement(0, G4ThreeVector(0., 0., -(botACDTh + cuShieldTh / 2.)), "Shield_CuIn",
//      logicShield_CuIn, physiWorld, false, 0, checkOverlaps);

  solidShield_CuOut = new G4Cons("solidShield_CuOut", // name
      inRad,            // innerRadius
      inRad + cuShieldTh / sin(conAng),          // outerRadius
      outRad,            // innerRadius
      outRad + cuShieldTh / sin(conAng),          // outerRadius
      conHgt / 2.,           // half length
      0. * deg,           // starting Phi
      360. * deg          // segment angle
          );

  logicShield_CuOut = new G4LogicalVolume(solidShield_CuOut, shieldMaterial1, "Shield_CuOut", 0, 0, 0);
  logicShield_CuOut->SetVisAttributes(VisAtt_Shield_Cu);
//  physiShield_CuOut = new G4PVPlacement(0, G4ThreeVector(0., 0., -botACDTh + conHgt / 2.), "Shield_CuOut",
//      logicShield_CuOut, physiWorld, false, 0, checkOverlaps);

  G4double wShieldTh = 0.6 * cm;

  solidShield_WIn = new G4Cons("solidShield_WIn", // name
//  solidShield_WIn = new G4Tubs("solidShield_WIn", // name
      0. * cm,            // innerRadius
      inRad,          // outerRadius
      0. * cm,            // innerRadius
      inRad + wShieldTh / sin(conAng),          // outerRadius
      wShieldTh / 2.,           // half length
      0. * deg,           // starting Phi
      360. * deg          // segment angle
          );

  logicShield_WIn = new G4LogicalVolume(solidShield_WIn, shieldMaterial2, "Shield_WIn", 0, 0, 0);
  logicShield_WIn->SetVisAttributes(VisAtt_Shield_W);
//  physiShield_WIn = new G4PVPlacement(0, G4ThreeVector(0., 0., -(botACDTh + cuShieldTh + wShieldTh / 2.)), "Shield_WIn",
//      logicShield_WIn, physiWorld, false, 0, checkOverlaps);

  solidShield_WOut = new G4Cons("solidShield_WOut", // name
      inRad,            // innerRadius
      inRad + wShieldTh / sin(conAng),          // outerRadius
      outRad + cuShieldTh / sin(conAng),            // innerRadius
      outRad + cuShieldTh / sin(conAng) + wShieldTh / sin(conAng),          // outerRadius
      (conHgt + cuShieldTh) / 2.,           // half length
      0. * deg,           // starting Phi
      360. * deg          // segment angle
          );

  logicShield_WOut = new G4LogicalVolume(solidShield_WOut, shieldMaterial2, "Shield_WOut", 0, 0, 0);
  logicShield_WOut->SetVisAttributes(VisAtt_Shield_W);
//  physiShield_WOut = new G4PVPlacement(0, G4ThreeVector(0., 0., -(botACDTh + cuShieldTh) + (conHgt + cuShieldTh) / 2.),
//      "Shield_WOut", logicShield_WOut, physiWorld, false, 0, checkOverlaps);

  //
  // ---------  2 mm thick Al housing  -------
  //

  G4Sphere *solidHouse = new G4Sphere("solidHouse", // name
      14.7 * cm,            // innerRadius
      14.9 * cm,          // outerRadius
      0. * deg,           // starting Phi
      360. * deg,          // end Phi
      0. * deg,           // starting Theta
      90. * deg          // end Theta
          );

  G4LogicalVolume *logicHouse = new G4LogicalVolume(solidHouse, structureMaterial, "alHouse", 0, 0, 0);
//  logicHouse->SetVisAttributes(VisAtt_Shield_W);
//  G4PVPlacement *physiHouse = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), "alHouse", logicHouse, physiWorld, false,
//      0, checkOverlaps);

  //
  //always return the physical World
  //
  return physiWorld;
}

//------------------------------------------------------------  

void CrystalEyeDetectorConstruction::ConstructSDandField() {
  //  -------  Set Sensitive Detectors: Anticoincidence --------
  if (anticoincidenceSD.Get() == 0) {
    CrystalEyeAnticoincidenceSD *SD = new CrystalEyeAnticoincidenceSD("AnticoincidenceSD");
    anticoincidenceSD.Put(SD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(anticoincidenceSD.Get());

  for (int i = 0; i < NbOfPixel; i++) {
    SetSensitiveDetector("ACD" + std::to_string(i), anticoincidenceSD.Get());
  }
  for (int i = 0; i < 1; i++) { // i<2 for double structure bottom acd
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

void CrystalEyeDetectorConstruction::SetNbOfLayers(G4int val) {
  NbOfLayers = val;
}

//------------------------------------------------------------

void CrystalEyeDetectorConstruction::SetNbOfPixel(G4int val) {
  NbOfPixel = val;
}

//------------------------------------------------------------

void CrystalEyeDetectorConstruction::SetACDThickness(G4double val) {
  ACDThickness = val;
}

//------------------------------------------------------------

void CrystalEyeDetectorConstruction::SetMagField(G4double fieldValue) {
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

void CrystalEyeDetectorConstruction::UpdateGeometry() {
  //  delete payloadSD;
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructPayload());
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  G4RegionStore::GetInstance()->UpdateMaterialList(physiWorld);

  G4RunManager::GetRunManager()->ReinitializeGeometry();

}

//------------------------------------------------------------

