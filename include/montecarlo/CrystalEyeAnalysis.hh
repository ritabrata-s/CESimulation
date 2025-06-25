// **************************************************************
//
//      ------ CrystalEyeAnalysis  ------
//           by Libo Wu (23 Sep 2021)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (09 Feb, 2024)
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CrystalEyeAnalysis_h
#define CrystalEyeAnalysis_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4ThreadLocalSingleton.hh"
#include "G4AnalysisManager.hh"

//class CrystalEyeAnalysisMessenger;
class CrystalEyeDetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeAnalysis {
  friend class G4ThreadLocalSingleton<CrystalEyeAnalysis> ;

private:
  CrystalEyeAnalysis();

public:
  virtual ~CrystalEyeAnalysis();

  static CrystalEyeAnalysis* GetInstance();

  void SetRootFileName(G4String val) {
    fRootFileName = val;
  }

  void SetRootFilePath(G4String val) {
    fRootFilePath = val;
  }

  void BeginOfRun();
  void EndOfRun();
  void BeginOfEvent();
  void EndOfEvent(G4int evtid);

  // Primary information container structure
  typedef struct PrimInfo {
    G4int evtId;
    G4int pdg;
    G4float energy;
    G4ThreeVector position;
    G4ThreeVector direction;

    void Reset() {
      evtId = -1;
      pdg = -1;
      energy = 0.;
      position = G4ThreeVector(-999, -999, -999);
      direction = G4ThreeVector(-999, -999, -999);
    }
  } PrimInfo;

  void SetPrimaryData(PrimInfo);

  // Event data container structure
  typedef struct EventData {
    G4int evtId;
    G4int gammaN;
    G4int tagFirstHit;
    G4int tagInteract;
    G4int tagEvtInFov;
    G4float edepStruct;
    G4ThreeVector firstPos;

    void Reset() {
      evtId = -1;
      gammaN = -1;
      tagFirstHit = 0.;
      tagInteract = 0.;
      tagEvtInFov = 0.;
      edepStruct = 0.;
      firstPos = G4ThreeVector(-999, -999, -999);
    }
  } EventData;

  void SetEventData(EventData);

  // Calorimeter data container structure
  typedef struct CalData {
    G4int evtId;
    G4int nHits;
    std::vector<G4int> crystalId;
    std::vector<G4float> edep;
    std::vector<G4float> edepDel;
    std::vector<G4int> order;
    std::vector<G4float> posX;
    std::vector<G4float> posY;
    std::vector<G4float> posZ;
    std::vector<G4float> tim;
    G4float totEdep;
    G4float totEdepDel;

    void Reset() {
      evtId = -1;
      nHits = 0;
      crystalId.clear();
      edep.clear();
      edepDel.clear();
      order.clear();
      posX.clear();
      posY.clear();
      posZ.clear();
      tim.clear();
      totEdep = 0.;
      totEdepDel = 0.;
    }
  } CalData;

  void SetDataCAL(CalData);

  // Anticoincidence data container structure
  typedef struct AcdData {
    G4int evtId;
    G4int nHits;
    std::vector<G4int> crystalId;
    std::vector<G4float> edep;
    std::vector<G4float> edepDel;
    std::vector<G4int> order;
    G4float totEdep;
    G4float totEdepDel;

    void Reset() {
      evtId = -1;
      nHits = 0;
      crystalId.clear();
      edep.clear();
      edepDel.clear();
      order.clear();
      totEdep = 0.;
      totEdepDel = 0.;
    }
  } AcdData;

  void SetDataACD(AcdData);

  // Total data container structure
  typedef struct TotalData {
    G4int evtId;
    G4float totEdep;
    G4float totEdepAcd;
    G4float totEdepAcdBot;
    G4float totEdepCalUp;
    G4float totEdepCalBot;
    G4float edepStruct;
    G4int tagInteract;
    G4int tagEvtInFov;
    G4float totEdepDel;
    G4float totEdepDelCal;
    G4float totEdepDelAcd;
    G4float totEdepDelAcdBot;
    G4ThreeVector firstPos;

    void Reset() {
      evtId = -1;
      totEdep = 0.;
      totEdepAcd = 0.;
      totEdepAcdBot = 0.;
      totEdepCalUp = 0.;
      totEdepCalBot = 0.;
      edepStruct = 0.;
      tagInteract = 0;
      tagEvtInFov = 0;
      totEdepDel = 0.;
      totEdepDelCal = 0.;
      totEdepDelAcd = 0.;
      totEdepDelAcdBot = 0.;
      firstPos = G4ThreeVector(-999, -999, -999);
    }
  } TotalData;

private:
  void FEventAnalysis();

  static CrystalEyeAnalysis *AnalysisInstance;
  const CrystalEyeDetectorConstruction *fDetector;

  G4String fRootFileName;
  G4String fRootFilePath;

  G4int fNofCells;
  G4int fNofCellsACD;

  PrimInfo fPrimInfo;
  EventData fEvtData;
  CalData fCalData;
  AcdData fAcdData;
  TotalData fTotalData;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
