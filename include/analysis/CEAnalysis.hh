/********************************************************************************
 *
 * CEAnalysis.hh
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef CEAnalysis_h
#define CEAnalysis_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include<iostream>

#include "Rtypes.h"
#include "TVector3.h"

#include "analysis/CEIOStreamer.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
// Constants and definitions
//static const Int_t NPIXEL = 3;
//static const Int_t NPIXEL = 112;
//static const Float_t SRCRADIUS = 16.0; // (for bkg: 15.0 cm; for effArea: 16.0 cm passive shld: 40.0 cm)
//static const Float_t DETRADIUS = 14.5; // cm (14.6)

// Constants and definitions config2
//static const Int_t NPIXEL = 126; // v2, v2r1, v2r2, v2r4, v2r5, v2r6, v2r7:126; v2r3:96
//static const Float_t SRCRADIUS = 18.0; // cm; v2, v2r1, v2r2:17.0;  v2r3:18.0; v2r4:19.0; v2r5:22.0; v2r6:20.0; v2r7:17.0; v2r7str:18.0;
//static const Float_t DETRADIUS = 16.7; // cm; v2:16.26; v2r1:16.26; v2r2:14.76;  v2r3:17.6; v2r4:18.57; v2r5:21.2; v2r6:19.1; v2r7:16.7;

//static const Float_t SRCMAXTHETA = 90.0; // degree; maximum theta angle for particle generation

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CEAnalysis {
public:
  CEAnalysis();
  ~CEAnalysis();

  void SetStreamer(CEIOStreamer *streamer) {
    fStreamer = streamer;
  }

  void Init();
  void SetGeomVersion(string val);

  TVector3* UpCrystalPos();
  TVector3* BotCrystalPos();
  TVector3* UpAcdPos();
  TVector3 BotAcdPos();
  // Get the array[NPIXEL] of bottom crystal Ids. corresponding to up Ids (or vice versa for flag = 1)
  Int_t* UpToBotCrystalMap(bool botPix = false);
  // Get the array[NPIXEL] of ACD Ids. corresponding to up Ids (or vice versa for flag = 1)
  Int_t* UpCrystalToAcdMap(bool acdPix = false);
  // Get vector array of near by crystal Ids [ranging 0--(2*NPIXEL-1)] given a particular crystal (Id)
  vector<Int_t> NearbyCrystals(Int_t pid);

  // Get energy bins equal in log scale
  vector<Float_t> LogEnergyBin(Int_t nBins, Float_t minE, Float_t maxE);
  // Get energy bins according to detector resolution
  vector<Float_t> ResoEnergyBin(Float_t minE, Float_t maxE, Int_t nBins = 5, Float_t par0 = 7.003,
      Float_t par1 = 7.361);

  Int_t GetNEntries() {
    return fNEntries;
  }

  Int_t GetEvtId(Int_t ent);

  Int_t GetNPixels() {
    return fNPIXEL;
  }

  Float_t GetDetRadius() {
    return fDETRAD;
  }

  Float_t GetSigTimeWidth() {
    return SIGTIMEWIDTH;
  }

  // Set flag = 'e' for effective area and 's' for passive shielding (only for V1R0)
  Float_t GetSrcRadius(char flag = 'b');

  // Set flag = 's' for passive shielding (only for V1R0)
  Float_t GetSrcMaxTheta(char flag = 'b');

  Float_t GetIntBkgRate(bool bothLyr = true);

protected:
  // Fixed parameters for the analysis
  const Float_t ACDTHRESHOLD = 7.0; // keV
  const Float_t CALTHRESHOLD = 30.0; // keV
  const Float_t ACDUPEDEPCUT = 200.0; // keV
  const Float_t ACDBOTEDEPCUT = 0.0; // keV
  const Float_t SIGTIMEWIDTH = 90.0; // nS (signal time width)

  map<string, Int_t> fGeomNPix = { { "PIX1", 1 }, { "PROTO", 3 }, { "V1R0", 112 }, { "V2R0", 126 }, { "V2R1", 126 }, {
      "V2R2", 126 }, { "V2R3", 96 }, { "V2R4", 126 }, { "V2R5", 126 }, { "V2R6", 126 }, { "V2R7", 126 },
      { "V2R8", 126 }, { "V2R9", 126 } };

  map<string, Float_t> fGeomDetRad = { { "PIX1", 14.5 }, { "PROTO", 14.5 }, { "V1R0", 14.5 }, { "V2R0", 16.26 }, {
      "V2R1", 16.26 }, { "V2R2", 14.76 }, { "V2R3", 17.60 }, { "V2R4", 18.57 }, { "V2R5", 21.20 }, { "V2R6", 19.10 }, {
      "V2R7", 16.70 }, { "V2R8", 16.70 }, { "V2R9", 16.70 } };

  // Pixel (LYSO) mass (g) Top/Bot: V1R0: 143.72/26.95; V2R0: 41.88/16.22; V2R1: 42.89/16.22; V2R2: 43.62/16.22
  // V2R3: 128.43/32.16; V2R4: 98.65/25.14; V2R5: 95.92/24.68; V2R6: 87.77/24.01; V2R7: 119.23/21.93; V2R8: 151.21/21.93; V2R9: 176.41/21.93
  map<string, Float_t> fGeomCrystMassTop = { { "PIX1", 143.72 }, { "PROTO", 143.72 }, { "V1R0", 143.72 }, { "V2R0",
      41.88 }, { "V2R1", 42.89 }, { "V2R2", 43.62 }, { "V2R3", 128.43 }, { "V2R4", 98.65 }, { "V2R5", 95.92 }, { "V2R6",
      87.77 }, { "V2R7", 119.23 }, { "V2R8", 151.21 }, { "V2R9", 176.41 } };

  map<string, Float_t> fGeomCrystMassBot = { { "PIX1", 26.95 }, { "PROTO", 26.95 }, { "V1R0", 26.95 },
      { "V2R0", 16.22 }, { "V2R1", 16.22 }, { "V2R2", 16.22 }, { "V2R3", 32.16 }, { "V2R4", 25.14 }, { "V2R5", 24.68 },
      { "V2R6", 24.01 }, { "V2R7", 21.93 }, { "V2R8", 21.93 }, { "V2R9", 21.93 } };

  map<string, Float_t> fGeomSrcRad = { { "PIX1", 15.0 }, { "PROTO", 15.0 }, { "V1R0", 15.0 }, { "V2R0", 17.0 }, {
      "V2R1", 17.0 }, { "V2R2", 17.0 }, { "V2R3", 18.0 }, { "V2R4", 19.0 }, { "V2R5", 22.0 }, { "V2R6", 20.0 }, {
      "V2R7", 18.0 }, { "V2R8", 18.0 }, { "V2R9", 18.0 } };

  void FGetEntry(Int_t ent);

  CEIOStreamer *fStreamer = nullptr;

  TString fDataPath;
  vector<TVector3> fUpCrystPos;
  vector<TVector3> fBotCrystPos;
  vector<TVector3> fUpAcdPos;
  TVector3 fBotAcdPos;
  vector<Int_t> fUpToBotCrystMap;
  vector<Int_t> fUpCrystToAcdMap;

  TTree *fTree = nullptr;
  Int_t fEntId = -1;
  Int_t fNEntries = 0;
  Int_t fEvtId = -1;

  string fGeomVer = "";
  Int_t fNPIXEL = 0;
  Float_t fDETRAD = 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // CEAnalysis_h

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
