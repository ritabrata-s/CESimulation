/********************************************************************************
 *
 * CEAna.cc
 *
 * Main function for the analysis
 *
 * Author: Ritabrata Sarkar
 * Created on: 14 Feb, 2024
 *
 ********************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "analysis/CEAnalysisAction.hh"

// ------------------------------------------------------------
void usage() {
  cout << "Usage: CEAna [options] \n";
  cout << "This is the CrystalEye main analysis program of the simulated data\n";
  cout << "Options:\n";
  cout << " -i, --input-file FILE        input filename (default: not set; concatenate multiple files with '+' )\n";
  cout << " -o, --output-file FILE       output filename (default: output.root)\n";
  cout << " -t, --task-name STRING       name of the task [options: EFFAREA, BKG_PHOT_P, BKG_PHOT_S, BKG_NEUT,\n";
  cout << "                              BKG_PROT_P, BKG_PROT_S, BKG_ELEC, BKG_POSI, BKG_INT_ALL, BKG_INT_BOT,\n";
  cout << "                              GRB_ALL, GRB_FIDU] (default: not set)\n";
  cout << " -per, --prim-eng-rng STRING  primary energy range and no. of bins [format: min_max_nbin]\n";
  cout << "                              (needed to be set for the tasks: EFFAREA, BKG_PHOT, BKG_NEUT, BKG_PROT,\n";
  cout << "                              BKG_ELEC, BKG_POSI, GRB)(default: 30_1E6_100)\n";
  cout << " -der, --depo-eng-rng STRING  deposition energy range and no. of bins [format: min_max_nbin]\n";
  cout << "                              (needed to be set for all the tasks) (default: 30_1E5_100)\n";
  cout << " -g, --geom SRING             geometry version of the detector [options: PIX1, PROTO, V1R0,\n";
  cout << "                              V2R0 ... V2R9](default: V1R0)\n";
  cout << " -tr, --trig SRING            trigger configuration for the detector [options: BT, TT](default: BT)\n";
  cout << " -v, --verbose                be verbose (default: false)\n";
  cout << " -h, --help                   display this help and exit\n";
  cout << "\nTo run CEAna the -i [filename] option MUST be set!\n";
  cout << "Report bugs to: ritabrata.sarkar@gssi.it \n";
}

// ------------------------------------------------------------
// Global variables
static char *inputFile = (char*) "";
static char *outputFile = (char*) "output.root";
static char *taskName = (char*) "";
static char *primEngRng = (char*) "30_1E6_100";
static float minEPrim = 30.;
static float maxEPrim = 1.E6;
static int nBinPrim = 100;
static char *depoEngRng = (char*) "30_1E5_100";
static float minEDepo = 30.;
static float maxEDepo = 1.E5;
static int nBinDepo = 100;
static string geomVer = "V1R0";
static string trig = "BT";
static bool verbose = false;

static const TString programName = "[CEAna] ";

// ------------------------------------------------------------
void ExtractEngRange(char *str, char type) {
  if (type == 'p')
    sscanf(str, "%f_%f_%d", &minEPrim, &maxEPrim, &nBinPrim);
  else if (type == 'd')
    sscanf(str, "%f_%f_%d", &minEDepo, &maxEDepo, &nBinDepo);
  else
    cout << "[CEAna::ExtractEngRange] Not a valid type...!!!\n";
}

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
      else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input-file")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        inputFile = argv[i];
        if (verbose)
          cout << programName << "inputFile set to " << inputFile << endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output-file")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        outputFile = argv[i];
        if (verbose)
          cout << programName << "outputFile set to " << outputFile << endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--task-name")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        taskName = argv[i];
        if (verbose)
          cout << programName << "taskName set to " << taskName << endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-per") || !strcmp(argv[i], "--prim-eng-rng")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        primEngRng = argv[i];
        ExtractEngRange(primEngRng, 'p');

        if (verbose)
          cout << programName << "primEngRng set to " << primEngRng << endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-der") || !strcmp(argv[i], "--depo-eng-rng")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        depoEngRng = argv[i];
        ExtractEngRange(depoEngRng, 'd');

        if (verbose)
          cout << programName << "depoEngRng set to " << depoEngRng << endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--geom")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        geomVer = argv[i];
        if (verbose)
          cout << programName << "geomVer set to " << geomVer << endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-tr") || !strcmp(argv[i], "--trig")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        trig = argv[i];
        if (verbose)
          cout << programName << "trig set to " << trig << endl;
      }
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
void DumpGlobals() {
  cout << "[CEAna::DumpGlobals] ==============> Monte-Carlo globals used for this run <===============" << endl;
  cout << "[CEAna::DumpGlobals] inputFile       : " << inputFile << endl;
  cout << "[CEAna::DumpGlobals] outputFile      : " << outputFile << endl;
  cout << "[CEAna::DumpGlobals] taskName        : " << taskName << endl;
  cout << "[CEAna::DumpGlobals] primEngRng      : " << primEngRng << endl;
  cout << "[CEAna::DumpGlobals] depoEngRng      : " << depoEngRng << endl;
  cout << "[CEAna::DumpGlobals] geomVer         : " << geomVer << endl;
  cout << "[CEAna::DumpGlobals] trig            : " << trig << endl;
  cout << "[CEAna::DumpGlobals] verbose         : " << verbose << endl;
  cout << "[CEAna::DumpGlobals] ======================================================================" << endl;
}

// ------------------------------------------------------------
int CheckInputPar() {

  if (!strcmp(inputFile, "")) {
    cout << "[CEAna:CheckInputPar] No input file supplied!\n";
    return 1;
  }

  if (!strcmp(taskName, "")) {
    cout << "[CEAna:CheckInputPar] No task scheduled!\n";
    return 1;
  }

  return 0;
}

// ------------------------------------------------------------
int main(int argc, char **argv) {

  if (HandleInputPar(argc, argv))
    return 0;

  if (CheckInputPar()) {
    usage();
    return 1;
  }

  DumpGlobals();

  CEAnalysisAction *analyse = new CEAnalysisAction();
  analyse->SetInputFileName(inputFile);
  analyse->SetOutputFileName(outputFile);
  analyse->SetGeomVersion(geomVer);
  analyse->SetTrigConfig(trig);

  analyse->BeginOfAnalysis();

  analyse->SetPrimEngBin(nBinPrim, minEPrim, maxEPrim);
  analyse->SetDepEngBin(nBinDepo, minEDepo, maxEDepo);

// Effective Area (gamma)
  if (!strcmp(taskName, "EFFAREA"))
    analyse->CalcEffArea();
//  analyse->SetPrimEngBin(100, 30., 1.E5);
//  analyse->SetDepEngBin(100, 30., 1.E5);
// type: particle type [gamma, proton, neutron, electron, positron]; dir: 0 (all), 1 (downward), 2 (upward)
// Gamma
  else if (!strcmp(taskName, "BKG_PHOT_P"))
    analyse->AnalyzeBkg("gamma", 1);
  else if (!strcmp(taskName, "BKG_PHOT_S"))
    analyse->AnalyzeBkg("gamma", 2);
// Neutron
  else if (!strcmp(taskName, "BKG_NEUT"))
    analyse->AnalyzeBkg("neutron", 2);
//  analyse->SetPrimEngBin(100, 30., 1.E7);
//  analyse->SetDepEngBin(100, 30., 1.E5);
// Electron
  else if (!strcmp(taskName, "BKG_ELEC"))
    analyse->AnalyzeBkg("electron", 0);
////  analyse->SetPrimEngBin(100, 10., 1.E7);
//  analyse->SetPrimEngBin(100, 30., 1.E6);
//  analyse->SetDepEngBin(100, 30., 1.E5);
// Positron
  else if (!strcmp(taskName, "BKG_POSI"))
    analyse->AnalyzeBkg("positron", 0);
//  analyse->SetPrimEngBin(100, 30., 1.E6);
//  analyse->SetDepEngBin(100, 30., 1.E5);
// Proton
  else if (!strcmp(taskName, "BKG_PROT_S"))
    analyse->AnalyzeBkg("proton", 0);
  else if (!strcmp(taskName, "BKG_PROT_P"))
    analyse->AnalyzeBkg("proton", 1);
//  analyse->SetPrimEngBin(100, 30., 1.E6);
//  analyse->SetPrimEngBin(100, 4.E6, 1.E8);
//  analyse->SetDepEngBin(100, 30., 1.E5);
// Intrinsic bkg
  else if (!strcmp(taskName, "BKG_INT_ALL"))
    analyse->AnaIntrinsic();
  else if (!strcmp(taskName, "BKG_INT_BOT"))
    analyse->AnaIntrinsic(false);
//  Float_t rate = 6685.484; // 1 pixel
//  Float_t rate = 20056.452; // prototype
//  Float_t rate = 118227.20; // lower pixels
//  analyse->SetDepEngBin(100, 30., 1.E5);
//  analyse->AnaIntrinsic();
//  analyse->AnaIntrinsic(rate);
// GRB
  else if (!strcmp(taskName, "GRB_ALL"))
    analyse->AnalyzeGRB();
  else if (!strcmp(taskName, "GRB_FIDU"))
    analyse->AnalyzeFiducialGRB();
//  analyse->SetPrimEngBin(100, 30., 1.E5);
//  analyse->SetDepEngBin(100, 30., 1.E5);
  else
    cout << "[CEAna:main] task name is undefined!...\n";

// End of analysis
  analyse->EndOfAnalysis();

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
