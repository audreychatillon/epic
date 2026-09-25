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

    timehf_ref_raw = 0;
    timefc_ref_raw = 0;
    timefc_ref_phy = 0;

    // general histograms
    m_canT0 = new TCanvas("T0", "T0", 1200, 800);
    m_canT0->Divide(1, 2);

    m_TimeHF = new TH1F("TimeHF", "TimeHF", 86400, 0, 86400);
    m_HF_DeltaTimeHF = new TH1F("DT_TimeHF_fHF_ifBeamOn", "DT_TimeHF_fHF_ifBeamOn", 3000, 2500000, 2500030);
    m_HF_DeltaTimeHF->SetLineColor(kBlack);

    m_TimeHF->GetXaxis()->SetTitle("TimeHF [s] 1s/bin");
    m_HF_DeltaTimeHF->GetXaxis()->SetTitle("Delta TimeHF [ns] 10ps/bin");

    m_canT0->cd(1); m_TimeHF->Draw();
    m_canT0->cd(2); gPad->SetLogy();  m_HF_DeltaTimeHF->Draw(); 

    // loop over nDets for raw histos
    if(m_app->HasFlag("--input-raw")){
        dir_raw->cd();
        size_t offset = 0;
        for (unsigned int det = 1; det <= nDets; det++) {
          // create the canvas and histos per detector
          int ncol = ceil(0.5 * nAnodes[det - 1]);
          string base = "EPIC" + to_string(det);
          string can_name;
          string his_name;

          can_name = base + "_MULT";
          m_raw_can[can_name] = new TCanvas(can_name.c_str(), can_name.c_str());
          his_name = base + "_Mult"; 
          m_raw_h1[his_name] = new TH1F(his_name.c_str(),his_name.c_str(), 13, -0.5, 12.5);
          m_raw_can[can_name]->cd();
          m_raw_can[can_name]->cd();
          gPad->SetLogy();
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
          
          can_name = base + "_Q";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q4";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q1vT";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q2Q3vQ1";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q4QmaxvQ1";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_WaveformA";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_WaveformF";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q2vQ1";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q2vQ3";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_QmaxvQ1";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q4vQmax";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q4vQ1";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Qmax";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_TofRaw";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_TofRaw_zoom";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_TofRaw_offset";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q1vTofRaw";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_DT_Tqmax_Tcfd";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_DTvQ1";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q2Q3vQ1_mult1";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q2Q3vQ1_mult2_upstr";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q2Q3vQ1_mult2_downstr";
          m_raw_can[can_name] = CreateCanvas(can_name, ncol);

          // loop over the anodes and create the histo per anode
          for (unsigned int a = 0; a < nAnodes[det - 1]; a++) {
            int anode = anodes[offset + a];
            int i = m_detector->GetIndex(det, anode);

            ostringstream name;
            name << "det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[i];
            string prefix = name.str();

            his_name = prefix + "_Q1vT";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(), 1440,0, 86400, 1000, 0, 200000);
            m_raw_h2[his_name]->GetXaxis()->SetTitle("Time [s] : 60s / bin");
            m_raw_h2[his_name]->GetYaxis()->SetTitle("Q1");
            can_name = base + "_Q1vT";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q2Q3vQ1";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 500000, 250, -0.1, 2.4);
            can_name = base + "_Q2Q3vQ1";
            m_raw_can[can_name]->cd(a+1);
            //gPad->SetLogx(); 
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");
            vector<double> xA = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_A_X");
            vector<double> yA = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_A_Y");
            his_name = prefix + "_TCutG_discriA";
            m_tcutg[his_name] = new TCutG(his_name.c_str(),xA.size(),xA.data(),yA.data());
            m_tcutg[his_name]->Draw("same");
            vector<double> xF = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_F_X");
            vector<double> yF = m_Cal.GetCorrection("EPIC_" + to_string(det) + "_ANODE_" + to_string(anode) + "_TCUTG_DISCRI_F_Y");
            his_name = prefix + "_TCutG_discriF";
            m_tcutg[his_name] = new TCutG(his_name.c_str(),xF.size(),xF.data(),yF.data());
            m_tcutg[his_name]->Draw("same");

            his_name = prefix + "_WaveFormA";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),110, -10, 210, 400, -1000, 3000);
            can_name = base + "_WaveformA";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_WaveFormF";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),110, -10, 210, 1300, -1000, 12000);
            can_name = base + "_WaveformF";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q4QmaxvQ1";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 500, -5, 5);
            m_raw_h2[his_name]->GetXaxis()->SetTitle("Q1");
            m_raw_h2[his_name]->GetYaxis()->SetTitle("Q4/Qmax");
            can_name = base + "_Q4QmaxvQ1";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q2vQ1";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 500, 0, 200000);
            can_name = base + "_Q2vQ1";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q2vQ3";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),500, 0, 200000, 500, 0, 200000);
            can_name = base + "_Q2vQ3";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_QmaxvQ1";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 200, 0, 20000);
            can_name = base + "_QmaxvQ1";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz();
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q4vQmax";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),200, 0, 20000,500,-50000,50000);
            m_raw_h2[his_name]->GetXaxis()->SetTitle("Qmax");
            m_raw_h2[his_name]->GetYaxis()->SetTitle("Q4");
            can_name = base + "_Q4vQmax";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q4vQ1";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000, 0, 400000, 500, -50000, 50000);
            m_raw_h2[his_name]->GetXaxis()->SetTitle("Q1");
            m_raw_h2[his_name]->GetYaxis()->SetTitle("Q4");
            can_name = base + "_Q4vQ1";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            can_name = base + "_Q";
            his_name = prefix + "_Q1";
            m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
            m_raw_can[can_name]->cd(a+1);
	        gPad->SetLogy();
            m_raw_h1[his_name]->Draw();

            his_name = prefix + "_Q2";
            m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
            m_raw_h1[his_name]->SetLineColor(8);
            m_raw_can[can_name]->cd(a+1);
            m_raw_h1[his_name]->Draw("same");

            his_name = prefix + "_Q3";
            m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),25000, 0, 500000);
            m_raw_h1[his_name]->SetLineColor(kCyan);
            m_raw_can[can_name]->cd(a+1);
            m_raw_h1[his_name]->Draw("same");

            his_name = prefix + "_Q4";
            m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(),5000, -50000, 50000);
            can_name = base + "_Q4";
            m_raw_can[can_name]->cd(a+1);
            m_raw_h1[his_name]->Draw();

            his_name = prefix + "_Qmax";
            m_raw_h1[his_name]  = new TH1F(his_name.c_str(), his_name.c_str(), 2500, 0, 25000);
            can_name = base + "_Qmax";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_raw_h1[his_name]->Draw();

            his_name = prefix + "_TofRaw";
            m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  2600, -10000, 2590000);
            m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 1us / bin ");
            can_name = base + "_TofRaw";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_raw_h1[his_name]->Draw();
            his_name = prefix + "_TofRaw_cutF";
            m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  2600, -10000, 2590000);
            m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 1us / bin ");
            can_name = base + "_TofRaw";
            m_raw_can[can_name]->cd(a+1);
            m_raw_h1[his_name]->SetLineColor(kRed+3);
            m_raw_h1[his_name]->Draw("same");

            int gp = (int)m_Cal.GetValue("EPIC_"+ to_string(det)+"_ANODE_"+to_string(anode) + "_GAMMA_PEAK_RAW",0);

            his_name = prefix + "_TofRaw_cutF_zoom";
            m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  20000, gp-200, gp+1800);
            m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin ");
            can_name = base + "_TofRaw_zoom";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_raw_h1[his_name]->Draw();

            his_name = prefix + "_TofRaw_cutF_offset";
            m_raw_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  20000, 0, 2000);
            m_raw_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin ");
            can_name = base + "_TofRaw_offset";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_raw_h1[his_name]->Draw();

            his_name = prefix + "_Q1vTofRaw";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(), 1000, gp-200, gp+1800, 1000, 0, 200000);
            m_raw_h2[his_name]->GetXaxis()->SetTitle("TofRaw [ns] 500 ps / bin");
            m_raw_h2[his_name]->GetYaxis()->SetTitle("Q1");
            can_name = base + "_Q1vTofRaw";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Tqmax_Tcfd";
            m_raw_h1[his_name]  = new TH1F(his_name.c_str(),his_name.c_str(), 1000, -10, 90);
            m_raw_h1[his_name]->GetXaxis()->SetTitle("DT = Tqmax - Tcfd");
            can_name = base + "_DT_Tqmax_Tcfd";
            m_raw_can[can_name]->cd(a+1);
            m_raw_h1[his_name]->Draw();

            his_name = prefix + "_DTvQ1";
            m_raw_h2[his_name]  = new TH2F(his_name.c_str(),his_name.c_str(),1000,0,400000, 1000, -10, 90);
            m_raw_h2[his_name]->GetXaxis()->SetTitle("Q1");
            m_raw_h2[his_name]->GetYaxis()->SetTitle("DT = Tqmax - Tcfd");
            can_name = base + "_DTvQ1";
            m_raw_can[can_name]->cd(a+1);
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q2Q3vQ1_mult1";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 400000, 110, -0.1, 1.1);
            can_name = base + "_Q2Q3vQ1_mult1";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q2Q3vQ1_mult2_upstr";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 400000, 110, -0.1, 1.1);
            can_name = base + "_Q2Q3vQ1_mult2_upstr";
            m_raw_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_raw_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q2Q3vQ1_mult2_downstr";
            m_raw_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),2000, 0, 400000, 110, -0.1, 1.1);
            can_name = base + "_Q2Q3vQ1_mult2_downstr";
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
        for (unsigned int det = 1; det <= nDets; det++) {
          // create the canvas and histos per detector
          int ncol = ceil(0.5 * nAnodes[det - 1]);
          string base = "phy_EPIC" + to_string(det);
          string can_name;
          string his_name;
          
          can_name = base + "_TofRaw";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_TofRaw_zoom";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_TofCal";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_E";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Elow";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q1vE";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);
          
          can_name = base + "_Q1vElow";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);

          // loop over the anodes and create the histo per anode
          for (unsigned int a = 0; a < nAnodes[det - 1]; a++) {
            int anode = anodes[offset+a]; 
            int i = m_detector->GetIndex(det, anode);

            ostringstream name;
            name << "phy_det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[i];
            string prefix = name.str();

            int gp = (int)m_Cal.GetValue("EPIC_"+ to_string(det)+"_ANODE_"+to_string(anode) + "_GAMMA_PEAK_PHY",0);

            his_name = prefix + "_TofRaw";
            m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  2600, -2590000, 10000);
            m_phy_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 1ns / bin");
            can_name = base + "_TofRaw";
            m_phy_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_phy_h1[his_name]->Draw();

            his_name = prefix + "_TofRaw_cutF_zoom";
            m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(), 20000, gp-200 , gp+1800);
            m_phy_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin");
            can_name = base + "_TofRaw_zoom";
            m_phy_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_phy_h1[his_name]->Draw();

            his_name = prefix + "_TofCal";
            m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  20000, 0, 2000);
            m_phy_h1[his_name]->GetXaxis()->SetTitle("Time [ns] 100ps / bin");
            can_name = base + "_TofCal";
            m_phy_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_phy_h1[his_name]->Draw();

            his_name = prefix + "_E";
            m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
            m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
            can_name = base + "_E";
            m_phy_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_phy_h1[his_name]->Draw();

            his_name = prefix + "_Elow";
            m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  1000, 0, 1);
            m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [eV] 1 meV / bin ");
            can_name = base + "_Elow";
            m_phy_can[can_name]->cd(a+1);
            gPad->SetLogy();
            m_phy_h1[his_name]->Draw();

            his_name = prefix + "_Q1vE";
            m_phy_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),200,0,20,1500, 0, 300000);
            can_name = base + "_Q1vE";
            m_phy_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_phy_h2[his_name]->Draw("colz");

            his_name = prefix + "_Q1vElow";
            m_phy_h2[his_name] = new TH2F(his_name.c_str(), his_name.c_str(),1000,0,1,1500, 0, 300000);
            m_phy_h2[his_name]->GetXaxis()->SetTitle("Energy [eV]");
            can_name = base + "_Q1vElow";
            m_phy_can[can_name]->cd(a+1);
            gPad->SetLogz(); 
            m_phy_h2[his_name]->Draw("colz");

          } // end of loop over nAnodes[d-1]
          offset += nAnodes[det-1];
          
          can_name = base + "_E_per_actinide";
          m_phy_can[can_name] = CreateCanvas(can_name, ncol);

	  for(short act = 0 ; act < (short)actinides_per_det[det-1].size(); act++){

            ostringstream name;
            name << "phy_det" << det << "_" << actinides_per_det[det-1][act];
            string prefix = name.str();

            his_name = prefix + "_E";
            m_phy_h1[his_name] = new TH1F(his_name.c_str(), his_name.c_str(),  200, 0, 20);
            m_phy_h1[his_name]->GetXaxis()->SetTitle("Energy [MeV] 100 keV / bin ");
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                m_phy_h1[his_name]->ls();
            can_name = base + "_E_per_actinide";
            m_phy_can[can_name]->cd(act+1);
            gPad->SetLogy();
            m_phy_h1[his_name]->Draw();

	  }

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
          if (m_RawData->GetDetNbr(0) == -1) {
          
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
          else {
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
              int det = m_RawData->GetDetNbr(i);
              if (det < 0)          continue;
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
               his_name = baseD + "_Mult";
               m_raw_h1[his_name]->Fill(multPerFC[d]);
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
        if(m_Physics->GetIsFission()){

	    // to by-pass nponline bug : comment this lines if you want to process several runs 
            bool replay = m_Cal.GetValue("REPLAY_DATA",0);
            if(!replay){
	            double t_fc = m_Physics->GetTime(); 
 	            if( t_fc - timefc_ref_phy <= 0 && timefc_ref_phy != 0 ) return;
 	            else timefc_ref_phy = t_fc;
            }
            // init
            short  det    = m_Physics->GetDetNbr();
            short  anode  = m_Physics->GetAnodeNbr();
            int    index  = m_detector->GetIndex(det, anode);
            double tofraw = m_Physics->GetTofRaw();
            double tofcal = m_Physics->GetTofCal();
            double e      = m_Physics->GetE();
            double q1     = m_Physics->GetQ1();
            name.str("");
            name.clear();
            name << "phy_det" << det << "_A" << std::setw(2) << std::setfill('0') << anode << "_" << actinide[index];
            baseA = name.str();
            // fill spectra
            his_name = baseA + "_TofRaw";            m_phy_h1[his_name]->Fill(tofraw);
            his_name = baseA + "_TofRaw_cutF_zoom";  m_phy_h1[his_name]->Fill(tofraw);
            his_name = baseA + "_TofCal";            m_phy_h1[his_name]->Fill(tofcal);
	    if (e>1.e-06){ //e>1eV
            	his_name = baseA + "_E";             m_phy_h1[his_name]->Fill(e); // MeV
            	his_name = baseA + "_Q1vE";          m_phy_h2[his_name]->Fill(e,q1);
            }
	    else{
            	his_name = baseA + "_Elow";          m_phy_h1[his_name]->Fill(e*1.e+06); //eV
            	his_name = baseA + "_Q1vElow";       m_phy_h2[his_name]->Fill(e*1.e+06,q1);
	    }

	    name.clear();
            name << "phy_det" << det << "_" << actinide[index];
            string prefix = name.str();
            his_name = prefix + "_E";     
		cout << "FillPhy() !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		cout << hisname << endl;
	              m_phy_h1[his_name]->Fill(e); // MeV 
        }// end of if FF
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
