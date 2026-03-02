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

  m_nFC = 0;

  m_Cal.InitCalibration();

  m_total_raw_event = 0;
  m_good_raw_event = 0;
  m_event_to_recovered = 0;
  m_TimeHF_prev = 0.;
  m_TimeHF_current = 0.;

  m_Get_Sampler_Qmax = 0;
}
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::ReadConfiguration(nptool::InputParser parser) {

  cout << "//// EpicDetector::ReadConfiguration" << endl;
  auto blocks = parser.GetAllBlocksWithToken("epic");
  vector<string> posFC     = {"POS"};        // x y z of the center of the EPIC fission chamber
  vector<string> typeFC    = {"TYPE"};       // TODO remove and find a solution for the interdistance anode  
  vector<string> nAnodesFC = {"nAnodes"};    // number of anodes in the EPIC fission chamber
  vector<string> zOffsetA1 = {"zOFFSET_A1"}; // Offset position of the first anode

  vector<double> Pos;
  string         Type;
  int            nA;
  int            nAtot=0;
  double         zOff;

  for (auto block : blocks) {
    // read position of the EPIC fission chamber
    if (block->HasTokenList(posFC)) {
      Pos = block->GetVector3("POS", "mm");
    } else {
      cout << "ERROR: could not find POS, check your input file formatting "<< endl;
      exit(1);
    }
    // TODO remove read type of the EPIC fission chamber
    if (block->HasTokenList(typeFC)) {
      Type = block->GetString("TYPE");
    } else {
      cout << "ERROR: could not find TYPE, check your input file formatting "<< endl;
      exit(1);
    }
    // read the number of anodes of the EPIC fission chamber
    if (block->HasTokenList(nAnodesFC)) {
      nA = block->GetInt("nANODES");
    } else {
      cout << "ERROR: could not find nAnodes, check your input file formatting "<< endl;
      exit(1);
    }
    // read the offset position of the first anode
    if (block->HasTokenList(zOffsetA1)) {
      zOff = block->GetDouble("zOFFSET_A1","mm");
    } else {
      cout << "ERROR: could not find zOFFSET_A1, check your input file formatting "<< endl;
      exit(1);
    }
    nAtot += nA;
    m_nFC++;
    m_nAnodes.push_back(nA);
    AddEpic(Pos, Type, nA, zOff);
  }

  // initialization prior to the ReadConversionConfiguration
  m_actinide.resize(nAtot,"EMPTY");
  m_cfd_fract.resize(nAtot, 0.25);
  m_cfd_delay.resize(nAtot, 4.);
  m_cfd_thres.resize(nAtot, 50.);
  m_Q1_gate_start.resize(nAtot, 6.); // TODO change for negative value
  m_Q1_gate_stop.resize(nAtot, 40.);
  m_Q2_gate_start.resize(nAtot, 6.); // TODO change for negative value
  m_Q2_gate_stop.resize(nAtot, 10.);
  m_Q3_gate_start.resize(nAtot, 10.);
  m_Q3_gate_stop.resize(nAtot, 40.);
  m_TofRaw_max.resize(nAtot, -1.); // ns 
  ReadConversionConfig();
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::ReadConversionConfig() {

  cout << "//// EpicDetector::ReadConversionConfig" << endl;
  cout << "     Number of EPIC fission chamber found : " << m_nFC << endl;
  std::ifstream ifs("./configs/ConfigEPIC.dat");
  if (ifs.is_open()) {

    vector<string> info_sample = {"get_sampler_qmax"};
    nptool::InputParser parser("./configs/ConfigEPIC.dat", false);

    auto blocks = parser.GetAllBlocksWithToken("ConfigEPIC");
    for (auto block : blocks) {
      int det = block->GetInt("det", 1);
      int anode = block->GetInt("anode", 1);
      int index = GetIndex(det,anode) ;
      if (index>= m_cfd_fract.size()) 
        cout << " ///// ERROR : index of the input is " << index << " >= " << m_cfd_fract.size() << " = size of vector" << endl;
      else{
        cout << "//// found FCblock: det = " << det << ", anode = " << anode << ", index " << index << endl;
      }
      if (block->HasTokenList(info_sample)) {
        m_Get_Sampler_Qmax = block->GetInt("get_sampler_qmax",1);
      }
      m_actinide[index] = block->GetString("actinide",1);
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
      cout << "actinide: " << m_actinide[index] << endl; 
      cout << "     CFD: frac = " << m_cfd_fract[index] << " , delay = " << m_cfd_delay[index] << endl; 
      cout << "     Q1 [-" << m_Q1_gate_start[index] << " ; " << m_Q1_gate_stop[index] << "]" << endl; 
      cout << "     Q2 [-" << m_Q2_gate_start[index] << " ; " << m_Q2_gate_stop[index] << "]" << endl; 
      cout << "     Q3 [" << m_Q3_gate_start[index] << " ; " << m_Q3_gate_stop[index] << "]" << endl; 
      if (m_TofRaw_max[index] > 0 )
        cout << "     all events with a non physical tof_raw > " << m_TofRaw_max[index] << " are rejected (alpha decay between macro-pulse) " << endl;
      else
        cout << "     no filtered on incoming neutron RawTof: keep all events (source or sf run)" << endl;
    }
  } 
  else cout << "//// No FC conversion file found, using default parameters" << endl;
}


////////////////////////////////////////////////////////////////////////////////
void EpicDetector::AddEpic(vector<double> pos, string Type, int nA, double zOff) {

  if(Type=="Cf"){
  // TODO should be changed to remove hard coding
        for (int i = 0; i < nA; i++) {
          //TVector3 AnodePos(pos[0], pos[1], pos[2] - 27.5 + i * 5.5 - 2.5);
          //TVector3 AnodePos(pos[0], pos[1], pos[2] - 30.0 + i * 5.5);
          TVector3 AnodePos(pos[0], pos[1], pos[2] + zOff + i * 5.5);
          m_posA.push_back(AnodePos);
        }
  }
  else{ // "Pu" or "multi" TODO should be changed to remove hard coding
        // for Pu: anodes are separated by 5.8mm and 6 mm and anode 6 in center,
        //         anode is the middle point betwen the two cathodes containing the
        //         deposits
        //double Zpos = -29.6; // position of anode 1 (i=0)
        double Zpos = zOff; // position of anode 1 (i=0)
        for (int i = 0; i < nA; i++) {
          TVector3 AnodePos(pos[0], pos[1], pos[2] + Zpos);
          m_posA.push_back(AnodePos);
          if (i % 2 == 0) {
            Zpos += 6.0; // Add 6 for even iterations
          } else {
            Zpos += 5.8; // Add 5.8 for odd iterations
          }
        }
  
  }

}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataInputConversion(std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("FC_", "", this);
  input->Attach("HF", "", this);
  input->Attach("PULSER", "", this);
  input->Attach("FAKE_FISSION", "", this);
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataInputRaw(std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("epic", "epic::EpicData",&m_RawData);
}
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataOutputRaw(std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("epic", "epic::EpicData",&m_RawData);
}

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataInputPhysics(std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("epic", "epic::EpicPhysics",&m_Physics);
};
////////////////////////////////////////////////////////////////////////////////
void EpicDetector::InitializeDataOutputPhysics(std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("epic", "epic::EpicPhysics",&m_Physics);
};

////////////////////////////////////////////////////////////////////////////////
void EpicDetector::BuildPhysicalEvent() {
 

    // Copy RawData in PhysicalTree to be able to run macro at raw level on TTree after npanalysis
    // difficult to have access to the branches on RawTree after npconversion
    // TODO handle the Sampler to optimize CFD

    // FILL FC part
    unsigned int multFC = m_RawData->GetFCMult();
    double q_qmax = 0 ;
    int    i_qmax = -1;
    for (int i = 0; i < multFC; i++) {
        short   det  = m_RawData->GetDetNbr(i);
        short anode  = m_RawData->GetAnodeNbr(i);
        bool    kFF  = m_RawData->GetIsFakeFission(i);
        double t_fc   = m_RawData->GetTimeFC(i);
        double tofraw = m_RawData->GetTofRaw(i);  
        double t_cfd  = m_RawData->GetTimeCfd(i);
        double t_qm   = m_RawData->GetTimeQmax(i);
        double qm = m_RawData->GetQmax(i);
        double q1 = m_RawData->GetQ1(i);
        double q2 = m_RawData->GetQ2(i);
        double q3 = m_RawData->GetQ3(i);
        m_Physics->SetHit_fFC(det, anode, kFF, t_fc, tofraw, t_cfd, t_qm, qm, q1, q2, q3);
        if (qm > q_qmax){
            q_qmax = qm;
            i_qmax = i ;
        }
    } 

    // FILL HF part
    double t_hf = m_RawData->GetTimeHF();
    double tprev_hf = m_RawData->GetTimePrevHF();
    m_Physics->SetHit_fHF(t_hf,tprev_hf);

    // FILL SAMPLER PART
    vector<double> v_q;
    if (m_Get_Sampler_Qmax == 1 && m_RawData->GetSamplerSize()>0 ) {
        short index = m_RawData->GetQmaxIndex();
        v_q = m_RawData->GetSampler();
        m_Physics->SetHit_fSampler(m_RawData->GetDetNbr(index),m_RawData->GetAnodeNbr(index),v_q);
        if(i_qmax != index){
            cout << "index = " << index << " : fSampler_DetNbr = " << m_RawData->GetDetNbr(index) << " : fSampler_AnodeNbr = " << m_RawData->GetAnodeNbr(index) << " : Qmax = " << m_RawData->GetQmax(index) << endl;
            for (int i = 0; i < multFC; i++) {
                cout << "i = " << i << "    RAW : fFC_DetNbr = " << m_RawData->GetDetNbr(i) << " : fFC_AnodeNbr = " << m_RawData->GetAnodeNbr(i) << " : Qmax = " << m_RawData->GetQmax(i) << endl;
            }
        }
    } 
    else{
        v_q.clear();
        m_Physics->SetHit_fSampler(-1, -1, v_q);
    }

    /*
    nptool::Particle *neutron = new nptool::Particle("1n");

    for (int i = 0; i < multFC; i++) {
      short AnodeNumber = m_RawData->GetAnodeNbr(i);
      bool isFakeFission = false;
      isFakeFission = m_RawData->GetIsFakeFission(i);
      double Time_FC = m_RawData->GetTimeFC(i);
      double cfd = m_RawData->GetTimeCfd(i);
      // TODO offset with det number
      double FC_offset = m_Cal.GetValue(
          "Epic_ANODE" + nptool::itoa(AnodeNumber) + "_TIMEOFFSET",
    0); double incomingToF = Time_FC - Time_HF - FC_offset;

      // Get Anode position, in pulser mode (fakefission) we use the anode 6 by
      // default
      TVector3 AnodePos = GetAnodePosition(6);
      if (AnodeNumber > 0) {
        TVector3 AnodePos = GetAnodePosition(AnodeNumber);
      }

      if (incomingToF < 0) {
        incomingToF += m_Cal.GetValue("Epic_PULSE_TIMEOFFSET", 0);
      }

      double FlyPath = 21000;
      double incomingE = 0;
      neutron->SetBeta((FlyPath / incomingToF) / nptool::c_light);
      incomingE = neutron->GetEnergy();
      m_Physics->SetHitFC(AnodeNumber, m_RawData->GetQ1(i),
                          m_RawData->GetQ2(i), m_RawData->GetQ3(i),
                          Time_FC,
                          m_RawData->GetQmax(i), incomingE, incomingToF,
                          Time_HF, isFakeFission, m_RawData->GetTimeCfd(i));
    }
    */
}

////////////////////////////////////////////////////////////////////////////////

void EpicDetector::BuildRawEvent(const std::string &daq,
                                           const std::string &label,
                                           void *data) {
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
  alias = faster_data_type_alias(data);    //  type of the data
  //double Qmax_per_evt = 0 ; 

  if (alias == QDC_TDC_X1_TYPE_ALIAS) {
    if (label == "HF") {
      faster_data_load(data, &hf_data);
      m_TimeHF_prev    = m_TimeHF_current;
      m_TimeHF_current = (double)timestamp + (double)(qdc_conv_dt_ns(hf_data.tdc));
      //cout << setprecision(25) << " --> t_hf = " << m_TimeHF_current <<  "(DELTA = " << m_TimeHF_current - m_TimeHF_prev << ")" << endl;
    }
    if (label == "PULSER" || label == "FAKE_FISSION") {
      faster_data_load(data, &fc_data);
      double TimeFC = timestamp + (double)(qdc_conv_dt_ns(fc_data.tdc));
      double tof_raw = TimeFC - m_TimeHF_current ; 
      m_RawData->SetAnodeNbr(-1);
      m_RawData->SetTimeFC(TimeFC);
      m_RawData->SetTofRaw(tof_raw);
      m_RawData->SetQ1(fc_data.q1);
      m_RawData->SetQ2(0);
      m_RawData->SetQ3(0);
      m_RawData->SetQmax(0);
      m_RawData->SetFakeFissionStatus(true);
      m_RawData->SetTimePrevHF(m_TimeHF_prev);
      m_RawData->SetTimeHF(m_TimeHF_current);
      m_RawData->SetTimeCfd(-1);
    }
  } 
  else if (alias == QDC_TDC_X2_TYPE_ALIAS) {
    if (label == "PULSER" || label == "FAKE_FISSION") {
      faster_data_load(data, &fc_data);
      double TimeFC = timestamp + (double)(qdc_conv_dt_ns(fc_data.tdc));
      double tof_raw = TimeFC - m_TimeHF_current ; 
      m_RawData->SetAnodeNbr(-1);
      m_RawData->SetTimeFC(TimeFC);
      m_RawData->SetQ1(fc_data.q1);
      m_RawData->SetQ2(fc_data.q2);
      m_RawData->SetQ3(0);
      m_RawData->SetQmax(0);
      m_RawData->SetFakeFissionStatus(true);
      m_RawData->SetTimePrevHF(m_TimeHF_prev);
      m_RawData->SetTimeHF(m_TimeHF_current);
      m_RawData->SetTimeCfd(-1);
    }
  } 
  else if (alias == QDC_TDC_X4_TYPE_ALIAS) {
    nptool::message("yellow", "epic",
                    "Epic::BuildRawEvent",
                    "Warning: QDC_TDC_X4_TYPE_ALIAS found on label " + label);
  } 
  else if (alias == SAMPLER_DATA_TYPE_ALIAS) {

    if (label.front() == 'F') {
      m_total_raw_event++;  // total number of counts in all anodes, if mult=2, m_total_raw_event is incremented by 2

      // identification of the electronic channel
      index = Label2index(label);
      anode = Label2anode(label);
      det = Label2det(label);

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
      for (int i = 0; i < nbr_of_samples; i++){
        Signal.push_back(samp.data[i] - BLRoffset);
      }


      // initialize arguments of EpicSamplerProcessor class
      bool   ApplyFilter = false;
      EpicSamplerProcessor sample(Signal, ApplyFilter); 
      // calculate CFD time
      double Qmax   = -10000.;
      double T_qmax = -20000.; 
      double T_cfd  ;
      static bool FC_Triggered = false;
      static bool FC_Threshold = false;

      T_cfd = sample.calculateCFDnew(
          m_cfd_fract[index], m_cfd_delay[index], m_cfd_thres[index], 
          faster_sample_size_ns,
          Qmax, T_qmax, FC_Triggered, FC_Threshold);


      double Q1=-1;
      double Q2=-1;
      double Q3=-1;
      if (FC_Triggered && FC_Threshold && T_cfd != -10000) {
        m_good_raw_event++;
        double signal_size = Signal.size();

        /////
        /// gate integration
        /////
        double start, stop;
        // Q1 - long gate
        start = max(0., T_cfd - m_Q1_gate_start[index]);           // -: before Tcfd
        stop = min(T_cfd + m_Q1_gate_stop[index],signal_size * 2); // +: after Tcfd
        if (start<stop) Q1 = sample.integrateSignal(2, start, stop); // TODO check bounding of integration
        // Q2 - gate around the rising time
        start = max(0., T_cfd - m_Q2_gate_start[index]);           // -: before Tcfd
        stop = min(T_cfd + m_Q2_gate_stop[index],signal_size * 2); // +: after Tcfd
        if (start<stop) Q2 = sample.integrateSignal(2, start, stop);
        // Q3 - gate around the falling time
        start = min(signal_size * 2,T_cfd + m_Q3_gate_start[index]); // +: after Tcfd
        stop = min(signal_size * 2,T_cfd + m_Q3_gate_stop[index]);   // +: after Tcfd
        if (start<stop) Q3 = sample.integrateSignal(2, start, stop);

        if(Q1>0 && Q2>0 && Q3>0){
            double TimeFC = (double)timestamp + (double)T_cfd - sampler_before_threshold_ns;
            double tof_raw = TimeFC - m_TimeHF_current ; 
            if(tof_raw < m_TofRaw_max[index] || m_TofRaw_max[index] < 0 ){
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
                m_RawData->SetFakeFissionStatus(false);
                if(m_RawData->GetFCMult()==1){ 
                    // no need to overwrite the same data
                    m_RawData->SetTimePrevHF(m_TimeHF_prev);
                    m_RawData->SetTimeHF(m_TimeHF_current);
                }

                // sample for anode with Qmax
                if (m_Get_Sampler_Qmax == 1) {
                  if(m_RawData->GetFCMult()==1){
                        m_RawData->SetSampler(Signal);
                        m_RawData->SetQmaxIndex(0);
                  }
                  else{
                    if(Qmax > m_RawData->GetQmax(m_RawData->GetQmaxIndex())){
                        m_RawData->SetSampler(Signal);
                        m_RawData->SetQmaxIndex(m_RawData->GetFCMult()-1);
                    }
                  }
                }
                else{
                        m_RawData->SetQmaxIndex(-1);
                }
            }// end of rejection or not of events as a function of its tof_raw
            /////// MACRO TO DRAW SIGNALS
            /*if(ID ==1){
              auto c = new TCanvas();
              c->Divide(2,1);
              c->cd(1);
              signalProcessor.drawRawSignal();
              auto l = new TLine(startT,-100000,startT,100000);
              l->SetLineWidth(3);
              l->SetLineColor(4);
              l->Draw();
              auto l2 = new TLine(endT,-100000,endT,100000);
              l2->SetLineWidth(3);
              l2->SetLineColor(2);
              l2->Draw();
              auto l4 = new TLine(endT2,-100000,endT2,100000);
              l4->SetLineWidth(3);
              l4->SetLineColor(kRed-8);
              l4->Draw();
              auto l3 = new TLine(T_cfd,-100000,T_cfd,100000);
              l3->SetLineWidth(3);
              l3->SetLineColor(6);
              l3->Draw();

              c->cd(2);
              sample.drawSignalCFD();
              l3->Draw();
              cout << ID << " Tcfd: " << T_cfd << " Q1: " << Qlong << " Q2: " <<
              Q2
              << endl; gPad->WaitPrimitive(); c->Close();
              }*/
              ///////////////////// END MACRO
            }// end if Qi>0
      } // If FC_Triggered && FC_Threshold && T_cfd
      else {
        if (Qmax > 350)  m_event_to_recovered++;
      }
    } // end FC sampler
  } // end if SAMPLER data
  else {
    nptool::message("yellow", "epic",
                    "Epic::BuildRawEvent",
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
unsigned int EpicDetector::GetIndex(int det, int anode){
    int index = 0 ;
    for(int i = 0 ; i < det; i++) index += i * m_nAnodes[i];
    index += anode - 1;
    return index;
    
}
////////////////////////////////////////////////////////////////////////////////
unsigned int EpicDetector::Label2index(const std::string &label) {
  // generic to handle FC_det_anode or FC_anode
  int id ;
  string number;
  size_t pos1 = label.find("_");
  size_t pos2 = label.find("_", pos1 + 1);
  if (pos2 == string::npos) { // format: FC_anode => only one FC
    number = label.substr(pos1 + 1);
    id = stoi(number) - 1;
  } else { // format: FC_det_anode
    number = label.substr(pos1 + 1, pos2 - pos1 - 1);
    int det = stoi(number);
    number = label.substr(pos2 + 1);
    int anode = stoi(number);
    id = GetIndex(det,anode);
  }
  return id;
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
