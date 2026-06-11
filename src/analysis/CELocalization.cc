/********************************************************************************
 *
 * CELocalization.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 13 Dec, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CELocalization.hh"

#include "TSystem.h"
#include "TROOT.h"
#include "TF1.h"
#include "TRandom3.h"

//#include<sstream>
//#include <bits/stdc++.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CELocalization::CELocalization() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CELocalization::~CELocalization() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CELocalization::SetPixPosFile(TString fName) {

  // Get positions of the pixels
  auto pixPos = new TTree("pixPos", "pixel positions from ascii file");
  fNPIX = pixPos->ReadFile(fName, "n:x:y:z");
//  if (fNPIX != NPIXEL) {
//    cout << "[CELocalization::SetPixPosFile] Please check pixel position file!!\n";
//    exit(1);
//  }

  Float_t iPix, xx, yy, zz;
  pixPos->SetBranchAddress("n", &iPix);
  pixPos->SetBranchAddress("x", &xx);
  pixPos->SetBranchAddress("y", &yy);
  pixPos->SetBranchAddress("z", &zz);

  for (int i = 0; i < fNPIX; i++) {
    pixPos->GetEntry(i);
    fVPixPos.push_back(TVector3(xx, yy, zz));
  }

  cout << "[CELocalization::SetPixPosFile] Number of pixels = " << fNPIX << endl;

  delete pixPos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CELocalization::SetEffAreaFile(TString fName) {
  // Effective area file
  fEffAreaFile = new TFile(fName);
  if (!fEffAreaFile || fEffAreaFile->IsZombie()) {
    printf("[CELocalization::SetEffAreaFile] Could not open the effective area file %s ..... \n\n",
        fEffAreaFile->GetName());
    exit(1);
  }

  printf("[CELocalization::SetEffAreaFile] Effective area file -> %s \n\n", fEffAreaFile->GetName());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CELocalization::SetTemPosInfoFile(TString fName) {
  // Read theta phi positions of the template data
  auto temPosThPh = new TTree("tThPh", "read theta phi");
  auto nTemPos = temPosThPh->ReadFile(fName, "t:p");

  if ((nTemPos < 1) || !temPosThPh) {
    printf("[CELocalization::SetTemPosInfoFile] Could not read the template position information from %s ..... \n\n",
        fName.Data());
    exit(1);
  }

  Float_t theta, phi;
  temPosThPh->SetBranchAddress("t", &theta);
  temPosThPh->SetBranchAddress("p", &phi);

  for (int i = 0; i < nTemPos; i++) {
    temPosThPh->GetEntry(i);

    // Template position
    fVTemPos.push_back(make_pair(theta, phi));
  }

  printf("[CELocalization::SetTemPosInfoFile] Number of templates = %d. \n\n", (Int_t) nTemPos);

  delete temPosThPh;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CELocalization::SetTemDataFile(TString fName) {
  // Template data file
  auto temDataFile = new TFile(fName);
  if (!temDataFile || temDataFile->IsZombie()) {
    printf("[CELocalization::SetTemDataFile] Could not open the template data file %s ..... \n\n",
        temDataFile->GetName());
    exit(1);
  }

  if (fVTemPos.size() < 1) {
    printf("[CELocalization::SetTemDataFile] template position information not found ..... \n\n");
    exit(1);
  }

  // Read histograms from the template data file
  for (auto v : fVTemPos) {
    // Get pixel edep data
    TString hName;
    hName.Form("hEdepPix_%d_%d", (Int_t) v.first, (Int_t) v.second);
    auto hTemHist = (TH2F*) (temDataFile->Get(hName));
    hTemHist->SetDirectory(0);

    TVector3 vTP;
    vTP.SetMagThetaPhi(1, v.first * DegToRad(), v.second * DegToRad());
    fVTemPosHist.push_back(make_pair(vTP, hTemHist));
  }

  printf("[CELocalization::SetTemDataFile] template histograms set ..... \n\n");

  delete temDataFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CELocalization::SetSamPosInfoFile(TString fName) {
  // Read theta phi positions of the sample data
  auto samPosThPh = new TTree("sPos", "sample positions from ascii file");
  auto nSamPos = samPosThPh->ReadFile(fName, "s/C:x/F:y/F");

  if ((nSamPos < 1) || !samPosThPh) {
    printf("[CELocalization::SetSamPosInfoFile] Could not read the sample position information from %s ..... \n\n",
        fName.Data());
    exit(1);
  }

  char *str = new char[256];
  Float_t th, ph;
  samPosThPh->SetBranchAddress("s", str);
  samPosThPh->SetBranchAddress("x", &th);
  samPosThPh->SetBranchAddress("y", &ph);

  for (int i = 0; i < nSamPos; i++) {
    samPosThPh->GetEntry(i);

    // Sample position vector
    TVector3 vSP;
    vSP.SetMagThetaPhi(1, th * DegToRad(), ph * DegToRad());
    fVSamPos.push_back(make_pair(str, vSP));
  }

  delete samPosThPh;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CELocalization::SetBkgFile(TString fName) {
  auto bkgFile = new TFile(fName);
  if (!bkgFile || bkgFile->IsZombie()) {
    printf("[CELocalization::SetBkgFile] Could not open the background data file %s ..... \n\n", bkgFile->GetName());
    exit(1);
  }

  // Read histograms from the background data file
  fHBkgTot = (TH1F*) bkgFile->Get("hNormEdepTotCal");
  fHBkgTot->SetDirectory(0);
  fHBkgPix = (TH1F*) bkgFile->Get("hNormEdepPix");
  fHBkgPix->SetDirectory(0);

  printf("[CELocalization::SetBkgFile] background histograms retrieved ..... \n\n");

  delete bkgFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
TVector3 CELocalization::CalcRecDirection(TH1F *hSamPix, TH1F *hSamCal) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Subtract (if there is) background

  if (fHBkgTot)
    hSamCal->Add(fHBkgTot, -1);

  gROOT->ProcessLine("gErrorIgnoreLevel = 2001;"); // to suppress warning message from Add
  if (fHBkgPix)
    hSamPix->Add(fHBkgPix, -1);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Calculate provisional direction

  Float_t sC = hSamPix->Integral();

  TVector3 vAvg = TVector3(0, 0, 0);

  Int_t ind = 1;
  for (auto v : fVPixPos) {
    vAvg += ((hSamPix->GetBinContent(ind) / sC) * v);
    ind++;
  }

  auto cenTh = vAvg.Theta() * RadToDeg();
  if (cenTh > 90.0)
    cenTh = 90.0;
//  cout << "Calculated centroid theta = " << cenTh << endl;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Get effective area
  TString name;
  name.Form("hEffArea%d", (Int_t) cenTh);
  auto hEff = (TH1F*) fEffAreaFile->Get(name);

  // Get back the incident flux spectrum from the deposited
  // TODO: apply actual unfolding method
  hSamCal->Divide(hEff);

  // Convert to per unit energy interval
  for (int i = 1; i <= hSamCal->GetNbinsX(); i++) {
    hSamCal->SetBinContent(i, hSamCal->GetBinContent(i) / hSamCal->GetBinWidth(i));
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Template data files

  fVProb.clear();
  Float_t sProb(0);

  // Loop over templates
  for (auto vtem : fVTemPosHist) {
    // Get pixel edep data
    auto hTemPix = (TH2F*) vtem.second->Clone("hTemPix");

    // Normalize pixel edep data with the calculated sample incident flux
    for (int i = 1; i <= hTemPix->GetNbinsX(); i++)
      for (int j = 1; j <= hTemPix->GetNbinsY(); j++) {
        hTemPix->SetBinContent(i, j, hTemPix->GetBinContent(i, j) * hSamCal->GetBinContent(j));
      }

    // Get total normalized counts in each pixel
    auto hTem = (TH1F*) hTemPix->ProjectionX("hTem");

    // Reset similar error as the sample (for better KS comparison)
    for (int i = 1; i <= hSamPix->GetNbinsX(); i++)
      hTem->SetBinError(i, hSamPix->GetBinError(i));

    // Calculate the probability
    auto prob = hSamPix->KolmogorovTest(hTem);

//    if (prob > 0.001) { // nominal cut over prob value
    fVProb.push_back(make_pair(prob, vtem.first));
//      sProb += prob;
//    }

    delete hTem;
    delete hTemPix;
  } // end loop templates

  // Get maximum probability
  sort(fVProb.begin(), fVProb.end(), [](const pair<Float_t, TVector3> &a, const pair<Float_t, TVector3> &b) {
    return a.first > b.first;
  });
  auto phMax = (fVProb.begin())->first; // max prob

  // Nominal cut (1% of the max prob) on the probability values
  for (auto v : fVProb) {
    if (v.first < (phMax * 1E-2))
      fVProb.erase(remove(fVProb.begin(), fVProb.end(), v), fVProb.end());
    else
      sProb += v.first;
  }

  // Weighted average of the reconstructed directions
  TVector3 recVec = TVector3(0, 0, 0);
  for (auto v : fVProb)
    recVec += ((v.first / sProb) * v.second);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  return recVec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<pair<TVector3, TVector3>> CELocalization::MultiRecDirection(TString dPath) {
  vector<pair<TVector3, TVector3>> vDirs;
  Int_t ctr(0);

  for (auto v : fVSamPos) {
    TString fName;
    fName.Form("%s.root", v.first.Data());
    if (!gSystem->FindFile(dPath, fName)) {
      printf("[CELocalization::MultiRecDirection] Could not found sample file %s ..... \n\n", fName.Data());
      continue;
    }

    TVector3 vOrg = TVector3(0, 0, 1);
    vOrg.SetMagThetaPhi(1, v.second.Theta(), v.second.Phi());

    auto sFile = new TFile(fName);
    auto hPix = (TH1F*) sFile->Get("hNormEdepPix");
    auto hCal = (TH1F*) sFile->Get("hNormEdepTotCal");

    TVector3 vCal = CalcRecDirection(hPix, hCal);
    delete sFile;

    vDirs.push_back(make_pair(vOrg, vCal));

    ctr++;
    if (!(ctr % 10))
      cout << "Finished analyzing " << ctr << " files!" << endl;
  }

  return vDirs;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
vector<pair<TVector3, TVector3>> CELocalization::MultiSpecRecDirection(TString dPath) {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Get different spectra
  TString dataPath = std::getenv("CRYSTALEYE_DATA");
  dataPath += "Spec/";
  TString dataFile = dataPath + "GRBCompParam.txt";

  TTree *tGRB = new TTree("tGRB", "GRB param data from ascii file");
  const Int_t nGRB = tGRB->ReadFile(dataFile, "x:y:z:a");

  Float_t ampl, epeak, index;
  tGRB->SetBranchAddress("x", &ampl);
  tGRB->SetBranchAddress("y", &epeak);
  tGRB->SetBranchAddress("z", &index);

  TF1 *f1[nGRB];
//  auto hFidu = new TProfile("hFidu", "Profile", hPeng->GetNbinsX(), hPeng->GetXaxis()->GetXbins()->GetArray(), "");

  char name[20];
  for (int i = 0; i < nGRB; i++) {
    tGRB->GetEntry(i);

    sprintf(name, "f1%d", i);
    f1[i] = new TF1(name, "[0]*((x/100)**[1])*exp(-([1]+2)*x/[2])", 30.0, 1.0e5);
    f1[i]->SetParameter(0, ampl);
    f1[i]->SetParameter(1, index);
    f1[i]->SetParameter(2, epeak);

//    for (int j = 1; j <= hPeng->GetNbinsX(); j++)
//      hFidu->Fill(hPeng->GetBinCenter(j), f1[i]->Eval(hPeng->GetBinCenter(j)));
  }

  vector<pair<TVector3, TVector3>> vDirs;
  Int_t ctr(0);
  Float_t peng, totEdep;
  vector<Int_t> *pixId = 0;
  vector<Float_t> *pixEdep = 0;

  for (auto v : fVSamPos) {
    TString fName;
    fName.Form("%s.root", v.first.Data());
    if (!gSystem->FindFile(dPath, fName)) {
      printf("[CELocalization::MultiRecDirection] Could not found sample file %s ..... \n\n", fName.Data());
      continue;
    }

    TVector3 vOrg = TVector3(0, 0, 1);
    vOrg.SetMagThetaPhi(1, v.second.Theta(), v.second.Phi());

    // Choose a random GRB function
    Int_t iGRB = gRandom->Uniform(nGRB - 1);
//    cout << fName << "\tGRB no. " << iGRB << endl;

    // Read source edep data
    auto sFile = new TFile(fName);

    auto hPix = (TH1F*) sFile->Get("hNormEdepPix");
    auto hCal = (TH1F*) sFile->Get("hNormEdepTotCal");
    auto hPeng = (TH1F*) sFile->Get("hPeng");
    auto hNorm = (TH1F*) sFile->Get("hNorm");
    auto tPix = (TTree*) sFile->Get("tEdepPix");

    // Normalization of primary spectrum
    auto hNormNew = (TH1F*) hNorm->Clone("hNormNew");
    hNormNew->Reset();

    Float_t srcRad = 18.0;
    Float_t srcArea = 4. * srcRad * srcRad;  // cm2

    for (Int_t i = 1; i <= hPeng->GetNbinsX(); i++) {
      Float_t lEdge = hPeng->GetBinLowEdge(i);
      Float_t uEdge = lEdge + hPeng->GetBinWidth(i);
      Float_t norm = f1[iGRB]->Integral(lEdge, uEdge) * srcArea / hPeng->GetBinContent(i); // # s^-1
      //    Float_t norm = hFidu->GetBinContent(i) * hPeng->GetBinWidth(i) * srcArea / hPeng->GetBinContent(i); // # s^-1
      hNormNew->SetBinContent(i, norm);
    }

    // New normalized histograms
    auto hPixNew = (TH1F*) hPix->Clone("hPixNew");
    hPixNew->Reset();
    auto hCalNew = (TH1F*) hCal->Clone("hCalNew");
    hCalNew->Reset();

    tPix->SetBranchAddress("peng", &peng);
    tPix->SetBranchAddress("totEdep", &totEdep);
    tPix->SetBranchAddress("pixID[px]", &pixId);
    tPix->SetBranchAddress("pixEdep[px]", &pixEdep);

    auto nEnt = tPix->GetEntriesFast();
    for (auto i = 0; i < nEnt; i++) {
      tPix->GetEntry(i);
      hCalNew->Fill(totEdep, hNormNew->GetBinContent(hNormNew->FindBin(peng)));

      for (int j = 0; j < pixId->size(); j++) {
        hPixNew->Fill(pixId->at(j), (pixEdep->at(j) / totEdep) * hNormNew->GetBinContent(hNormNew->FindBin(peng)));
      }
      pixId->clear();
      pixEdep->clear();
    }

    TVector3 vCal = CalcRecDirection(hPixNew, hCalNew);
    delete sFile;

    vDirs.push_back(make_pair(vOrg, vCal));

    ctr++;
    if (!(ctr % 10))
      cout << "Finished analyzing " << ctr << " files!" << endl;
  }

  return vDirs;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
