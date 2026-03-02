#include "EpicData.h"
ClassImp(epic::EpicData);

//////////////////////////////////////////////////////////////////////
void epic::EpicData::Clear() {
  fFC_DetNbr.clear();
  fFC_AnodeNbr.clear();
  fFC_PulserTrig.clear();
  fFC_Time.clear();
  fFC_TofRaw.clear();
  fFC_TimeCfd.clear();
  fFC_TimeQmax.clear();
  fFC_Qmax.clear();
  fFC_Q1.clear();
  fFC_Q2.clear();
  fFC_Q3.clear();
  fQmax_Sampler.clear();
}

