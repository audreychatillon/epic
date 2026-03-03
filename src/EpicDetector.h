#ifndef EpicDetector_h
#define EpicDetector_h

#include "EpicData.h"
#include "EpicPhysics.h"
#include "Math/Vector3D.h"
#include "NPCalibrationManager.h"
#include "NPVDetector.h"
#include <TApplication.h>
#include <TCanvas.h>

#ifdef Geant4_FOUND
/* #include "NPG4VDetector.h" */
#endif

namespace epic {

class EpicSpectra;

class EpicDetector : public nptool::VDetector {

  // === Constructor and Destructor
public: 
  EpicDetector();
  ~EpicDetector() {};

  // === Data members
private: 
  epic::EpicData *m_RawData{nullptr};
  epic::EpicPhysics *m_Physics{nullptr};
  std::shared_ptr<epic::EpicSpectra> m_Spectra{nullptr};
  nptool::CalibrationManager m_Cal;

  // Event counters and timing
  int m_total_raw_event{0};
  int m_good_raw_event{0};
  double m_TimeHF_prev{0.};
  double m_TimeHF_current{0.};
  
  // detector configuration
  unsigned int         m_nDets{0}; 
  vector<unsigned int> m_nAnodes;
  vector<string>       m_actinide;
  vector<TVector3>     m_posD; 
  vector<TVector3>     m_posA; 

  // configuration parameters
  int m_Get_Sampler_Qmax{0};
  vector<double> m_cfd_fract;
  vector<double> m_cfd_delay;
  vector<double> m_cfd_thres;
  vector<double> m_Q1_gate_start;
  vector<double> m_Q1_gate_stop;
  vector<double> m_Q2_gate_start;
  vector<double> m_Q2_gate_stop;
  vector<double> m_Q3_gate_start;
  vector<double> m_Q3_gate_stop;
  vector<double> m_TofRaw_max;

  
  void AddEpic(vector<double>& Pos, int nA, double zOff, vector<double>& dz);

public:
  // === Getters
  epic::EpicData*             GetRawData() const {return m_RawData;}
  epic::EpicPhysics*          GetPhysics() const {return m_Physics;}
  unsigned int                GetNumberOfDets()      const { return m_nDets; }
  unsigned int                GetNumberOfAnodesTot() const { return (unsigned int)m_actinide.size(); }
  const vector<unsigned int>& GetNumberOfAnodes()    const { return m_nAnodes;}
  const vector<string>&       GetActinideMaterial()  const { return m_actinide;}
  TVector3 GetEpicPosition(const unsigned int det)   const { return m_posD[det - 1];}
  TVector3 GetAnodePosition(const unsigned int det, const unsigned int anode) const {
    int index = GetIndex(det,anode); 
    return m_posA[index];
  }

  // === Label / Index mapping
  unsigned int Label2det(const std::string &label);
  unsigned int Label2anode(const std::string &label);
  unsigned int Label2index(const std::string &label);
  unsigned int GetIndex(int det, int anode) const; // det is 1-based, anode is 1-based

  double CalculateNeutronEnergy(double &flightTime, double &distance);


  // === inherrited from nptool::VPlugin
  std::vector<std::string> GetDependencies() { return {"root"}; };

  // === inherrited from nptool::VDetector
  //     Read stream at ConfigFile to pick-up parameters of detector (Position,...)
  //     using Token
  void ReadConfiguration(nptool::InputParser);
  void ReadConversionConfig();
  void PrintConfig();

  //  === Add Parameter to the CalibrationManger
  void AddParameterToCalibrationManager() {};

  void InitializeDataInputConversion(std::shared_ptr<nptool::VDataInput>);

  //  === Activated associated Branches and link it to the private member
  //      DetectorData address In this method mother Branches (Detector) have to be activated
  void InitializeDataInputRaw(std::shared_ptr<nptool::VDataInput>);
  //      DetectorPhysics address In this method mother Branches (Detector) AND
  //      daughter leaf (parameter) have to be activated
  void InitializeDataInputPhysics(std::shared_ptr<nptool::VDataInput>);

  //      Create associated branches and associate private member DetectorPhysics address
  void InitializeDataOutputRaw(std::shared_ptr<nptool::VDataOutput>);
  void InitializeDataOutputPhysics(std::shared_ptr<nptool::VDataOutput>);

  //      This method is called at each event read from the Input Tree. 
  //      Aim is to build treat Raw dat in order to extract physical parameter.
  void BuildPhysicalEvent();

  void BuildRawEvent(const std::string &daq, const std::string &label, void *ptr);

  //      Those two methods to clear the Event Physics or Data
  void ClearEventData() { m_RawData->Clear(); };
  void ClearEventPhysics() { m_Physics->Clear(); };

  // === Method related to the TSpectra classes
  //     aim: to provide a framework for online applications 
  //          instantiate the Spectra class and the histogramm throught it
  void InitSpectra();
  void FillSpectra();
  void WriteSpectra();
  void CheckSpectra();
  void ClearSpectra();

  // === Used for interoperability with other framework
  void SetRawDataPointer(void *) {};


  // === Simulation
public:
  void InitSimulation(std::string simtype);
  void ConstructGeometry();
  std::string m_simtype;

#ifdef Geant4_FOUND
  /* private: */
  /*  std::shared_ptr<nptool::geant4::VDetector> m_Geant4; */
#endif
};
} // namespace epic
#endif
