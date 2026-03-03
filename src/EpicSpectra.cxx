#include "EpicSpectra.h"
#include "NPApplication.h"

using namespace epic;
////////////////////////////////////////////////////////////////////////////////
EpicSpectra::EpicSpectra() {

  cout << "//// EpicSpectra::EpicSpectra()" << endl;

  // Set Pointers:
  m_detector = std::dynamic_pointer_cast<EpicDetector>(
      nptool::Application::GetApplication()->GetDetector("epic"));
  m_RawData = m_detector->GetRawData();
  m_Physics = m_detector->GetPhysics();

  check_time = 0;
  int nDets = m_detector->GetNumberOfDets() ;
  vector<unsigned int> nAnodes = m_detector->GetNumberOfAnodes();
  vector<string> actinide = m_detector->GetActinideMaterial();

  char name[100];

  for(int d = 1 ; d <= nDets ; d++){

    // Build Raw Canvas
    sprintf(name,"EPIC%i_Q2vQ1",d);
    auto c_raw1 = new TCanvas(name);
    c_raw1->Divide(6,2);

    sprintf(name,"EPIC%i_QmaxvQ1",d);
    auto c_raw2 = new TCanvas(name);
    c_raw2->Divide(6,2);
    
    sprintf(name,"EPIC%i_Q1",d);
    auto c_raw3 = new TCanvas(name);
    c_raw3->Divide(6,2);
    
    sprintf(name,"EPIC%i_Qmax",d);
    auto c_raw4 = new TCanvas(name);
    c_raw4->Divide(6,2);
    
    sprintf(name,"EPIC%i_Tof",d);
    auto c_raw5 = new TCanvas(name);
    c_raw5->Divide(6,2);

    sprintf(name,"EPIC%i_Q2Q3vQ1",d);
    auto c_raw6 = new TCanvas(name);
    c_raw6->Divide(6,2);

    sprintf(name,"EPIC%i_DT_Tqmax_Tcfd",d);
    auto c_raw7 = new TCanvas(name);
    c_raw7->Divide(6,2);

    // Declare Raw Spectra
    for(int a = 1 ; a <=nAnodes[d-1] ; a ++){
        int i = m_detector->GetIndex(d,a);

        sprintf(name,"det%i_A%02i_%s_Q2vQ1",d,a,actinide[i].c_str());
        m_raw_hist2d[name] = new TH2F(name,name,1500,0,300000,500,0,200000);
        c_raw1->cd(a); gPad->SetLogz(); m_raw_hist[name]->Draw("colz");

        sprintf(name,"det%i_A%02i_%s_QmaxvQ1",d,a,actinide[i].c_str());
        m_raw_hist2d[name] = new TH2F(name,name,1000,0,200000,200,0,20000);
        c_raw2->cd(a); gPad->SetLogz(); m_raw_hist[name]->Draw("colz");

        sprintf(name,"det%i_A%02i_%s_Q1",d,a,actinide[i].c_str());
        m_raw_hist[name] = new TH1F(name,name,15000,0,300000);
        c_raw3->cd(a); gPad->SetLogy(); m_raw_hist[name]->Draw();
        
        sprintf(name,"det%i_A%02i_%s_Q2",d,a,actinide[i].c_str());
        m_raw_hist[name] = new TH1F(name,name,15000,0,300000);
        m_raw_hist[name]->SetLineColor(8);
        c_raw3->cd(a); m_raw_hist[name]->Draw("same");
        
        sprintf(name,"det%i_A%02i_%s_Q3",d,a,actinide[i].c_str());
        m_raw_hist[name] = new TH1F(name,name,15000,0,300000);
        m_raw_hist[name]->SetLineColor(kCyan);
        c_raw3->cd(a); m_raw_hist[name]->Draw("same");

        sprintf(name,"det%i_A%02i_%s_Qmax",d,a,actinide[i].c_str());
        m_raw_hist[name] = new TH1F(name,name,2500,0,25000);
        c_raw4->cd(a); gPad->SetLogy(); m_raw_hist[name]->Draw();

        sprintf(name,"det%i_A%02i_%s_Tof",d,a,actinide[i].c_str());
        m_raw_hist[name] = new TH1F(name,name,3000,-200,2800);
        c_raw5->cd(a); m_raw_hist[name]->Draw();

        sprintf(name,"det%i_A%02i_%s_Q2Q3vQ1",d,a,actinide[i].c_str());
        m_raw_hist2d[name] = new TH2F(name,name,1500,0,300000,500,0,10);
        c_raw6->cd(a); gPad->SetLogz(); m_raw_hist[name]->Draw("colz");

        sprintf(name,"det%i_A%02i_%s_Tqmax_Tcfd",d,a,actinide[i].c_str());
        m_raw_hist[name] = new TH1F(name,name,1000,-50,50);
        c_raw7->cd(a); m_raw_hist[name]->Draw();
    }


    sprintf(name,"det%i_AnodeID",d);
    m_raw_hist[name] = new TH1F(name,name,13,-0.5,12.5);
    m_raw_hist[name]->SetLineColor(kBlack);
    c_raw1->cd(12); m_raw_hist[name]->Draw();
    sprintf(name,"det%i_AnodeID_ifQmax",d);
    m_raw_hist[name] = new TH1F(name,name,13,-0.5,12.5);
    c_raw1->cd(12); m_raw_hist[name]->Draw("same");

    sprintf(name,"det%i_Q1vAnodeID",d);
    m_raw_hist2d[name] = new TH2F(name,name,13,-0.5,12.5,1500,0,300000);
    c_raw3->cd(12); gPad->SetLogz(); m_raw_hist[name]->Draw("colz"); 
  }


  //auto c_raw2 = new TCanvas("FC raw inToF");
  //m_raw_hist["inToF"] = new TH1F("Raw inToF ", "Raw inToF", 4000, -2000, 5000);
  //m_raw_hist["inToF"]->Draw();


    //TODO
  //m_graph_sampler = new TGraph();
  //TH2F *h2 = new TH2F("FC Sample", "FC Sample", 100, -50, 100, 100, -200, 5000);
  //auto c_raw_sampler = new TCanvas("FC Sampler");
  //h2->GetXaxis()->SetTitle("Time (ns)");
  //h2->Draw();
  //m_graph_sampler->Draw("plsame");

    //TODO
  //// Declare Phy Spectra
  //m_phy_hist["inToF"] = new TH1F("inToF", "inToF", 4000, 0, 2000);
  //m_phy_hist["inEnergy"] = new TH1F("inEnergy", "inEnergy", 8000, 0, 700);
  //m_phy_hist["ToFvsID"] =
  //    new TH2F("ToFvsID", "ToFvsID", 11, 1, 12, 4000, 0, 2000);
  //m_phy_hist["inEvsID"] =
  //    new TH2F("inEvsID", "inEvsID", 11, 1, 12, 4000, 0, 700);

    //TODO
  //// Build Phy Canvas
  //auto c_phy1 = new TCanvas("FC Phy 1");
  //c_phy1->Divide(1, 2);
  //c_phy1->cd(1);
  //m_phy_hist["inToF"]->Draw();
  //gPad->SetLogy();
  //c_phy1->cd(2);
  //m_phy_hist["inEnergy"]->Draw();
  //gPad->SetLogy();

  //auto c_phy2 = new TCanvas("FC Phy 2");
  //c_phy2->Divide(1, 2);
  //c_phy2->cd(1);
  //m_phy_hist["ToFvsID"]->Draw("colz");
  //c_phy2->cd(2);
  //m_phy_hist["inEvsID"]->Draw("colz");
}

////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillRaw() {

  char name[100];

  unsigned int nAnodesTot = m_detector->GetNumberOfAnodesTot();
  unsigned int nDets = m_detector->GetNumberOfDets() ;
  vector<unsigned int> nAnodes = m_detector->GetNumberOfAnodes();
  vector<string> actinide = m_detector->GetActinideMaterial();

  TH2F * hQ2vQ1[nAnodesTot];
  TH2F * hQ2Q3vQ1[nAnodesTot];
  TH2F * hQmvQ1[nAnodesTot];
  TH1F * hQm[nAnodesTot];
  TH1F * hQ1[nAnodesTot];
  TH1F * hQ2[nAnodesTot];
  TH1F * hQ3[nAnodesTot];
  TH1F * hAid[nDets];
  TH1F * hAid_ifQmax[nDets];
  TH2F * hQ1vAid[nDets];
  TH1F * hTof[nAnodesTot];
  TH1F * hDT[nAnodesTot];

  for(int d = 1 ; d <= nDets ; d++){
    sprintf(name,"det%i_AnodeID",d);
    hAid[d-1] = (TH1F*)(m_raw_hist[name]);

    sprintf(name,"det%i_AnodeID_ifQmax",d);
    hAid_ifQmax[d-1] = (TH1F*)(m_raw_hist[name]);
    
    sprintf(name,"det%i_Q1vAnodeID",d);
    hQ1vAid[d-1] = (TH2F*)(m_raw_hist2d[name]);

    for(int a = 1 ; a <=nAnodes[d-1] ; a ++){
      int index = m_detector->GetIndex(d,a) ;
        sprintf(name,"det%i_A%02i_%s_Q2Q3vQ1",d,a,actinide[index].c_str());
        hQ2Q3vQ1[index] = (TH2F*)(m_raw_hist2d[name]);

        sprintf(name,"det%i_A%02i_%s_Q2vQ1",d,a,actinide[index].c_str());
        hQ2vQ1[index] = (TH2F*)(m_raw_hist2d[name]);

        sprintf(name,"det%i_A%02i_%s_QmaxvQ1",d,a,actinide[index].c_str());
        hQmvQ1[index] = (TH2F*)(m_raw_hist2d[name]);

        sprintf(name,"det%i_A%02i_%s_Q1",d,a,actinide[index].c_str());
        hQ1[index] = (TH1F*)(m_raw_hist[name]);

        sprintf(name,"det%i_A%02i_%s_Q2",d,a,actinide[index].c_str());
        hQ2[index] = (TH1F*)(m_raw_hist[name]);

        sprintf(name,"det%i_A%02i_%s_Q3",d,a,actinide[index].c_str());
        hQ3[index] = (TH1F*)(m_raw_hist[name]);
        
        sprintf(name,"det%i_A%02i_%s_Qmax",d,a,actinide[index].c_str());
        hQm[index] = (TH1F*)(m_raw_hist[name]);
        
        sprintf(name,"det%i_A%02i_%s_Tof",d,a,actinide[index].c_str());
        hTof[index] = (TH1F*)(m_raw_hist[name]);
        
        sprintf(name,"det%i_A%02i_%s_Tqmax_Tcfd",d,a,actinide[index].c_str());
        hDT[index] = (TH1F*)(m_raw_hist[name]);
    }
  }


  int FC_mult = m_RawData->GetFCMult();
  double Qmax[nDets]; 
  int IndexMax[nDets]; 
  for (int d = 0 ; d < nDets ; d++){
    Qmax[d] = 0.;
    IndexMax[d] = -1;
  }

  // loop over the raw data
  // search for qmax per FC
  for(int i = 0 ; i < FC_mult ; i++){
    if(m_RawData->GetPulserTrig(i)) continue;
    int det = m_RawData->GetDetNbr(i);
    int anode = m_RawData->GetAnodeNbr(i);
    double qmax = m_RawData->GetQmax(i); 
    hAid[det-1]->Fill(anode);
    if (qmax > Qmax[det-1]){
        Qmax[det-1] = qmax;
        IndexMax[det-1] = i;
    }
  }// end of loop over the raw data

  // spectra for Qmax only: suppress cross-talk
  for(int d = 0 ; d < nDets ; d++){
    if(Qmax[d] > 0 && IndexMax[d] >= 0){
        int det = m_RawData->GetDetNbr(IndexMax[d]);
        if(det != (d+1)) cout << "ERROR: didn't recover Qmax data to fill raw spectra" << endl;
        int anode = m_RawData->GetAnodeNbr(IndexMax[d]);
        int index = m_detector->GetIndex(det,anode);
        double qm = m_RawData->GetQmax(IndexMax[d]);
        double q1 = m_RawData->GetQ1(IndexMax[d]);
        double q2 = m_RawData->GetQ2(IndexMax[d]);
        double q3 = m_RawData->GetQ3(IndexMax[d]);
        double t_fc = m_RawData->GetTimeFC(IndexMax[d]);
        double t_hf = m_RawData->GetTimeHF();
        double t_qmax = m_RawData->GetTimeQmax(IndexMax[d]);
        double t_cfd  = m_RawData->GetTimeCfd(IndexMax[d]);
        hAid_ifQmax[d]->Fill(anode) ;
        hQ1vAid[d]->Fill(anode,q1);
        hQ2vQ1[index]->Fill(q1,q2);
        hQmvQ1[index]->Fill(q1,qm);
        hQ1[index]->Fill(q1);
        hQ2[index]->Fill(q2);
        hQ3[index]->Fill(q3);
        hQm[index]->Fill(qm);
        hTof[index]->Fill(t_fc-t_hf);
        hDT[index]->Fill(t_qmax-t_cfd);
        if(q3>0) hQ2Q3vQ1[index]->Fill(q1,q2/q3);
    }

  }

  //auto g_sampler = (TGraph *)(m_graph_sampler);
  //int size_sampler = m_RawData->GetFCSampleSize();
  //int sampler_before_th;
  //if (size_sampler > 0) {
  //  sampler_before_th = m_RawData->GetFCSample(0);
  //}
  //for (unsigned int i = 1; i < size_sampler; i++) {
  //  double sample = m_RawData->GetFCSample(i);
  //  int time_sample = -2 * sampler_before_th + 2 * (i - 1);
  //  g_sampler->SetPoint(i - 1, time_sample, sample);
  //}
}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillPhy() {

  //int FC_mult = m_Physics->GetMult();

  //TH1F *hinTof = (TH1F *)(m_phy_hist["inToF"]);
  //TH1F *hinEnergy = (TH1F *)(m_phy_hist["inEnergy"]);
  //TH2F *hTofvsID = (TH2F *)(m_phy_hist["ToFvsID"]);
  //TH2F *hinEvsID = (TH2F *)(m_phy_hist["inEvsID"]);
  //if (FC_mult == 1 && !m_Physics->FakeFission[0]) {
  //  double inTOF = m_Physics->inToF[0];
  //  double inE = m_Physics->inEnergy[0];
  //  double ID = m_Physics->Anode_ID[0];

  //  hinTof->Fill(m_Physics->inToF[0]);
  //  hinEnergy->Fill(m_Physics->inEnergy[0]);

  //  hTofvsID->Fill(ID, inTOF);
  //  hinEvsID->Fill(ID, inE);
  //}
}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::Clear() {
  for (auto h : m_raw_hist)
    h.second->Reset();
  for (auto h : m_phy_hist)
    h.second->Reset();
}
