#ifndef EpicGeant4_h
#define EpicGeant4_h 1

// C++ header
#include <map>
#include <string>
#include <vector>
using namespace std;

// G4 headers
#include "G4LogicalVolume.hh"
#include "G4AssemblyVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

// NPTool header
#include "NPG4VDetector.h"
#include "NPInputParser.h"
#include "EpicData.h"
#include "EpicDetector.h"


// TODO TODO TODO to clean and write it for the EPIC fission chamber only !!!!
namespace epic {
  class EpicGeant4 : public nptool::geant4::VDetector {
    ////////////////////////////////////////////////////
    /////// Default Constructor and Destructor /////////
    ////////////////////////////////////////////////////
   public:
    EpicGeant4();
    virtual ~EpicGeant4();

    ////////////////////////////////////////////////////
    //////// Specific Function of this Class ///////////
    ////////////////////////////////////////////////////
   public:
    G4LogicalVolume* BuildModule();
    G4LogicalVolume* BuildVeto();

   private:
    std::map<unsigned int, G4ThreeVector> m_PositionBar;
    std::map<unsigned int, bool> m_IsVetoBar;
    G4LogicalVolume* m_Module;
    G4LogicalVolume* m_Veto;
    double Energy;
    double Light;

    ////////////////////////////////////////////////////
    //////  Inherite from NPS::VDetector class /////////
    ////////////////////////////////////////////////////
   public:
    // Construct detector and inialise sensitive part.
    // Called After DetecorConstruction::AddDetector Method
    void ConstructDetector();

    // Read sensitive part and fill the Root tree.
    // Called at in the EventAction::EndOfEventAvtion
    void ReadSensitive(const G4Event* event);

   public: // Scorer
    //   Initialize all Scorer used by the MUST2Array
    void InitializeScorers();

    //   Associated Scorer
    //    G4MultiFunctionalDetector* m_ModuleScorer;
    //   G4MultiFunctionalDetector* m_VetoScorer;
    ////////////////////////////////////////////////////
    ///////////Event class to store Data////////////////
    ////////////////////////////////////////////////////
   private:
    epic::EpicData* m_Event;

    ////////////////////////////////////////////////////
    ///////////////Private intern Data//////////////////
    ////////////////////////////////////////////////////
   private:
    std::shared_ptr<epic::EpicDetector> m_detector;

   private: // Geometry
    // Detector Coordinate
    vector<G4ThreeVector> m_Pos;
    vector<int> m_NbrModule;
    int m_TotalModule = 0;
    vector<bool> m_HasVeto;
    vector<bool> m_HasFrame;

    // Visualisation Attribute
    G4VisAttributes* m_VisModule;
    G4VisAttributes* m_VisVeto;
    G4VisAttributes* m_VisPMT;
    G4VisAttributes* m_VisFrame;
    G4VisAttributes* m_VisAl;
    G4VisAttributes* m_VisInox;
    G4VisAttributes* m_VisEJ309;
    G4VisAttributes* m_VisMuMetal;
    G4VisAttributes* m_VisPyrex;
    G4VisAttributes* m_VisEJ560;

    G4Material* m_Vacuum;
    G4Material* m_Al;
    G4Material* m_Inox;
    G4Material* m_EJ309;
    G4Material* m_EJ560;
    G4Material* m_Pyrex;
    G4Material* m_MuMetal;

    G4AssemblyVolume* m_EpicDetector;
    G4LogicalVolume* m_SensitiveCell;
    G4AssemblyVolume* m_MecanicalStructure;
    G4LogicalVolume* m_MecanicalStructure_Al;
    G4LogicalVolume* m_MecanicalStructure_Steel;

  };
} // namespace epic
#endif

