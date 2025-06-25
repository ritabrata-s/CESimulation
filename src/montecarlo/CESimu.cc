// **************************************************************
//
//      ------ CESimu.cc  ------
//      Author: Ritabrata Sarkar
//      Date: 09 Feb, 2024
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4RunManagerFactory.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "geometry/CrystalEyeDetectorConstruction.hh"
#include "geometry/CrystalEyeDetectorConstructionV2.hh"
#include "geometry/CrystalEyePrototypeConstruction.hh"
#include "montecarlo/CrystalEyePhysicsList.hh"
#include "montecarlo/CrystalEyeActionInitialization.hh"
#include "montecarlo/CrystalEyeAnalysis.hh"

#include "G4PhysListFactory.hh"
//#include "QGSP_BIC.hh"
#include "QGSP_BERT.hh"
//#include "FTFP_BERT.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4DecayPhysics.hh"

#include "G4GDMLParser.hh"

using namespace std;

// ------------------------------------------------------------
void usage() {
  G4cout << "Usage: CESimu [options] \n";
  G4cout << "This is the CrystalEye main Monte-Carlo simulation program\n";
  G4cout << "Options:\n";
  G4cout << " -X, --interactive            interactive mode [default: false]\n";
  G4cout << " -m, --mac-file FILE          macro filename [default: not set]\n";
  G4cout << " -pm, --phymac-file FILE      physicslist macro filename [default: physics.mac]\n";
  G4cout << " -o, --output-file FILE       output filename (default: test.root)\n"; // (may be overridden in datacard for each action)\n";
  G4cout << " --seed1 INT                  first random seed [default: -1, random seed from time stamp]\n"; // do not put default 0!
  G4cout << " --seed2 INT                  second random seed [default: -1]\n";
  G4cout << " -v, --verbose                be verbose [default: false]\n";
  G4cout << " -h, --help                   display this help and exit\n";
  G4cout << "\nTo run CESimu either the -X (interactive) or -m (macro file) option MUST be set!\n";
//  G4cout << "\nTo run G4Zire either the -d (data cards) or the -X (interactive) option MUST be set!\n";
  G4cout << "Report bugs to: ritabrata.sarkar@gssi.it \n";
}

// ------------------------------------------------------------
// Global variables
G4bool interactiveMode = false;
char *macFile = (char*) "";
char *phymacFile = (char*) ""; // default name "physics.mac" set in the main function
char *outputFile = (char*) "test.root";
G4long seed1 = -1;
G4long seed2 = -1;
G4bool verbose = false;

const G4String programName = "[CESimu] ";

// ------------------------------------------------------------
void DumpGlobals() {
  G4cout << "[CESimu::DumpGlobals] ==============> Monte-Carlo globals used for this run <===============" << G4endl;
  G4cout << "[CESimu::DumpGlobals] interactiveMode : " << interactiveMode << G4endl;
  G4cout << "[CESimu::DumpGlobals] macFile         : " << macFile << G4endl;
  G4cout << "[CESimu::DumpGlobals] phymacFile      : " << phymacFile << G4endl;
  G4cout << "[CESimu::DumpGlobals] outputFile      : " << outputFile << G4endl;
  G4cout << "[CESimu::DumpGlobals] seed1           : " << seed1 << G4endl;
  G4cout << "[CESimu::DumpGlobals] seed2           : " << seed2 << G4endl;
  G4cout << "[CESimu::DumpGlobals] verbose         : " << verbose << G4endl;
  G4cout << "[CESimu::DumpGlobals] ======================================================================" << G4endl;
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
      else if (!strcmp(argv[i], "-X") || !strcmp(argv[i], "--interactive")) {
        interactiveMode = true;
        if (verbose)
          G4cout << programName << "interactive mode is set " << G4endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--mac-file")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        macFile = argv[i];
        if (verbose)
          G4cout << programName << "macFile set to " << macFile << G4endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-pm") || !strcmp(argv[i], "--phymac-file")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        phymacFile = argv[i];
        if (verbose)
          G4cout << programName << "phymacFile set to " << phymacFile << G4endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output-file")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        outputFile = argv[i];
        if (verbose)
          G4cout << programName << "outputFile set to " << outputFile << G4endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "--seed1")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        seed1 = atoi(argv[i]);
        if (verbose)
          G4cout << programName << "Random seed1 set to " << seed1 << G4endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "--seed2")) {
        if (++i >= argc) {
          usage();
          return 1;
        }
        seed2 = atoi(argv[i]);
        if (verbose)
          G4cout << programName << "Random seed2 set to " << seed2 << G4endl;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
        verbose = true;
        G4cout << programName << "verbose flag is set " << G4endl;
      }
      // -----------------------------------------------------//
      else {
        if (verbose)
          G4cout << programName << "WARNING: unidentified input argument \"" << argv[i] << "\" Ignored." << G4endl;
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
int main(int argc, char **argv) {

  if (HandleInputPar(argc, argv))
    return 0;

  if (!strcmp(macFile, "") && !interactiveMode) {
    usage();
    return 1;
  }

  DumpGlobals();

  //---  Choose the random engine  ----
  G4Random::setTheEngine(new CLHEP::RanecuEngine); //MTwistEngine
  if (seed1 < 0) {
    G4long fRandomSeed = time(0) + getpid();
    CLHEP::HepRandom::setTheSeed(fRandomSeed);
  } else {
    G4long fRandomSeed[2] = { seed1, seed2 };
    CLHEP::HepRandom::setTheSeeds(fRandomSeed, 0);
  }
  CLHEP::HepRandom::showEngineStatus();

  //---  Construct the default run manager  ---
//  auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly); //SerialOnly
  auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
//  G4GDMLParser parser;

  // Set mandatory initialization classes
  //
  runManager->SetUserInitialization(new CrystalEyeDetectorConstruction);
//  runManager->SetUserInitialization(new CrystalEyeDetectorConstructionV2);
//  runManager->SetUserInitialization(new CrystalEyePrototypeConstruction);

  // For 176Lu intrinsic bkg simulation
  runManager->SetUserInitialization(new CrystalEyePhysicsList);

  // For general simulation
//  G4PhysListFactory *physListFactory = new G4PhysListFactory();
//  G4VUserPhysicsList *physicsList = physListFactory->GetReferencePhysList("QGSP_BERT");
//  runManager->SetUserInitialization(physicsList);

  runManager->SetUserInitialization(new CrystalEyeActionInitialization(outputFile));

  // Initialize G4 kernel
  // Initialization of the run is done in the macro file
  runManager->Initialize();

  // Initialize visualization
  G4VisManager *visManager = new G4VisExecutive;
  visManager->Initialize();

  //---  Get the pointer to the UI manager  ---
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  G4String macDir = std::getenv("CRYSTALEYE_MACRO");
  G4String command = "/control/execute ";
  G4String visMac = macDir + "vis.mac";
  G4String phyMac = macDir + "physics.mac";
  if (strcmp(phymacFile, ""))
    phyMac = phymacFile;

  if (!interactiveMode) { // batch mode
//    UImanager->ApplyCommand(command + phyMac);
    UImanager->ApplyCommand(command + macFile);

  } else { // interactive mode
    G4UIExecutive *ui = new G4UIExecutive(argc, argv);
    if (ui->IsGUI()) {
//      UImanager->ApplyCommand(command + phyMac);
      UImanager->ApplyCommand(command + visMac);

      ui->SessionStart();
    }
    delete ui;
  }

//  parser.SetRegionExport(true);
//  parser.Write("geometry.gdml",
//      G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume());

  delete visManager;
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
