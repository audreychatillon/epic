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
  m_app = nptool::Application::GetApplication();
  m_detector = std::dynamic_pointer_cast<EpicDetector>(nptool::Application::GetApplication()->GetDetector("epic"));
  m_RawData = m_detector->GetRawData();
  m_Physics = m_detector->GetPhysics();
  TDirectory* dir_raw = gROOT->mkdir("raw");
  TDirectory* dir_phy = gROOT->mkdir("phy");
  
  m_Cal.InitCalibration();
  
  // get some config constants
  nDets = m_detector->GetNumberOfDets();
  nAnodesTot = m_detector->GetNumberOfAnodesTot();
  nAnodes  = m_detector->GetNumberOfAnodes();
  actinide = m_detector->GetActinideMaterial();
  anodes   = m_detector->GetAnodeNumber();
  actinides_per_det = m_detector->GetActinidesPerDet();
  mass_per_actinide = m_detector->GetMassPerActinide();

  // JEFF4-0 cross sections
  ifstream XSfile("doc/XS_JEFF4-0_235U_238U_242Pu.dat");	
  if (!XSfile.is_open()) {
    cerr << "Erreur : impossible d'ouvrir le fichier doc/XS_JEFF4-0_235U_238U_242Pu.dat" << endl;
  }
  else{
    vector<double> XS_JEFF_235U;
    vector<double> XS_JEFF_238U;
    vector<double> XS_JEFF_242Pu;
    vector<double> XS_JEFF_nENE;
    vector<double> XS_JEFF_ratioU5_235U;
    vector<double> XS_JEFF_ratioU5_238U;
    vector<double> XS_JEFF_ratioU5_242Pu;
    vector<double> XS_JEFF_ratioU8_235U;
    vector<double> XS_JEFF_ratioU8_238U;
    vector<double> XS_JEFF_ratioU8_242Pu;
    double e, u8, u5, pu42;  //e is in eV
    while(XSfile >> e >> u5 >> u8 >> pu42){
      if (e<=20000000){
        XS_JEFF_nENE.push_back(e*1.e-06);
        XS_JEFF_235U.push_back(u5);
        XS_JEFF_238U.push_back(u8);
        XS_JEFF_242Pu.push_back(pu42);
        XS_JEFF_ratioU5_235U.push_back(u5/u5);
        XS_JEFF_ratioU5_238U.push_back(u8/u5);
        XS_JEFF_ratioU5_242Pu.push_back(pu42/u5);
        XS_JEFF_ratioU8_235U.push_back(u5/u8);
        XS_JEFF_ratioU8_238U.push_back(u8/u8);
        XS_JEFF_ratioU8_242Pu.push_back(pu42/u8);
      }
    }
    XSfile.close();
    //ratio U5
    m_phy_gr["XSratio_JEFF_235U_235U"]  = new TGraph(XS_JEFF_nENE.size(),XS_JEFF_nENE.data(),XS_JEFF_ratioU5_235U.data());   	
    m_phy_gr["XSratio_JEFF_235U_235U"]->SetName("XSratio_JEFF40_235U_235U");
    m_phy_gr["XSratio_JEFF_235U_235U"]->SetTitle("XSratio_JEFF40_235U_235U");
    m_phy_gr["XSratio_JEFF_235U_235U"]->SetLineColor(kRed);
    m_phy_gr["XSratio_JEFF_235U_235U"]->SetLineWidth(2);
    m_phy_gr["XSratio_JEFF_238U_235U"]  = new TGraph(XS_JEFF_nENE.size(),XS_JEFF_nENE.data(),XS_JEFF_ratioU5_238U.data());   	
    m_phy_gr["XSratio_JEFF_238U_235U"]->SetName("XSratio_JEFF40_238U_235U");
    m_phy_gr["XSratio_JEFF_238U_235U"]->SetTitle("XSratio_JEFF40_238U_235U");
    m_phy_gr["XSratio_JEFF_238U_235U"]->SetLineColor(kRed);
    m_phy_gr["XSratio_JEFF_238U_235U"]->SetLineWidth(2);
    m_phy_gr["XSratio_JEFF_242Pu_235U"] = new TGraph(XS_JEFF_nENE.size(),XS_JEFF_nENE.data(),XS_JEFF_ratioU5_242Pu.data());   	
    m_phy_gr["XSratio_JEFF_242Pu_235U"]->SetName("XSratio_JEFF40_242Pu_235U");
    m_phy_gr["XSratio_JEFF_242Pu_235U"]->SetTitle("XSratio_JEFF40_242Pu_235U");
    m_phy_gr["XSratio_JEFF_242Pu_235U"]->SetLineColor(kRed);
    m_phy_gr["XSratio_JEFF_242Pu_235U"]->SetLineWidth(2);
    //ratio U8
    m_phy_gr["XSratio_JEFF_235U_238U"]  = new TGraph(XS_JEFF_nENE.size(),XS_JEFF_nENE.data(),XS_JEFF_ratioU8_235U.data());   	
    m_phy_gr["XSratio_JEFF_235U_238U"]->SetName("XSratio_JEFF40_235U_238U");
    m_phy_gr["XSratio_JEFF_235U_238U"]->SetTitle("XSratio_JEFF40_235U_238U");
    m_phy_gr["XSratio_JEFF_235U_238U"]->SetLineColor(kRed);
    m_phy_gr["XSratio_JEFF_235U_238U"]->SetLineWidth(2);
    m_phy_gr["XSratio_JEFF_238U_238U"]  = new TGraph(XS_JEFF_nENE.size(),XS_JEFF_nENE.data(),XS_JEFF_ratioU8_238U.data());   	
    m_phy_gr["XSratio_JEFF_238U_238U"]->SetName("XSratio_JEFF40_238U_238U");
    m_phy_gr["XSratio_JEFF_238U_238U"]->SetTitle("XSratio_JEFF40_238U_238U");
    m_phy_gr["XSratio_JEFF_238U_238U"]->SetLineColor(kRed);
    m_phy_gr["XSratio_JEFF_238U_238U"]->SetLineWidth(2);
    m_phy_gr["XSratio_JEFF_242Pu_238U"] = new TGraph(XS_JEFF_nENE.size(),XS_JEFF_nENE.data(),XS_JEFF_ratioU8_242Pu.data());   	
    m_phy_gr["XSratio_JEFF_242Pu_238U"]->SetName("XSratio_JEFF40_242Pu_238U");
    m_phy_gr["XSratio_JEFF_242Pu_238U"]->SetTitle("XSratio_JEFF40_242Pu_238U");
    m_phy_gr["XSratio_JEFF_242Pu_238U"]->SetLineColor(kRed);
    m_phy_gr["XSratio_JEFF_242Pu_238U"]->SetLineWidth(2);
  }
  
  // BVIII cross sections
  ifstream XSfile2("doc/XS_BVIII_235U_238U_242Pu.dat");	
  if (!XSfile2.is_open()) {
    cerr << "Erreur : impossible d'ouvrir le fichier doc/XS_BVIII_235U_238U_242Pu.dat" << endl;
  }
  else{
    vector<double> XS_BVIII_235U;
    vector<double> XS_BVIII_238U;
    vector<double> XS_BVIII_242Pu;
    vector<double> XS_BVIII_nENE;
    vector<double> XS_BVIII_ratioU5_235U;
    vector<double> XS_BVIII_ratioU5_238U;
    vector<double> XS_BVIII_ratioU5_242Pu;
    vector<double> XS_BVIII_ratioU8_235U;
    vector<double> XS_BVIII_ratioU8_238U;
    vector<double> XS_BVIII_ratioU8_242Pu;
    double e, u8, u5, pu42;  //e is in eV
    while(XSfile2 >> e >> u5 >> u8 >> pu42){
      if (e<=20000000){
        XS_BVIII_nENE.push_back(e*1.e-06);
        XS_BVIII_235U.push_back(u5);
        XS_BVIII_238U.push_back(u8);
        XS_BVIII_242Pu.push_back(pu42);
        XS_BVIII_ratioU5_235U.push_back(u5/u5);
        XS_BVIII_ratioU5_238U.push_back(u8/u5);
        XS_BVIII_ratioU5_242Pu.push_back(pu42/u5);
        XS_BVIII_ratioU8_235U.push_back(u5/u8);
        XS_BVIII_ratioU8_238U.push_back(u8/u8);
        XS_BVIII_ratioU8_242Pu.push_back(pu42/u8);
      }
    }
    XSfile2.close();
    // ratio U5
    m_phy_gr["XSratio_BVIII_235U_235U"]  = new TGraph(XS_BVIII_nENE.size(),XS_BVIII_nENE.data(),XS_BVIII_ratioU5_235U.data());   	
    m_phy_gr["XSratio_BVIII_235U_235U"]->SetName("XSratio_BVIII40_235U_235U");
    m_phy_gr["XSratio_BVIII_235U_235U"]->SetTitle("XSratio_BVIII40_235U_235U");
    m_phy_gr["XSratio_BVIII_235U_235U"]->SetLineColor(kBlack);
    m_phy_gr["XSratio_BVIII_235U_235U"]->SetLineStyle(2);
    m_phy_gr["XSratio_BVIII_235U_235U"]->SetLineWidth(3);
    m_phy_gr["XSratio_BVIII_238U_235U"]  = new TGraph(XS_BVIII_nENE.size(),XS_BVIII_nENE.data(),XS_BVIII_ratioU5_238U.data());   	
    m_phy_gr["XSratio_BVIII_238U_235U"]->SetName("XSratio_BVIII40_238U_235U");
    m_phy_gr["XSratio_BVIII_238U_235U"]->SetTitle("XSratio_BVIII40_238U_235U");
    m_phy_gr["XSratio_BVIII_238U_235U"]->SetLineColor(kBlack);
    m_phy_gr["XSratio_BVIII_238U_235U"]->SetLineStyle(2);
    m_phy_gr["XSratio_BVIII_238U_235U"]->SetLineWidth(3);
    m_phy_gr["XSratio_BVIII_242Pu_235U"] = new TGraph(XS_BVIII_nENE.size(),XS_BVIII_nENE.data(),XS_BVIII_ratioU5_242Pu.data());   	
    m_phy_gr["XSratio_BVIII_242Pu_235U"]->SetName("XSratio_BVIII40_242Pu_235U");
    m_phy_gr["XSratio_BVIII_242Pu_235U"]->SetTitle("XSratio_BVIII40_242Pu_235U");
    m_phy_gr["XSratio_BVIII_242Pu_235U"]->SetLineColor(kBlack);
    m_phy_gr["XSratio_BVIII_242Pu_235U"]->SetLineStyle(2);
    m_phy_gr["XSratio_BVIII_242Pu_235U"]->SetLineWidth(3);
    // ratio U8
    m_phy_gr["XSratio_BVIII_235U_238U"]  = new TGraph(XS_BVIII_nENE.size(),XS_BVIII_nENE.data(),XS_BVIII_ratioU8_235U.data());   	
    m_phy_gr["XSratio_BVIII_235U_238U"]->SetName("XSratio_BVIII40_235U_235U");
    m_phy_gr["XSratio_BVIII_235U_238U"]->SetTitle("XSratio_BVIII40_235U_235U");
    m_phy_gr["XSratio_BVIII_235U_238U"]->SetLineColor(kBlack);
    m_phy_gr["XSratio_BVIII_235U_238U"]->SetLineStyle(2);
    m_phy_gr["XSratio_BVIII_235U_238U"]->SetLineWidth(3);
    m_phy_gr["XSratio_BVIII_238U_238U"]  = new TGraph(XS_BVIII_nENE.size(),XS_BVIII_nENE.data(),XS_BVIII_ratioU8_238U.data());   	
    m_phy_gr["XSratio_BVIII_238U_238U"]->SetName("XSratio_BVIII40_238U_235U");
    m_phy_gr["XSratio_BVIII_238U_238U"]->SetTitle("XSratio_BVIII40_238U_235U");
    m_phy_gr["XSratio_BVIII_238U_238U"]->SetLineColor(kBlack);
    m_phy_gr["XSratio_BVIII_238U_238U"]->SetLineStyle(2);
    m_phy_gr["XSratio_BVIII_238U_238U"]->SetLineWidth(3);
    m_phy_gr["XSratio_BVIII_242Pu_238U"] = new TGraph(XS_BVIII_nENE.size(),XS_BVIII_nENE.data(),XS_BVIII_ratioU8_242Pu.data());   	
    m_phy_gr["XSratio_BVIII_242Pu_238U"]->SetName("XSratio_BVIII40_242Pu_238U");
    m_phy_gr["XSratio_BVIII_242Pu_238U"]->SetTitle("XSratio_BVIII40_242Pu_238U");
    m_phy_gr["XSratio_BVIII_242Pu_238U"]->SetLineColor(kBlack);
    m_phy_gr["XSratio_BVIII_242Pu_238U"]->SetLineStyle(2);
    m_phy_gr["XSratio_BVIII_242Pu_238U"]->SetLineWidth(3);
  }

  timehf_ref_raw = 0;
  timefc_ref_raw = 0;
  timefc_ref_phy = 0;

  // general histograms
  m_canT0 = new TCanvas("T0", "T0", 1200, 800);
  m_canT0->Divide(1, 2);

  m_TimeHF = new TH1F("TimeHF", "TimeHF", 86400, 0, 86400);
  m_HF_DeltaTimeHF = new TH1F("DT_TimeHF_fHF_ifBeamOn", "DT_TimeHF_fHF_ifBeamOn", 5100, 0, 5100000);
  m_HF_DeltaTimeHF->SetLineColor(kBlack);

  m_TimeHF->GetXaxis()->SetTitle("TimeHF [s] 1s/bin");
  m_HF_DeltaTimeHF->GetXaxis()->SetTitle("Delta TimeHF [ns]");

  m_canT0->cd(1); m_TimeHF->Draw();
  m_canT0->cd(2); gPad->SetLogy();  m_HF_DeltaTimeHF->Draw(); 

  // loop over nDets for raw histos
  if(m_app->HasFlag("--input-raw")){
    dir_raw->cd();
    size_t offset = 0;
    for (short det = 1; det <= nDets; det++) {
      // create the canvas and histos per detector
      int ncol = ceil(0.5 * nAnodes[det - 1]);
      string baseD = "EPIC" + to_string(det);
      string can_name;
      string his_name;

      can_name = baseD + "_MULT";
      m_raw_can[can_name] = new TCanvas(can_name.c_str(), can_name.c_str());
      his_name = baseD + "_Mult"; 
      m_raw_h1[his_name] = new TH1F(his_name.c_str(),his_name.c_str(), 13, -0.5, 12.5);
      m_raw_can[can_name]->cd();
      m_raw_can[can_name]->cd();
      gPad->SetLogy();
      m_raw_h1[his_name]->Draw();

      can_name = baseD + "_A";
      m_raw_can[can_name] = new TCanvas(can_name.c_str(), can_name.c_str());
      his_name = baseD + "_AnodeID";
      m_raw_h1[his_name] = new TH1F(his_name.c_str(),his_name.c_str(), 13, -0.5, 12.5);
      m_raw_can[can_name]->cd();
      m_raw_h1[his_name]->Draw();
      his_name = baseD + "_AnodeID_ifQmax";
      m_raw_h1[his_name] = new TH1F(his_name.c_str(),his_name.c_str(), 13, -0.5, 12.5);
      m_raw_h1[his_name]->SetLineColor(kRed);
      m_raw_can[can_name]->cd();
      m_raw_h1[his_name]->Draw("same");
      
      can_name = baseD + "_Q1vA";
      m_raw_can[can_name] = new TCanvas(can_name.c_str(), can_name.c_str());
      his_name = baseD + "_Q1vAnodeID";
      m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),13, -0.5, 12.5, 1500, 0, 300000);
      m_raw_can[can_name]->cd();
      m_raw_h2[his_name]->Draw("colz");
      
      can_name = baseD + "_Q";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q4";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q1vT";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q2Q3vQ1";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q4QmaxvQ1";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_WaveformA";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_WaveformF";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q2vQ1";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q2vQ3";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_QmaxvQ1";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q4vQmax";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q4vQ1";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Qmax";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_TofRaw";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_TofRaw_zoom";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_TofRaw_offset";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q1vTofRaw";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_DT_Tqmax_Tcfd";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_DTvQ1";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q2Q3vQ1_mult1";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q2Q3vQ1_mult2_upstr";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q2Q3vQ1_mult2_downstr";
      m_raw_can[can_name] = CreateCanvas(can_name, ncol);

      // loop over the anodes and create the histo per anode
      for (short a = 0; a < nAnodes[det - 1]; a++) {
        short anode = anodes[offset + a];
        short i = m_detector->GetIndex(det, anode);

        ostringstream name;
        name << "det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[i];
        string baseA = name.str();

        his_name = baseA + "_Q1vT";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(), 1440,0, 86400, 1000, 0, 200000);
        m_raw_h2[his_name]->GetXaxis()->SetTitle("Time [s] : 60s / bin");
        m_raw_h2[his_name]->GetYaxis()->SetTitle("Q1");
        can_name = baseD + "_Q1vT";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q2Q3vQ1";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 500000, 250, -0.1, 2.4);
        can_name = baseD + "_Q2Q3vQ1";
        m_raw_can[can_name]->cd(a+1);
        //gPad->SetLogx(); 
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");
        vector<double> xA = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_A_X");
        vector<double> yA = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_A_Y");
        his_name = baseA + "_TCutG_discriA";
        m_tcutg[his_name] = new TCutG(his_name.c_str(),xA.size(),xA.data(),yA.data());
        m_tcutg[his_name]->Draw("same");
        vector<double> xF = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_F_X");
        vector<double> yF = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_F_Y");
        his_name = baseA + "_TCutG_discriF";
        m_tcutg[his_name] = new TCutG(his_name.c_str(),xF.size(),xF.data(),yF.data());
        m_tcutg[his_name]->Draw("same");

        his_name = baseA + "_WaveFormA";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),110, -10, 210, 400, -1000, 3000);
        can_name = baseD + "_WaveformA";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_WaveFormF";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),110, -10, 210, 1300, -1000, 12000);
        can_name = baseD + "_WaveformF";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q4QmaxvQ1";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 500, -5, 5);
        m_raw_h2[his_name]->GetXaxis()->SetTitle("Q1");
        m_raw_h2[his_name]->GetYaxis()->SetTitle("Q4/Qmax");
        can_name = baseD + "_Q4QmaxvQ1";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q2vQ1";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 500, 0, 200000);
        can_name = baseD + "_Q2vQ1";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q2vQ3";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),500, 0, 200000, 500, 0, 200000);
        can_name = baseD + "_Q2vQ3";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_QmaxvQ1";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 200, 0, 20000);
        can_name = baseD + "_QmaxvQ1";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz();
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q4vQmax";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),200, 0, 20000,500,-50000,50000);
        m_raw_h2[his_name]->GetXaxis()->SetTitle("Qmax");
        m_raw_h2[his_name]->GetYaxis()->SetTitle("Q4");
        can_name = baseD + "_Q4vQmax";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q4vQ1";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 500, -50000, 50000);
        m_raw_h2[his_name]->GetXaxis()->SetTitle("Q1");
        m_raw_h2[his_name]->GetYaxis()->SetTitle("Q4");
        can_name = baseD + "_Q4vQ1";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        can_name = baseD + "_Q";
        his_name = baseA + "_Q1";
        m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
        m_raw_can[can_name]->cd(a+1);
	      gPad->SetLogy();
        m_raw_h1[his_name]->Draw();

        his_name = baseA + "_Q2";
        m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
        m_raw_h1[his_name]->SetLineColor(8);
        m_raw_can[can_name]->cd(a+1);
        m_raw_h1[his_name]->Draw("same");

        his_name = baseA + "_Q3";
        m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
        m_raw_h1[his_name]->SetLineColor(kCyan);
        m_raw_can[can_name]->cd(a+1);
        m_raw_h1[his_name]->Draw("same");

        his_name = baseA + "_Q4";
        m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),5000, -50000, 50000);
        can_name = baseD + "_Q4";
        m_raw_can[can_name]->cd(a+1);
        m_raw_h1[his_name]->Draw();

        his_name = baseA + "_Qmax";
        m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(), 2500, 0, 25000);
        can_name = baseD + "_Qmax";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_raw_h1[his_name]->Draw();

        his_name = baseA + "_TofRaw";
        m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  2600, -10000, 2590000);
        m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 1us / bin ");
        can_name = baseD + "_TofRaw";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_raw_h1[his_name]->Draw();
        his_name = baseA + "_TofRaw_cutF";
        m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  2600, -10000, 2590000);
        m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 1us / bin ");
        can_name = baseD + "_TofRaw";
        m_raw_can[can_name]->cd(a+1);
        m_raw_h1[his_name]->SetLineColor(kRed+3);
        m_raw_h1[his_name]->Draw("same");

        int gp = (int)m_Cal.GetValue("EPIC_"+ to_string(det)+"_ANODE_"+to_string(anode) + "_GAMMA_PEAK_RAW",0);

        his_name = baseA + "_TofRaw_cutF_zoom";
        m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  20000, gp-200, gp+1800);
        m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin ");
        can_name = baseD + "_TofRaw_zoom";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_raw_h1[his_name]->Draw();

        his_name = baseA + "_TofRaw_cutF_offset";
        m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  20000, 0, 2000);
        m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin ");
        can_name = baseD + "_TofRaw_offset";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_raw_h1[his_name]->Draw();

        his_name = baseA + "_Q1vTofRaw";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(), 1000, gp-200, gp+1800, 1000, 0, 200000);
        m_raw_h2[his_name]->GetXaxis()->SetTitle("TofRaw [ns] 500 ps / bin");
        m_raw_h2[his_name]->GetYaxis()->SetTitle("Q1");
        can_name = baseD + "_Q1vTofRaw";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Tqmax_Tcfd";
        m_raw_h1[his_name]  = new TH1F(his_name.c_str(),his_name.c_str(), 1000, -10, 90);
        m_raw_h1[his_name]->GetXaxis()->SetTitle("DT = Tqmax - Tcfd");
        can_name = baseD + "_DT_Tqmax_Tcfd";
        m_raw_can[can_name]->cd(a+1);
        m_raw_h1[his_name]->Draw();

        his_name = baseA + "_DTvQ1";
        m_raw_h2[his_name]  = new TH2F(his_name.c_str(),his_name.c_str(),1000,0,400000, 1000, -10, 90);
        m_raw_h2[his_name]->GetXaxis()->SetTitle("Q1");
        m_raw_h2[his_name]->GetYaxis()->SetTitle("DT = Tqmax - Tcfd");
        can_name = baseD + "_DTvQ1";
        m_raw_can[can_name]->cd(a+1);
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q2Q3vQ1_mult1";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 400000, 110, -0.1, 1.1);
        can_name = baseD + "_Q2Q3vQ1_mult1";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q2Q3vQ1_mult2_upstr";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 400000, 110, -0.1, 1.1);
        can_name = baseD + "_Q2Q3vQ1_mult2_upstr";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");

        his_name = baseA + "_Q2Q3vQ1_mult2_downstr";
        m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 400000, 110, -0.1, 1.1);
        can_name = baseD + "_Q2Q3vQ1_mult2_downstr";
        m_raw_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_raw_h2[his_name]->Draw("colz");
      } // end of loop over nAnodes[det-1]
      offset += nAnodes[det-1];
    } // end of loop over nDets

  }// end of if --input raw

  // loop over nDets for phy histos
  if(m_app->HasFlag("--input-phy")){

    dir_phy->cd();
    size_t offset = 0;
    for (short det = 1; det <= nDets; det++) {
      // create the canvas and histos per detector
      int ncol = ceil(0.5 * nAnodes[det - 1]);
      string baseD = "phy_EPIC" + to_string(det);
      string can_name;
      string his_name;
      
      can_name = baseD + "_TofRaw";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_TofRaw_zoom";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_TofCal";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_TofCal_Q1_high_thres";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_E";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_E_Q1_high_thres";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q1vE";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);
      
      can_name = baseD + "_Q2Q3vQ1_nAexcluded";
      m_phy_can[can_name] = CreateCanvas(can_name, ncol);

      // loop over the anodes and create the histo per anode
      for (short a = 0; a < nAnodes[det - 1]; a++) {
        short anode = anodes[offset+a]; 
        short i = m_detector->GetIndex(det, anode);

        ostringstream name;
        name << "phy_det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[i];
        string baseA = name.str();

        int gp = (int)m_Cal.GetValue("EPIC_"+ to_string(det)+"_ANODE_"+to_string(anode) + "_GAMMA_PEAK_PHY",0);

        his_name = baseA + "_TofRaw";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  2600, -2590000, 10000);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 1ns / bin");
        can_name = baseD + "_TofRaw";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

        his_name = baseA + "_TofRaw_cutF_zoom";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(), 25000, gp-600 , gp+1900);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin");
        can_name = baseD + "_TofRaw_zoom";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

        his_name = baseA + "_TofCal";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  25000, -500, 2000);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin");
        can_name = baseD + "_TofCal";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

        his_name = baseA + "_TofCal_Q1_high_thres";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(), 25000, -500, 2000);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin");
        can_name = baseD + "_TofCal_Q1_high_thres";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

        his_name = baseA + "_E";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
        can_name = baseD + "_E";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

        his_name = baseA + "_E_Q1_high_thres";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
        can_name = baseD + "_E_Q1_high_thres";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

        his_name = baseA + "_Q1vE";
        m_phy_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),200,0,20,1500, 0, 300000);
        can_name = baseD + "_Q1vE";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_phy_h2[his_name]->Draw("colz");

        vector<double> xnA = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_Q1vE_nA_X");
        vector<double> ynA = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_Q1vE_nA_Y");
        his_name = baseA + "_TCutG_exclusion_nA";
        m_tcutg[his_name] = new TCutG(his_name.c_str(),xnA.size(),xnA.data(),ynA.data());
        m_tcutg[his_name]->Draw("same");
      
        his_name = baseA + "_Q1_THRES";
        m_phy_gr[his_name] = new TGraph();
        m_phy_gr[his_name]->SetName(his_name.c_str()); 
        m_phy_gr[his_name]->SetTitle(his_name.c_str());
        double tmp_q1 = m_Cal.GetValue("EPIC_"+to_string(det)+"_Q1_THRESHOLD_PERCENT",0) * m_Cal.GetValue("EPIC_"+to_string(det)+"_ANODE_"+to_string(anode)+"_Q1_HIGHEST_CH",0) ;
        m_phy_gr[his_name]->SetPoint(0,0.1,tmp_q1);
        m_phy_gr[his_name]->SetPoint(1,19.9,tmp_q1);
        m_phy_gr[his_name]->SetLineWidth(2) ;
        m_phy_gr[his_name]->Draw("sameLP");

        his_name = baseA + "_Q2Q3vQ1_nAexcluded";
        m_phy_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 500000, 250, -0.1, 2.4);
        can_name = baseD + "_Q2Q3vQ1_nAexcluded";
        m_phy_can[can_name]->cd(a+1);
        gPad->SetLogz(); 
        m_phy_h2[his_name]->Draw("colz");

      } // end of loop over nAnodes[d-1]
      offset += nAnodes[det-1];
      
      can_name = baseD + "_E_per_actinide";
      m_phy_can[can_name] = CreateCanvas(can_name, (short)actinides_per_det[det-1].size());
      
      can_name = baseD + "_Eratio";
      m_phy_can[can_name] = CreateCanvas(can_name, (short)actinides_per_det[det-1].size());
      
      ////can_name = baseD + "_E_per_actinide_Q1_high_thres";
      ////m_phy_can[can_name] = CreateCanvas(can_name, (short)actinides_per_det[det-1].size());
      ////
      ////can_name = baseD + "_Eratio_U235_Q1_high_thres";
      ////m_phy_can[can_name] = CreateCanvas(can_name, (short)actinides_per_det[det-1].size());

      for(short act = 0 ; act < (short)actinides_per_det[det-1].size(); act++){

        ostringstream name;
        name << "phy_det" << det << "_" << actinides_per_det[det-1][act].name;
        string baseA = name.str();

        his_name = baseA + "_E";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
        can_name = baseD + "_E_per_actinide";
        m_phy_can[can_name]->cd(act+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

      ////  his_name = baseA + "_E_Q1_high_thres";
      ////  m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
      ////  m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
      ////  can_name = baseD + "_E_per_actinide_Q1_high_thres";
      ////  m_phy_can[can_name]->cd(act+1);
      ////  gPad->SetLogy();
      ////  m_phy_h1[his_name]->Draw();

        his_name = baseA + "_Eweighted";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
        can_name = baseD + "_E_per_actinide";
        m_phy_can[can_name]->cd((short)actinides_per_det[det-1].size()+act+1);
        gPad->SetLogy();
        m_phy_h1[his_name]->Draw();

      ////  his_name = baseA + "_Eweighted_Q1_high_thres";
      ////  m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
      ////  m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
      ////  can_name = baseD + "_E_per_actinide_Q1_high_thres";
      ////  m_phy_can[can_name]->cd((short)actinides_per_det[det-1].size()+act+1);
      ////  gPad->SetLogy();
      ////  m_phy_h1[his_name]->Draw();

        his_name = baseA + "_Ew_ratioU235";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
        can_name = baseD + "_Eratio";
        m_phy_can[can_name]->cd(act+1);
        m_phy_h1[his_name]->Draw();
        if(m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]){ 
            m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]->Draw("LP same");
        }
        if(m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]){ 
            m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]->Draw("LP same");
        }

        his_name = baseA + "_Ew_ratioU238";
        m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
        m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
        can_name = baseD + "_Eratio";
        m_phy_can[can_name]->cd((short)actinides_per_det[det-1].size()+act+1);
        m_phy_h1[his_name]->Draw();
        if(m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_238U"]){ 
            m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_238U"]->Draw("LP same");
        }
        if(m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_238U"]){ 
            m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_238U"]->Draw("LP same");
        }


      ////  his_name = baseA + "_Ew_ratioU235_Q1_high_thres";
      ////  m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
      ////  m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
      ////  can_name = baseD + "_Eratio_Q1_high_thres";
      ////  m_phy_can[can_name]->cd(act+1);
      ////  m_phy_h1[his_name]->Draw();
      ////  if(m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]){ 
      ////      m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]->Draw("LP same");
      ////      m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]->SetLineColor(kRed);
      ////      m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]->SetLineWidth(2);
      ////  }
      ////  if(m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]){ 
      ////      m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]->Draw("LP same");
      ////      m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]->SetLineColor(kBlack);
      ////      m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]->SetLineStyle(2);
      ////      m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]->SetLineWidth(3);
      ////  }

      ////  can_name = baseD + "_Eratio_Q1_high_thres";
      ////  m_phy_can[can_name]->cd((short)actinides_per_det[det-1].size()+act+1);
      ////  m_phy_h1[his_name]->Draw("HIST");
      ////  his_name = baseA + "_Ew_ratioU235";
      ////  m_phy_h1[his_name]->Draw("HIST same");
      ////  
      ////  if(m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]){ 
      ////      m_phy_gr["XSratio_JEFF_"+actinides_per_det[det-1][act].name+"_235U"]->Draw("LP same");
      ////  }
      ////  if(m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]){ 
      ////      m_phy_gr["XSratio_BVIII_"+actinides_per_det[det-1][act].name+"_235U"]->Draw("LP same");
      ////  }
      }// end of loop over the actinides per detector
    } // end of loop over nDets
  }// end of if --input-phy
}

////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillRaw() {
    if(m_app->HasFlag("--input-raw")){

        ostringstream name;
        string baseD;
        string baseA;
        string his_name;

        int FC_mult = m_RawData->GetFCMult();
        if (FC_mult > 0) {
          bool replay = m_Cal.GetValue("REPLAY_DATA",0);
          if (m_RawData->GetHFIndex()>=0) {
	          // to by-pass nponline bug
            if(!replay){
	            double t_hf = m_RawData->GetTimeHF(); 
 	            if( t_hf - timehf_ref_raw <= 0 && timehf_ref_raw !=0 ) return;
 	            else timehf_ref_raw = t_hf;
            }
	          // fill spectra
            m_TimeHF->Fill(m_RawData->GetTimeHF() * 1.e-09); // s
            m_HF_DeltaTimeHF->Fill(m_RawData->GetDeltaTHF()); //ns
          }

          if (m_RawData->GetQmaxIndex()>=0){
            // init
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

	            // to by-pass nponline bug 
              // this should be commented if processing several runs
              // at new runs restart t from 0
              if(!replay){
	              double t_fc = m_RawData->GetTimeFC(i); 
 	              if( t_fc - timefc_ref_raw <= 0 && timefc_ref_raw != 0 ) return;
 	              else timefc_ref_raw = t_fc;
              }

              if (m_RawData->GetPulserTrig(i))           continue;

              short det = m_RawData->GetDetNbr(i);
              if (det < 0)                               continue;

              short anode = m_RawData->GetAnodeNbr(i);
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
            for (short d = 0; d < nDets; d++) {
               baseD = "EPIC" + to_string(d+1);
               his_name = baseD + "_Mult";
               m_raw_h1[his_name]->Fill(multPerFC[d]);
               if (Qmax[d] > 0 && IndexMax[d] >= 0 && multPerFC[d] > 0) {
                  short det = m_RawData->GetDetNbr(IndexMax[d]);
                  if (det != (d + 1)) std::cout << "ERROR: didn't recover Qmax data to fill raw spectra" << std::endl;
                  short anode = m_RawData->GetAnodeNbr(IndexMax[d]);
                  short index = m_detector->GetIndex(det, anode);
                  name.str("");
                  name.clear();
                  name << "det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[index];
                  baseA = name.str();
                  double qm = m_RawData->GetQmax(IndexMax[d]);
                  double q1 = m_RawData->GetQ1(IndexMax[d]);
                  double q2 = m_RawData->GetQ2(IndexMax[d]);
                  double q3 = m_RawData->GetQ3(IndexMax[d]);
                  double q4 = m_RawData->GetQ4(IndexMax[d]);
                  double t_fc = m_RawData->GetTimeFC(IndexMax[d]);
                  double tofraw = m_RawData->GetTofRaw(IndexMax[d]);
                  double t_qmax = m_RawData->GetTimeQmax(IndexMax[d]);
                  double t_cfd = m_RawData->GetTimeCfd(IndexMax[d]);
                  bool   b_fission = m_RawData->GetIsFission(IndexMax[d]);
                  his_name = baseD + "_AnodeID_ifQmax";    m_raw_h1[his_name]->Fill(anode);
                  his_name = baseD + "_Q1vAnodeID";        m_raw_h2[his_name]->Fill(anode, q1);
                  his_name = baseA + "_Q1vT";              m_raw_h2[his_name]->Fill(t_fc * 1.e-9, q1);
                  his_name = baseA + "_Q2vQ1";             m_raw_h2[his_name]->Fill(q1, q2);
                  his_name = baseA + "_Q2vQ3";             m_raw_h2[his_name]->Fill(q3, q2);
                  his_name = baseA + "_QmaxvQ1";           m_raw_h2[his_name]->Fill(q1, qm);
                  his_name = baseA + "_Q4vQmax";           m_raw_h2[his_name]->Fill(qm, q4);
                  his_name = baseA + "_Q4vQ1";             m_raw_h2[his_name]->Fill(q1, q4);
                  his_name = baseA + "_Q1";                m_raw_h1[his_name]->Fill(q1);
                  his_name = baseA + "_Q2";                m_raw_h1[his_name]->Fill(q2);
                  his_name = baseA + "_Q3";                m_raw_h1[his_name]->Fill(q3);
                  his_name = baseA + "_Q4";                m_raw_h1[his_name]->Fill(q4);
                  his_name = baseA + "_Qmax";              m_raw_h1[his_name]->Fill(qm);
                  his_name = baseA + "_TofRaw";            m_raw_h1[his_name]->Fill(tofraw);
                  his_name = baseA + "_Tqmax_Tcfd";        m_raw_h1[his_name]->Fill(t_qmax - t_cfd);
                  his_name = baseA + "_DTvQ1";             m_raw_h2[his_name]->Fill(q1,t_qmax - t_cfd);
                  his_name = baseA + "_Q1vTofRaw";    m_raw_h2[his_name]->Fill(tofraw, q1);
                  if (q3 > 0){
                      his_name = baseA + "_Q2Q3vQ1";       m_raw_h2[his_name]->Fill(q1, q2 / q3);
                      his_name = baseA + "_TCutG_discriA";
                      if(m_tcutg[his_name]->IsInside(q1, q2 / q3) && IndexMax[d]==m_RawData->GetQmaxIndex()){
                        vector<double> signal = m_RawData->GetSampler();
                        his_name = baseA + "_WaveFormA";   
                        for(int i = 0 ; i < m_RawData->GetSamplerSize(); i++)  m_raw_h2[his_name]->Fill(i*2,signal[i]);
                      }
                      if(b_fission && IndexMax[d]==m_RawData->GetQmaxIndex()){
                        vector<double> signal = m_RawData->GetSampler();
                        his_name = baseA + "_WaveFormF";    
                        for(int i = 0 ; i < m_RawData->GetSamplerSize(); i++)  m_raw_h2[his_name]->Fill(i*2,signal[i]);
                        his_name = baseA + "_TofRaw_cutF";         m_raw_h1[his_name]->Fill(tofraw);
                        his_name = baseA + "_TofRaw_cutF_zoom";    m_raw_h1[his_name]->Fill(tofraw);
                        his_name = baseA + "_TofRaw_cutF_offset";  m_raw_h1[his_name]->Fill(tofraw);
                      }
                      his_name = baseA + "_Q4QmaxvQ1";          m_raw_h2[his_name]->Fill(q1, q4 / qm);
                      if(multPerFC[d]==1) {
			            his_name = baseA + "_Q2Q3vQ1_mult1"; 
			            m_raw_h2[his_name]->Fill(q1, q2 / q3);
		              }
                      if(multPerFC[d]==2) {
			            int index_m2 = 1 - IndexMax[d] ; // multFC==2 -> IndexMax = 0 or = 1
                        int anode_m2 = m_RawData->GetAnodeNbr(index_m2);
			            if ((anode - anode_m2) == 1) { 
				            his_name = baseA + "_Q2Q3vQ1_mult2_upstr"; 
				            m_raw_h2[his_name]->Fill(q1, q2 / q3);
			            }
			            else if ((anode_m2 - anode) == 1) { 
				        his_name = baseA + "_Q2Q3vQ1_mult2_downstr"; 
				        m_raw_h2[his_name]->Fill(q1, q2 / q3);
			          }
                      }  
		          }
                }
            }
          } // end of if else Det[0] != -1
        } // end of if FC_mult > 0
    }// end of if --input-raw
}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillPhy() {
  if(m_app->HasFlag("--input-phy")){

    ostringstream name;
    string baseA;
    string his_name;
    
    // to by-pass nponline bug : comment this lines if you want to process several runs 
    bool replay = m_Cal.GetValue("REPLAY_DATA",0);
    if(!replay){
      double t_fc = m_Physics->GetTime(); 
      if( t_fc - timefc_ref_phy <= 0 && timefc_ref_phy != 0 ) return;
      else timefc_ref_phy = t_fc;
    }
    
    // check if physical event
    short  det    = m_Physics->GetDetNbr();
    
    if(det >= 0){

      // Get Physics Data
      short  anode  = m_Physics->GetAnodeNbr();
      short  index  = m_detector->GetIndex(det, anode);
      double tofraw = m_Physics->GetTofRaw();
      double tofcal = m_Physics->GetTofCal();
      double e      = m_Physics->GetE();
      double q1     = m_Physics->GetQ1();
      double q2     = m_Physics->GetQ2();
      double q3     = m_Physics->GetQ3();

      // histogram Q1vE
      name.str("");
      name.clear();
      name << "phy_det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[index];
      baseA = name.str();
      his_name = baseA + "_Q1vE";                  m_phy_h2[his_name]->Fill(e,q1);

      // histogram Q2/Q3 vs Q1 if (n,alpha) removed
      his_name = baseA + "_TCutG_exclusion_nA";
      if(q3>0 && !m_tcutg[his_name]->IsInside(e,q1)){
        his_name = baseA + "_Q2Q3vQ1_nAexcluded";
        m_phy_h2[his_name]->Fill(q1,q2/q3);
      }

      // if fission at raw level : 
      if(m_Physics->GetIsFission()){
        // histograms: Tof and E 
        his_name = baseA + "_TofRaw";            m_phy_h1[his_name]->Fill(tofraw);
        his_name = baseA + "_TofRaw_cutF_zoom";  m_phy_h1[his_name]->Fill(tofraw);
        his_name = baseA + "_TofCal";            m_phy_h1[his_name]->Fill(tofcal);
        his_name = baseA + "_E";                 m_phy_h1[his_name]->Fill(e); // MeV
       
        // if fission and Q1>high_thres 
        // histogram: Tof and E
        if(q1 >= m_Cal.GetValue("EPIC_"+to_string(det)+"_Q1_THRESHOLD_PERCENT",0) * m_Cal.GetValue("EPIC_"+to_string(det)+"_ANODE_"+to_string(anode)+"_Q1_HIGHEST_CH",0)){
          his_name = baseA + "_TofCal_Q1_high_thres"; m_phy_h1[his_name]->Fill(tofcal);
          his_name = baseA + "_E_Q1_high_thres";      m_phy_h1[his_name]->Fill(e); // MeV
        }
       
        // histograms per actinide
        name.str("");
        name.clear();
        name << "phy_det" << det << "_" << actinide[index];
        
        string prefix = name.str();
        his_name = prefix + "_E";               m_phy_h1[his_name]->Fill(e);
        his_name = prefix + "_Eweighted";       m_phy_h1[his_name]->Fill(e,1./mass_per_actinide[det-1][actinide[index]]);
        
        string his_name_N = his_name;
        string his_name_D = "phy_det" + to_string(det) + "_235U_Eweighted" ;
        his_name = prefix + "_Ew_ratioU235";
        m_phy_h1[his_name]->Reset();
        m_phy_h1[his_name]->Add(m_phy_h1[his_name_N]);
        m_phy_h1[his_name]->Divide(m_phy_h1[his_name_D]);
        
        //his_name_D = "phy_det" + to_string(det) + "_238U_Eweighted" ;
        //his_name = prefix + "_Ew_ratioU238";
        //m_phy_h1[his_name]->Reset();
        //m_phy_h1[his_name]->Add(m_phy_h1[his_name_N]);
        //m_phy_h1[his_name]->Divide(m_phy_h1[his_name_D]);
      /////  
      /////  if(q1 >= m_Cal.GetValue("EPIC_" + to_string(det) + "_Q1_THRESHOLD_PERCENT",0) * m_Cal.GetValue("EPIC_"+to_string(det)+"_ANODE_"+to_string(anode)+"_Q1_HIGHEST_CH",0)){
      /////    his_name = prefix + "_E_Q1_high_thres";               m_phy_h1[his_name]->Fill(e);
      /////    his_name = prefix + "_Eweighted_Q1_high_thres";       m_phy_h1[his_name]->Fill(e,1./mass_per_actinide[det-1][actinide[index]]);
      /////    string his_name_N = his_name;
      /////    string his_name_D = "phy_det" + to_string(det) + "_235U_Eweighted_Q1_high_thres" ;
      /////    his_name = prefix + "_Ew_ratioU235_Q1_high_thres";
      /////    m_phy_h1[his_name]->Reset();;
      /////    m_phy_h1[his_name]->Add(m_phy_h1[his_name_N]);
      /////    m_phy_h1[his_name]->Divide(m_phy_h1[his_name_D]);
      /////  }
      /////
      }// end of if FF
    } // end of else (det>=0)
  }// end of if --input-phy
}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::Clear() {

  // === raw histograms
  for (auto h : m_raw_h1)        h.second->Reset();
  for (auto h : m_raw_h2)        h.second->Reset();
  // === phys histograms
  for (auto h : m_phy_h1)       h.second->Reset();
  for (auto h : m_phy_h2)       h.second->Reset();
  // === general histograms
  m_TimeHF->Reset();
  m_HF_DeltaTimeHF->Reset();
}
