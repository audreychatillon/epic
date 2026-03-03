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

  // === histograms
  std::vector<TH2F*> m_hQ2vQ1;
  std::vector<TH2F*> m_hQ2Q3vQ1;
  std::vector<TH2F*> m_hQmvQ1;

  std::vector<TH1F*> m_hQm;
  std::vector<TH1F*> m_hQ1;
  std::vector<TH1F*> m_hQ2;
  std::vector<TH1F*> m_hQ3;
  std::vector<TH1F*> m_hTof;
  std::vector<TH1F*> m_hDT;

  std::vector<TH1F*> m_hAid;
  std::vector<TH1F*> m_hAid_ifQmax;
  std::vector<TH2F*> m_hQ1vAid;

  // === canvas
  std::vector<std::array<TCanvas*,7>> m_can;
  TCanvas* CreateCanvas(const std::string& name);

public:
  void FillRaw();
  void FillPhy();
  void Clear();
};

} // namespace epic 
#endif
