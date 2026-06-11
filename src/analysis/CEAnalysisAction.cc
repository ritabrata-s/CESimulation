/********************************************************************************
 *
 * CEAnalysisAction.cc
 *
 * Author: Ritabrata Sarkar
 * Created on: 17 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CEAnalysisAction.hh"
#include "TRandom.h"
#include "TProfile.h"
#include "TTree.h"

#include<fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEAnalysisAction::CEAnalysisAction() :
    fNUpgoing(0), fNDngoing(0), fNTrigCut1(0), fNTrigCut2(0), fNTrigCut3(0), fNTrigCut4(0), fNTrigCut5(0), fNTrigCut6(
        0), fNTrigCut7(0) {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CEAnalysisAction::~CEAnalysisAction() {
  delete fPrimAna;
  delete fCalAna;
  delete fAcdAna;
  delete fStreamer;
  if (fPrimFunc)
    delete fPrimFunc;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::BeginOfAnalysis() {
  // Set the data streamer
  fStreamer = new CEIOStreamer(fInFileName);

  // Prim data analysis
  fPrimAna = new CEPrimAnalysis();
  fPrimAna->SetStreamer(fStreamer);
  fPrimAna->Init();

  fNEntPrim = fPrimAna->GetNEntries();
  cout << "[CEAnalysisAction::BeginOfAnalysis] No. of primary entries = " << fNEntPrim << endl;

  // Cal data analysis
  fCalAna = new CECalAnalysis();
  fCalAna->SetStreamer(fStreamer);
  fCalAna->SetGeomVersion(fGeomVer);
  fCalAna->Init();

  fNEntCal = fCalAna->GetNEntries();
  cout << "[CEAnalysisAction::BeginOfAnalysis] No. of cal entries = " << fNEntCal << endl;

  // Acd data analysis
  fAcdAna = new CEAcdAnalysis();
  fAcdAna->SetStreamer(fStreamer);
  fAcdAna->SetGeomVersion(fGeomVer);
  fAcdAna->Init();

  fNEntAcd = fAcdAna->GetNEntries();
  cout << "[CEAnalysisAction::BeginOfAnalysis] No. of cal entries = " << fNEntAcd << endl;

  if ((fNEntPrim != fNEntCal) || (fNEntPrim != fNEntAcd)) {
    printf(
        "[CEAnalysisAction::BeginOfAnalysis] No. of entries in prim, cal and acd tree are not same, please check the data... \n");
    return;
  }

  if (fOutFileName)
    fStreamer->SetOutputFileName(fOutFileName);
  fOutfile = fStreamer->GetOutputFile();

  fNPIXEL = fCalAna->GetNPixels();
  if (fNPIXEL < 1) {
    cout << "[CEAnalysisAction::BeginOfAnalysis] No. of pixels (" << fNPIXEL << ") is wrong..." << endl;
    exit(1);
  }

  fDETRADIUS = fCalAna->GetDetRadius();
  if (fDETRADIUS < 1.0) {
    cout << "[CEAnalysisAction::BeginOfAnalysis] Detector radius value (" << fDETRADIUS << ") is wrong..." << endl;
    exit(1);
  }

  fSigTW = fCalAna->GetSigTimeWidth();
  if (fSigTW < 1.0) {
    cout << "[CEAnalysisAction::BeginOfAnalysis] Signal time width (" << fSigTW << ") is wrong..." << endl;
    exit(1);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::EndOfAnalysis() {
  fOutfile->Close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::SetPrimEngBin(Int_t nB, Float_t minE, Float_t maxE) {
  fPNBin = nB;
  fPMinE = minE;
  fPMaxE = maxE;
  fPEngB = fPrimAna->LogEnergyBin(fPNBin, fPMinE, fPMaxE);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::SetDepEngBin(Int_t nB, Float_t minE, Float_t maxE) {
  fDMinE = minE;
  fDMaxE = maxE;
  if (nB > 0) {
    fDNBin = nB;
    fDEngB = fPrimAna->LogEnergyBin(fDNBin, fDMinE, fDMaxE);
  } else {
    fDEngB = fPrimAna->ResoEnergyBin(fDMinE, fDMaxE);
    fDNBin = fDEngB.size() - 1;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::AnaGamma(bool dnward) {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Histograms
  TH1F *hThAll = new TH1F("hThAll", "; Cos(#theta); Entires", 180, -1, 1);
  TH1F *hThDn = new TH1F("hThDn", "(downward only); Cos(#theta); Entires", 180, -1, 1);
  TH1F *hPeng = new TH1F("hPeng", "Primary energy; Energy (keV); Entires", fPNBin, &fPEngB[0]);

  TH2F *h2PDengCal = new TH2F("hPDengCal", "Cal; Incident energy (keV); Depositied energy (keV); Counts", fPNBin,
      &fPEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2PDengAcdUp = new TH2F("hPDengAcdUp", "Acd up; Incident energy (keV); Depositied energy (keV); Counts", fPNBin,
      &fPEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2PDengAcdBot = new TH2F("hPDengAcdBot", "Acd dn; Incident energy (keV); Depositied energy (keV); Counts",
      fPNBin, &fPEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2PDengCalTrig = new TH2F("hPDengCalTrig", "Cal triger; Incident energy (keV); Depositied energy (keV); Counts",
      fPNBin, &fPEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2PDengTot = new TH2F("hPDengTot", "Total edep; Incident energy (keV); Depositied energy (keV); Counts", fPNBin,
      &fPEngB[0], fDNBin, &fDEngB[0]);
  Float_t frVal[12];
  for (Int_t i = 0; i <= 11; i++)
    frVal[i] = i * 0.1;
  TH2F *h2PengFrac = new TH2F("hPengFrac", "Frac. edep; Incident energy (keV); Frac. Edep; Counts", fPNBin, &fPEngB[0],
      11, &frVal[0]);
  TH2F *h2EdepFracAU = new TH2F("hEdepFracAU", "Frac. edep ACDU; Deposited energy (keV); Frac. Edep; Counts", fDNBin,
      &fDEngB[0], 11, &frVal[0]);
  TH2F *h2EdepFracAB = new TH2F("hEdepFracAB", "Frac. edep ACDB; Deposited energy (keV); Frac. Edep; Counts", fDNBin,
      &fDEngB[0], 11, &frVal[0]);
  TH2F *h2EdepFracCal = new TH2F("hEdepFracCal", "Frac. edep Cal; Deposited energy (keV); Frac. Edep; Counts", fDNBin,
      &fDEngB[0], 11, &frVal[0]);
  TH2F *h2EdepFracTot = new TH2F("hEdepFracTot", "Frac. edep Tot; Deposited energy (keV); Frac. Edep; Counts", fDNBin,
      &fDEngB[0], 11, &frVal[0]);
  TH2F *h2CalAcdAll = new TH2F("hCalAcdAll", "Cal vs. Acd All frac edep; Cal energy (keV); Acd energy (keV); Counts",
      fDNBin, &fDEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2CalAcd09 = new TH2F("hCalAcd09", "Cal vs. Acd > 99% frac edep; Cal energy (keV); Acd energy (keV); Counts",
      fDNBin, &fDEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2CalAcd09L = new TH2F("hCalAcd09L", "Cal vs. Acd < 99% frac edep; Cal energy (keV); Acd energy (keV); Counts",
      fDNBin, &fDEngB[0], fDNBin, &fDEngB[0]);
  TH1F *hFracACD = new TH1F("hFracACD", "Edep only in ACD; Frac. Edep; Counts", 11, &frVal[0]);
  TH1F *hFracCal = new TH1F("hFracCal", "Edep only in Cal; Frac. Edep; Counts", 11, &frVal[0]);
  TH1F *hFracACDCal = new TH1F("hFracACDCal", "Edep both in ACD and Cal; Frac. Edep; Counts", 11, &frVal[0]);
  TH1F *hFracCalU = new TH1F("hFracCalU", "Edep CalU > CalB; Frac. Edep; Counts", 11, &frVal[0]);
  TH1F *hFracCalB = new TH1F("hFracCalB", "Edep CalU < CalB; Frac. Edep; Counts", 11, &frVal[0]);

  TH1F *hMPix = new TH1F("hMPix", "; No. of pixels; Entires", fNPIXEL, 0, fNPIXEL);
  TH1F *hEdepPix = new TH1F("hEdepPix", "; Pixel Id.; Entires", fNPIXEL, 0, fNPIXEL);
  TH1F *hMaxEdepPix = new TH1F("hMaxEdepPix", "; Pixel Id.; Entires", fNPIXEL, 0, fNPIXEL);
  TH1F *hEdep1Pix = new TH1F("hEdep1Pix", "; Pixel Id.; Entires", fNPIXEL, 0, fNPIXEL);

  TH1F *hEdepCal = new TH1F("hEdepCal", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);

  TH3F *h3PosCal = new TH3F("hPosCal", "Cal edep pos dist.; X (mm); Y (mm); Counts", 29, -145, 145, 29, -145, 145, 15,
      -5, 145);

  Float_t pixVal[fNPIXEL + 1];
  for (Int_t i = 0; i <= fNPIXEL; i++)
    pixVal[i] = i;

  TH2F *h2NpixFrac = new TH2F("hNpixFrac", "No. pixel vs. Frac. edep; No. of Pixels; Frac. Edep; Counts", fNPIXEL,
      &pixVal[0], 11, &frVal[0]);

  TH2F *h2PengNpCal = new TH2F("hPengNpCal", "Cal peng npix; Incident energy (keV); Number of Pixel; Counts", fPNBin,
      &fPEngB[0], fNPIXEL, &pixVal[0]);
  TH2F *h2NpFedepCal = new TH2F("hNpFedepCal", "Cal npix frac edep; Number of Pixel; Frac. energy dep.; Counts",
      fNPIXEL, 0., fNPIXEL, 100, 0., 1.);
  TH1F *hFedep1pCal = new TH1F("hFedep1pCal", "Cal frac edep in 1st max; Frac. energy dep.; Counts", 100, 0., 1.);
  TH1F *hFedep2pCal = new TH1F("hFedep2pCal", "Cal frac edep in 2nd max; Frac. energy dep.; Counts", 100, 0., 1.);
  TH1F *hFedep3pCal = new TH1F("hFedep3pCal", "Cal frac edep in 3rd max; Frac. energy dep.; Counts", 100, 0., 1.);
  TH1F *hFedepRpCal = new TH1F("hFedepRpCal", "Cal frac edep in rest pixels; Frac. energy dep.; Counts", 100, 0., 1.);
  TH1F *hMedepViewCal = new TH1F("hMedepViewCal", "Cal max edep in up/down crystal; Up/down crystal; Counts", 2, 0.,
      2.);
  TH1F *hMedepSameCal = new TH1F("hMedepSameCal", "Cal max edep in same pixel; Same pixel or no; Counts", 2, 0., 2.);
  TH2F *h2maxEdepPosXY = new TH2F("hmaxEdepPosXY", "Max edep pos; X (mm); Y (mm); Counts", 60, -150., 150., 60, -150.,
      150.);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Get the position of up crystals
  TVector3 *upCrPos = fCalAna->UpCrystalPos();

  // Get the up to bottom crystal mapping
  Int_t *upBotMap = fCalAna->UpToBotCrystalMap();

  fSrcRad = fCalAna->GetSrcRadius();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events for normalization
  for (Int_t i = 0; i < fNEntCal; i++) {
    TVector3 dir = fPrimAna->GetDirection(i);
    // Consider up/down ward particles
    if (dnward) {
      if (dir.Z() >= 0) {
        fNUpgoing++;
        continue;
      }
    } else {
      if (dir.Z() < 0) {
        fNDngoing++;
        continue;
      }
    }

    Float_t peng = fPrimAna->GetEnergy(i);
    hPeng->Fill(peng);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Normalization of primary spectrum
  if (dnward)
    fPrimFunc = new TF1("fCosmicPhoton", fPrimAna, &CEPrimAnalysis::FCosmicPhoton, fPMinE, fPMaxE, 0);
  else
    fPrimFunc = new TF1("fAlbedoPhoton", fPrimAna, &CEPrimAnalysis::FAlbedoPhoton, fPMinE, fPMaxE, 0);

  fPrimFunc->SetTitle("Primary Gamma;Energy (keV); Photons cm^{-2} sr^{-2} kev^{-1} s^{-1}");
  TH1F *hNorm = (TH1F*) hPeng->Clone("hNorm");
  TH1F *hNormPeng = (TH1F*) hPeng->Clone("hNormPeng");

  for (Int_t i = 1; i <= hPeng->GetNbinsX(); i++) {
    Float_t lEdge = hPeng->GetBinLowEdge(i);
    Float_t uEdge = lEdge + hPeng->GetBinWidth(i);
//    Float_t norm = fPrimFunc->Integral(lEdge, uEdge) * 2 * TMath::Pi() * SRCRADIUS * SRCRADIUS * TMath::Pi()
    Float_t norm = fPrimFunc->Integral(lEdge, uEdge) * 2 * TMath::Pi() * fSrcRad * fSrcRad * TMath::Pi()
        / hPeng->GetBinContent(i); // # s^-1
    hNorm->SetBinContent(i, norm);
    Float_t cnt = hPeng->GetBinContent(i) * norm
        / (2 * TMath::Pi() * fSrcRad * fSrcRad * TMath::Pi() * hPeng->GetBinWidth(i)); // # cm^-2 sr^-2 kev^-1 s^-1
//        / (2 * TMath::Pi() * SRCRADIUS * SRCRADIUS * TMath::Pi() * hPeng->GetBinWidth(i)); // # cm^-2 sr^-2 kev^-1 s^-1
    hNormPeng->SetBinContent(i, cnt);
  }

  hNormPeng->SetTitle("Primary Gamma;Energy (keV); Photons cm^{-2} sr^{-2} kev^{-1} s^{-1}");
  hNormPeng->Sumw2(kFALSE);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events
  for (Int_t i = 0; i < fNEntCal; i++) {
    TVector3 dir = fPrimAna->GetDirection(i);
    hThAll->Fill(dir.CosTheta());

    // Consider up/down ward particles
    if (dnward) {
      if (dir.Z() >= 0) {
        fNUpgoing++;
        continue;
      }
    } else {
      if (dir.Z() < 0) {
        fNDngoing++;
        continue;
      }
    }

    hThDn->Fill(dir.CosTheta());
    Float_t peng = fPrimAna->GetEnergy(i);
//    hPeng->Fill(peng);

    Float_t eCalUp(0.), eCalBot(0.);
    Float_t totEdepCal = fCalAna->GetTotalEdep(i, eCalUp, eCalBot);

    Float_t eAcdUp(0.), eAcdBot(0.);
    Float_t totEdepAcd = fAcdAna->GetTotalEdep(i, eAcdUp, eAcdBot);
    Float_t totEdep = totEdepAcd + totEdepCal;
    Int_t nHits = fCalAna->GetNHits(i);

//    if (!totEdep) {
//      continue;
//    }

    h2PDengCal->Fill(peng, totEdepCal);
    h2PDengAcdUp->Fill(peng, eAcdUp);
    h2PDengAcdBot->Fill(peng, eAcdBot);

    h2PDengTot->Fill(peng, totEdep);
    h2PengFrac->Fill(peng, totEdep / peng);

//    h2EdepFracAU->Fill(eAcdUp, totEdep / peng);
//    h2EdepFracAB->Fill(eAcdBot, totEdep / peng);
//    h2EdepFracCal->Fill(totEdepCal, totEdep / peng);
    h2EdepFracTot->Fill(totEdep, totEdep / peng);

    h2CalAcdAll->Fill(totEdepCal, totEdepAcd);
    if ((totEdep / peng) > 0.99)
      h2CalAcd09->Fill(totEdepCal, totEdepAcd);
    else
      h2CalAcd09L->Fill(totEdepCal, totEdepAcd);

    // Edep only in ACD
    if (totEdepAcd && !totEdepCal) {
      hFracACD->Fill(totEdep / peng);

      h2EdepFracAU->Fill(eAcdUp, totEdep / peng);
      h2EdepFracAB->Fill(eAcdBot, totEdep / peng);
      h2EdepFracCal->Fill(totEdepAcd, totEdep / peng);
    }

    // Edep only in Cal
    if (!totEdepAcd && totEdepCal) {
      hFracCal->Fill(totEdep / peng);
      h2NpixFrac->Fill(nHits, (totEdep / peng));

      if (eCalUp > eCalBot)
        hFracCalU->Fill(totEdep / peng);
      else
        hFracCalB->Fill(totEdep / peng);

    }

    // Edep both in Cal and ACD
    if (totEdepAcd && totEdepCal)
      hFracACDCal->Fill(totEdep / peng);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Trigger conditions
    // cond 1:
    if (!totEdepCal) {
//    if (!totEdep) {
      fNTrigCut1++;
      continue;
    }

    // cond 2:
//    if (eAcdUp > 200.0) {
    if (eAcdUp > 100.0) {
      fNTrigCut2++;
      continue;
    }

    // cond 3:
    if (eAcdBot) {
//    if (eAcdBot > 100.0) {
      fNTrigCut3++;
      continue;
    }

    // cond 4: TODO: need to check also for individual pixel depositions in a single event
    if (eCalUp < eCalBot) {
      fNTrigCut4++;
      continue;
    }

    // cond 5:
    if (totEdepAcd > (2.E5 / totEdepCal)) {
      fNTrigCut5++;
      continue;
    }

    // cond 6:
    if (nHits > 6) {
      fNTrigCut6++;
      continue;
    }

//    h2EdepFracTot->Fill(totEdep, totEdep / peng);
//    h2PDengCalTrig->Fill(peng, totEdepCal);
    h2PDengCalTrig->Fill(peng, totEdep);
    hEdepCal->Fill(totEdepCal);

    continue;

    hMPix->Fill(nHits);
    h2PengNpCal->Fill(peng, nHits);
    h2NpFedepCal->Fill(nHits, totEdepCal / peng);

    vector<Float_t> edepCal;
    vector<Int_t> pixCal;
    fCalAna->GetSortedEdeps(i, edepCal, pixCal);

    hMaxEdepPix->Fill(pixCal[0], hNorm->GetBinContent(hNorm->FindBin(peng)));

    for (auto v : pixCal) {
      v = (v >= fNPIXEL) ? v - fNPIXEL : v;
      hEdepPix->Fill(v);
      h3PosCal->Fill(upCrPos[v].x(), upCrPos[v].y(), upCrPos[v].z());

      if (nHits == 1)
        hEdep1Pix->Fill(v);
    }

    hFedep1pCal->Fill(edepCal[0] / totEdepCal);
    hFedep2pCal->Fill(edepCal[1] / totEdepCal);
    hFedep3pCal->Fill(edepCal[2] / totEdepCal);
    hFedepRpCal->Fill(1 - (edepCal[0] + edepCal[1] + edepCal[2]) / totEdepCal);

    if (pixCal[0] < fNPIXEL)
      hMedepViewCal->Fill(0);
    else
      hMedepViewCal->Fill(1);

    if (pixCal[0] == upBotMap[(pixCal[1] - fNPIXEL)])
      hMedepSameCal->Fill(0);
    else
      hMedepSameCal->Fill(1);

    // Position analysis
    vector<TVector3> posVec = fCalAna->GetEdepPos(i);
    vector<Int_t> pixId = fCalAna->GetPixelIds(i);
    Int_t id = 0;
    for (auto v : pixId) {
      if (v == pixCal[0])
        break;
      id++;
    }
    TVector3 maxEdepPos = posVec[id];
    maxEdepPos.SetMag(150.); // project on the sphere of 15 cm radius
    h2maxEdepPosXY->Fill(maxEdepPos.X(), maxEdepPos.Y());
  } // end loop over entries

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Discarded counts
  if (dnward)
    cout << "Discarded upwards = " << fNUpgoing << '\n';
  else
    cout << "Discarded downwards = " << fNDngoing << '\n';

  cout << "Discarded: No edep = " << fNTrigCut1 << '\n' << "Acd up edep = " << fNTrigCut2 << '\n' << "Acd dn edep = "
      << fNTrigCut3 << '\n' << "Cal dn > up edep = " << fNTrigCut4 << '\n' << "Acd > Factor * Cal = " << fNTrigCut5
      << '\n' << "nHits > 6 = " << fNTrigCut6 << '\n';

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Normalization of cal edep spectrum
  TH1F *hNormEdepCal = new TH1F("hNormEdepCal", "; Energy (keV);  Counts cm^{-2} keV^{-1} s^{-1}", fDNBin, &fDEngB[0]);
  TH1F *hNormEdepTotCal = new TH1F("hNormEdepTotCal", "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);

  if ((h2PDengCalTrig->GetNbinsY() != hNormEdepCal->GetNbinsX())
      || (h2PDengCalTrig->GetNbinsX() != hNorm->GetNbinsX())) {
    cout << "Problem with energy bins...\n";
    return;
  }

  for (Int_t i = 1; i <= h2PDengCalTrig->GetNbinsX(); i++) {
    Float_t norm = hNorm->GetBinContent(i);
    for (Int_t j = 1; j <= h2PDengCalTrig->GetNbinsY(); j++) {
      Float_t cnt = h2PDengCalTrig->GetBinContent(i, j) * norm; // # s^-1
      hNormEdepTotCal->AddBinContent(j, cnt);
      cnt /= (2 * TMath::Pi() * fDETRADIUS * fDETRADIUS * hNormEdepCal->GetBinWidth(j)); // # cm^-2 kev^-1 s^-1
      hNormEdepCal->AddBinContent(j, cnt);
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fOutfile->cd();

  hThAll->Write();
  hThDn->Write();
  hPeng->Write();
  h2PDengCal->Write();
  h2PDengAcdUp->Write();
  h2PDengAcdBot->Write();
  h2PDengTot->Write();
  h2PDengCalTrig->Write();
  h2PengFrac->Write();
  h2EdepFracAU->Write();
  h2EdepFracAB->Write();
  h2EdepFracCal->Write();
  h2EdepFracTot->Write();
  h2CalAcdAll->Write();
  h2CalAcd09->Write();
  h2CalAcd09L->Write();
  hFracACD->Write();
  hFracCal->Write();
  hFracACDCal->Write();
  h2NpixFrac->Write();
  hFracCalU->Write();
  hFracCalB->Write();
  hMPix->Write();
  hEdepPix->Write();
  hMaxEdepPix->Write();
  hEdep1Pix->Write();
  hEdepCal->Write();
  h3PosCal->Write();
  h2PengNpCal->Write();
  h2NpFedepCal->Write();
  hFedep1pCal->Write();
  hFedep2pCal->Write();
  hFedep3pCal->Write();
  hFedepRpCal->Write();
  hMedepViewCal->Write();
  hMedepSameCal->Write();
  h2maxEdepPosXY->Write();
  hNormPeng->Write();
  hNormEdepCal->Write();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::AnalyzeBkg(TString type, Int_t dir) {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Histograms
  TH1F *hThAll = new TH1F("hThAll", "; Cos(#theta); Entires", 180, -1, 1);
  TH1F *hThDir = new TH1F("hThDir", "; Cos(#theta); Entires", 180, -1, 1);
  TH1F *hPeng = new TH1F("hPeng", "Primary energy; Energy (keV); Entires", fPNBin, &fPEngB[0]);

  TH2F *h2PDengCal = new TH2F("hPDengCal", "Cal; Incident energy (keV); Depositied energy (keV); Counts", fPNBin,
      &fPEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2PDengAcdUp = new TH2F("hPDengAcdUp", "Acd up; Incident energy (keV); Depositied energy (keV); Counts", fPNBin,
      &fPEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2PDengAcdBot = new TH2F("hPDengAcdBot", "Acd dn; Incident energy (keV); Depositied energy (keV); Counts",
      fPNBin, &fPEngB[0], fDNBin, &fDEngB[0]);
  TH2F *h2PDengCalTrig = new TH2F("hPDengCalTrig", "Cal triger; Incident energy (keV); Depositied energy (keV); Counts",
      fPNBin, &fPEngB[0], fDNBin, &fDEngB[0]);

  Float_t pixVal[fNPIXEL + 1];
  for (Int_t i = 0; i <= fNPIXEL; i++)
    pixVal[i] = i;

  TH3F *h3PDengCalPixTrig = new TH3F("hPDengCalPixTrig",
      "Cal triger; Incident energy (keV); Depositied energy (keV); No of Pixel; Counts", fPNBin, &fPEngB[0], fDNBin,
      &fDEngB[0], fNPIXEL, &pixVal[0]);

  TH1F *hEdepCalNoT = new TH1F("hEdepCalNoT", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  TH1F *hEdepCalBT = new TH1F("hEdepCalBT", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  TH1F *hEdepCal = new TH1F("hEdepCal", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  TH1F *hEdepPix = new TH1F("hEdepPix", "; Pixel Id.; Entires", fNPIXEL, 0, fNPIXEL);
  TH1F *hMaxEdepPix = new TH1F("hMaxEdepPix", "; Pixel Id.; Entires", 2 * fNPIXEL, 0, 2 * fNPIXEL);
  TH3F *h3PosCal = new TH3F("hPosCal", "Cal edep pos dist.; X (mm); Y (mm); Z (mm); Counts s^{-1}", 29, -145, 145, 29,
      -145, 145, 29, -145, 145);
//      -145, 145, 15, -5, 145);
  TH2F *h2maxEdepPosXY = new TH2F("hmaxEdepPosXY", "Max edep pos; X (mm); Y (mm); Counts", 60, -150., 150., 60, -150.,
      150.);
  TH1F *hNormEdepCal = new TH1F("hNormEdepCal", "; Energy (keV);  Counts cm^{-2} keV^{-1} s^{-1}", fDNBin, &fDEngB[0]);
  TH1F *hNormEdepTotCal = new TH1F("hNormEdepTotCal", "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);
  TH2F *h2NormEdepTotCalPix = new TH2F("hNormEdepTotCalPix", ";No. of Pixel; Energy (keV); Counts s^{-1}", fNPIXEL,
      &pixVal[0], fDNBin, &fDEngB[0]);
  TH1F *hNormEdepPix = new TH1F("hNormEdepPix", "; Pixel Id.; Entires", fNPIXEL, 0, fNPIXEL);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Get the position of up crystals
  TVector3 *upCrPos = fCalAna->UpCrystalPos();

  // Get the up to bottom crystal mapping
//  Int_t *upBotMap = fCalAna->UpToBotCrystalMap();
  auto botToUpMap = fCalAna->UpToBotCrystalMap(1);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events for event normalization
  for (Int_t i = 0; i < fNEntCal; i++) {
    TVector3 dirV = fPrimAna->GetDirection(i);

    // Consider up/down ward particles
    if (dir == 1) {
      if (dirV.Z() >= 0) {
        continue;
      }
    } else if (dir == 2) {
      if (dirV.Z() < 0) {
        continue;
      }
    }

    Float_t peng = fPrimAna->GetEnergy(i);
    hPeng->Fill(peng);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Normalization of primary spectrum
  fPrimFunc = fPrimAna->GetPrimarySpec(type, dir, fPMinE, fPMaxE);
  if (!fPrimFunc) {
    cout << "[CEAnalysisAction::AnalyzeBkg] No primary spectrum was found...!" << endl;
    return;
  }

  TH1F *hNorm = (TH1F*) hPeng->Clone("hNorm");
  TH1F *hNormPeng = (TH1F*) hPeng->Clone("hNormPeng");

  Float_t srcArea;
  fSrcRad = fCalAna->GetSrcRadius();

  if (dir == 1)
    srcArea = 2 * TMath::Pi() * fSrcRad * fSrcRad;
//    srcArea = 2 * TMath::Pi() * SRCRADIUS * SRCRADIUS;
  else
    //if (dir == 0)
    srcArea = (3 * TMath::Pi() * fSrcRad * fSrcRad) + (2 * TMath::Pi() * fSrcRad * 1.0);
//    srcArea = (3 * TMath::Pi() * SRCRADIUS * SRCRADIUS) + (2 * TMath::Pi() * SRCRADIUS * 1.0);
//  else
//    srcArea = TMath::Pi() * SRCRADIUS * SRCRADIUS;

//  Float_t maxTheta = 68.*TMath::DegToRad();
//  Float_t maxTheta = SRCMAXTHETA * TMath::DegToRad();
  Float_t maxTheta = fCalAna->GetSrcMaxTheta() * TMath::DegToRad();

  for (Int_t i = 1; i <= hPeng->GetNbinsX(); i++) {
    Float_t lEdge = hPeng->GetBinLowEdge(i);
    Float_t uEdge = lEdge + hPeng->GetBinWidth(i);
    Float_t norm = 0;
    if (hPeng->GetBinContent(i) > 0)
      norm = fPrimFunc->Integral(lEdge, uEdge) * srcArea * TMath::Pi() * pow(sin(maxTheta), 2.)
          / hPeng->GetBinContent(i); // # s^-1
    hNorm->SetBinContent(i, norm);
    Float_t cnt = hPeng->GetBinContent(i) * norm / (srcArea * TMath::Pi() * hPeng->GetBinWidth(i)); // # cm^-2 sr^-2 kev^-1 s^-1
    hNormPeng->SetBinContent(i, cnt);
  }

  hNormPeng->SetTitle(";Energy (keV); Particles cm^{-2} sr^{-2} kev^{-1} s^{-1}");
  hNormPeng->Sumw2(kFALSE);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events
  for (Int_t i = 0; i < fNEntCal; i++) {
    TVector3 dirV = fPrimAna->GetDirection(i);
    hThAll->Fill(dirV.CosTheta());

    // Consider up/down ward particles
    if (dir == 1) {
      if (dirV.Z() >= 0) {
        fNUpgoing++;
        continue;
      }
    } else if (dir == 2) {
      if (dirV.Z() < 0) {
        fNDngoing++;
        continue;
      }
    }

    hThDir->Fill(dirV.CosTheta());

    Float_t peng = fPrimAna->GetEnergy(i);

    Float_t eCalUp(0.), eCalBot(0.);
    Float_t totEdepCal = fCalAna->GetTotalEdep(i, eCalUp, eCalBot);

    Float_t eAcdUp(0.), eAcdBot(0.);
    Float_t totEdepAcd = fAcdAna->GetTotalEdep(i, eAcdUp, eAcdBot);

    Int_t nHits = (fCalAna->GetEdeps(i)).size();

    h2PDengCal->Fill(peng, totEdepCal);
    h2PDengAcdUp->Fill(peng, eAcdUp);
    h2PDengAcdBot->Fill(peng, eAcdBot);
    hEdepCalNoT->Fill(totEdepCal);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Trigger conditions
    if (!(fAcdAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fAcdAna->IsTriggerOK(i, 2)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 2)))
      continue;

    hEdepCalBT->Fill(totEdepCal);

    if (!strcmp(fTrigConf.c_str(), "TT"))
      if (!(fCalAna->IsTriggerOK(i, 4)))
        continue;

    vector<Float_t> edepCal;
    vector<Int_t> pixCal;
    fCalAna->GetSortedEdeps(i, edepCal, pixCal);

    h2PDengCalTrig->Fill(peng, totEdepCal);
    hEdepCal->Fill(totEdepCal);

    hMaxEdepPix->Fill(pixCal[0], hNorm->GetBinContent(hNorm->FindBin(peng))); // normalized max edep pixel count
    hNormEdepTotCal->Fill(totEdepCal, hNorm->GetBinContent(hNorm->FindBin(peng))); // normalized calo edep spectrum (cnt/s)
    // normalized calo edep spectrum (cnt/s/cm^2/keV)
    hNormEdepCal->Fill(totEdepCal,
        hNorm->GetBinContent(hNorm->FindBin(peng))
            / ((2 * TMath::Pi() * fDETRADIUS * fDETRADIUS * hNormEdepCal->GetBinWidth(hNormEdepCal->FindBin(totEdepCal)))));

    for (int np = 0; np < pixCal.size(); np++) {
      auto v = pixCal[np];
      v = (v >= fNPIXEL) ? botToUpMap[v - fNPIXEL] : v; // for bottom pixels, first convert Ids from 0--2NPIX;
      // then find the corresponding up crystal Ids

      h3PDengCalPixTrig->Fill(peng, edepCal[np], v);
      hEdepPix->Fill(v);
      h3PosCal->Fill(upCrPos[v].x(), upCrPos[v].y(), upCrPos[v].z());
      if (totEdepCal > 0) {
        h2NormEdepTotCalPix->Fill(v, totEdepCal,
            (edepCal[np] / totEdepCal) * hNorm->GetBinContent(hNorm->FindBin(peng)));
        hNormEdepPix->Fill(v, (edepCal[np] / totEdepCal) * hNorm->GetBinContent(hNorm->FindBin(peng)));
      }
    }

    // Position analysis
    vector<TVector3> posVec = fCalAna->GetEdepPos(i);
    vector<Int_t> pixId = fCalAna->GetPixelIds(i);
    Int_t id = 0;
    for (auto v : pixId) {
      if (v == pixCal[0])
        break;
      id++;
    }
    TVector3 maxEdepPos = posVec[id];
    maxEdepPos.SetMag(150.); // project on the sphere of 15 cm radius
    h2maxEdepPosXY->Fill(maxEdepPos.X(), maxEdepPos.Y());

    if (!(i % 1000))
      cout << "Analysis of event no. " << i << " complete!\n";
  } // end loop over entries

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Discarded counts
  if (dir == 1)
    cout << "Discarded upwards = " << fNUpgoing << '\n';
  if (dir == 2)
    cout << "Discarded downwards = " << fNDngoing << '\n';

//  cout << "Discarded: No edep cal = " << fNTrigCut1 << '\n' << "Acd up edep = " << fNTrigCut2 << '\n'
//      << "Acd dn edep = " << fNTrigCut3 << '\n' << "Cal dn > up edep = " << fNTrigCut4 << '\n'
//      << "Edep only in a single crystal = " << fNTrigCut5 << '\n';

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Normalization of cal edep spectrum
//  if ((h2PDengCalTrig->GetNbinsY() != hNormEdepCal->GetNbinsX())
//      || (h2PDengCalTrig->GetNbinsX() != hNorm->GetNbinsX())) {
//    cout << "Problem with energy bins...\n";
//    return;
//  }
//
//  for (Int_t i = 1; i <= h2PDengCalTrig->GetNbinsX(); i++) {
//    Float_t norm = hNorm->GetBinContent(i);
//    for (Int_t j = 1; j <= h2PDengCalTrig->GetNbinsY(); j++) {
//      Float_t cnt = h2PDengCalTrig->GetBinContent(i, j) * norm; // # s^-1
////      hNormEdepTotCal->AddBinContent(j, cnt);
//      cnt /= (2 * TMath::Pi() * fDETRADIUS * fDETRADIUS * hNormEdepCal->GetBinWidth(j)); // # cm^-2 kev^-1 s^-1
//      hNormEdepCal->AddBinContent(j, cnt);
//    }
//  }

//  for (Int_t i = 1; i <= h3PDengCalPixTrig->GetNbinsX(); i++) {
//    Float_t norm = hNorm->GetBinContent(i);
//    for (Int_t j = 1; j <= h3PDengCalPixTrig->GetNbinsY(); j++) {
//      for (Int_t k = 1; k <= h3PDengCalPixTrig->GetNbinsZ(); k++) {
//        Double_t cnt = h3PDengCalPixTrig->GetBinContent(i, j, k) * norm; // # s^-1
////        cout << "i, j, k = " << i << " " << j << " " << k << " bin = " << h2NormEdepTotCalPix->GetBin(k, j) << " cnt = " << cnt << endl;
//        h2NormEdepTotCalPix->SetBinContent(k, j, cnt);
////        getchar();
//      }
//    }
//  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fOutfile->cd();

  hThAll->Write();
  hThDir->Write();
  hPeng->Write();
  hNorm->Write();
  h2PDengCal->Write();
  h2PDengAcdUp->Write();
  h2PDengAcdBot->Write();
  h2PDengCalTrig->Write();
  h3PDengCalPixTrig->Write();
  hEdepCalNoT->Write();
  hEdepCalBT->Write();
  hEdepCal->Write();
  hEdepPix->Write();
  hNormEdepPix->Write();
  hMaxEdepPix->Write();
  h3PosCal->Write();
  h2maxEdepPosXY->Write();
  hNormPeng->Write();
  hNormEdepCal->Write();
  hNormEdepTotCal->Write();
  h2NormEdepTotCalPix->Write();
  fPrimFunc->Write();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//void CEAnalysisAction::AnaIntrinsic(Float_t activityRate) {
void CEAnalysisAction::AnaIntrinsic(bool bothLyr) {
  fActivityRate = fCalAna->GetIntBkgRate(bothLyr);
  cout << "[CEAnalysisAction::AnaIntrinsic] LYSO activity rate = " << fActivityRate << " disintegration/s" << endl;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Histograms
  TH3F *hPos = new TH3F("hPos", ";X (mm); Y (mm); Z (mm)", 300, -150, 150, 300, -150, 150, 300, -150, 150);
  TH1F *hEdepCalNoT = new TH1F("hEdepCalNoT", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  TH1F *hEdepCalBT = new TH1F("hEdepCalBT", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  TH1F *hEdepCal = new TH1F("hEdepCal", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  TH1F *hEdepCalTrig = new TH1F("hEdepCalTrig", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  TH1F *hNormEdepTotCal = new TH1F("hNormEdepTotCal", "; Energy (keV); Counts/s", fDNBin, &fDEngB[0]);
  TH1F *hNormEdepTotCalAnd = new TH1F("hNormEdepTotCalAnd", "; Energy (keV); Counts/s", fDNBin, &fDEngB[0]);
  TH1F *hMaxEdepPix = new TH1F("hMaxEdepPix", "; Pixel Id.; Entires", 2 * fNPIXEL, 0, 2 * fNPIXEL);
  TH1F *hNormEdepPix = new TH1F("hNormEdepPix", "; Pixel Id.; Entires", fNPIXEL, 0, fNPIXEL);

  Float_t pixVal[fNPIXEL + 1];
  for (Int_t i = 0; i <= fNPIXEL; i++)
    pixVal[i] = i;
  TH2F *h2NormEdepTotCalPix = new TH2F("hNormEdepTotCalPix", ";No. of Pixel; Energy (keV); Counts s^{-1}", fNPIXEL,
      &pixVal[0], fDNBin, &fDEngB[0]);

  // No. of disintigration/s in the whole calo = 748774.21 /s
  // So simulation time for 10^6 events = 10^6 / 748774.21 s
  Float_t simT = 1E9 * fNEntPrim / fActivityRate; // in ns

  // Resolution function
  TF1 *fReso = new TF1("fReso", "sqrt((7.003*7.003/x)+(7.361*7.361))", 0.01, 100.); // x in MeV, output in dE/E %

  // Store event information
//  const size_t NP = 2 * fNPIXEL;
  struct Event {
    Float_t tim;
    Float_t pixE[500];
//    Float_t pixE[2 * fNPIXEL];

    bool operator<(const Event &a) const {
      return tim < a.tim;
    }
  };

  vector<Event> vEvt;

  // Get the up to bottom crystal mapping
  auto botToUpMap = fCalAna->UpToBotCrystalMap(1);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events
  for (Int_t i = 0; i < fNEntCal; i++) {
    TVector3 pos = fPrimAna->GetVertexPos(i);
    hPos->Fill(pos.X(), pos.Y(), pos.Z());

    Float_t eCalUp(0.), eCalBot(0.);
    Float_t totEdepCal(0.);  // = fCalAna->GetTotalEdep(i, eCalUp, eCalBot);

//    cout << i << " " << fCalAna->GetTotalEdep(i, eCalUp, eCalBot) << endl;
//    getchar();

    Float_t eAcdUp(0.), eAcdBot(0.);
    Float_t totEdepAcd = fAcdAna->GetTotalEdep(i, eAcdUp, eAcdBot);

    Int_t nHits = (fCalAna->GetEdeps(i)).size();

    vector<Float_t> edepVS;
    vector<Int_t> pixVS;
    fCalAna->GetSortedEdeps(i, edepVS, pixVS);

    Int_t nPix = 0;

    // pixel edep and Id
    auto edepV = fCalAna->GetEdeps(i);
    auto pixV = fCalAna->GetPixelIds(i);

    // Time (random) of the event
    Float_t evtTime = gRandom->Uniform(0, simT);

    struct Event evt = { evtTime, { 0. } };

    // Loop on the edep pixels
    for (Int_t i = 0; i < pixV.size(); i++) {
      Int_t pid = pixV[i];
      Float_t edep = edepV[i];

      // Calculate the spread in the edep from sigma of the energy resolution curve
//      Float_t sig = (1. / 2.355) * edep * 0.01 * fReso->Eval(edep / 1000.); // edep in MeV
//      Float_t dE = gRandom->Gaus(0, sig);
//            cout << pid << '\t' << edep << '\t' << sig << '\t' << dE << endl;
//            getchar();

//      edep += dE;

      // Consider the threshold edep in each pixel
//      if (edep < 20)
//        continue;

      // Fill the event structure with edep
      evt.pixE[pid] += edep;

      // No. of pixel hit and total edep in the calo
      nPix++;
      totEdepCal += edep;

      // Edep in upper and bottom pixels
      if (pid < fNPIXEL)
        eCalUp += edep;
      else
        eCalBot += edep;
    } // end loop on pixels

    hEdepCalNoT->Fill(totEdepCal);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Trigger conditions

//    if (fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 1)
//        && fCalAna->IsTriggerOK(i, 2)) {
//    if (fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 1)
//        && fCalAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 4)) {
    if (!(fAcdAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fAcdAna->IsTriggerOK(i, 2)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 2)))
      continue;

    hEdepCalBT->Fill(totEdepCal);

    if (!strcmp(fTrigConf.c_str(), "TT"))
      if (!(fCalAna->IsTriggerOK(i, 4)))
        continue;

    vEvt.push_back(evt);

    hEdepCal->Fill(totEdepCal);
    hEdepCalTrig->Fill(totEdepCal);

    if (pixVS.size())
      hMaxEdepPix->Fill(pixVS[0]);
//    }

    if (!(i % 1000))
      cout << "Analysis of event no. " << i << " complete!\n";
  } // end loop over entries

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Discarded counts
//  cout << "Discarded: No edep = " << fNTrigCut1 << '\n' << "Acd up edep = " << fNTrigCut2 << '\n' << "Acd dn edep = "
//      << fNTrigCut3 << '\n' << "Cal dn > up edep = " << fNTrigCut4 << '\n' << "Acd > Factor * Cal = " << fNTrigCut5
//      << '\n' << "nHits > 6 = " << fNTrigCut6 << '\n' << "Max edep in bottom crystal = " << fNTrigCut7 << '\n';

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sort events by time
  sort(vEvt.begin(), vEvt.end());

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check for coincidence
  Int_t nCoinc(0);
  for (Int_t i = 0; i < vEvt.size() - 1; i++) {
    if (abs(vEvt[i].tim - vEvt[i + 1].tim) < fSigTW) { // check for coincidence
      nCoinc++;
      //      cout << i << "   coincidence\n";
      int pi = 0;
      for (auto pe : vEvt[i].pixE) // loop on pixels
        vEvt[i].pixE[pi] += vEvt[i + 1].pixE[pi++];

      vEvt.erase(vEvt.begin() + i + 1);
      --i;
    }
    if (!(i % 10000))
      cout << "Coincidence check of event no. " << i << " complete!\n";
  }

  cout << "Total number of coincidence = " << nCoinc << endl;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Normalized cal edep spectrum
  for (auto evt : vEvt) { // loop on all events
    Float_t tEdep(0);
    Int_t np(0);

    for (auto pe : evt.pixE) { // loop on pixels
      tEdep += pe;
      if (pe > 0)
        np++;
    }

    hNormEdepTotCal->Fill(tEdep, 1.E9 / simT);

    // Coincidence in multiple pixels
    if (np > 1)
      hNormEdepTotCalAnd->Fill(tEdep, 1.E9 / simT);

    np = 0;
    for (auto pe : evt.pixE) {
//      if (pe) {
      if (tEdep > 0) {
        //        auto v = pixCal[np];
        //              v = (v >= fNPIXEL) ? botToUpMap[v - fNPIXEL] : v; // for bottom pixels, first convert Ids from 0--2NPIX;
        auto v = (np >= fNPIXEL) ? botToUpMap[np - fNPIXEL] : np; // for bottom pixels, first convert Ids from 0--2NPIX;
        // then find the corresponding up crystal Ids
        hNormEdepPix->Fill(v, (pe / tEdep) * (1.E9 / simT));
        h2NormEdepTotCalPix->Fill(v, tEdep, (pe / tEdep) * (1.E9 / simT));
      }
      np++;
    }

  } // end loop on events

//  hNormEdepTotCal->Sumw2(kFALSE);
//  hNormEdepTotCalAnd->Sumw2(kFALSE);
//  hNormEdepPix->Sumw2(kFALSE);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Write outputs
  fOutfile->cd();

  hPos->Write();
  hEdepCalNoT->Write();
  hEdepCalBT->Write();
  hEdepCal->Write();
  hEdepCalTrig->Write();
  hNormEdepTotCal->Write();
  hNormEdepTotCalAnd->Write();
  hMaxEdepPix->Write();
  hNormEdepPix->Write();
  h2NormEdepTotCalPix->Write();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::CalcEffArea() {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Histograms
  TH1F *hPeng = new TH1F("hPeng", "; Energy (keV); Entries", fPNBin, &fPEngB[0]);
  TH1F *hEffArea = new TH1F("hEffArea", "; Energy (keV); Effective Area (cm^{2})", fPNBin, &fPEngB[0]);
  TH1F *hEffArea1P = new TH1F("hEffArea1P", "; Energy (keV); Effective Area (cm^{2})", fPNBin, &fPEngB[0]);
  TH1F *hEffAreaN = new TH1F("hEffAreaN", "; Energy (keV); Effective Area (cm^{2})", fPNBin, &fPEngB[0]);
  TH1F *hEffAreaAll = new TH1F("hEffAreaAll", "; Energy (keV); Effective Area (cm^{2})", fPNBin, &fPEngB[0]);
//  TH2F *h2PDengCalTrig = new TH2F("hPDengCalTrig", "Cal triger; Incident energy (keV); Depositied energy (keV); Counts",
//      fPNBin, &fPEngB[0], fDNBin, &fDEngB[0]);

  hEffArea->Sumw2(kTRUE);
  hEffArea1P->Sumw2(kTRUE);
  hEffAreaN->Sumw2(kTRUE);
  hEffAreaAll->Sumw2(kTRUE);

  fSrcRad = fCalAna->GetSrcRadius('e');
  Float_t srcArea = 4. * fSrcRad * fSrcRad;  // cm2
//  Float_t srcArea = 4. * SRCRADIUS * SRCRADIUS;  // cm2

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events
  for (Int_t i = 0; i < fNEntCal; i++) {
//    cout << "Event " << i << endl;
    Float_t peng = fPrimAna->GetEnergy(i);
    hPeng->Fill(peng);

//    Float_t eCalUp(0.), eCalBot(0.);
//    Float_t totEdepCal = fCalAna->GetTotalEdep(i, eCalUp, eCalBot);
//
//    Float_t eAcdUp(0.), eAcdBot(0.);
//    Float_t totEdepAcd = fAcdAna->GetTotalEdep(i, eAcdUp, eAcdBot);
//
    Int_t nHits = (fCalAna->GetEdeps(i)).size();
//
//    vector<Float_t> edepV;
//    vector<Int_t> pixV;
//    fCalAna->GetSortedEdeps(i, edepV, pixV);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Trigger conditions
    if (fCalAna->IsTriggerOK(i, 1))
      hEffAreaAll->Fill(peng);

    // Trigger conditions
    if (!(fAcdAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fAcdAna->IsTriggerOK(i, 2)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 2)))
      continue;
    if (!strcmp(fTrigConf.c_str(), "TT"))
      if (!(fCalAna->IsTriggerOK(i, 4)))
        continue;

//    if (fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 1)
//        && fCalAna->IsTriggerOK(i, 2)) {
    hEffArea1P->Fill(peng);
//      h2PDengCalTrig->Fill(peng, totEdepCal);
//    }

//    if (fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 1)
//        && fCalAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 4))
//      hEffAreaN->Fill(peng);

//    if (fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2) && fAcdAna->IsTriggerOK(i, 3)
//        && fCalAna->IsTriggerOK(i, 1) && fCalAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 4) && (nHits > 1))
//      hEffArea->Fill(peng);

//    if (fCalAna->IsTriggerOK(i, 4))
//      hEffArea->Fill(peng);
//    if (fCalAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2)
//        && fCalAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 4))
//      hEffAreaN->Fill(peng);
  } // end loop over entries

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Calculate effective area from the histos
  hEffArea->Divide(hPeng);
  hEffArea1P->Divide(hPeng);
//  hEffAreaN->Divide(hPeng);
  hEffAreaAll->Divide(hPeng);

  hEffArea->Scale(srcArea);
  hEffArea1P->Scale(srcArea);
//  hEffAreaN->Scale(srcArea);
  hEffAreaAll->Scale(srcArea);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fOutfile->cd();

  hEffArea->Write();
  hEffArea1P->Write();
//  hEffAreaN->Write();
  hEffAreaAll->Write();
//  h2PDengCalTrig->Write();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::AnalyzeGRB() {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Histograms
  TH1F *hPeng = new TH1F("hPeng", "Primary energy; Energy (keV); Entires", fPNBin, &fPEngB[0]);
  TH2F *h2PDengCalTrig = new TH2F("hPDengCalTrig", "Cal triger; Incident energy (keV); Depositied energy (keV); Counts",
      fPNBin, &fPEngB[0], fDNBin, &fDEngB[0]);
  TH1F *hEdepCal = new TH1F("hEdepCal", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events
  for (Int_t i = 0; i < fNEntCal; i++) {
    Float_t peng = fPrimAna->GetEnergy(i);
    hPeng->Fill(peng);

    Float_t eCalUp(0.), eCalBot(0.);
    Float_t totEdepCal = fCalAna->GetTotalEdep(i, eCalUp, eCalBot);

    Float_t eAcdUp(0.), eAcdBot(0.);
    Float_t totEdepAcd = fAcdAna->GetTotalEdep(i, eAcdUp, eAcdBot);

//    Int_t nHits = (fCalAna->GetEdeps(i)).size();
//
//    vector<Float_t> edepV;
//    vector<Int_t> pixV;
//    fCalAna->GetSortedEdeps(i, edepV, pixV);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Trigger conditions

//    if (fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 1)
//        && fCalAna->IsTriggerOK(i, 2) /*&& fCalAna->IsTriggerOK(i, 4)*/) {

    if (!(fAcdAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fAcdAna->IsTriggerOK(i, 2)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 2)))
      continue;
    if (!strcmp(fTrigConf.c_str(), "TT"))
      if (!(fCalAna->IsTriggerOK(i, 4)))
        continue;

    h2PDengCalTrig->Fill(peng, totEdepCal);
    hEdepCal->Fill(totEdepCal);
//    }

  } // end loop over entries

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Discarded counts
//  cout << "Discarded: No edep cal = " << fNTrigCut1 << '\n' << "Acd up edep = " << fNTrigCut2 << '\n'
//      << "Acd dn edep = " << fNTrigCut3 << '\n' << "Cal dn > up edep = " << fNTrigCut4 << '\n'
//      << "Edep only in a single crystal = " << fNTrigCut5 << '\n';

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Normalization of primary spectrum
  fPrimFunc = fPrimAna->GetGRBSpec(fPMinE, fPMaxE);
  if (!fPrimFunc) {
    cout << "[CEAnalysisAction::AnalyzeGRB] No primary GRB spectrum was found...!" << endl;
    return;
  }

  TString dataPath = std::getenv("CRYSTALEYE_DATA");
  dataPath += "Spec/";
  TString dataFile = dataPath + "GRBCompParam.txt";

  TTree *tGRB = new TTree("tGRB", "GRB param data from ascii file");
  const Int_t nGRB = tGRB->ReadFile(dataFile, "x:y:z:a");
//  printf("Found %d lines\n", nGRB);

  Float_t ampl, epeak, index;
  tGRB->SetBranchAddress("x", &ampl);
  tGRB->SetBranchAddress("y", &epeak);
  tGRB->SetBranchAddress("z", &index);

  TH1F *hNorm = (TH1F*) hPeng->Clone("hNorm");
  TH1F *hNormPeng[nGRB], *hNormEdepCal[nGRB], *hNormEdepTotCal[nGRB];

  //  Float_t srcArea = 32. * 32.;  // cm2
  fSrcRad = fCalAna->GetSrcRadius('e');
  Float_t srcArea = 4. * fSrcRad * fSrcRad;  // cm2
//  Float_t srcArea = 4. * SRCRADIUS * SRCRADIUS;  // cm2

  Char_t name[50];
  for (int iGRB = 0; iGRB < nGRB; iGRB++) { // loop on all GRB models
    tGRB->GetEntry(iGRB);

    fPrimFunc->SetParameters(ampl, epeak, index);
    sprintf(name, "hNormPeng%d", iGRB);
    hNormPeng[iGRB] = (TH1F*) hPeng->Clone(name);
    sprintf(name, "hNormEdepCal%d", iGRB);
    hNormEdepCal[iGRB] = new TH1F(name, "; Energy (keV);  Counts cm^{-2} keV^{-1} s^{-1}", fDNBin, &fDEngB[0]);
    sprintf(name, "hNormEdepTotCal%d", iGRB);
    hNormEdepTotCal[iGRB] = new TH1F(name, "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);

    for (Int_t iBin = 1; iBin <= hPeng->GetNbinsX(); iBin++) {
      Float_t lEdge = hPeng->GetBinLowEdge(iBin);
      Float_t uEdge = lEdge + hPeng->GetBinWidth(iBin);
      Float_t norm = fPrimFunc->Integral(lEdge, uEdge) * srcArea / hPeng->GetBinContent(iBin); // # s^-1
      hNorm->SetBinContent(iBin, norm);
      Float_t cnt = hPeng->GetBinContent(iBin) * norm / (srcArea * hPeng->GetBinWidth(iBin)); // # cm^-2 kev^-1 s^-1
      hNormPeng[iGRB]->SetBinContent(iBin, cnt);
    }

    hNormPeng[iGRB]->SetTitle(";Energy (keV); Photons cm^{-2} kev^{-1} s^{-1}");
    hNormPeng[iGRB]->Sumw2(kFALSE);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Normalization of cal edep spectrum
    if ((h2PDengCalTrig->GetNbinsY() != hNormEdepCal[iGRB]->GetNbinsX())
        || (h2PDengCalTrig->GetNbinsX() != hNorm->GetNbinsX())) {
      cout << "Problem with energy bins...\n";
      return;
    }

    for (Int_t i = 1; i <= h2PDengCalTrig->GetNbinsX(); i++) {
      Float_t norm = hNorm->GetBinContent(i);
      for (Int_t j = 1; j <= h2PDengCalTrig->GetNbinsY(); j++) {
        Float_t cnt = h2PDengCalTrig->GetBinContent(i, j) * norm; // # s^-1
        hNormEdepTotCal[iGRB]->AddBinContent(j, cnt);
        cnt /= (2 * TMath::Pi() * fDETRADIUS * fDETRADIUS * hNormEdepCal[iGRB]->GetBinWidth(j)); // # cm^-2 kev^-1 s^-1
        hNormEdepCal[iGRB]->AddBinContent(j, cnt);
      }
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fOutfile->cd();

  hPeng->Write();
  h2PDengCalTrig->Write();
  hEdepCal->Write();

  for (int i = 0; i < nGRB; i++) {
    hNormPeng[i]->Write();
    hNormEdepCal[i]->Write();
    hNormEdepTotCal[i]->Write();
  }
//  fPrimFunc->Write();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CEAnalysisAction::AnalyzeFiducialGRB() {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Histograms
  auto hPeng = new TH1F("hPeng", "Primary energy; Energy (keV); Entires", fPNBin, &fPEngB[0]);
  auto hFidu = new TProfile("hFidu", "Profile", fPNBin, &fPEngB[0], "");
  auto hFlat = new TH1F("hFlat", "Flat spectrum", fPNBin, &fPEngB[0]);
  auto h2PDengCalTrig = new TH2F("hPDengCalTrig", "Cal triger; Incident energy (keV); Depositied energy (keV); Counts",
      fPNBin, &fPEngB[0], fDNBin, &fDEngB[0]);
  auto hEdepCal = new TH1F("hEdepCal", "; Energy (keV); Entires", fDNBin, &fDEngB[0]);
  auto hNormEdepCal = new TH1F("hNormEdepCal", "; Energy (keV);  Counts cm^{-2} keV^{-1} s^{-1}", fDNBin, &fDEngB[0]);
  auto hNormEdepTotCal = new TH1F("hNormEdepTotCal", "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);
  auto hNormEdepTotCalF = new TH1F("hNormEdepTotCalF", "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);
  auto hNormEdepTotCalG = new TH1F("hNormEdepTotCalG", "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);
  auto hNormEdepTotCalB = new TH1F("hNormEdepTotCalB", "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);
  auto hNormEdepTotCalU = new TH1F("hNormEdepTotCalU", "; Energy (keV); Counts s^{-1}", fDNBin, &fDEngB[0]);
  Float_t pixVal[fNPIXEL + 1];
  for (Int_t i = 0; i <= fNPIXEL; i++)
    pixVal[i] = i;
  auto hNormEdepPix = new TH1F("hNormEdepPix", "; Pixel ID; Entires", fNPIXEL, &pixVal[0]);
  auto hNormEdepPixF = new TH1F("hNormEdepPixF", "; Pixel ID; Entires", fNPIXEL, &pixVal[0]);
  auto hNormEdepPixG = new TH1F("hNormEdepPixG", "; Pixel ID; Entires", fNPIXEL, &pixVal[0]);
  auto hNormEdepPixB = new TH1F("hNormEdepPixB", "; Pixel ID; Entires", fNPIXEL, &pixVal[0]);
  auto hNormEdepPixU = new TH1F("hNormEdepPixU", "; Pixel ID; Entires", fNPIXEL, &pixVal[0]);
  auto hEdepPix = new TH2F("hEdepPix", "; Pixel ID; Energy (keV); Entires", fNPIXEL, &pixVal[0], fPNBin, &fPEngB[0]);

  // Output tree containing peng, totEdepCal, pixId[px], and edepCal[px]
  auto tEdepPix = new TTree("tEdepPix", "Pixel energy deposition");
  Float_t bPeng, bTotedep;
  vector<Float_t> bPixedep;
  vector<Int_t> bPixid;
  vector<Int_t> bPixlyr;
  vector<Float_t> bAcdedep;
  vector<Int_t> bAcdid;
  tEdepPix->Branch("peng", &bPeng, "peng/F");
  tEdepPix->Branch("totEdep", &bTotedep, "totEdep/F");
  tEdepPix->Branch("pixEdep[px]", &bPixedep);
  tEdepPix->Branch("pixID[px]", &bPixid);
  tEdepPix->Branch("pixLyr[px]", &bPixlyr);
  tEdepPix->Branch("acdEdep[px]", &bAcdedep);
  tEdepPix->Branch("acdID[px]", &bAcdid);

  // Get the up to bottom crystal mapping
//  Int_t *upBotMap = fCalAna->UpToBotCrystalMap();
  auto botToUpMap = fCalAna->UpToBotCrystalMap(1);
  auto acdToUpMap = fAcdAna->UpCrystalToAcdMap(1);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Calculate the fiducial GRB
  TString dataPath = std::getenv("CRYSTALEYE_DATA");
  dataPath += "Spec/";
  TString dataFile = dataPath + "GRBCompParam.txt";

  TTree *tGRB = new TTree("tGRB", "GRB param data from ascii file");
  const Int_t nGRB = tGRB->ReadFile(dataFile, "x:y:z:a");
  //  printf("Found %d lines\n", nGRB);

  Float_t ampl, epeak, index;
  tGRB->SetBranchAddress("x", &ampl);
  tGRB->SetBranchAddress("y", &epeak);
  tGRB->SetBranchAddress("z", &index);

  TF1 *f1[nGRB];
  char name[20];
  for (int i = 0; i < nGRB; i++) {
    tGRB->GetEntry(i);

    sprintf(name, "f1%d", i);
    f1[i] = new TF1(name, "[0]*((x/100)**[1])*exp(-([1]+2)*x/[2])", 30.0, 1.0e6);
    f1[i]->SetParameter(0, ampl);
    f1[i]->SetParameter(1, index);
    f1[i]->SetParameter(2, epeak);

    for (int j = 1; j <= hPeng->GetNbinsX(); j++)
      hFidu->Fill(hPeng->GetBinCenter(j), f1[i]->Eval(hPeng->GetBinCenter(j)));
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // The flat spectrum
  auto ff = new TF1("fflat", "pow(x,-1)", 30.0, 1.0e5);
  hFlat->FillRandom("fflat", 100000);
  hFlat->Scale(hFidu->Integral() / 100000);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // The good spectrum
  auto fG = new TF1("fG", "[0]*((x/100)**[1])*exp(-([1]+2)*x/[2])", 30.0, 1.0e6);
  fG->SetParameter(0, 1.6078);
  fG->SetParameter(1, -0.51);
  fG->SetParameter(2, 805.5);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // The bad spectrum
  auto fB = new TF1("fB", "[0]*((x/100)**[1])*exp(-([1]+2)*x/[2])", 30.0, 1.0e6);
  fB->SetParameter(0, 0.4358);
  fB->SetParameter(1, 0.40);
  fB->SetParameter(2, 31.6);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // The ugly spectrum (GRB170817A)
  auto fU = new TF1("fU", "[0]*((x/100)**[1])*exp(-([1]+2)*x/[2])", 30.0, 1.0e5);
  fU->SetParameter(0, 0.0148);
  fU->SetParameter(1, -0.62);
  fU->SetParameter(2, 185.0);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events for event normalization
  for (Int_t i = 0; i < fNEntCal; i++) {
    hPeng->Fill(fPrimAna->GetEnergy(i));
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Normalization of primary spectrum
  TH1F *hNorm = (TH1F*) hPeng->Clone("hNorm");
  TH1F *hNormF = (TH1F*) hPeng->Clone("hNormF");
  TH1F *hNormG = (TH1F*) hPeng->Clone("hNormG");
  TH1F *hNormB = (TH1F*) hPeng->Clone("hNormB");
  TH1F *hNormU = (TH1F*) hPeng->Clone("hNormU");
//  TH1F *hNormPeng = (TH1F*) hPeng->Clone("hNormPeng");

  fSrcRad = fCalAna->GetSrcRadius('e');
  Float_t srcArea = 4. * fSrcRad * fSrcRad;  // cm2
//  Float_t srcArea = 4. * SRCRADIUS * SRCRADIUS;  // cm2

  for (Int_t i = 1; i <= hPeng->GetNbinsX(); i++) {
    // Fiducial
    Float_t norm = hFidu->GetBinContent(i) * hPeng->GetBinWidth(i) * srcArea / hPeng->GetBinContent(i); // # s^-1
    hNorm->SetBinContent(i, norm);

    // Flat
//    norm = hFlat->GetBinContent(i) * hPeng->GetBinWidth(i) * srcArea / hPeng->GetBinContent(i); // # s^-1
    norm = hPeng->GetBinWidth(i) * srcArea / hPeng->GetBinContent(i); // norm without specific src flux spectrum
    hNormF->SetBinContent(i, norm);
//    Float_t cnt = hPeng->GetBinContent(i) * norm / (srcArea * hPeng->GetBinWidth(i)); // # cm^-2 sr^-2 kev^-1 s^-1
//    hNormPeng->SetBinContent(i, cnt);

    // Good
    Float_t lEdge = hPeng->GetBinLowEdge(i);
    Float_t uEdge = lEdge + hPeng->GetBinWidth(i);
    norm = fG->Integral(lEdge, uEdge) * srcArea / hPeng->GetBinContent(i); // # s^-1
    hNormG->SetBinContent(i, norm);

    // Bad
    norm = fB->Integral(lEdge, uEdge) * srcArea / hPeng->GetBinContent(i); // # s^-1
    hNormB->SetBinContent(i, norm);

    // Ugly
    norm = fU->Integral(lEdge, uEdge) * srcArea / hPeng->GetBinContent(i); // # s^-1
    hNormU->SetBinContent(i, norm);
  }

//  hNormPeng->SetTitle(";Energy (keV); Photons cm^{-2} kev^{-1} s^{-1}");
//  hNormPeng->Sumw2(kFALSE);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop on events
  for (Int_t i = 0; i < fNEntCal; i++) {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Trigger conditions

//    if (!(fAcdAna->IsTriggerOK(i, 1) && fAcdAna->IsTriggerOK(i, 2) && fCalAna->IsTriggerOK(i, 1)
//        && fCalAna->IsTriggerOK(i, 2) /*&& fCalAna->IsTriggerOK(i, 4)*/))
//      continue;

    if (!(fAcdAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fAcdAna->IsTriggerOK(i, 2)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 1)))
      continue;
    if (!(fCalAna->IsTriggerOK(i, 2)))
      continue;
    if (!strcmp(fTrigConf.c_str(), "TT"))
      if (!(fCalAna->IsTriggerOK(i, 4)))
        continue;

    Float_t peng = fPrimAna->GetEnergy(i);
    bPeng = peng;

    Float_t eCalUp(0.), eCalBot(0.);
    Float_t totEdepCal = fCalAna->GetTotalEdep(i, eCalUp, eCalBot);
    bTotedep = totEdepCal;

    Float_t eAcdUp(0.), eAcdBot(0.);
    Float_t totEdepAcd = fAcdAna->GetTotalEdep(i, eAcdUp, eAcdBot);

    h2PDengCalTrig->Fill(peng, totEdepCal);
    hEdepCal->Fill(totEdepCal);

    vector<Float_t> edepCal;
    vector<Int_t> pixCal;
    vector<Float_t> edepAcd;
    vector<Int_t> pixAcd;
    edepCal.clear();
    pixCal.clear();
    bPixedep.clear();
    bPixid.clear();
    bPixlyr.clear();
    edepAcd.clear();
    pixAcd.clear();
    fCalAna->GetSortedEdeps(i, edepCal, pixCal);

    hNormEdepTotCal->Fill(totEdepCal, hNorm->GetBinContent(hNorm->FindBin(peng))); // normalized calo edep spectrum (cnt/s)
    hNormEdepTotCalF->Fill(totEdepCal, hNormF->GetBinContent(hNormF->FindBin(peng))); // normalized calo edep spectrum (cnt/s)
    hNormEdepTotCalG->Fill(totEdepCal, hNormG->GetBinContent(hNormG->FindBin(peng))); // normalized calo edep spectrum (cnt/s)
    hNormEdepTotCalB->Fill(totEdepCal, hNormB->GetBinContent(hNormB->FindBin(peng))); // normalized calo edep spectrum (cnt/s)
    hNormEdepTotCalU->Fill(totEdepCal, hNormU->GetBinContent(hNormU->FindBin(peng))); // normalized calo edep spectrum (cnt/s)

    // normalized calo edep spectrum (cnt/s/cm^2/keV)
    hNormEdepCal->Fill(totEdepCal,
        hNorm->GetBinContent(hNorm->FindBin(peng))
            / ((2 * TMath::Pi() * fDETRADIUS * fDETRADIUS * hNormEdepCal->GetBinWidth(hNormEdepCal->FindBin(totEdepCal)))));

    for (int np = 0; np < pixCal.size(); np++) {
      auto v = pixCal[np];
      bPixlyr.push_back((v >= fNPIXEL) ? 1 : 0); // for bottom pixels, first convert Ids from 0--2NPIX;
      v = (v >= fNPIXEL) ? botToUpMap[v - fNPIXEL] : v; // for bottom pixels, first convert Ids from 0--2NPIX;
      pixCal[np] = v; // then find the corresponding up crystal Ids

      hNormEdepPix->Fill(v, (edepCal[np] / totEdepCal) * hNorm->GetBinContent(hNorm->FindBin(peng)));
      hNormEdepPixF->Fill(v,
          (edepCal[np] / totEdepCal) * hFlat->GetBinContent(hNormF->FindBin(peng))
              * hNormF->GetBinContent(hNormF->FindBin(peng)));
      hNormEdepPixG->Fill(v, (edepCal[np] / totEdepCal) * hNormG->GetBinContent(hNormG->FindBin(peng)));
      hNormEdepPixB->Fill(v, (edepCal[np] / totEdepCal) * hNormB->GetBinContent(hNormB->FindBin(peng)));
      hNormEdepPixU->Fill(v, (edepCal[np] / totEdepCal) * hNormU->GetBinContent(hNormU->FindBin(peng)));

      hEdepPix->Fill(v, peng, (edepCal[np] / totEdepCal) * hNormF->GetBinContent(hNormF->FindBin(peng)));
//      h3PDengPixTrig->Fill(peng, (edepCal[np] / totEdepCal), v);
    }

    edepAcd = fAcdAna->GetEdeps(i);
    pixAcd = fAcdAna->GetPixelIds(i);
    for (int np = 0; np < pixAcd.size(); np++) {
      auto v = pixAcd[np];
      if (v >= fNPIXEL) {
        edepAcd.erase(edepAcd.begin() + np);
        pixAcd.erase(pixAcd.begin() + np);
        continue;
      }

      pixAcd[np] = acdToUpMap[v - fNPIXEL]; // for bottom pixels, first convert Ids from 0--2NPIX;
    }

    bPixedep = edepCal;
    bPixid = pixCal;
    bAcdedep = edepAcd;
    bAcdid = pixAcd;
    tEdepPix->Fill();

    if (!(i % 1000))
      cout << "Analysis of event no. " << i << " complete!\n";

  } // end loop over entries

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Discarded counts
//  cout << "Discarded: No edep cal = " << fNTrigCut1 << '\n' << "Acd up edep = " << fNTrigCut2 << '\n'
//      << "Acd dn edep = " << fNTrigCut3 << '\n' << "Cal dn > up edep = " << fNTrigCut4 << '\n'
//      << "Edep only in a single crystal = " << fNTrigCut5 << '\n';

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fOutfile->cd();

  hFidu->Write();
  hFlat->Write();
  hPeng->Write();
  hNorm->Write();
//  fG->Write();
//  fB->Write();
//  fU->Write();
//  hNormPeng->Write();
  h2PDengCalTrig->Write();
//  hEdepCal->Write();
  hNormEdepCal->Write();
  hNormEdepTotCal->Write();
  hNormEdepTotCalF->Write();
//  hNormEdepTotCalG->Write();
//  hNormEdepTotCalB->Write();
//  hNormEdepTotCalU->Write();
  hNormEdepPix->Write();
  hNormEdepPixF->Write();
//  hNormEdepPixG->Write();
//  hNormEdepPixB->Write();
//  hNormEdepPixU->Write();
  hEdepPix->Write();
//  h3PDengPixTrig->Write();
  tEdepPix->Write();

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
