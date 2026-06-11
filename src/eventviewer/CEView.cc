/***********************************************************************************
 *
 *      CEView.cc
 *
 * Main function for the event display utility for the Crystal Eye simulation and
 * analysis software.
 *
 * Created on: 23 February, 2024
 *
 * Author: Ritabrata Sarkar
 *
 ***********************************************************************************/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <iostream>

#include <TApplication.h>

#include "eventviewer/CEEventViewer.hh"

using namespace std;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Usage() {

  cout << "Usage: ./CEView -f filename [options]" << endl;
  cout << "Options:\n";
  cout << " -f, --file NAME            name of the file to be analysed. \n";
  cout << " -g, --geom NAME            detector geometry version. [Options: V1R0, V2R8; (default: V1R0)]\n";
  cout << " -dl, --detector-list       detectors to include(+). e.g., +CAL (default: +ACD+CAL). \n";
  cout << " -v, --verbose              verbose level [default: 0]. \n";
  cout << " -h, --help                 print this help. \n";
  cout << "Report bugs to: ritabrata.rarkar@gssi.it \n";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define globals
char *fileName = (char*) ".root";
char *geom = (char*) "V1R0";
char *detectorFlag = (char*) "+ACD+CAL";
Int_t verbose = 0;
TString progName = "CEView";

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int HandleInputPar(int argc, char **argv) {

  if (argc > 1) {

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
      Usage();
      return (1);
    }

    // -----------------------
    // Read input parameters
    // -----------------------
    cout << endl;
    for (int i = 1; i < argc; i++) {
      // -----------------------------------------------------//
      if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
        if (++i >= argc)
          throw -1;
        verbose = atoi(argv[i]);
        cout << "[" << progName << "::HandleInputPar] Verbose level is set to: " << verbose << endl;
        continue;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file")) {
        if (++i >= argc)
          throw -1;
        fileName = argv[i];
        cout << "[" << progName << "::HandleInputPar] Input file name: " << fileName << endl;
        continue;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--geom")) {
        if (++i >= argc)
          throw -1;
        geom = argv[i];
        cout << "[" << progName << "::HandleInputPar] Detector geometry version: " << geom << endl;
        continue;
      }
      // -----------------------------------------------------//
      else if (!strcmp(argv[i], "-dl") || !strcmp(argv[i], "--detector-list")) {
        if (++i >= argc)
          throw -1;
        detectorFlag = argv[i];
        cout << "[" << progName << "::HandleInputPar] Detector Flag set to: " << detectorFlag << endl;
        continue;
      }
      // -----------------------------------------------------//
      else {
        cout << "[" << progName << "::HandleInputPar] Unidentified input parameter." << argv[i] << " Ignored." << endl;
      }
    }
    cout << endl;
  } else {
    Usage();
    return 1;
  }

  // -----------------------
  // Check input parameters
  // -----------------------
  if (!strcmp(fileName, "")) {
    Usage();
    return 1;
  }

  return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char **argv) {

  if (HandleInputPar(argc, argv))
    return 1;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (verbose > 0)
    cout << "[" << progName << "::main] In event display....." << endl;

  TApplication theApp("CEEventViewer", &argc, argv);

  // The event viewer
  CEEventViewer *eview = new CEEventViewer(fileName, geom);
  eview->SetDetectors(detectorFlag);

  // Start the viewer
  eview->Init();

  theApp.Run();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
