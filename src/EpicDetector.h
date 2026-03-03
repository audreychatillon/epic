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

public: // Constructor and Destructor
  EpicDetector();
  ~EpicDetector() {};

public: // Data member
  epic::EpicData *m_RawData;
  epic::EpicPhysics *m_Physics;
  std::shared_ptr<epic::EpicSpectra> m_Spectra;
  nptool::CalibrationManager m_Cal;

public: // inherrited from nptool::VPlugin
  std::vector<std::string> GetDependencies() { return {"root"}; };

public: // inherrited from nptool::VDetector
  //  Read stream at ConfigFile to pick-up parameters of detector (Position,...)
  //  using Token
  void ReadConfiguration(nptool::InputParser);
  void ReadConversionConfig();
  void PrintConfig();

  //  Add Parameter to the CalibrationManger
  void AddParameterToCalibrationManager() {};

  void InitializeDataInputConversion(std::shared_ptr<nptool::VDataInput>);

  //  Activated associated Branches and link it to the private member
  //  DetectorData address In this method mother Branches (Detector) have to be
  //  activated
  void InitializeDataInputRaw(std::shared_ptr<nptool::VDataInput>);

  //  Activated associated Branches and link it to the private member
  //  DetectorPhysics address In this method mother Branches (Detector) AND
  //  daughter leaf (parameter) have to be activated
  void InitializeDataInputPhysics(std::shared_ptr<nptool::VDataInput>);

  //  Create associated branches and associate private member DetectorPhysics
  //  address
  void InitializeDataOutputRaw(std::shared_ptr<nptool::VDataOutput>);

  //  Create associated branches and associate private member DetectorPhysics
  //  address
  void InitializeDataOutputPhysics(std::shared_ptr<nptool::VDataOutput>);

  //  This method is called at each event read from the Input Tree. Aime is to
  //  build treat Raw dat in order to extract physical parameter.
  void BuildPhysicalEvent();

  void BuildRawEvent(const std::string &daq, const std::string &label,
                     void *ptr);

  //  Those two method all to clear the Event Physics or Data
  void ClearEventPhysics() { m_Physics->Clear(); };
  void ClearEventData() { m_RawData->Clear(); };

  // Method related to the TSpectra classes
  // aim: to provide a framework for online applications 
  //      instantiate the Spectra class and the histogramm throught it
  void InitSpectra();
  void FillSpectra();
  void WriteSpectra();
  // Used for Online mainly, perform check on the histo and for example change
  // their color if issues are found
  void CheckSpectra();
  // Used for Online only, clear all the spectra hold by the Spectra class
  void ClearSpectra();
  // Used for interoperability with other framework
  void SetRawDataPointer(void *) {};

public:
  unsigned int Label2det(const std::string &label);
  unsigned int Label2anode(const std::string &label);
  unsigned int Label2index(const std::string &label);
  unsigned int GetIndex(int det, int anode); // det is 1-based, anode is 1-based
  double CalculateNeutronEnergy(double &flightTime, double &distance);

private:
  int m_total_raw_event;
  int m_good_raw_event;
  int m_event_to_recovered;
  double m_TimeHF_prev;
  double m_TimeHF_current;

  // thresholds
  float m_Q_Threshold;
  float m_V_Threshold;

  // array settings
  unsigned int         m_nFC; 
  vector<unsigned int> m_nAnodes;
  vector<TVector3> m_posFC; 
  vector<TVector3> m_posA; 
  void AddEpic(vector<double> Pos, int nA, double zOff, vector<double> dz);

  // configuration parameters
  int m_Get_Sampler_Qmax;
  vector<string> m_actinide;
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


public:
  unsigned int GetNumberEpic() { return m_nFC; };
  TVector3 GetEpicPosition(const unsigned int N) {
    return m_posFC[N - 1];
  }
  TVector3 GetAnodePosition(const unsigned int N) {
    return m_posA[N - 1];
  }

public:
  void SetQThreshold(float t) { m_Q_Threshold = t; };
  void SetVThreshold(float t) { m_V_Threshold = t; };

  // simulation
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
