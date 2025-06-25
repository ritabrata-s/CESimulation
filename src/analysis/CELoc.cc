/********************************************************************************
 *
 * CELoc.cc
 *
 * Main function for the location reconstruction
 *
 * Author: Ritabrata Sarkar
 * Created on: 13 Dec, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CELocalization.hh"

#include<TString.h>

#include<iostream>
#include<fstream>
#include<cstring>
#include<sstream>

using namespace std;

// ------------------------------------------------------------
void usage() {
  cout << "Usage: CELoc [options] \n";
  cout << "This is the CrystalEye localization code\n";
  cout << "Options:\n";
  cout << " -d, --data-card FILE         data card containing task and required files (default: not set)\n";
  cout << "                              [-h, --help for data card format]\n";
//  cout << " -i, --input-file FILE        input filename or data path (default: not set;)\n";
//  cout << " -o, --output-file FILE       output filename (default: output.root)\n";
  cout << " -v, --verbose                be verbose [default: false]\n";
  cout << " -h, --help                   display this help and exit\n";
  cout << "\nTo run CELoc the -d [filename] option MUST be set!\n";
  cout << "Report bugs to: ritabrata.sarkar@gssi.it \n";
}

// ------------------------------------------------------------
void DataCardUsage() {
  cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  cout << "Usage: CELoc data card format\n";
  cout << "Options:\n";
  cout << " TASKNAME STRING              name of the task [options: POSI, UNCER] (default: not set)\n";
  cout << " INPUTFILE FILE               input filename or data path (default: not set;)\n";
  cout << " OUTPUTFILE FILE              output filename (default: output.root)\n";
  cout << " PIXPOSFILE FILE              full path and name of the file containing\n";
  cout << "                              detector pixel positions (default: not set)\n";
  cout << " EFFAREAFILE FILE             full path and name of the file containing\n";
  cout << "                              detector effective area (default: not set)\n";
  cout << " TEMPOSINFOFILE FILE          full path and name of the file containing\n";
  cout << "                              template position information (default: not set)\n";
  cout << " TEMDATAFILE FILE             full path and name of the file containing template data (default: not set)\n";
  cout << " BKGDATAFILE FILE/NONE        full path and name of the file containing\n";
  cout << "                              background data (default: not set)\n";
  cout << " SAMPOSINFOFILE FILE          full path and name of the file containing\n";
  cout << "                              sample position information (default: not set)\n";
  cout << " WRITEHIST TRUE/FALSE         flag to write the probability distribution histogram (default: FALSE)\n";
  cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
}

// ------------------------------------------------------------
// Global variables
static string dataCard = "";
static string taskName = "";
static string inputFile = "";
static string outputFile = "output.root";
static string pixPosFile = "";
static string effAreaFile = "";
static string temPosInfoFile = "";
static string temDataFile = "";
static string bkgDataFile = "";
static string samPosInfoFile = "";
static bool verbose = false;

const TString programName = "[CELoc] ";

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int HandleInputPar(int argc, char **argv) {
  if (argc > 1) {
    int i = 1;
    while (i < argc) {
      if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
        usage();
        return 1;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--data-card")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        dataCard = argv[i];
        if (verbose)
          cout << programName << "dataCard set to " << dataCard << endl;
      }
      // -----------------------------------------------------//
//      else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input-file")) {
//        if (++i >= argc) {
//          usage();
//          return 1;
//        }
//        inputFile = argv[i];
//        if (verbose)
//          cout << programName << "inputFile set to " << inputFile << endl;
//      }
      // -----------------------------------------------------//
//      else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output-file")) {
//        if (++i >= argc) {
//          usage();
//          return 1;
//        }
//        outputFile = argv[i];
//        if (verbose)
//          cout << programName << "outputFile set to " << outputFile << endl;
//      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
        verbose = true;
        cout << programName << "verbose flag is set " << endl;
      }
      // -----------------------------------------------------//
      else {
        if (verbose)
          cout << programName << "WARNING: unidentified input argument \"" << argv[i] << "\" Ignored." << endl;
      }
      i++;
    }
  } else {
    usage();
    return 1;
  }
  return 0;
}

// ------------------------------------------------------------
int ReadDataCard(string datacard) {
  if (!strcmp(datacard.data(), "-h") || !strcmp(datacard.data(), "--help")) {
    DataCardUsage();
    return 1;
  }

  ifstream cardfile(datacard);
  if (!cardfile) { // check if file exists
    cout << programName << " ERROR: FILE " << datacard << " does not exist!" << endl;
    return 1;
  }

//  const short NPAR = 20;
  string par[20];
  string line;
  int npar = 0;

  while (1) {
    cardfile >> par[npar++];
    if (cardfile.eof())
      break;
  }

//  if (npar == NPAR) {
  int i = 0;
  while (i < npar) {

    if (!strcmp(par[i].data(), "TASKNAME")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      taskName = par[i].data();
      if (verbose)
        cout << programName << "TASKNAME set to " << taskName << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].data(), "INPUTFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      inputFile = par[i].data();
      if (verbose)
        cout << programName << "INPUTFILE set to " << inputFile << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].c_str(), "OUTPUTFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      outputFile = par[i];
      if (verbose)
        cout << programName << "OUTPUTFILE set to " << outputFile << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].c_str(), "PIXPOSFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      pixPosFile = par[i];
      if (verbose)
        cout << programName << "PIXPOSFILE set to " << pixPosFile << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].c_str(), "EFFAREAFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      effAreaFile = par[i];
      if (verbose)
        cout << programName << "EFFAREAFILE set to " << effAreaFile << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].c_str(), "TEMPOSINFOFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      temPosInfoFile = par[i];
      if (verbose)
        cout << programName << "TEMPOSINFOFILE set to " << temPosInfoFile << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].c_str(), "TEMDATAFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      temDataFile = par[i];
      if (verbose)
        cout << programName << "TEMDATAFILE set to " << temDataFile << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].c_str(), "BKGDATAFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      if (strcmp(par[i].c_str(), "NONE"))
        bkgDataFile = par[i];
      if (verbose)
        cout << programName << "BKGDATAFILE set to " << bkgDataFile << endl;
    }
    // -----------------------------------------------------//
    else if (!strcmp(par[i].c_str(), "SAMPOSINFOFILE")) {
      if (++i >= npar) {
        DataCardUsage();
        return 1;
      }
      samPosInfoFile = par[i];
      if (verbose)
        cout << programName << "SAMPOSINFOFILE set to " << samPosInfoFile << endl;
    }
    // -----------------------------------------------------//
    else {
      cout << programName << "WARNING: unidentified input argument \"" << par[i] << "\" Ignored." << endl;
    }
    i++;
  }
//  } else {
//    cout << programName << "ERROR: Not a valid data card" << endl;
//    DataCardUsage();
//    return 1;
//  }

  return 0;
}

// ------------------------------------------------------------
int CheckInputs() {
  if (((strcmp(taskName.data(), "POSI") != 0) && (strcmp(taskName.data(), "UNCER") != 0))) {
    cout << programName << "ERROR: Invalid task name!" << endl;
    return 1;
  }

  if (!strcmp(inputFile.c_str(), "")) {
    cout << programName << "ERROR: Provide a valid input file or directory " << endl;
    return 1;
  }
  if (!strcmp(pixPosFile.c_str(), "")) {
    cout << programName << "ERROR: Provide a valid pixel position file!" << endl;
    return 1;
  }
  if (!strcmp(effAreaFile.c_str(), "")) {
    cout << programName << "ERROR: Provide a valid effective area file!" << endl;
    return 1;
  }
  if (!strcmp(temPosInfoFile.c_str(), "")) {
    cout << programName << "ERROR: Provide a valid template position information file!" << endl;
    return 1;
  }
  if (!strcmp(temDataFile.c_str(), "")) {
    cout << programName << "ERROR: Provide a valid template data file!" << endl;
    return 1;
  }
  if (!strcmp(taskName.c_str(), "UNCER"))
    if (!strcmp(samPosInfoFile.c_str(), "")) {
      cout << programName << "ERROR: Provide a valid sample position information file!" << endl;
      return 1;
    }

  return 0;
}

// ------------------------------------------------------------
void DumpGlobals() {
  cout << "[CELoc::DumpGlobals] ==============> Monte-Carlo globals used for this run <===============" << endl;
  cout << "[CELoc::DumpGlobals] dataCard        : " << dataCard << endl;
  cout << "[CELoc::DumpGlobals] taskName        : " << taskName << endl;
  cout << "[CELoc::DumpGlobals] inputFile       : " << inputFile << endl;
  cout << "[CELoc::DumpGlobals] outputFile      : " << outputFile << endl;
  cout << "[CELoc::DumpGlobals] pixPosFile      : " << pixPosFile << endl;
  cout << "[CELoc::DumpGlobals] effAreaFile     : " << effAreaFile << endl;
  cout << "[CELoc::DumpGlobals] temPosInfoFile  : " << temPosInfoFile << endl;
  cout << "[CELoc::DumpGlobals] temDataFile     : " << temDataFile << endl;
  cout << "[CELoc::DumpGlobals] bkgDataFile     : " << bkgDataFile << endl;
  cout << "[CELoc::DumpGlobals] samPosInfoFile  : " << samPosInfoFile << endl;
  cout << "[CELoc::DumpGlobals] verbose         : " << verbose << endl;
  cout << "[CELoc::DumpGlobals] ======================================================================" << endl;
}

// ------------------------------------------------------------
int main(int argc, char **argv) {

  if (HandleInputPar(argc, argv))
    return 0;

  if (!strcmp(dataCard.data(), "")) {
    usage();
    return 1;
  }

  if (ReadDataCard(dataCard))
    return 1;

  DumpGlobals();

  if (CheckInputs())
    return 1;

  CELocalization *loc = new CELocalization();

  // Set detector related information
  loc->SetPixPosFile(pixPosFile);
  loc->SetEffAreaFile(effAreaFile);

//  loc->SetPixPosFile("/home/ritabrata/Work/CrystalEye/Prod/Geom/upCal.txt");
//  loc->SetEffAreaFile("/home/ritabrata/Work/CrystalEye/Analysis/Localization/EffArea/allEffArea.root");
  // For geom V2R7
//  loc->SetPixPosFile("/home/ritabrata/Work/CrystalEye/Prod/Geom/V2R7/upCal.txt");
//  loc->SetEffAreaFile("/home/ritabrata/Work/CrystalEye/Analysis/Localization/V2R7/EffArea/allEffArea.root");

  // Set template data
  loc->SetTemPosInfoFile(temPosInfoFile);
  loc->SetTemDataFile(temDataFile);

//  loc->SetTemPosInfoFile("/home/ritabrata/Work/CrystalEye/Prod/Localization/srcpos-5deg.txt");
//  loc->SetTemDataFile("/home/ritabrata/Work/CrystalEye/Analysis/Localization/temEdepPix5deg.root");
//  loc->SetTemPosInfoFile("/home/ritabrata/Work/CrystalEye/Prod/Localization/srcpos-2deg.txt");
//  loc->SetTemDataFile("/home/ritabrata/Work/CrystalEye/Analysis/Localization/temEdepPix2deg.root");
  // For geom V2R7
//  loc->SetTemPosInfoFile("/home/ritabrata/Work/CrystalEye/Prod/Localization/V2R7/srcpos-5deg.txt");
//  loc->SetTemDataFile("/home/ritabrata/Work/CrystalEye/Analysis/Localization/V2R7/temEdepPix5deg.root");

  // Set background data
  if (strcmp(bkgDataFile.c_str(), ""))
    loc->SetBkgFile(bkgDataFile);

//  loc->SetBkgFile("/home/ritabrata/Work/CrystalEye/Analysis/Localization/all-bkg2.root");

  // Single reconstruction
  if (!strcmp(taskName.c_str(), "POSI")) {
    TVector3 vec = loc->CalcRecDirection(inputFile, outputFile);
    cout << "Centroid Theta/Phi from vector average = " << vec.Theta() * RadToDeg() << " deg " << vec.Phi() * RadToDeg()
        << " deg\n";
  }
  // Batch reconstruction
  else if (!strcmp(taskName.c_str(), "UNCER")) {
    loc->SetSamPosInfoFile(samPosInfoFile);

    //  loc->SetSamPosInfoFile("/home/ritabrata/Work/CrystalEye/Prod/Localization/srcpos-sam.txt");
    // For geom V2R7
    //  loc->SetSamPosInfoFile("/home/ritabrata/Work/CrystalEye/Prod/Localization/V2R7/srcpos-sam.txt");

    vector<pair<TVector3, TVector3>> allVec = loc->MultiRecDirection(inputFile);

    auto *hAngDev = new TH1F("hAngDev", ";Angle (deg); Entries", 900, 0, 90);
    auto *hAngTh = new TH2F("hAngTh", ";Theta (deg); Deviation (deg); Entries", 90, 0, 90, 900, 0, 90);

    for (auto v : allVec) {
      hAngDev->Fill(v.first.Angle(v.second) * RadToDeg());
      hAngTh->Fill(v.first.Theta() * RadToDeg(), v.first.Angle(v.second) * RadToDeg());

      //    if (v.first.Angle(v.second) * RadToDeg() < 0.1)
      //      cout << v.first.Angle(v.second) * RadToDeg() << " " << v.first.Theta() * RadToDeg() << " " << v.first.Phi() * RadToDeg() << endl;
    }

    // Write histograms
    auto ofile = new TFile(outputFile.data(), "RECREATE");
    ofile->cd();
    hAngDev->Write();
    hAngTh->Write();
    ofile->Close();
  } else
    cout << "[CELoc::main] Not a valid task...\n";

  delete loc;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
