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

  // === histograms per fission chambers  // number of canvas
  std::vector<TH1F*> m_hAid;              // 1
  std::vector<TH1F*> m_hAid_ifQmax;       // 1
  std::vector<TH2F*> m_hQ1vAid;           // 2

  // === histograms per fission chambers and per anode
  std::vector<TH2F*> m_hQ2vQ1;            // 3
  std::vector<TH2F*> m_hQ2Q3vQ1;          // [1]
  std::vector<TH2F*> m_hQmvQ1;            // [2]
  std::vector<TH2F*> m_hQ1vT;             // [3]

  std::vector<TH1F*> m_hQm;               // [5]
  std::vector<TH1F*> m_hQ1;               // [6]
  std::vector<TH1F*> m_hQ2;               // [6]
  std::vector<TH1F*> m_hQ3;               // [6]
  std::vector<TH1F*> m_hTof;              // [7]
  std::vector<TH1F*> m_hDT;               // [8]

  // === canvas
  std::vector<std::array<TCanvas*,11>> m_can;
  TCanvas* CreateCanvas(const std::string& name, int ncol);

  // === general histograms and canvas
  TH1F * m_TimeHF;
  TH1F * m_DeltaTimeHF;
  TCanvas * m_canT0;

public:
  void FillRaw();
  void FillPhy();
  void Clear();
};

} // namespace epic 
#endif
