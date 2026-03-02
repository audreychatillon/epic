// C++ headers
#include <cmath>
#include <limits>
#include <sstream>
// G4 Geometry object
#include "G4Box.hh"
#include "G4Tubs.hh"

// G4 sensitive
#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"

// G4 various object
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"

// NPTool header
// #include "InteractionScorers.hh"
// #include "NPOptionManager.h"
// #include "NPSDetectorFactory.hh"
// #include "NPSHitsMap.hh"
// #include "EpicGeant4.hh"
// s#include "PlasticBar.hh"
// #include "ProcessScorers.hh"
// CLHEP header
#include "CLHEP/Random/RandGauss.h"

#include "NPApplication.h"
#include "NPFunction.h"
#include "NPG4MaterialManager.h"
#include "NPG4Session.h"
#include "EpicGeant4.h"
using namespace std;
using namespace CLHEP;
using namespace epic;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace epic {
 const string Material = "BC400";
  
    // Energy and time Resolution
  const double EnergyThreshold = 0.1*MeV;
  /* const double ResoTime = 0.454*ns ; // reso Cf */
  const double ResoTime = 0.61*ns ; // reso U8
  /* const double ResoTime = 0.*ns ; // reso U8 */
  const double ResoEnergy = 0.1*MeV ;
  const double Thickness = 51.*mm ;
  const double Radius = 127./2*mm ;
  
  ////////////////////////////////////////////////////////////////////////////////
  // Nebula Specific Method
  EpicGeant4::EpicGeant4() {
    nptool::message("green", "epic", "EpicGeant4", "Initializing simulation");
    m_detector = std::dynamic_pointer_cast<epic::EpicDetector>(
        nptool::Application::GetApplication()->GetDetector("epic"));
        
  m_Event = new epic::EpicData() ;

  // RGB Color + Transparency
  m_VisAl      = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));   
  m_VisEJ309   = new G4VisAttributes(G4Colour(0.2, 0.85, 0.85, 1));   
  m_VisMuMetal = new G4VisAttributes(G4Colour(0.55, 0.5, 0.5, 0.7));   
  m_VisPyrex   = new G4VisAttributes(G4Colour(0.1, 0.5, 0.7, 1));   
  m_VisEJ560   = new G4VisAttributes(G4Colour(0.6, 0.6, 0.2, 1));   
  m_VisInox    = new G4VisAttributes(G4Colour(0.6, 0.5, 0.6, 1));   

  // Material definition
  //m_Vacuum  = MaterialManager::getInstance()->GetMaterialFromLibrary("Vacuum");
 // m_Al      = MaterialManager::getInstance()->GetMaterialFromLibrary("Al");
  //m_Inox    = MaterialManager::getInstance()->GetMaterialFromLibrary("Inox");
  //m_EJ309   = MaterialManager::getInstance()->GetMaterialFromLibrary("EJ309");
  //m_EJ560   = MaterialManager::getInstance()->GetMaerialFromLibrary("EJ560");
  //m_Pyrex   = MaterialManager::getInstance()->GetMaterialFromLibrary("Pyrex");
  //m_MuMetal = MaterialManager::getInstance()->GetMaterialFromLibrary("mumetal");
  }
  ////////////////////////////////////////////////////////////////////////////////
  EpicGeant4::~EpicGeant4() {}
  ////////////////////////////////////////////////////////////////////////////////
/*
G4LogicalVolume* Epic::BuildSensitiveCell(){

  if(!m_SensitiveCell){
    G4Tubs* scin_cell = new G4Tubs("Epic_scin", 0, Epic_NS::Radius, Epic_NS::Thickness*0.5, 0., 360*deg);
    m_SensitiveCell = new G4LogicalVolume(scin_cell,m_EJ309,"logic_Epic_scin",0,0,0);
    m_SensitiveCell->SetVisAttributes(m_VisEJ309);
   // m_SensitiveCell->SetSensitiveDetector(m_EpicScorer);
  }

  return m_SensitiveCell;
  }
  */
  // Construct detector and inialise sensitive part.
  // Called After DetecorConstruction::AddDetector Method
  void EpicGeant4::ConstructDetector() {
    nptool::message("green", "epic", "EpicGeant4", "Constructing Geometry");

    // FIXME add dummy wall until use of input file is made
    auto g4session = nptool::geant4::Session::GetSession();
    auto world = g4session->GetWorldLogicalVolume();

  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  // Read sensitive part and fill the Root tree.
  // Called at in the EventAction::EndOfEventAvtion
  void EpicGeant4::ReadSensitive(const G4Event*) {
    m_Event->Clear();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  ////////////////////////////////////////////////////////////////
  void EpicGeant4::InitializeScorers() {

  }
  ////////////////////////////////////////////////////////////////////////////////
  extern "C" {
    std::shared_ptr<nptool::geant4::VDetector> ConstructDetectorSimulation() {
      return make_shared<epic::EpicGeant4>();
    };
  }
} // namespace epic
