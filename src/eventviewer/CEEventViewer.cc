/********************************************************************************
 *
 * CEEventViewer.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 23 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "eventviewer/CEEventViewer.hh"

#include "TObjArray.h"
#include "TObjString.h"
#include "TGeoManager.h"
#include "TEveManager.h"
#include "TEveViewer.h"
#include "TGLViewer.h"
#include "TEveBrowser.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TGeoMatrix.h"

ClassImp(CEEventViewer)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEEventViewer::CEEventViewer(TString fName, string geomVer) :
    fGeomVer(geomVer) {
  // Set the data streamer and analyzers
  fStreamer = new CEIOStreamer(fName);
  fPrimAna = new CEPrimAnalysis();
  fCalAna = new CECalAnalysis();
  fAcdAna = new CEAcdAnalysis();

  fCalAna->SetGeomVersion(fGeomVer);
  fNPIXEL = fCalAna->GetNPixels();
//  cout << "No. of pixels: " << fNPIXEL << endl;
  if (fNPIXEL < 1) {
    cout << "[CEEventViewer::CEEventViewer] No. of pixels (" << fNPIXEL << ") is wrong..." << endl;
    exit(1);
  }

  // Geometry from gdml file
  fGeomFile = std::getenv("CRYSTALEYE_DATA");
  if (!fGeomVer.compare("V1R0"))
    fGeomFile += "Geom/geometry.gdml";
  else if (!fGeomVer.compare("V2R8"))
    fGeomFile += "Geom/geometry-V2R8.gdml";
  else {
    cout << "[CEEventViewer::CEEventViewer] Detector geometry version " << fGeomVer.data()
        << " has not been implemented yet!" << endl;
    exit(1);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEEventViewer::~CEEventViewer() {
  delete fStreamer;
  delete fPrimAna;
  delete fCalAna;
  delete fAcdAna;

  delete fMainFrame;
  delete fHFrame;
  delete fPicBut;
  delete fNumEntry;
  delete fTextEntry;
  delete fAnnot;
  delete fPalette;
  delete fPalOver;

  delete fCEGeom;
  delete fPrimTrk;
  delete fPrimVertex;
  for (auto v : fCalCrst)
    delete v;
  for (auto v : fAcdCrst)
    delete v;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::SetDetectors(TString detFlags) {
  // Get the detector names from the flag
  TObjArray *str = detFlags.Tokenize("+");
  TIterator *it = str->MakeIterator();
  TObjString *st = NULL;

  // Set the logical volumes to be drawn
  while ((st = (TObjString*) it->Next())) {
    fUseDet.push_back(st->GetString());
  }

  if (fVerbose > 0) {
    cout << "[CEEventViewer::SetDetectors] Detectors in the current show-->" << endl;
    for (auto det : fUseDet)
      cout << det.Data() << endl;
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::Init() {
  // Prim data analysis
  fPrimAna->SetStreamer(fStreamer);
  fPrimAna->Init();
  fNEntPrim = fPrimAna->GetNEntries();
//  cout << "No. of primary entries = " << fNEntPrim << endl;

// Cal data analysis
  fCalAna->SetStreamer(fStreamer);
  fCalAna->SetGeomVersion(fGeomVer);
  fCalAna->Init();
  fNEntCal = fCalAna->GetNEntries();
//  cout << "No. of CAL entries = " << fNEntCal << endl;

// Acd data analysis
  fAcdAna->SetStreamer(fStreamer);
  fAcdAna->SetGeomVersion(fGeomVer);
  fAcdAna->Init();
  fNEntAcd = fAcdAna->GetNEntries();
//  cout << "No. of ACD entries = " << fNEntAcd << endl;

  if ((fNEntPrim != fNEntCal) || (fNEntPrim != fNEntAcd)) {
    printf("No. of entries in prim, cal and acd tree are not same, please check the data... \n");
    return;
  }

  // Edep colour scheme
  Double_t Red[5] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t Green[5] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t Blue[5] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  Double_t Length[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };

  Int_t FI = TColor::CreateGradientColorTable(5, Length, Red, Green, Blue, fNCOL);
  for (int i = 0; i < fNCOL; i++)
    fColors[i] = FI + i;

  // Palette
  fPalette = new TEveRGBAPalette();

  // Initiate TGeoManager
  TGeoManager::Import(fGeomFile.Data());

  // Create and customize TEveManager
  TEveManager::Create();
  gEve->GetDefaultViewer()->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
  gEve->GetDefaultViewer()->GetGLViewer()->SetClearColor(kBlack);

  // Call other functions
  this->FMakeGUI();
  this->FAddGeom();
  this->FDeleteEvent();
  this->FLoadEvent();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FMakeGUI() {
  // Create minimal GUI for event navigation.
  TEveBrowser *browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fMainFrame = new TGMainFrame(gClient->GetRoot(), 2000, 1600);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fHFrame = new TGHorizontalFrame(fMainFrame);

  TString icondir(Form("%s/icons/", gSystem->Getenv("ROOTSYS")));

  fPicBut = new TGPictureButton(fHFrame, gClient->GetPicture(icondir + "GoBack.gif"));
  fHFrame->AddFrame(fPicBut);
  fPicBut->Connect("Clicked()", "CEEventViewer", this, "Bck()");

  fPicBut = new TGPictureButton(fHFrame, gClient->GetPicture(icondir + "GoForward.gif"));
  fHFrame->AddFrame(fPicBut);
  fPicBut->Connect("Clicked()", "CEEventViewer", this, "Fwd()");

  fNumEntry = new TGNumberEntry(fHFrame);
  fHFrame->AddFrame(fNumEntry);
  fNumEntry->Connect("ValueSet(Int_t)", "CEEventViewer", this, "SetEvt()");
  (fNumEntry->GetNumberEntry())->Connect("ReturnPressed()", "CEEventViewer", this, "SetEvt()");

  fMainFrame->AddFrame(fHFrame);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fTextEntry = new TGTextEntry(fMainFrame);
  fTextEntry->SetEnabled(kFALSE);
  fMainFrame->AddFrame(fTextEntry, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

  browser->StopEmbedding();
  browser->SetTabTitle("EveNav", 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FAddGeom() {
  // Add geom
  gGeoManager->GetVolume("ACD0")->SetTransparency(60);
  gGeoManager->GetVolume("UpPixel0")->SetTransparency(60);
  gGeoManager->GetVolume("DownPixel0")->SetTransparency(60);

  TGeoNode *top = gGeoManager->GetTopNode();

  fCEGeom = new TEveGeoTopNode(gGeoManager, top);

  gEve->AddGlobalElement(fCEGeom);

  // Annotation
  fAnnot = new TGLAnnotation((TGLViewerBase*) gEve->GetDefaultGLViewer(), fText.Data(), 0.03, 0.9);
  fAnnot->SetUseColorSet(0);
  fAnnot->SetTextColor(kGray + 1);
  fAnnot->SetTextSize(0.03);  // % of window diagonal
  //  _annot->SetTransparency(0);

  //  gEve->FullRedraw3D(kTRUE);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::Fwd() {
  if (fEvtId < (fNEntPrim - 1)) {
    ++fEvtId;
    this->FDeleteEvent();
    this->FLoadEvent();
  } else {
    fTextEntry->SetTextColor(0xff0000);
    fTextEntry->SetText("Already at last event");
    printf("Already at last event.\n");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::Bck() {
  if (fEvtId > 0) {
    --fEvtId;
    this->FDeleteEvent();
    this->FLoadEvent();
  } else {
    fTextEntry->SetTextColor(0xff0000);
    fTextEntry->SetText("Already at first event");
    printf("Already at first event.\n");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::SetEvt() {
  fEvtId = fNumEntry->GetNumberEntry()->GetIntNumber();

  if ((fEvtId < (fNEntPrim - 1)) && (fEvtId >= 0)) {
    this->FDeleteEvent();
    this->FLoadEvent();
  } else {
    fTextEntry->SetTextColor(0xff0000);
    fTextEntry->SetText("Beyond valid event numbers");
    printf("Beyond valid event numbers.\n");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FDeleteEvent() {
  // Palette
  if (fPalOver) {
    gEve->GetDefaultGLViewer()->RemoveOverlayElement(fPalOver);
    delete fPalOver;
  }

  // Primary
  if (fPrimTrk)
    fPrimTrk->Destroy();

  if (fPrimVertex)
    fPrimVertex->Destroy();

  // CAL
  for (auto v : fCalCrst)
    if (v)
      v->Destroy();

  // ACD
  for (auto v : fAcdCrst)
    if (v)
      v->Destroy();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FLoadEvent() {
  cout << "EVENT NUMBER-------> " << fEvtId << endl;

  this->FPrimEvent();

  for (size_t ii = 0; ii < fUseDet.size(); ii++) {
//    cout << fUseDet[ii] << endl;
    if (fUseDet[ii].Contains("CAL"))
      this->FCalEvent();

    if (fUseDet[ii].Contains("ACD"))
      this->FAcdEvent();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fPalette->SetMinMax(0, (Int_t) fPrimEng);
  fPalette->SetupColorArray();
  fPalOver = new TEveRGBAPaletteOverlay(fPalette, 0.55, 0.1, 0.4, 0.05);
  gEve->GetDefaultGLViewer()->AddOverlayElement(fPalOver);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  this->FLabel();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fMainFrame->MapSubwindows();
  fMainFrame->Resize();
  fMainFrame->MapWindow();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  gEve->Redraw3D(kTRUE);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fTextEntry->SetTextColor(kBlack, true);
  fTextEntry->SetText(Form("Event %d loaded", fEvtId));

  if (fAnnot)
    fAnnot->SetText(fText);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FPrimEvent() {
  fPrimPos = fPrimAna->GetVertexPos(fEvtId); // in mm
  fPrimDir = fPrimAna->GetDirection(fEvtId);
  fPrimEng = fPrimAna->GetEnergy(fEvtId);

  TVector3 v1 = TVector3(fPrimPos.x() / 10., fPrimPos.y() / 10., fPrimPos.z() / 10.); // in cm
  TVector3 v2 = fPrimDir;
  v2.SetMag(30.); // 30 cm prim track
  TVector3 v3 = v1 + v2;

  fPrimTrk = new TEveStraightLineSet("Primary", "primary");
  fPrimTrk->AddLine(v1.x(), v1.y(), v1.z(), v3.x(), v3.y(), v3.z());
  fPrimTrk->SetLineColor(kCyan + 2);
  fPrimTrk->SetLineWidth(3);

  if (fPrimTrk)
    gEve->AddElement(fPrimTrk);

  fPrimVertex = new TEvePointSet("PrimVertex", 1);
  fPrimVertex->SetPoint(0, v1.x(), v1.y(), v1.z());
  fPrimVertex->SetMarkerColor(kRed + 2);
  fPrimVertex->SetMarkerStyle(20);
  fPrimVertex->SetMarkerSize(2);

  if (fPrimVertex)
    gEve->AddElement(fPrimVertex);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FCalEvent() {
  Int_t nHitsCal = fCalAna->GetNHits(fEvtId);
  vector<Float_t> edepCal;
  vector<Int_t> pixCal;
  edepCal.clear();
  pixCal.clear();
  fCalAna->GetSortedEdeps(fEvtId, edepCal, pixCal);

  char volname[100];
  char edepname[100];
  Int_t nN(0);
  TGeoRotation rot("rot", 0., 0., 0.);
  rot.RotateX(90.0);
  fCalCrst.clear();

  for (auto pId : pixCal) {
    if (pId < fNPIXEL) {
      sprintf(volname, "UpPixel%d", pId);
      sprintf(edepname, "EUpPixel%d", pId);
    } else {
      pId -= fNPIXEL;
      sprintf(volname, "DownPixel%d", pId);
      sprintf(edepname, "EDownPixel%d", pId);
    }
    Float_t cnt = /*1E-3 */edepCal[nN];
    Int_t col = (Int_t) ((cnt - 1) * (fNCOL - 1) / (fPrimEng - 1));
//    Int_t col = (Int_t)((cnt - edepCal[nHitsCal]) * (fNCOL - 1) / (edepCal[0] - edepCal[nHitsCal]));
//    cout << nN << '\t' << pId << '\t' << cnt << '\t' << col << '\t' << volname << endl;

    fCalCrst.push_back(new TEveGeoShape(edepname));
    fCalCrst[nN]->SetMainColor(fColors[col]);
    fCalCrst[nN]->SetMainTransparency(0);
    fCalCrst[nN]->SetMainAlpha(1);
    fCalCrst[nN]->SetShape(
        (TGeoShape*) gGeoManager->GetTopVolume()->FindNode(volname)->GetVolume()->GetShape()->Clone());
    if (!fGeomVer.compare("V2R8"))
      fCalCrst[nN]->RefMainTrans().SetFrom(rot);

    gEve->AddElement(fCalCrst[nN]);
    nN++;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FAcdEvent() {
  Float_t eAcdUp(0.), eAcdBot(0.);
  Float_t totEdepAcd = fAcdAna->GetTotalEdep(fEvtId, eAcdUp, eAcdBot);

  vector<Float_t> edepAcd = fAcdAna->GetEdeps(fEvtId);
  vector<Int_t> pixAcd = fAcdAna->GetPixelIds(fEvtId);
  Int_t nHitsAcd = pixAcd.size();

  //  vector < Float_t > vE = edepAcd;
  //  sort(vE.begin(), vE.end(), greater<Float_t>());

  char volname[100];
  char edepname[100];
  Int_t nN(0);
  TGeoRotation rot("rot", 0., 0., 0.);
  rot.RotateX(90.0);
  fAcdCrst.clear();

  for (auto pId : pixAcd) {
    sprintf(volname, "ACD%d", pId);
    sprintf(edepname, "EACD%d", pId);
    Float_t cnt = edepAcd[nN];
    Int_t col = (Int_t) ((cnt - 1) * (fNCOL - 1) / (fPrimEng - 1));
    //    Int_t col = (Int_t)((cnt - vE[nHitsAcd]) * (fNCOL - 1) / (vE[0] - vE[nHitsAcd]));
//    cout << nN << '\t' << pId << '\t' << cnt << '\t' << col << '\t' << volname << endl;

    fAcdCrst.push_back(new TEveGeoShape(edepname));
    fAcdCrst[nN]->SetMainColor(fColors[col]);
    fAcdCrst[nN]->SetMainTransparency(0);
    fAcdCrst[nN]->SetMainAlpha(1);
    TGeoShape *shp = gGeoManager->GetTopVolume()->FindNode(volname)->GetVolume()->GetShape();
    if (pId == fNPIXEL) {
      TGeoTranslation t1(0, 0, -0.5);
      fAcdCrst[nN]->RefMainTrans().SetFrom(t1);
    } else {
      if (!fGeomVer.compare("V2R8"))
        fAcdCrst[nN]->RefMainTrans().SetFrom(rot);
    }
    fAcdCrst[nN]->SetShape((TGeoShape*) shp->Clone());

    gEve->AddElement(fAcdCrst[nN]);
    nN++;
  }
/*
  TEveGeoShape *acdout = new TEveGeoShape("acdout");
  acdout->SetMainColor(fColors[1]);
  acdout->SetMainTransparency(0);
  acdout->SetMainAlpha(1);
  TGeoShape *shp = gGeoManager->GetTopVolume()->FindNode("ACD30")->GetVolume()->GetShape();
  acdout->SetShape((TGeoShape*) shp->Clone());
//  acdout->RefMainTrans().SetFrom(rot);
  TGeoTranslation t1(0., 0., 0.);
  TGeoCombiTrans mat(t1, rot);
//  Double_t *pos = mat.GetTranslation();
  acdout->RefMainTrans().SetFrom(mat);
//  TVector3 pos = acdout->RefMainTrans().GetPos();
//  cout << pos[0] << "\t" << pos[1] << "\t" << pos[2] << endl;
//  pos.SetMag(40.);
//  acdout->RefMainTrans().SetPos(pos.X(), pos.Y(), pos.Z());

  gEve->AddElement(acdout);
*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEEventViewer::FLabel() {
  fText.Form("Event Id = %d", fEvtId);

  TString text;
  text.Form("\n_____________________");
  fText = fText + text;

  text.Form("\n");
  fText = fText + text;

  // Primary particle info
//  _primVec = -_primVec;
  Double_t prTh = fPrimDir.Theta() * TMath::RadToDeg();
  Double_t prPh = fPrimDir.Phi() * TMath::RadToDeg();

  // Get calorimeter total energy deposition
  Float_t eCalUp(0.), eCalBot(0.);
  Float_t totEdepCal = fCalAna->GetTotalEdep(fEvtId, eCalUp, eCalBot);
  Int_t nHitsCal = fCalAna->GetNHits(fEvtId);

  // Get ACD total energy deposition
  Float_t eAcdUp(0.), eAcdBot(0.);
  Float_t totEdepAcd = fAcdAna->GetTotalEdep(fEvtId, eAcdUp, eAcdBot);
  Int_t nHitsAcd = (fAcdAna->GetPixelIds(fEvtId)).size();

//  cout << fEvtId << "\t" << fPrimEng << "\t" << nHitsCal << "\t" << totEdepCal << "\t" << totEdepAcd << endl;

  text.Form("\n\nPrimary:");
  fText = fText + text;
  text.Form("\nEnergy = %0.2e keV", fPrimEng);
  fText = fText + text;
  text.Form("\nX, Y, Z = %0.1f, %0.1f, %0.1f (mm)", fPrimPos.x(), fPrimPos.y(), fPrimPos.z());
  fText = fText + text;
  text.Form("\nTh, Ph = %0.1f, %0.1f (deg)", prTh, prPh);
  fText = fText + text;
  text.Form("\n\n");
  fText = fText + text;

  for (size_t ii = 0; ii < fUseDet.size(); ii++) {
    if (fUseDet[ii].Contains("CAL") && totEdepCal) {
      // Calorimeter info
      text.Form("\n_____________________");
      fText = fText + text;
      text.Form("\n\n\n\nCalorimeter:");
      fText = fText + text;
      text.Form("\nEdep (nCrystal) = %0.2e keV (%d) \n[up = %0.2e; bot = %0.2e]", totEdepCal, nHitsCal, eCalUp,
          eCalBot);
      fText = fText + text;
      text.Form("\n\n");
      fText = fText + text;
    } // end if CAL

    if (fUseDet[ii].Contains("ACD") && totEdepAcd) {
      // Anticoincident info
      text.Form("\n_____________________");
      fText = fText + text;
      text.Form("\n\n\n\nAnticoincidence:");
      fText = fText + text;
      text.Form("\nEdep (nCrystal) = %0.2e keV (%d) \n[up = %0.2e; bot = %0.2e]", totEdepAcd, nHitsAcd, eAcdUp,
          eAcdBot);
      fText = fText + text;
      text.Form("\n");
      fText = fText + text;
    } // end if ACD

  } // end for _useDet

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
