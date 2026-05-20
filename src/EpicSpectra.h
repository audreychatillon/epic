#ifndef EpicSpectra_h
#define EpicSpectra_h

#include "EpicDetector.h"

// root
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"

// std
#include <map>
#include <memory>
#include <array>
#include <vector>
#include <string>

namespace epic {

class EpicSpectra {
public:
  EpicSpectra();
  ~EpicSpectra() {};

private:
  // === detector pointers
  std::shared_ptr<epic::EpicDetector> m_detector;
  EpicData*    m_RawData;
  EpicPhysics* m_Physics;


  // === general histograms and canvas
  TH1F * m_TimeHF;
  TH1F * m_DeltaTimeHF;
  TCanvas * m_canT0;

  // === raw and phys histograms and canvas per FC and/or anode
  std::map<std::string,TH1*> m_raw_h1;
  std::map<std::string,TH2*> m_raw_h2;
  std::map<std::string,TH1*> m_phys_h1;
  std::map<std::string,TH2*> m_phys_h2;
  std::map<std::string,TCanvas*> m_raw_can;
  std::map<std::string,TCanvas*> m_phys_can;



  //// === histograms per fission chambers and per anode

  //std::vector<TH1F*> m_hQm;               // 8 
  //std::vector<TH1F*> m_hQ1;               // 9
  //std::vector<TH1F*> m_hQ2;               // 9
  //std::vector<TH1F*> m_hQ3;               // 9
  //std::vector<TH1F*> m_hTof;              // 10
  //std::vector<TH1F*> m_hDT;               // 11

  // === canvas
  //std::vector<std::array<TCanvas*,11>> m_can;
  TCanvas* CreateCanvas(const std::string& name, int ncol);

public:
  void FillRaw();
  void FillPhy();
  void Clear();
};

} // namespace epic 
#endif
