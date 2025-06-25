/********************************************************************************
 *
 * CEEventViewer.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 23 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CEEventViewer_h
#define CEEventViewer_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include<iostream>

#include "TGFrame.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGLAnnotation.h"
#include "TEveStraightLineSet.h"
#include "TEvePointSet.h"
#include "TEveBoxSet.h"
#include "TEveGeoNode.h"
#include "TEveGeoShape.h"
#include "TEveRGBAPaletteOverlay.h"

#include "analysis/CEPrimAnalysis.hh"
#include "analysis/CECalAnalysis.hh"
#include "analysis/CEAcdAnalysis.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CEEventViewer {
public:
  CEEventViewer(TString fName, string geomVer = "V1R0");
  virtual ~CEEventViewer();

  void SetVerbose(Int_t verb) {
    fVerbose = verb;
  }

  void SetDetectors(TString detFlags);

  void Init();
  void Geom();

  /*! @brief Event navigation utility; go forward one event. */
  void Fwd();

  /*! @brief Event navigation utility; go backward one event. */
  void Bck();

  /*! @brief Event navigation utility; set the event as given in the interface. */
  void SetEvt();

private:
  /*! @brief Make the GUI. */
  void FMakeGUI();
  /*! @brief Add a dummy geometry to the scene. */
  void FAddGeom();
  /*! @brief Delete the information of the previous event during event navigation. */
  void FDeleteEvent();
  /*! @brief Load the current event in the scene. */
  void FLoadEvent();
  /*! @brief Handle the primary particle information of the event. */
  void FPrimEvent();
  /*! @brief Handle the calorimeter information of the event. */
  void FCalEvent();
  /*! @brief Handle the ACT information of the event. */
  void FAcdEvent();
  // Handle the text information to be shown in the viewer for the event
  void FLabel();

  Int_t fVerbose = 0;
  TString fGeomFile;
  vector<TString> fUseDet;

  CEIOStreamer *fStreamer;
  CEPrimAnalysis *fPrimAna;
  CECalAnalysis *fCalAna;
  CEAcdAnalysis *fAcdAna;

  Int_t fNEntPrim = 0;
  Int_t fNEntCal = 0;
  Int_t fNEntAcd = 0;
  Int_t fEvtId = 0; ///< event Id

  TGMainFrame *fMainFrame = nullptr; ///< main frame
  TGHorizontalFrame *fHFrame = nullptr; ///< horizontal frame
  TGPictureButton *fPicBut = nullptr; ///< picture button used for navigation
  TGNumberEntry *fNumEntry = nullptr; ///< event navigation number entry
  TGTextEntry *fTextEntry = nullptr; ///< event navigation text
  TGLAnnotation *fAnnot = nullptr; ///< event info on the display

  TVector3 fPrimPos;
  TVector3 fPrimDir;
  Float_t fPrimEng = 0;

  TEveStraightLineSet *fPrimTrk = nullptr; ///< primary track
  TEvePointSet *fPrimVertex = nullptr; ///< primary vertex

  static const Int_t fNCOL = 255;
  Int_t fColors[fNCOL] = { 0 };
  TEveRGBAPalette *fPalette = nullptr;
  TEveRGBAPaletteOverlay *fPalOver = nullptr;

  TEveGeoTopNode *fCEGeom = nullptr;
  vector<TEveGeoShape*> fCalCrst = { nullptr };
  vector<TEveGeoShape*> fAcdCrst = { nullptr };

  TString fText = ""; ///< text string

  string fGeomVer = "V1R0";
  Int_t fNPIXEL = 0;

  ClassDef(CEEventViewer, 1)
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CEEventViewer_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
