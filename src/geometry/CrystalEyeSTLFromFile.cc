//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include <iostream>
#include <stdio.h> //FILE*
#include <filesystem>
#include <exception>

#include "geometry/CrystalEyeSTLFromFile.hh"

#include "G4TriangularFacet.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeSTLFromFile::CrystalEyeSTLFromFile(const char *fname, const float scaleFactor) {
  //check existance
  std::filesystem::path path = fname;
  if (!std::filesystem::exists(path)) {
    std::string tmpstr("STLFromFile::STLFromFile: ");
    tmpstr += path;
    tmpstr += " does not exist";
    throw std::runtime_error(tmpstr);
  }
  FILE *fd = fopen(fname, "rb");
  //skip header
  fseek(fd, 80, SEEK_SET);
  unsigned int chunk[1];
  fread(chunk, sizeof(unsigned int), 1, fd);
  //STL counter is littleendian
  nFaces = chunk[0];
  //std::cout<<"No of triangles: "<<nFaces<<std::endl;
  if (nFaces > STL_MAX_FACES) {
    std::cerr << "Too many triangles! " << nFaces << std::endl;
    std::cerr << fname << std::endl;
    throw std::out_of_range("STLFromFile::STLFromFile loading file");
  }
  //allocate containers for normals and vertexes
  normals = new float[nFaces * 3];
  vertexes = new float[nFaces * 9];

  //read in
  float fbuf[3];
  for (unsigned int iTr = 0; iTr < nFaces; iTr++) { //for triangles
    //read normal
    fread(fbuf, sizeof(float), 3, fd);
    //std::cout<<" N: "<<fbuf[0]<<" "<<fbuf[1]<<" "<<fbuf[2]<<std::endl;
    normals[iTr * 3] = fbuf[0];
    normals[iTr * 3 + 1] = fbuf[1];
    normals[iTr * 3 + 2] = fbuf[2];
    //read vertexes
    for (unsigned int iVtx = 0; iVtx < 3; iVtx++) {
      fread(fbuf, sizeof(float), 3, fd);
      //std::cout<<" V: "<<fbuf[0]<<" "<<fbuf[1]<<" "<<fbuf[2]<<std::endl;
      vertexes[iTr * 9 + iVtx * 3] = fbuf[0];
      vertexes[iTr * 9 + iVtx * 3 + 1] = fbuf[1];
      vertexes[iTr * 9 + iVtx * 3 + 2] = fbuf[2];
    } //for vertexes
    //read attribute
    fread(chunk, sizeof(char), 2, fd);
  } // for triangles
  Scale(scaleFactor);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyeSTLFromFile::~CrystalEyeSTLFromFile() {
  delete[] normals;
  delete[] vertexes;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
unsigned int CrystalEyeSTLFromFile::GetNFaces() {
  return nFaces;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CrystalEyeSTLFromFile::Report() {
  std::cout << "No of triangles: " << nFaces << std::endl;
  for (unsigned int iTr = 0; iTr < nFaces; iTr++) { //for triangles
    std::cout << " Nrm: " << normals[iTr * 3] << " " << normals[iTr * 3 + 1] << " " << normals[iTr * 3 + 2]
        << std::endl;
    for (unsigned int iVtx = 0; iVtx < 3; iVtx++) {
      std::cout << "  Vtx: " << vertexes[iTr * 9 + iVtx * 3] << " " << vertexes[iTr * 9 + iVtx * 3 + 1] << " "
          << vertexes[iTr * 9 + iVtx * 3 + 2] << std::endl;
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CrystalEyeSTLFromFile::Scale(const float scaleFactor) {
  for (unsigned int iTr = 0; iTr < nFaces; iTr++)
    for (unsigned int iVtx = 0; iVtx < 3; iVtx++)
      for (unsigned int j = 0; j < 3; j++)
        vertexes[iTr * 9 + iVtx * 3 + j] *= scaleFactor;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4ThreeVector CrystalEyeSTLFromFile::GetNormal(const unsigned int iFace) {
  if (iFace >= nFaces)
    throw std::out_of_range("CrystalEyeSTLFromFile::getNormal not that many faces");
  return G4ThreeVector(normals[iFace * 3], normals[iFace * 3 + 1], normals[iFace * 3] + 2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4ThreeVector CrystalEyeSTLFromFile::GetVertex(const unsigned int iFace, const unsigned int iVtx) {
  if (iFace >= nFaces)
    throw std::out_of_range("CrystalEyeSTLFromFile::getVertex not that many faces");
  if (iVtx >= 3)
    throw std::out_of_range("CrystalEyeSTLFromFile::getVertex only 3 vertexes in a triangle");

  return G4ThreeVector(vertexes[iFace * 9 + iVtx * 3], vertexes[iFace * 9 + iVtx * 3 + 1],
      vertexes[iFace * 9 + iVtx * 3 + 2]);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4TessellatedSolid* CrystalEyeSTLFromFile::CreateG4Solid(const G4String name, const bool testNormals) {
  G4TessellatedSolid *tessellated = new G4TessellatedSolid(name);
  G4TriangularFacet *f;
  for (unsigned int i = 0; i < nFaces; i++) {
    G4ThreeVector norm = GetNormal(i);
    G4ThreeVector v0 = GetVertex(i, 0);
    G4ThreeVector v1 = GetVertex(i, 1);
    G4ThreeVector v2 = GetVertex(i, 2);
    if (testNormals) {
      G4ThreeVector v01 = v1 - v0;
      G4ThreeVector v02 = v2 - v0;
      G4ThreeVector cp = v01.cross(v02);
      double dot = cp.dot(norm);
      if (dot < 0) {
        G4cout << "STLFromFile::createSolid: STL normal does not match ordering of vertexes. Reordering." << std::endl;
        //reorder
        f = new G4TriangularFacet(v2, v1, v0, ABSOLUTE);
      } else { //dot<0
        f = new G4TriangularFacet(v0, v1, v2, ABSOLUTE);
      } //dot<0
    } else { // testNormals
      f = new G4TriangularFacet(v0, v1, v2, ABSOLUTE);
    } // testNormals
    tessellated->AddFacet((G4VFacet*) f);
  } //for i in nFaces

  tessellated->SetSolidClosed(true);
  return tessellated;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
