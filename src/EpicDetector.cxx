// Main header
#include "EpicDetector.h"
#include "EpicSpectra.h"

// nptool core
#include "NPApplication.h"
#include "NPCalibrationManager.h"
#include "NPException.h"
#include "NPFunction.h"
#include "NPParticle.h"
#include "NPRootPlugin.h"

#include <TCanvas.h>
#include <TLine.h>

// For faster data conversion
#ifdef FASTERAC_FOUND
#include "fasterac/fasterac.h" // mandatory for any data type
#include "fasterac/group.h"    // group
#include "fasterac/qdc.h"      // qdc, qdc_counter
#include "fasterac/sampler.h"  // sampler counter
#include "fasterac/sampling.h" // sampler
#include "fasterac/utils.h"    // some specific tools
#define faster_sample_size_ns 2
#endif
#include "EpicSamplerProcessor.h"

// For Geant4 Simulation
#ifdef Geant4_FOUND
/* #include "EpicGeant4.h" */
#endif

// std
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace epic;
using namespace std;
using namespace ROOT::Math;
using namespace nptool;

/* TApplication app("app", nullptr, nullptr); */

////////////////////////////////////////////////////////////////////////////////
EpicDetector::EpicDetector() {

  m_RawData = new ::EpicData();
  m_Physics = new ::EpicPhysics();

  m_nDets = 0;
  m_nAtot = 0;

  m_Cal.InitCalibration();

  m_total_raw_event = 0;
  m_good_raw_event = 0;
  m_TimeHF_prev = 0.;
  m_TimeHF_current = 0.;

  m_Get_Sampler_Qmax = 0;
}
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::BuildEpicChannelMaps(){
    cout << "//// EpicDetector::BuildEpicChannelMaps" << endl;

    m_anode2index.clear();
    m_index2channel.clear();
    m_anode2index.resize(m_nDets);
    int global_index = 0;
    int offset = 0;
    for(int d = 0 ; d < m_nDets; d++){
        int nA = m_nAnodes[d];
        for(int a = 0 ; a < nA ; a++){
            int anode = m_AnodeNumber[offset+a];
            m_anode2index[d][anode] = global_index;
            m_index2channel.push_back({d+1, anode});
            global_index++;
        } // end for(a)
        offset += nA;
    }// end for(d)


    /*
    cout << "m_nAtot = " << m_nAtot << endl;
    cout << "m_index2channel.size() = " << m_index2channel.size() << endl;
    for(int index = 0 ; index < m_nAtot ; index++){
        epic_channel ch = m_index2channel[index];
        cout << "index = " << index << ", det = " << ch.det << ", anode = " << ch.anode << endl;
    } 
    offset = 0 ;
    for(int d = 0 ; d < m_nDets ; d++){
        for(int a = 0 ; a < m_nAtot ; a++){
            auto it = m_anode2index[d].find(m_AnodeNumber[offset+a]);
            cout << "det = " << d+1 << ", anode = " << m_AnodeNumber[offset+a] << ", index = " << it->second << endl;
        }
        offset += m_nAnodes[d];
    }
    */

}
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::ReadConfiguration(nptool::InputParser parser) {

  cout << "//// EpicDetector::ReadConfiguration" << endl;
  auto blocks = parser.GetAllBlocksWithToken("epic");
  vector<string> pos = {"POS"};                 // x y z of the center of the EPIC fission chamber
  vector<string> nAnodes = {"nAnodes"};         // number of anodes in the EPIC fission chamber
  vector<string> zOffsetA1 = {"zOFFSET_A1"};    // Offset position of the origin of the anode compare to the center of the chamber
  vector<string> DZprevA = {"DZ_prevA"};        // Offset position of each anode with the previous anode 
  vector<string> actinide = {"actinide"};       // isotope of the actinide on the cathode
  vector<string> mass = {"mass_ug"};            // mass of the actinide material per anode
  vector<string> AnodeNumber = {"AnodeNumber"}; // Anode number as labeled in FC_det_AnodeNumber

  vector<double> Pos;
  int            nA;
  double         zOff;
  vector<double> dz;
  vector<string> material;
  vector<double> mass_material;
  vector<int>    num;

  for (auto block : blocks) {
    // read position of the EPIC fission chamber
    if (block->HasTokenList(pos)) {
      Pos = block->GetVector3("POS", "mm");
    } else {
      cout << "ERROR: could not find POS, check your input file formatting "
           << endl;
      exit(1);
    }
    // read the number of anodes of the EPIC fission chamber
    if (block->HasTokenList(nAnodes)) {
      nA = block->GetInt("nANODES");
      if (block->HasTokenList(DZprevA)) {
        if (nA == 1)
          dz.push_back(block->GetDouble("DZ_prevA", "mm"));
        else
          dz = block->GetVectorDouble("DZ_prevA", "mm");
      }
      if (block->HasTokenList(actinide)) {
        if (nA == 1)
          m_actinide.push_back(block->GetString("actinide"));
        else {
          material = block->GetVectorString("actinide");
          m_actinide.insert(m_actinide.end(), material.begin(), material.end());
        }
      }
      if (block->HasTokenList(mass)) {
        if (nA == 1)
          m_actinide_mass.push_back(block->GetDouble("mass_ug","ug"));
        else {
          mass_material = block->GetVectorDouble("mass_ug","ug");
          m_actinide_mass.insert(m_actinide_mass.end(), mass_material.begin(), mass_material.end());
        }
      }
      if (block->HasTokenList(AnodeNumber)) {
        if (nA == 1){
          m_AnodeNumber.push_back(block->GetInt("AnodeNumber"));
        }
        else {
          num = block->GetVectorInt("AnodeNumber");
          m_AnodeNumber.insert(m_AnodeNumber.end(), num.begin(), num.end());
        }
      }
    } else {
      cout << "ERROR: could not find nAnodes, check your input file formatting "
           << endl;
      exit(1);
    }
    // read the offset position of the first anode
    if (block->HasTokenList(zOffsetA1)) {
      zOff = block->GetDouble("zOFFSET_A1", "mm");
    } else {
      cout << "ERROR: could not find zOFFSET_A1, check your input file "
              "formatting "
           << endl;
      exit(1);
    }
    m_nAtot += nA;
    m_nDets++;
    m_nAnodes.push_back(nA);
    AddEpic(Pos, nA, zOff, dz);

  }

  // initialization prior to the ReadConversionConfiguration
  m_cfd_fract.resize(m_nAtot, 0.25);
  m_cfd_delay.resize(m_nAtot, 4.);
  m_cfd_thres.resize(m_nAtot, 50.);
  m_Q1_gate_start.resize(m_nAtot, 6.); // TODO change for negative value
  m_Q1_gate_stop.resize(m_nAtot, 40.);
  m_Q2_gate_start.resize(m_nAtot, 6.); // TODO change for negative value
  m_Q2_gate_stop.resize(m_nAtot, 10.);
  m_Q3_gate_start.resize(m_nAtot, 10.);
  m_Q3_gate_stop.resize(m_nAtot, 40.);
  m_TofRaw_max.resize(m_nAtot, -1.); // ns

  BuildEpicChannelMaps();
  ReadConversionConfig();
  PrintConfig();
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::ReadConversionConfig() {
  //TODO : check that ConfigEPIC.dat file and detector.yaml 
  //       are coherent with the number of anodes per det
  cout << "//// EpicDetector::ReadConversionConfig" << endl;
  std::ifstream ifs("./config_files/ConfigEPIC.dat");
  if (ifs.is_open()) {

    vector<string> info_sample = {"get_sampler_qmax"};
    nptool::InputParser parser("./config_files/ConfigEPIC.dat", false);

    auto blocks = parser.GetAllBlocksWithToken("ConfigEPIC");
    for (auto block : blocks) {
      int det = block->GetInt("det", 1);
      int anode = block->GetInt("anode", 1);
      int index = GetIndex(det, anode);
      if (index >= m_cfd_fract.size() || anode != m_index2channel[index].anode || det != m_index2channel[index].det){ 
        cout << "     ERROR OF MAPPING "<< endl;
        cout << "            - index of the input is " << index << " >= " << m_cfd_fract.size() << " = size of vector" << endl;
        cout << "            - det from ConfigEPIC.dat : " << det << "!= det from detector.yaml : " << m_index2channel[index].det  << endl;
        cout << "            - anode from ConfigEPIC.dat : " << anode << "!= anode from detector.yaml : " << m_index2channel[index].anode  << endl;
      }
      else         
        cout << "     Found EPIC block: det = " << det << ", anode = " << anode << ", index " << index << endl;
     
      if (block->HasTokenList(info_sample)) {
        m_Get_Sampler_Qmax = block->GetInt("get_sampler_qmax", 1);
      }
      m_cfd_fract[index] = 1. / (double)block->GetInt("cfd_frac", 1);
      m_cfd_delay[index] = (double)block->GetInt("cfd_delay", 1);
      m_cfd_thres[index] = (double)block->GetInt("cfd_thres", 1);
      m_Q1_gate_start[index] = (double)block->GetInt("Q1_gate_start", 1);
      m_Q1_gate_stop[index] = (double)block->GetInt("Q1_gate_stop", 1);
      m_Q2_gate_start[index] = (double)block->GetInt("Q2_gate_start", 1);
      m_Q2_gate_stop[index] = (double)block->GetInt("Q2_gate_stop", 1);
      m_Q3_gate_start[index] = (double)block->GetInt("Q3_gate_start", 1);
      m_Q3_gate_stop[index] = (double)block->GetInt("Q3_gate_stop", 1);
      m_TofRaw_max[index] = (double)block->GetInt("RawTof_MaxLimit", 1);
    }
  } else
    cout << "//// No EPIC conversion file found, using default parameters"
         << endl;
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::PrintConfig() {
  constexpr int colWidth = 20;
  cout << "//// EpicDetector::PringConversion Config" << endl;
  cout << "     Number of EPIC fission chamber found : " << m_nDets << endl;
  cout << "                   (Total number of anodes : " << m_nAtot << ")" << endl;
  size_t offset = 0;
  for (int d = 0; d < m_nDets; d++) {
    cout << "     ==== ===================================================== ====" << endl; 
    cout << "     ==== EPIC fission chamber # " << d + 1 << endl;
    cout << "          number of anodes : " << m_nAnodes[d] << endl;
    // actinide material
    cout << "          sample material        : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++)
      cout << left << setw(colWidth) << m_actinide[offset + a];
    cout << endl;
    // actinide material
    cout << "          label                  : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++){
      ostringstream oss;
      oss << "FC_" << d+1 << "_" << m_AnodeNumber[offset + a] ;
      cout << left << setw(colWidth) << oss.str();
    }
    cout << endl;
    // CFD parameters
    cout << "          CFD (frac, dly, thrs)  : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++) {
      ostringstream oss;
      oss << "(" << m_cfd_fract[offset + a] << "," << m_cfd_delay[offset + a]
          << "," << m_cfd_thres[offset + a] << ")";
      cout << left << setw(colWidth) << oss.str();
    }
    cout << endl;
    // Q1 gate
    cout << "          Q1 gate                : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++) {
      ostringstream oss;
      oss << "[" << m_Q1_gate_start[offset + a] * -1. << ":"
          << m_Q1_gate_stop[offset + a] << "]";
      cout << left << setw(colWidth) << oss.str();
    }
    cout << endl;
    // Q2 gate
    cout << "          Q2 gate                : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++) {
      ostringstream oss;
      oss << "[" << m_Q2_gate_start[offset + a] * -1. << ":"
          << m_Q2_gate_stop[offset + a] << "]";
      cout << left << setw(colWidth) << oss.str();
    }
    cout << endl;
    // Q3 gate
    cout << "          Q3 gate                : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++) {
      ostringstream oss;
      oss << "[" << m_Q3_gate_start[offset + a] << ":"
          << m_Q3_gate_stop[offset + a] << "]";
      cout << left << setw(colWidth) << oss.str();
    }
    cout << endl;
    cout << "     ---- ----------------------------------------------------- ----" << endl; 
    cout << "      calibration parameters " << endl;
    cout << "     ---- ----------------------------------------------------- ----" << endl; 
    // TofRaw max
    cout << "          TofRawMax (*)          : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++)
      cout << left << setw(colWidth) << m_TofRaw_max[offset + a];
    cout << endl;
    // Gamma Peak
    cout << "          Gamma Peak [ch]        : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++)
      cout << left << setw(colWidth) << m_Cal.GetValue("EPIC_" + to_string(d+1) + "_ANODE_" + to_string(m_AnodeNumber[offset + a]) + "_GAMMA_PEAK",0);
    cout << endl;
    // Alpha cut
    cout << "          Alpha Cut [ch]         : ";
    for (size_t a = 0; a < m_nAnodes[d]; a++)
      cout << left << setw(colWidth) << m_Cal.GetValue("EPIC_" + to_string(d+1) + "_ANODE_" + to_string(m_AnodeNumber[offset + a]) + "_ALPHA",0);
    cout << endl;
    cout << "     ==== ===================================================== ====" << endl; 
    cout << endl;
    offset += m_nAnodes[d];
  }
  cout << " (*) TofRawMax: if > 0, alpha filter on incoming TofRaw: tof_raw > TofRawMax are rejected " << endl;
  cout << "                if < 0, no filter on TofRaw" << endl;
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::AddEpic(vector<double> &pos, int nA, double zOff,
                           vector<double> &dz) {

  TVector3 DetectorPos(pos[0], pos[1], pos[2]);
  m_posD.push_back(DetectorPos);

  // zOff = origin of the z position compared to the center of the chamber
  double Zpos = zOff; // position of anode 1 (i=0)
  for (int i = 0; i < nA; i++) {
    Zpos += dz[i];
    TVector3 AnodePos(pos[0], pos[1], pos[2] + Zpos);
    m_posA.push_back(AnodePos);
  }
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataInputConversion(
    std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("FC_", "", this);
  input->Attach("HF", "", this);
  input->Attach("PULSER", "", this);
  input->Attach("FAKE_FISSION", "", this);
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataInputRaw(
    std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("epic", "epic::EpicData", &m_RawData);
}
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataOutputRaw(
    std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("epic", "epic::EpicData", &m_RawData);
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataInputPhysics(
    std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("epic", "epic::EpicPhysics", &m_Physics);
};
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataOutputPhysics(
    std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("epic", "epic::EpicPhysics", &m_Physics);
};

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::BuildPhysicalEvent() {

    short  det    = -1;
    short  anode  = -1;
    double tofraw = -1;
    double tofcal = -1;
    double e      = -1;
    bool   alpha  = true;

  if(m_RawData->GetFCMult()>0 && m_RawData->GetQmaxIndex()>=0){
    short  imax   = m_RawData->GetQmaxIndex();
    if(!m_RawData->GetPulserTrig(imax) && imax < m_RawData->GetFCMult()){
        double q1 = m_RawData->GetQ1(imax);
        det    = m_RawData->GetDetNbr(imax);
        anode  = m_RawData->GetAnodeNbr(imax); 
        tofraw = m_RawData->GetTofRaw(imax);
        if (q1 > m_Cal.GetValue("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_ALPHA",0))
            alpha = false;
        if (!alpha) 
            e  = TofRaw2Ene(det, anode, tofraw, tofcal);
    }
  }
  
  m_Physics->SetHit_fFC(det, anode, tofraw, tofcal, e, alpha);

}

////////////////////////////////////////////////////////////////////////////////

void EpicDetector::BuildRawEvent(const std::string &daq,
                                 const std::string &label, void *data) {
#ifdef FASTERAC_FOUND

  // Static variable
  static unsigned int index, det, anode;
  static long double timestamp;
  static unsigned char alias;
  static qdc_t_x1 hf_data;
  static qdc_t_x2 fc_data;
  static faster_data_p group_data;
  static double TimeHF;

  // Extract Data
  timestamp = faster_data_clock_ns(data);
  alias = faster_data_type_alias(data); //  type of the data
  // double Qmax_per_evt = 0 ;

  if (alias == QDC_TDC_X1_TYPE_ALIAS) {
    if (label == "HF") {
      faster_data_load(data, &hf_data);
      m_TimeHF_prev    = m_TimeHF_current;
      m_TimeHF_current = (double)timestamp + (double)(qdc_conv_dt_ns(hf_data.tdc));
      //cout << setprecision(25) << " --> t_hf = " << m_TimeHF_current << "(DELTA = " << m_TimeHF_current - m_TimeHF_prev << ")" << endl;
      m_RawData->SetTimePrevHF(m_TimeHF_prev);
      m_RawData->SetTimeHF(m_TimeHF_current);
      m_RawData->SetDetNbr(-1);
      m_RawData->SetAnodeNbr(-1);
      m_RawData->SetQ1(-1);
      m_RawData->SetQ2(-1);
      m_RawData->SetQ3(-1);
      m_RawData->SetQmax(-1);
      m_RawData->SetPulserTrig(false);
    }
    if (label == "PULSER" || label == "FAKE_FISSION") {
      faster_data_load(data, &fc_data);
      double TimeFC = timestamp + (double)(qdc_conv_dt_ns(fc_data.tdc));
      double tof_raw = TimeFC - m_TimeHF_current;
      m_RawData->SetAnodeNbr(-1);
      m_RawData->SetTimeFC(TimeFC);
      m_RawData->SetTofRaw(tof_raw);
      m_RawData->SetQ1(fc_data.q1);
      m_RawData->SetQ2(-1);
      m_RawData->SetQ3(-1);
      m_RawData->SetQmax(-1);
      m_RawData->SetPulserTrig(true);
      m_RawData->SetTimeLastHF(m_TimeHF_current);
      m_RawData->SetTimeCfd(-1);
    }
  } else if (alias == QDC_TDC_X2_TYPE_ALIAS) {
    if (label == "PULSER" || label == "FAKE_FISSION") {
      faster_data_load(data, &fc_data);
      double TimeFC = timestamp + (double)(qdc_conv_dt_ns(fc_data.tdc));
      double tof_raw = TimeFC - m_TimeHF_current;
      m_RawData->SetAnodeNbr(-1);
      m_RawData->SetTimeFC(TimeFC);
      m_RawData->SetQ1(fc_data.q1);
      m_RawData->SetQ2(fc_data.q2);
      m_RawData->SetQ3(0);
      m_RawData->SetQmax(0);
      m_RawData->SetPulserTrig(true);
      m_RawData->SetTimeLastHF(m_TimeHF_current);
      m_RawData->SetTimeCfd(-1);
    }
  } else if (alias == QDC_TDC_X4_TYPE_ALIAS) {
    nptool::message("yellow", "epic", "Epic::BuildRawEvent",
                    "Warning: QDC_TDC_X4_TYPE_ALIAS found on label " + label);
  } else if (alias == SAMPLER_DATA_TYPE_ALIAS) {

    if (label.front() == 'F') {
      m_total_raw_event++; // total number of counts in all anodes, if mult=2,
                           // m_total_raw_event is incremented by 2

      // identification of the electronic channel
      anode = Label2anode(label);
      det   = Label2det(label);
      index = GetIndex(det,anode);

      // sampler_data samp = (sampler_data)faster_data_load_p(data);
      // load sampler data:
      sampler samp;
      faster_data_load(data, &samp);
      int width_ns = (int)faster_data_load_size(data);
      int nbr_of_samples = sampler_samples_num(data);
      int sampler_before_threshold = sampler_before_th(data);
      int sampler_before_threshold_ns = sampler_before_th_ns(data);

      // get the sample, remove BLR, create input to process the signal
      static vector<double> Signal;
      Signal.clear();
      double BLRoffset = 0;
      for (int i = 0; i < nbr_of_samples; i++) {
        Signal.push_back(samp.data[i] - BLRoffset);
      }

      // initialize arguments of EpicSamplerProcessor class
      bool ApplyFilter = false;
      EpicSamplerProcessor sample(Signal, ApplyFilter);
      // calculate CFD time
      double Qmax = -10000.;
      double T_qmax = -20000.;
      double T_cfd;
      static bool FC_Triggered = false;
      static bool FC_Threshold = false;

      T_cfd = sample.calculateCFDnew(m_cfd_fract[index], m_cfd_delay[index],
                                     m_cfd_thres[index], faster_sample_size_ns,
                                     Qmax, T_qmax, FC_Triggered, FC_Threshold);

      double Q1 = -1;
      double Q2 = -1;
      double Q3 = -1;
      if (FC_Triggered && FC_Threshold && T_cfd != -10000) {
        m_good_raw_event++;
        double signal_size = Signal.size();

        /////
        /// gate integration
        /////
        double start, stop;
        // Q1 - long gate
        start = max(0., T_cfd - m_Q1_gate_start[index]); // -: before Tcfd
        stop = min(T_cfd + m_Q1_gate_stop[index],
                   signal_size * 2); // +: after Tcfd
        if (start < stop)
          Q1 = sample.integrateSignal(
              2, start, stop); // TODO check bounding of integration
        // Q2 - gate around the rising time
        start = max(0., T_cfd - m_Q2_gate_start[index]); // -: before Tcfd
        stop = min(T_cfd + m_Q2_gate_stop[index],
                   signal_size * 2); // +: after Tcfd
        if (start < stop)
          Q2 = sample.integrateSignal(2, start, stop);
        // Q3 - gate around the falling time
        start = min(signal_size * 2,
                    T_cfd + m_Q3_gate_start[index]); // +: after Tcfd
        stop = min(signal_size * 2,
                   T_cfd + m_Q3_gate_stop[index]); // +: after Tcfd
        if (start < stop)
          Q3 = sample.integrateSignal(2, start, stop);

        if (Q1 > 0 && Q2 > 0 && Q3 > 0) {
          double TimeFC = (double)timestamp + (double)T_cfd - sampler_before_threshold_ns;
          double tof_raw = TimeFC - m_TimeHF_current;
          if (tof_raw < m_TofRaw_max[index] || m_TofRaw_max[index] < 0) {
            m_RawData->SetDetNbr(det);
            m_RawData->SetAnodeNbr(anode);
            m_RawData->SetQ1(Q1);
            m_RawData->SetQ2(Q2);
            m_RawData->SetQ3(Q3);
            m_RawData->SetQmax(Qmax);
            m_RawData->SetTofRaw(tof_raw);
            m_RawData->SetTimeFC(TimeFC);
            m_RawData->SetTimeCfd(T_cfd);
            m_RawData->SetTimeQmax(T_qmax);
            m_RawData->SetPulserTrig(false);
            if (m_RawData->GetFCMult() == 1) {
              // no need to overwrite the same data
              m_RawData->SetTimeLastHF(m_TimeHF_current);
            }

            // sample for anode with Qmax
            if (m_RawData->GetFCMult() == 1) {
                if (m_Get_Sampler_Qmax == 1) m_RawData->SetSampler(Signal);
                m_RawData->SetQmaxIndex(0);
            } 
            else if(m_RawData->GetFCMult()>1 && m_RawData->GetQmaxIndex()>=0) {
                //TODO need to find out how GetQmaxIndex could be <= at this stage !!!!
                if (Qmax > m_RawData->GetQmax(m_RawData->GetQmaxIndex())) {
                    if (m_Get_Sampler_Qmax == 1) m_RawData->SetSampler(Signal);
                    m_RawData->SetQmaxIndex(m_RawData->GetFCMult() - 1);
                }
            }
            else m_RawData->SetQmaxIndex(-1);
          } // end of rejection or not of events as a function of its tof_raw
        } // end if Qi>0
      } // If FC_Triggered && FC_Threshold && T_cfd
    } // end FC sampler
  } // end if SAMPLER data
  else {
    nptool::message("yellow", "epic", "Epic::BuildRawEvent",
                    "Warning: unexpected data type on label " + label);
    return;
  }
#endif
};

////////////////////////////////////////////////////////////////////////////////
unsigned int EpicDetector::Label2det(const std::string &label) {
  // generic to handle FC_det_anode or FC_anode
  size_t pos1 = label.find("_");
  size_t pos2 = label.find("_", pos1 + 1);
  if (pos2 == string::npos) // format: FC_anode => only one FC
    return 1;
  else { // format: FC_det_anode
    string number = label.substr(pos1 + 1, pos2 - pos1 - 1);
    return stoi(number);
  }
}
////////////////////////////////////////////////////////////////////////////////
unsigned int EpicDetector::Label2anode(const std::string &label) {
  // generic to handle FC_det_anode or FC_anode
  size_t pos1 = label.find("_");
  size_t pos2 = label.find("_", pos1 + 1);
  if (pos2 == string::npos) { // format: FC_anode => only one FC
    string number = label.substr(pos1 + 1);
    return stoi(number);
  } else { // format: FC_det_anode
    string number = label.substr(pos2 + 1);
    return stoi(number);
  }
}
////////////////////////////////////////////////////////////////////////////////
// det is 1-based, anode is 1-based
unsigned int EpicDetector::GetIndex(int det, int anode) const {
  if (det == 0 || det > m_nDets || anode == 0 ) {
    cout << "ERROR : EpicDetector::GetIndex(" << det << ", " << anode
         << "): but m_nDets=" << m_nDets << " and m_nAnodes[" << det - 1
         << "]=" << m_nAnodes[det - 1] << endl;
  }
  auto it = m_anode2index[det-1].find(anode);
  return it->second;
}
////////////////////////////////////////////////////////////////////////////////
double EpicDetector::TofRaw2Ene(int det, int anode, double tofraw, double &tofcal) {

  const double mn_MeV = 939.565;
  int index = GetIndex(det, anode);
  double gammapeak = m_Cal.GetValue("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_GAMMA_PEAK",0);
  double offset = m_posA[index].Z() / 299.792458 - gammapeak;
  tofcal = tofraw + offset;
  double beta = (m_posA[index].Z() / tofcal) / 299.792458;
  double gamma = 1.0 / sqrt(1.0 - pow(beta, 2));
  return ((gamma - 1.0) * mn_MeV);
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitSimulation(std::string simtype) {
  // store the loaded simulation
  m_simtype = simtype;

  // load the plugin
  auto handle = nptool::Application::GetApplication()->LoadPlugin(
      "epic-" + m_simtype, true);
  // build the class
#ifdef Geant4_FOUND
  /* auto func = (std::shared_ptr<nptool::geant4::VDetector>(*)())dlsym(handle,
   * "ConstructDetectorSimulation"); */
  /* if (func) */
  /*   m_Geant4 = (*func)(); */
  /* else */
  /* throw(nptool::Error("EpicDetector", "Fail to load Geant4
   * module")); */
#endif
}
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::ConstructGeometry() {
#ifdef Geant4_FOUND
  /* if (m_Geant4) { */
  /*   // should load the library here and find the external constructor */
  /*   m_Geant4->ConstructDetector(); */
  /* } */
#endif
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitSpectra() {
  m_Spectra = std::make_shared<epic::EpicSpectra>();
};
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::FillSpectra() {
  m_Spectra->FillRaw();
  m_Spectra->FillPhy();
};
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::WriteSpectra() {};
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::CheckSpectra() {};
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::ClearSpectra() { m_Spectra->Clear(); };

///////////////////////////////////////////////////////////////////////////////
extern "C" {
std::shared_ptr<nptool::VDetector> ConstructDetector() {
  return make_shared<::EpicDetector>();
};
}
