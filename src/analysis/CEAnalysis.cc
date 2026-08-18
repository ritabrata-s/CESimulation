/********************************************************************************
 *
 * CEAnalysis.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CEAnalysis.hh"

#include<fstream>
#include<sstream>
#include<TF1.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEAnalysis::CEAnalysis() {
  fDataPath = std::getenv("CRYSTALEYE_DATA");
  fDataPath += "Geom/";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEAnalysis::~CEAnalysis() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysis::Init() {
  if (!fStreamer) {
    printf("[CEAnalysis::Init] Set the streamer object for this analysis... \n");
    return;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysis::SetGeomVersion(string val) {
  fGeomVer = val;
  fNPIXEL = fGeomNPix[val];
  fDETRAD = fGeomDetRad[val];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Float_t CEAnalysis::GetIntBkgRate(bool bothLyr) {
  Float_t rate;
  if (bothLyr)
    rate = fNPIXEL * 40. * (fGeomCrystMassTop[fGeomVer] + fGeomCrystMassBot[fGeomVer]); // activity rate 40. bq/g
  else
    rate = fNPIXEL * 40. * fGeomCrystMassBot[fGeomVer];

  return rate;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Float_t CEAnalysis::GetSrcRadius(char flag) {
  Float_t rad;
  if (!strcmp(fGeomVer.c_str(), "V1R0") && (flag == 'e'))
    rad = 16.0;
  else if (!strcmp(fGeomVer.c_str(), "V1R0") && (flag == 's'))
    rad = 40.0;
  else
    rad = fGeomSrcRad[fGeomVer];

  return rad;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Float_t CEAnalysis::GetSrcMaxTheta(char flag) {
  Float_t ang;
  if (!strcmp(fGeomVer.c_str(), "V1R0") && (flag == 's'))
    ang = 68.0;
  else
    ang = 90.0;

  return ang;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TVector3* CEAnalysis::UpCrystalPos() {
  TString filename = fDataPath + "upCrystalPos-" + fGeomVer + ".txt";
  fstream vpfile(filename.Data());
  if (!vpfile.is_open()) {
    cout << "[CEAnalysis::UpCrystalPos] Input file for the pixel positions could not be opened!\n";
    exit(1);
//    return nullptr;
  }

  Int_t pid;
  Float_t xP, yP, zP;
  fUpCrystPos.resize(fNPIXEL);

  while (1) {
    vpfile >> pid >> xP >> yP >> zP;
    if (vpfile.eof())
      break;

    fUpCrystPos.at(pid) = TVector3(xP, yP, zP);
  }

  return &fUpCrystPos[0];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TVector3* CEAnalysis::BotCrystalPos() {
  TString filename = fDataPath + "botCrystalPos-" + fGeomVer + ".txt";
//  TString filename = fDataPath + "botCrystalPos.txt";
  fstream vpfile(filename.Data());
  if (!vpfile.is_open()) {
    cout << "[CEAnalysis::BotCrystalPos] Input file for the pixel positions could not be opened!\n";
    exit(1);
//    return nullptr;
  }

  Int_t pid;
  Float_t xP, yP, zP;
  fBotCrystPos.resize(fNPIXEL);

  while (1) {
    vpfile >> pid >> xP >> yP >> zP;
    if (vpfile.eof())
      break;

    fBotCrystPos.at(pid) = TVector3(xP, yP, zP);
  }

  return &fBotCrystPos[0];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TVector3* CEAnalysis::UpAcdPos() {
  TString filename = fDataPath + "upAcdPos-" + fGeomVer + ".txt";
//  TString filename = fDataPath + "upAcdPos.txt";
  fstream vpfile(filename.Data());
  if (!vpfile.is_open()) {
    cout << "[CEAnalysis::UpAcdPos] Input file for the pixel positions could not be opened!\n";
    exit(1);
//    return nullptr;
  }

  Int_t pid;
  Float_t xP, yP, zP;
  fUpAcdPos.resize(fNPIXEL);

  while (1) {
    vpfile >> pid >> xP >> yP >> zP;
    if (vpfile.eof())
      break;

    fUpAcdPos.at(pid) = TVector3(xP, yP, zP);
  }

  return &fUpAcdPos[0];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TVector3 CEAnalysis::BotAcdPos() {
  return fBotAcdPos = TVector3(0., 0., -5.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Int_t* CEAnalysis::UpToBotCrystalMap(bool botPix) {
  TString filename = fDataPath + "upToBotCrystalMap-" + fGeomVer + ".txt";
//  TString filename = fDataPath + "upToBotCrystalMap.txt";
  fstream mapfile(filename.Data());
  if (!mapfile.is_open()) {
    cout << "[CEAnalysis::UpToBotCrystalMap] Input file for the pixel id mapping " << filename
        << " could not be opened!\n";
    exit(1);
//    return nullptr;
  }

  Int_t pid;

  vector<pair<Int_t, Int_t>> map;
  fUpToBotCrystMap.resize(fNPIXEL);

  while (1) {
    mapfile >> pid >> fUpToBotCrystMap[pid];
    if (mapfile.eof())
      break;
    map.push_back(make_pair(fUpToBotCrystMap[pid], pid));
  }

  if (botPix) {
    sort(map.begin(), map.end());
    for (int i = 0; i < fNPIXEL; i++)
      fUpToBotCrystMap[i] = map[i].second;
  }

  return &fUpToBotCrystMap[0];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Int_t* CEAnalysis::UpCrystalToAcdMap(bool botPix) {
  TString filename = fDataPath + "upCrystalToAcdMap-" + fGeomVer + ".txt";
//  TString filename = fDataPath + "upToBotCrystalMap.txt";
  fstream mapfile(filename.Data());
  if (!mapfile.is_open()) {
    cout << "[CEAnalysis::UpCrystalToAcdMap] Input file for the pixel id mapping " << filename
        << " could not be opened!\n";
    exit(1);
  }

  Int_t pid;

  vector<pair<Int_t, Int_t>> map;
  fUpCrystToAcdMap.resize(fNPIXEL);

  while (1) {
    mapfile >> pid >> fUpCrystToAcdMap[pid];
    if (mapfile.eof())
      break;
    map.push_back(make_pair(fUpCrystToAcdMap[pid], pid));
  }

  if (botPix) {
    sort(map.begin(), map.end());
    for (int i = 0; i < fNPIXEL; i++)
      fUpCrystToAcdMap[i] = map[i].second;
  }

  return &fUpCrystToAcdMap[0];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<Int_t> CEAnalysis::NearbyCrystals(Int_t pid) {
  vector<Int_t> nears;
  bool botPix(0);

  TString filename;
  if (pid > fNPIXEL) {
//    filename = fDataPath + "botCrystalNears.txt";
    filename = fDataPath + "botCrystalNears-" + fGeomVer + ".txt";
    pid -= fNPIXEL;
    botPix = 1;
  } else
//    filename = fDataPath + "upCrystalNears.txt";
    filename = fDataPath + "upCrystalNears-" + fGeomVer + ".txt";

  fstream mapfile(filename.Data());
  if (!mapfile.is_open()) {
    cout << "[CEAnalysis::NearbyCrystals] Input file for the neighboring crystals could not be opened!\n";
    exit(1);
  }

  // Get the nearby up crystals from the file
  string line;
  Int_t id;
  while (getline(mapfile, line)) {
    stringstream str_strm;
    str_strm << line; //convert the string into stringstream
    string temp_str;
    int temp_int;
    int ind(0);
    nears.clear();

    while (!str_strm.eof()) {
      str_strm >> temp_str; //take words into temp_str one by one

      if (stringstream(temp_str) >> temp_int) { // convert string to int
        if (!ind) {
          id = temp_int;
          if (id != pid)
            break;
        } else {
          nears.push_back((botPix) ? temp_int + fNPIXEL : temp_int);
        }
      }

      ind++;
      temp_str = ""; //clear temp string
    }

    if (id == pid)
      break;
  }

  // Get the nearby down crystals from the map
  auto nearcopy = nears;
  auto map = UpToBotCrystalMap(botPix);
  nears.push_back((botPix) ? map[pid] : map[pid] + fNPIXEL);
  for (auto n : nearcopy)
    nears.push_back((botPix) ? map[n - fNPIXEL] : map[n] + fNPIXEL);

  return nears;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<Float_t> CEAnalysis::LogEnergyBin(Int_t nBins, Float_t minE, Float_t maxE) {
  vector<Float_t> engB;

  Float_t minEng = log10(minE); // minimum energy keV
  Float_t maxEng = log10(maxE); // maximum energy keV
  Float_t bw = (maxEng - minEng) / nBins;
  for (int ii = 0; ii <= nBins; ii++)
    engB.push_back(pow(10., minEng + ii * bw));

  return engB;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysis::SetEngResFunction(Float_t minE, Float_t maxE, Float_t *par, Int_t flag) {
  fResFunc = new TF1("fResFunc", "sqrt([0]*[0]/sqrt(x/1E3) + [1]*[1])", minE, maxE);
  fResFunc->SetParameter(0, par[0]);
  fResFunc->SetParameter(1, par[1]);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//vector<Float_t> CEAnalysis::ResoEnergyBin(Float_t minE, Float_t maxE, Int_t nBins, Float_t par0, Float_t par1) {
vector<Float_t> CEAnalysis::ResoEnergyBin(Float_t minE, Float_t maxE, Int_t nBins) {
  vector<Float_t> engB;
//  auto fRes = new TF1("fRes", "sqrt([0]*[0]/sqrt(x/1E3) + [1]*[1])", minE, maxE);
//  fRes->SetParameter(0, par0);
//  fRes->SetParameter(1, par1);

  engB.push_back(minE);
  do {
    auto eng = engB.back();
    auto dE = fResFunc->Eval(eng) * eng / 100.;
    engB.push_back(eng + dE / nBins);
  } while (engB.back() < maxE);

  engB.back() = maxE;

  return engB;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysis::FGetEntry(Int_t ent) {
  if (!fTree)
    Init();

  if (ent != fEntId) {
    fTree->GetEntry(ent);
    fEntId = ent;
  }

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
Int_t CEAnalysis::GetEvtId(Int_t ent) {
  if (ent != fEntId)
    FGetEntry(ent);

  return fEvtId;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysis::PrintProgress(Float_t percentage) {
  // Source - https://stackoverflow.com/a/36315819
  // Posted by razz, modified by community. See post 'Timeline' for change history
  // Retrieved 2026-08-13, License - CC BY-SA 4.0

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

  int val = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
