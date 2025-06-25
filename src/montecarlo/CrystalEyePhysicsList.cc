// **************************************************************
//
//      ------ CrystalEyePhysicsList.cc  ------
//           Modified by Libo Wu (23 Sep 2021)
//           Email: libo.wu@gssi.it
//
// **************************************************************

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include "montecarlo/CrystalEyePhysicsList.hh"
#include "montecarlo/CrystalEyePhysicsListMessenger.hh"

#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"

#include "G4Decay.hh"
#include "G4StepLimiter.hh"
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ProcessManager.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
//#include "G4EmProcessOptions.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLivermorePolarizedPhysics.hh" // TBC

#include "G4HadronElasticPhysics.hh"
#include "G4HadronDElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4IonElasticPhysics.hh"

#include "G4PhysListUtil.hh"
#include "G4NuclideTable.hh"
#include "G4DeexPrecoParameters.hh"
#include "G4NuclearLevelData.hh"

#include "G4Radioactivation.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4EmBuilder.hh"
#include "G4HadronicParameters.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
CrystalEyePhysicsList::CrystalEyePhysicsList() :
    G4VModularPhysicsList() {

  pMessenger = new CrystalEyePhysicsListMessenger(this);

  verboseLevel = 1;
  SetVerboseLevel (verboseLevel);

  defaultCutValue = 100 * micrometer;
  cutForGamma = defaultCutValue;
  cutForElectron = defaultCutValue;
  cutForPositron = defaultCutValue;

  // set default cut in range value
  //
  SetDefaultCutValue (defaultCutValue);

  // Parameters
  // Mandatory for G4NuclideTable
  // Half-life threshold must be set small or many short-lived isomers
  // will not be assigned life times (default to 0)
  G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(1 * picosecond);
  G4NuclideTable::GetInstance()->SetLevelTolerance(1.0 * eV);

  // EM physics
  // define flags for the atomic de-excitation module
  G4EmParameters *param = G4EmParameters::Instance();
  param->SetDefaults();
  param->SetAugerCascade(true);
  param->SetStepFunction(1., 1 * CLHEP::mm);
  param->SetStepFunctionMuHad(1., 1 * CLHEP::mm);
  param->SetDeexcitationIgnoreCut(true);

  // Radioactive decay
  G4DeexPrecoParameters *deex = G4NuclearLevelData::GetInstance()->GetParameters();
  deex->SetCorrelatedGamma(false);
  deex->SetStoreAllLevels(true);
  deex->SetStoreICLevelData(true);
  deex->SetInternalConversionFlag(true);
  deex->SetIsomerProduction(true);
  deex->SetMaxLifeTime(G4NuclideTable::GetInstance()->GetThresholdOfHalfLife() / std::log(2.));

  // Instantiate Physics List infrastructure
  //
  G4PhysListUtil::InitialiseParameters();

  G4HadronicParameters::Instance()->SetTimeThresholdForRadioactiveDecay(1.0e+60 * year);

}

// ------------------------------------------------------------
CrystalEyePhysicsList::~CrystalEyePhysicsList() {
  delete pMessenger;

  delete fDecaPhys;
  for (size_t i = 0; i < fElecPhys.size(); i++) {
    delete fElecPhys[i];
  }
  for (size_t i = 0; i < fHadrPhys.size(); i++) {
    delete fHadrPhys[i];
  }

}

// ------------------------------------------------------------
void CrystalEyePhysicsList::AddPackage(const G4String &name) {
  //---   For PhysicsListMessenger  ----
  G4PhysListFactory factory;
  G4VModularPhysicsList *phys = factory.GetReferencePhysList(name);
  G4int i = 0;
  const G4VPhysicsConstructor *elem = phys->GetPhysics(i);
  G4VPhysicsConstructor *tmp = const_cast<G4VPhysicsConstructor*>(elem);
  while (elem != 0) {
    RegisterPhysics(tmp);
    elem = phys->GetPhysics(++i);
    tmp = const_cast<G4VPhysicsConstructor*>(elem);
  }
}

// ------------------------------------------------------------
void CrystalEyePhysicsList::ConstructParticle() {
  G4BosonConstructor pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();

//  G4GenericIon::GenericIon();
}

// ------------------------------------------------------------
void CrystalEyePhysicsList::ConstructProcess() {
  // transportation
  AddTransportation();

  // EM physics
  G4VPhysicsConstructor *fEmPhys = new G4EmStandardPhysics();
  fEmPhys->ConstructProcess();

  // decay physics list
  fDecaPhys = new G4DecayPhysics();
  fDecaPhys->ConstructProcess();

  // electromagnetic physics list
//  for (size_t i = 0; i < fElecPhys.size(); i++) {
//    fElecPhys[i]->ConstructProcess();
//    em_config.AddModels();
//  }

  // hadronic physics lists
//  for (size_t i = 0; i < fHadrPhys.size(); i++) {
//    fHadrPhys[i]->ConstructProcess();
//  }

  // Radioactive decay
  G4Radioactivation *radioactiveDecay = new G4Radioactivation();

  G4bool ARMflag = false;
  radioactiveDecay->SetARM(ARMflag);        //Atomic Rearangement

  G4LossTableManager *man = G4LossTableManager::Instance();
  G4VAtomDeexcitation *deex = man->AtomDeexcitation();
  if (!deex) {
    deex = new G4UAtomicDeexcitation();
    man->SetAtomDeexcitation(deex);
  }
  deex->InitialiseAtomicDeexcitation();

  // register radioactiveDecay
  //
  G4PhysicsListHelper *ph = G4PhysicsListHelper::GetPhysicsListHelper();
  ph->RegisterProcess(radioactiveDecay, G4GenericIon::GenericIon());

  // Hadron Elastic scattering
  G4VPhysicsConstructor *fHadElPhys = new G4HadronElasticPhysics();
  fHadElPhys->ConstructProcess();

  // Hadron Inelastic physics
  G4VPhysicsConstructor *fHadInElPhys = new G4HadronPhysicsFTFP_BERT();
  fHadInElPhys->ConstructProcess();
  ////RegisterPhysics( new G4HadronInelasticQBBC(verboseLevel));
  ////RegisterPhysics( new G4HadronPhysicsINCLXX(verboseLevel));

  // Ion Elastic scattering
  G4VPhysicsConstructor *fIonElPhys = new G4IonElasticPhysics();
  fIonElPhys->ConstructProcess();

  // Ion Inelastic physics
  G4VPhysicsConstructor *fIonInElPhys = new G4IonPhysics();
  fIonInElPhys->ConstructProcess();
  ////RegisterPhysics( new G4IonINCLXXPhysics(verb));

  // Gamma-Nuclear Physics
  G4EmExtraPhysics *gnuc = new G4EmExtraPhysics();
  gnuc->ElectroNuclear(false);
  gnuc->MuonNuclear(false);
  gnuc->ConstructProcess();

}

// ------------------------------------------------------------
void CrystalEyePhysicsList::AddPhysicsList(const G4String &name) {

  if (verboseLevel > 1) {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }

  //---  ELECTROMAGNETIC MODELS  ---
  if (name == "Em_Standard_opt3") {
    fElecPhys.push_back(new G4EmStandardPhysics_option3(verboseLevel));
    G4cout << ">>> PhysicsList:: G4EmStandardPhysics_option3 is activated! <<<" << G4endl;
  } else if (name == "Em_Livermore") {
    fElecPhys.push_back(new G4EmLivermorePhysics(verboseLevel));
    G4cout << ">>> PhysicsList:: G4EmLivermorePhysics is activated! <<<" << G4endl;
  } else if (name == "Em_Penelope") {
    fElecPhys.push_back(new G4EmPenelopePhysics(verboseLevel));
    G4cout << ">>> PhysicsList:: G4EmPenelopePhysics is activated! <<<" << G4endl;
  } else if (name == "Em_Polarized") {
    fElecPhys.push_back(new G4EmLivermorePolarizedPhysics(verboseLevel));
    G4cout << ">>> PhysicsList:: G4EmPenelopePhysics is activated! <<<" << G4endl;
  }

  //---  HADRONIC MODELS  ---
  //Hadron Elastic Physics List
  else if (name == "Hd_Elastic") {
    fHadrPhys.push_back(new G4HadronElasticPhysics(verboseLevel));
    G4cout << ">>> PhysicsList:: G4HadronElasticPhysics is activated! <<<" << G4endl;
  } else if (name == "Hd_DElastic") {
    fHadrPhys.push_back(new G4HadronDElasticPhysics(verboseLevel));
    G4cout << ">>> PhysicsList:: G4HadronDElasticPhysics is activated! <<<" << G4endl;
  } else if (name == "Hd_ElasticHP") {
    fHadrPhys.push_back(new G4HadronElasticPhysicsHP(verboseLevel));
    G4cout << ">>> PhysicsList:: G4HadronElasticPhysicsHP is activated! <<<" << G4endl;
  } else if (name == "QGSP_BIC_HP") {
    fHadrPhys.push_back(new G4HadronPhysicsQGSP_BIC_HP(verboseLevel));
    G4cout << ">>> PhysicsList:: G4HadronPhysicsQGSP_BIC_HP is activated! <<<" << G4endl;
  } else if (name == "Hd_IonBinary") {
    fHadrPhys.push_back(new G4IonBinaryCascadePhysics(verboseLevel));
    G4cout << ">>> PhysicsList:: G4IonBinaryCascadePhysics is activated! <<<" << G4endl;
  } else if (name == "Hd_RadioactiveDecay") {
    fHadrPhys.push_back(new G4RadioactiveDecayPhysics(verboseLevel));
    G4cout << ">>> PhysicsList:: G4RadioactiveDecayPhysics is activated! <<<" << G4endl;
  } else {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << " is not defined!  The choice list:"
        << " Em_Standard_opt3, Em_Livermore, Em_Penelope, Em_Polarized,"
        << " Hd_Elastic, Hd_DElastic, Hd_ElasticHP, Hd_IonBinary, Hd_RadioactiveDecay, QGSP_BIC_HP" << "  \n" << G4endl;

    exit(0);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CrystalEyePhysicsList::SetCuts() {

  if (verboseLevel > 0) {
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue, "Length") << G4endl;
  }

  G4double lowLimit = 250. * eV;
  G4double highLimit = 100. * GeV;
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");

  if (verboseLevel > 0)
    DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CrystalEyePhysicsList::SetCutForGamma(G4double cut) {
  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CrystalEyePhysicsList::SetCutForElectron(G4double cut) {
  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void CrystalEyePhysicsList::SetCutForPositron(G4double cut) {
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
