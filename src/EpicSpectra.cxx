#include "EpicSpectra.h"
#include "NPApplication.h"

using namespace epic;

////////////////////////////////////////////////////////////////////////////////
TCanvas *EpicSpectra::CreateCanvas(const std::string &name, int ncol) {
  TCanvas *c = new TCanvas(name.c_str(), name.c_str());
  c->Divide(ncol, 2);
  return c;
}

////////////////////////////////////////////////////////////////////////////////
EpicSpectra::EpicSpectra() {

  std::cout << "//// EpicSpectra::EpicSpectra()" << std::endl;

  // Set Pointers:
  m_detector = std::dynamic_pointer_cast<EpicDetector>(
      nptool::Application::GetApplication()->GetDetector("epic"));
  m_RawData = m_detector->GetRawData();
  m_Physics = m_detector->GetPhysics();

  // get some config constants
  unsigned int nDets = m_detector->GetNumberOfDets();
  unsigned int nAnodesTot = m_detector->GetNumberOfAnodesTot();
  std::vector<unsigned int> nAnodes = m_detector->GetNumberOfAnodes();
  std::vector<std::string> actinide = m_detector->GetActinideMaterial();

  // general histograms
  m_canT0 = new TCanvas("T0", "T0", 1200, 800);
  m_canT0->Divide(1, 2);

  m_TimeHF = new TH1F("TimeHF", "TimeHF", 86400, 0, 86400);
  m_DeltaTimeHF = new TH1F("DT_HF_ifBeamOn", "DT_HF_ifBeamOn", 5000, 0, 5);

  m_TimeHF->GetXaxis()->SetTitle("TimeHF [s] 1s/bin");
  m_DeltaTimeHF->GetXaxis()->SetTitle("Delta TimeHF [ms] 1us/bin");

  m_canT0->cd(1); m_TimeHF->Draw();
  m_canT0->cd(2); gPad->SetLogy();  m_DeltaTimeHF->Draw();

  // loop over nDets
  for (unsigned int d = 1; d <= nDets; d++) {

    // create the canvas and histos per detector
    int ncol = ceil(0.5 * nAnodes[d - 1]);
    string base = "EPIC" + to_string(d);
    string can_name;
    string his_name;

    can_name = base + "_MULT";
    m_raw_can[can_name] = new TCanvas(can_name.c_str(), can_name.c_str());
    his_name = base + "_Mult"; 
    m_raw_h1[his_name] = new TH1F(his_name.c_str(),his_name.c_str(), 13, -0.5, 12.5);
    m_raw_can[can_name]->cd();
    m_raw_h1[his_name]->Draw();

    can_name = base + "_A";
    m_raw_can[can_name] = new TCanvas(can_name.c_str(), can_name.c_str());
    his_name = base + "_AnodeID";
    m_raw_h1[his_name] = new TH1F(his_name.c_str(),his_name.c_str(), 13, -0.5, 12.5);
    m_raw_can[can_name]->cd();
    m_raw_h1[his_name]->Draw();
    his_name = base + "_AnodeID_ifQmax";
    m_raw_h1[his_name] = new TH1F(his_name.c_str(),his_name.c_str(), 13, -0.5, 12.5);
    m_raw_h1[his_name]->SetLineColor(kRed);
    m_raw_can[can_name]->cd();
    m_raw_h1[his_name]->Draw("same");
    
    can_name = base + "_Q1vA";
    m_raw_can[can_name] = new TCanvas(can_name.c_str(), can_name.c_str());
    his_name = base + "_Q1vAnodeID";
    m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),13, -0.5, 12.5, 1500, 0, 300000);
    m_raw_can[can_name]->cd();
    m_raw_h2[his_name]->Draw("colz");
    
    can_name = base + "_Q1vT";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);
    
    can_name = base + "_Q2Q3vQ1";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);
    
    can_name = base + "_Q2vQ1";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);
    
    can_name = base + "_QmaxvQ1";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);
    
    can_name = base + "_Q";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);
    
    can_name = base + "_Qmax";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);
    
    can_name = base + "_Tof";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);
    
    can_name = base + "_DT_Tqmax_Tcfd";
    m_raw_can[can_name] = CreateCanvas(can_name, ncol);


    // loop over the anodes and create the histo per anode
    for (unsigned int a = 1; a <= nAnodes[d - 1]; a++) {

      int i = m_detector->GetIndex(d, a);

      ostringstream name;
      name << "det" << d << "_A" << std::setw(2) << std::setfill('0') << a << "_" << actinide[i];
      string prefix = name.str();

      his_name = prefix + "_Q1vT";
      m_raw_h2[his_name] = new TH2F(his_name.c_str(), (his_name + "_Q1vT").c_str(), 1440,0, 86400, 1000, 0, 200000);
      m_raw_h2[his_name]->GetXaxis()->SetTitle("Time [s] : 60s / bin");
      m_raw_h2[his_name]->GetYaxis()->SetTitle("Q1");
      can_name = base + "_Q1vT";
      m_raw_can[can_name]->cd(a);
      gPad->SetLogz(); 
      m_raw_h2[his_name]->Draw("colz");

      his_name = prefix + "_Q2Q3vQ1";
      m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1500, 0, 300000, 500, 0, 10);
      can_name = base + "_Q2Q3vQ1";
      m_raw_can[can_name]->cd(a);
      gPad->SetLogz(); 
      m_raw_h2[his_name]->Draw("colz");

      his_name = prefix + "_Q2vQ1";
      m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1500, 0, 300000, 500, 0, 200000);
      can_name = base + "_Q2vQ1";
      m_raw_can[can_name]->cd(a);
      gPad->SetLogz(); 
      m_raw_h2[his_name]->Draw("colz");

      his_name = prefix + "_QmaxvQ1";
      m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 200000, 200, 0, 20000);
      can_name = base + "_QmaxvQ1";
      m_raw_can[can_name]->cd(a);
      gPad->SetLogz(); 
      m_raw_h2[his_name]->Draw("colz");

      can_name = base + "_Q";
      his_name = prefix + "_Q1";
      m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
      m_raw_can[can_name]->cd(a);
      m_raw_h1[his_name]->Draw();

      his_name = prefix + "_Q2";
      m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
      m_raw_h1[his_name]->SetLineColor(8);
      m_raw_can[can_name]->cd(a);
      m_raw_h1[his_name]->Draw("same");

      his_name = prefix + "_Q3";
      m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
      m_raw_h1[his_name]->SetLineColor(kCyan);
      m_raw_can[can_name]->cd(a);
      m_raw_h1[his_name]->Draw("same");

      his_name = prefix + "_Qmax";
      m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(), 2500, 0, 25000);
      can_name = base + "_Qmax";
      m_raw_can[can_name]->cd(a);
      gPad->SetLogy();
      m_raw_h1[his_name]->Draw();

      his_name = prefix + "_Tof";
      m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  3000, -200, 2800);
      can_name = base + "_Tof";
      m_raw_can[can_name]->cd(a);
      gPad->SetLogy();
      m_raw_h1[his_name]->Draw();

      his_name = prefix + "_Tqmax_Tcfd";
      m_raw_h1[his_name]  = new TH1F(his_name.c_str(),his_name.c_str(), 1000, -50, 50);
      can_name = base + "_DT_Tqmax_Tcfd";
      m_raw_can[can_name]->cd(a);
      m_raw_h1[his_name]->Draw();


    } // end of loop over nAnodes[d-1]
  } // end of loop over nDets
}

////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillRaw() {

  ostringstream name;
  string baseD;
  string baseA;
  string his_name;

  unsigned int nAnodesTot = m_detector->GetNumberOfAnodesTot();
  unsigned int nDets = m_detector->GetNumberOfDets();
  std::vector<unsigned int> nAnodes = m_detector->GetNumberOfAnodes();
  std::vector<string> actinide = m_detector->GetActinideMaterial();

  int FC_mult = m_RawData->GetFCMult();
  if (FC_mult > 0) {
    if (m_RawData->GetDetNbr(0) == -1) {
      m_TimeHF->Fill(m_RawData->GetTimeHF() * 1.e-09);
      m_DeltaTimeHF->Fill((m_RawData->GetTimeHF() - m_RawData->GetTimePrevHF()) * 1.e-06);
    } 
    else {
      int multPerFC[nDets];
      int IndexMax[nDets];
      double Qmax[nDets];
      for (int d = 0; d < nDets; d++) {
        multPerFC[d] = 0;
        IndexMax[d] = -1;
        Qmax[d] = 0.;
      }

      // loop over the raw data
      // search for qmax per FC
      for (int i = 0; i < FC_mult; i++) {
        if (m_RawData->GetPulserTrig(i))
          continue;
        int det = m_RawData->GetDetNbr(i);
        if (det < 0)
          continue;
        int anode = m_RawData->GetAnodeNbr(i);
        double qmax = m_RawData->GetQmax(i);
        multPerFC[det - 1]++;
        his_name = "EPIC" + to_string(det) + "_AnodeID";
        m_raw_h1[his_name]->Fill(anode);
        if (qmax > Qmax[det - 1]) {
          Qmax[det - 1] = qmax;
          IndexMax[det - 1] = i;
        }
      } // end of loop over the raw data

      // spectra for Qmax only: suppress cross-talk
      for (int d = 0; d < nDets; d++) {
         baseD = "EPIC" + to_string(d+1);
         his_name = baseD + "_MULT";
         //m_raw_h1[his_name]->Fill(multPerFC[d]);
         if (Qmax[d] > 0 && IndexMax[d] >= 0 && multPerFC[d] > 0) {
            int det = m_RawData->GetDetNbr(IndexMax[d]);
            if (det != (d + 1)) std::cout << "ERROR: didn't recover Qmax data to fill raw spectra" << std::endl;
            int anode = m_RawData->GetAnodeNbr(IndexMax[d]);
            int index = m_detector->GetIndex(det, anode);
            name.str("");
            name.clear();
            name << "det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[index];
            baseA = name.str();
            double qm = m_RawData->GetQmax(IndexMax[d]);
            double q1 = m_RawData->GetQ1(IndexMax[d]);
            double q2 = m_RawData->GetQ2(IndexMax[d]);
            double q3 = m_RawData->GetQ3(IndexMax[d]);
            double t_fc = m_RawData->GetTimeFC(IndexMax[d]);
            double t_hf = m_RawData->GetTimeLastHF();
            double t_qmax = m_RawData->GetTimeQmax(IndexMax[d]);
            double t_cfd = m_RawData->GetTimeCfd(IndexMax[d]);
            his_name = baseD + "_AnodeID_ifQmax";    m_raw_h1[his_name]->Fill(anode);
            //his_name = baseD + "_Q1vAnodeID";        m_raw_h2[his_name]->Fill(anode, q1);
            //his_name = baseA + "_Q1vT";              m_raw_h2[his_name]->Fill(t_fc * 1.e-9, q1);
            //his_name = baseA + "_Q2vQ1";             m_raw_h2[his_name]->Fill(q1, q2);
            //his_name = baseA + "_QmaxvQ1";           m_raw_h2[his_name]->Fill(q1, qm);
            //his_name = baseA + "_Q1";                m_raw_h1[his_name]->Fill(q1);
            //his_name = baseA + "_Q2";                m_raw_h1[his_name]->Fill(q2);
            //his_name = baseA + "_Q3";                m_raw_h1[his_name]->Fill(q3);
            //his_name = baseA + "_Qmax";              m_raw_h1[his_name]->Fill(qm);
            //his_name = baseA + "_Tof";               m_raw_h1[his_name]->Fill(t_fc - t_hf);
            //his_name = baseA + "_Tqmax_Tcfd";        m_raw_h1[his_name]->Fill(t_qmax - t_cfd);
            //if (q3 > 0){
            //    his_name = baseA + "_Q2Q3vQ1";      m_raw_h2[his_name]->Fill(q1, q2 / q3);
            //}
        }
      }
    } // end of if else Det[0] != -1
  } // end of if FC_mult > 0 
}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillPhy() {}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::Clear() {

  // === raw histograms
  for (auto h : m_raw_h1)        h.second->Reset();
  for (auto h : m_raw_h2)        h.second->Reset();
  // === phys histograms
  for (auto h : m_phys_h1)       h.second->Reset();
  for (auto h : m_phys_h2)       h.second->Reset();
  // === general histograms
  m_TimeHF->Reset();
  m_DeltaTimeHF->Reset();
}
