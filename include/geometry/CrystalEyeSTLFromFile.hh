/*****************************************************************************
 *
 * CrystalEyeSTLFromFile.hh
 *
 * Implemented by: Ritabrata Sarkar (09 Feb 2024)
 *
 *****************************************************************************/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#ifndef __STLFromFile__
#define __STLFromFile__

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "G4ThreeVector.hh"
#include "G4TessellatedSolid.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#define STL_MAX_FACES 65535

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class CrystalEyeSTLFromFile {
  unsigned int nFaces;
  float *normals;
  float *vertexes;

public:
  CrystalEyeSTLFromFile(const char *fname, const float scaleFactor = 1.0);
  ~CrystalEyeSTLFromFile();

  unsigned int GetNFaces();
  void Report();
  void Scale(const float scaleFactor);
  G4ThreeVector GetNormal(const unsigned int iFace);
  G4ThreeVector GetVertex(const unsigned int iFace, const unsigned int iVtx);
  G4TessellatedSolid* CreateG4Solid(const G4String name, const bool testNormals = false);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif //__STLFromFile__

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
