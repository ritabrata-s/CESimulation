// **************************************************************
//
//      ------ CrystalEyeAnalysis.cc  ------
//           by Libo Wu (23 Sep 2021)
//           Email: libo.wu@gssi.it
//
//           Modified by: Ritabrata Sarkar (09 Feb, 2024)
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyeAnalysis.hh"
#include "geometry/CrystalEyeDetectorConstruction.hh"
//#include "montecarlo/CrystalEyeAnalysisMessenger.hh"

#include "G4RunManager.hh"
#include "G4AutoDelete.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeAnalysis::CrystalEyeAnalysis() :
    fDetector(0), fRootFileName(""), fRootFilePath("") {
  fDetector =
      static_cast<const CrystalEyeDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  fNofCells = fDetector->GetNbOfPixel();
  fNofCellsACD = fNofCells + 1; //UpACDs+DownACD
}

//------------------------------------------------------------
CrystalEyeAnalysis::~CrystalEyeAnalysis() {
}

//------------------------------------------------------------
CrystalEyeAnalysis* CrystalEyeAnalysis::GetInstance() {
  static G4ThreadLocalSingleton<CrystalEyeAnalysis> theInstance;
  return theInstance.Instance();
}

//------------------------------------------------------------
void CrystalEyeAnalysis::BeginOfRun() {
  if (!fRootFileName.compare("")) {
    G4Exception("CrystalEyeAnalysis::BeginOfRun", "ANA0001", FatalException, "The output filename not found...!");
  }

  // Check for the .root extension
  if (fRootFileName.find(".root") > fRootFileName.size())
    fRootFileName.append(".root");

  G4AnalysisManager *man = G4AnalysisManager::Instance();
  man->OpenFile(fRootFilePath + fRootFileName);

  man->SetFirstNtupleId(1);

// ---  1  ---
  man->CreateNtuple("PRIMARY", "PrimaryParameter");
  man->CreateNtupleIColumn("EventID");    //0
  man->CreateNtupleIColumn("PDG");    //1
  man->CreateNtupleFColumn("Kinetic");    //2
  man->CreateNtupleFColumn("Dir_x");  //3
  man->CreateNtupleFColumn("Dir_y");  //4
  man->CreateNtupleFColumn("Dir_z");  //5
  man->CreateNtupleFColumn("Pos_x");  //6
  man->CreateNtupleFColumn("Pos_y");  //7
  man->CreateNtupleFColumn("Pos_z");  //8
  man->FinishNtuple();

// ---  2  ---
  man->CreateNtuple("TOTAL", "TotalEdepInfo");                   //
  man->CreateNtupleIColumn("EventID");                  // 0
  man->CreateNtupleFColumn("TotalEdep");			          // 1
  man->CreateNtupleFColumn("TotalEdep_ACD");            // 2
  man->CreateNtupleFColumn("TotalEdep_BottomACD");      // 3
  man->CreateNtupleFColumn("TotalEdep_UP");	            // 4
  man->CreateNtupleFColumn("TotalEdep_DOWN");           // 5
  man->CreateNtupleFColumn("StructureEdep");            // 6
  man->CreateNtupleIColumn("Interaction");              // 7
  man->CreateNtupleIColumn("EventInFOV");               // 8
  man->CreateNtupleFColumn("TotalEdepDelay");			      // 9
  man->CreateNtupleFColumn("TotalEdepDelay_CAL");       // 10
  man->CreateNtupleFColumn("TotalEdepDelay_ACD");       // 11
  man->CreateNtupleFColumn("TotalEdepDelay_BottomACD"); // 12
  man->CreateNtupleFColumn("FirstInt_x"); // 13
  man->CreateNtupleFColumn("FirstInt_y"); // 14
  man->CreateNtupleFColumn("FirstInt_z"); // 15
  man->FinishNtuple();

// ---  3  ---
  man->CreateNtuple("ACD", "Anticoincidence");
  man->CreateNtupleIColumn("EventID");        // 0
  man->CreateNtupleIColumn("nHits_ACD");      // 1
  man->CreateNtupleIColumn("PixelID_ACD[nHits]", fAcdData.crystalId);    // 2
  man->CreateNtupleFColumn("Edep_ACD[nHits]", fAcdData.edep);       // 3
  man->CreateNtupleIColumn("Order_ACD[nHits]", fAcdData.order);      // 4
//  man->CreateNtupleIColumn("PixelIDDelay_ACD[nHits]", fIdAcdDelay);    // 5
  man->CreateNtupleFColumn("EdepDelay_ACD[nHits]", fAcdData.edepDel);  // 6
  man->FinishNtuple();

// ---  4  ---
  man->CreateNtuple("CAL", "Calorimeter");
  man->CreateNtupleIColumn("EventID");        // 0
  man->CreateNtupleIColumn("nHits_CAL");      // 1
  man->CreateNtupleIColumn("PixelID_CAL[nHits]", fCalData.crystalId);    // 2
  man->CreateNtupleFColumn("Edep_CAL[nHits]", fCalData.edep);       // 3
  man->CreateNtupleIColumn("Order_CAL[nHits]", fCalData.order);      // 4
//  man->CreateNtupleIColumn("PixelIDDelay_CAL[nHits]", fIdCalDelay);    // 5
  man->CreateNtupleFColumn("EdepDelay_CAL[nHits]", fCalData.edepDel);  // 6
  man->CreateNtupleFColumn("EdepPosX_CAL[nHits]", fCalData.posX);       // 7
  man->CreateNtupleFColumn("EdepPosY_CAL[nHits]", fCalData.posY);       // 8
  man->CreateNtupleFColumn("EdepPosZ_CAL[nHits]", fCalData.posZ);       // 9
  man->CreateNtupleFColumn("EdepTime_CAL[nHits]", fCalData.tim);       // 10
  man->FinishNtuple();
}

//------------------------------------------------------------
void CrystalEyeAnalysis::EndOfRun() {
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  if (!(man->GetFileName()).compare("")) {
    G4Exception("ZireAnalysis::EndOfRun", "ANA0001", FatalException, "The output file not found...!");
  }

  man->Write();
  man->CloseFile();
}

//------------------------------------------------------------
void CrystalEyeAnalysis::BeginOfEvent() {
}

//------------------------------------------------------------
void CrystalEyeAnalysis::EndOfEvent(G4int evtid) {
// Calculate the total edep info of the eveny
  FEventAnalysis();

  G4AnalysisManager *man = G4AnalysisManager::Instance();

// ---  1: PrimaryInfo  ---
  man->FillNtupleIColumn(1, 0, fPrimInfo.evtId);
  man->FillNtupleIColumn(1, 1, fPrimInfo.pdg);
  man->FillNtupleFColumn(1, 2, fPrimInfo.energy);
  man->FillNtupleFColumn(1, 3, fPrimInfo.direction.x());
  man->FillNtupleFColumn(1, 4, fPrimInfo.direction.y());
  man->FillNtupleFColumn(1, 5, fPrimInfo.direction.z());
  man->FillNtupleFColumn(1, 6, fPrimInfo.position.x());
  man->FillNtupleFColumn(1, 7, fPrimInfo.position.y());
  man->FillNtupleFColumn(1, 8, fPrimInfo.position.z());
  man->AddNtupleRow(1);

// ---  2: TotalInfo  ---
  man->FillNtupleIColumn(2, 0, fTotalData.evtId);
  man->FillNtupleFColumn(2, 1, fTotalData.totEdep);
  man->FillNtupleFColumn(2, 2, fTotalData.totEdepAcd);
  man->FillNtupleFColumn(2, 3, fTotalData.totEdepAcdBot);
  man->FillNtupleFColumn(2, 4, fTotalData.totEdepCalUp);
  man->FillNtupleFColumn(2, 5, fTotalData.totEdepCalBot);
  man->FillNtupleFColumn(2, 6, fTotalData.edepStruct);
  man->FillNtupleIColumn(2, 7, fTotalData.tagInteract);
  man->FillNtupleIColumn(2, 8, fTotalData.tagEvtInFov);
  man->FillNtupleFColumn(2, 9, fTotalData.totEdepDel);
  man->FillNtupleFColumn(2, 10, fTotalData.totEdepDelCal);
  man->FillNtupleFColumn(2, 11, fTotalData.totEdepDelAcd);
  man->FillNtupleFColumn(2, 12, fTotalData.totEdepDelAcdBot);
  man->FillNtupleFColumn(2, 13, fTotalData.firstPos.x());
  man->FillNtupleFColumn(2, 14, fTotalData.firstPos.y());
  man->FillNtupleFColumn(2, 15, fTotalData.firstPos.z());
  man->AddNtupleRow(2);

// ---  3: AnticoincidenceInfo  ---
  man->FillNtupleIColumn(3, 0, fAcdData.evtId);
  man->FillNtupleIColumn(3, 1, fAcdData.nHits);
// Other tuples are vector and filled automatically at the setting of their values
  man->AddNtupleRow(3);

// ---  4: CalorimeterInfo  ---
  man->FillNtupleIColumn(4, 0, fCalData.evtId);
  man->FillNtupleIColumn(4, 1, fCalData.nHits);
// Other tuples are vector and filled automatically at the setting of their values
  man->AddNtupleRow(4);
}

//------------------------------------------------------------
void CrystalEyeAnalysis::SetPrimaryData(PrimInfo prim) {
  fPrimInfo.Reset();
  fPrimInfo = prim;
}

//------------------------------------------------------------
void CrystalEyeAnalysis::SetEventData(EventData evt) {
  fEvtData.Reset();
  fEvtData = evt;
}

//------------------------------------------------------------
void CrystalEyeAnalysis::SetDataCAL(CalData calo) {
  fCalData.Reset();
  fCalData = calo;

// Deep copy of the vectors
  fCalData.crystalId.assign(calo.crystalId.begin(), calo.crystalId.end());
  fCalData.edep.assign(calo.edep.begin(), calo.edep.end());
  fCalData.edepDel.assign(calo.edepDel.begin(), calo.edepDel.end());
  fCalData.order.assign(calo.order.begin(), calo.order.end());
  fCalData.posX.assign(calo.posX.begin(), calo.posX.end());
  fCalData.posY.assign(calo.posY.begin(), calo.posY.end());
  fCalData.posZ.assign(calo.posZ.begin(), calo.posZ.end());
  fCalData.tim.assign(calo.tim.begin(), calo.tim.end());
}

//------------------------------------------------------------
void CrystalEyeAnalysis::SetDataACD(AcdData acd) {
  fAcdData.Reset();
  fAcdData = acd;

// Deep copy of the vectors
  fAcdData.crystalId.assign(acd.crystalId.begin(), acd.crystalId.end());
  fAcdData.edep.assign(acd.edep.begin(), acd.edep.end());
  fAcdData.edepDel.assign(acd.edepDel.begin(), acd.edepDel.end());
  fAcdData.order.assign(acd.order.begin(), acd.order.end());
}

//------------------------------------------------------------
void CrystalEyeAnalysis::FEventAnalysis() {
  fTotalData.Reset();

  fTotalData.evtId = fPrimInfo.evtId;
  fTotalData.totEdep = fAcdData.totEdep + fCalData.totEdep;

  for (G4int i = 0; i < fAcdData.nHits; i++) {
    G4int pId = fAcdData.crystalId[i];
    G4float edep = fAcdData.edep[i];
    G4float edepDel = fAcdData.edepDel[i];

    if (pId < fNofCellsACD - 1) { // top acds
      fTotalData.totEdepAcd += edep;
      fTotalData.totEdepDelAcd += edepDel;
    } else { // bottom acd
      fTotalData.totEdepAcdBot += edep;
      fTotalData.totEdepDelAcdBot += edepDel;
    }
  }

  for (G4int i = 0; i < fCalData.nHits; i++) {
    G4int pId = fCalData.crystalId[i];
    G4float edep = fCalData.edep[i];

    if (pId < fNofCells) // up crystals
      fTotalData.totEdepCalUp += edep;
    else
      // bot crystals
      fTotalData.totEdepCalBot += edep;
  }

  fTotalData.edepStruct = fEvtData.edepStruct;
  fTotalData.tagInteract = fEvtData.tagInteract;
  fTotalData.tagEvtInFov = fEvtData.tagEvtInFov;
  fTotalData.totEdepDel = fAcdData.totEdepDel + fCalData.totEdepDel;
  fTotalData.totEdepDelCal = fCalData.totEdepDel;
  fTotalData.firstPos = fEvtData.firstPos;
}

//------------------------------------------------------------
