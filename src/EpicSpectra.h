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
namespace epic {

// forward declaration is necessary
// class EpicDetector;
class EpicSpectra {
public:
  EpicSpectra();
  ~EpicSpectra() {};

private:
  std::shared_ptr<epic::EpicDetector> m_detector;
  epic::EpicData *m_RawData;
  epic::EpicPhysics *m_Physics;
  std::map<std::string, TH1 *> m_raw_hist;
  std::map<std::string, TH2 *> m_raw_hist2d;
  std::map<std::string, TH1 *> m_phy_hist;
  TGraph *m_graph_sampler;
  double check_time;
  unsigned int nFC;

public:
  void FillRaw();
  void FillPhy();
  void Clear();
};

} // namespace epic 
#endif
