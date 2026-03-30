#include "EpicSpectra.h"
#include "NPApplication.h"

using namespace epic;

////////////////////////////////////////////////////////////////////////////////
TCanvas* EpicSpectra::CreateCanvas(const std::string& name, int ncol)
{
  TCanvas* c = new TCanvas(name.c_str(), name.c_str(), 1200, 800);
  c->Divide(ncol,2);
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

  // Resize histogram containers
  m_hQ2vQ1.resize(nAnodesTot);
  m_hQ2Q3vQ1.resize(nAnodesTot);
  m_hQmvQ1.resize(nAnodesTot);
  m_hQm.resize(nAnodesTot);
  m_hQ1.resize(nAnodesTot);
  m_hQ2.resize(nAnodesTot);
  m_hQ3.resize(nAnodesTot);
  m_hTof.resize(nAnodesTot);
  m_hDT.resize(nAnodesTot);
  m_hAid.resize(nDets);
  m_hAid_ifQmax.resize(nDets);
  m_hQ1vAid.resize(nDets);

  m_can.resize(nDets);

  // loop over nDets
  for(unsigned int d = 1 ; d <= nDets ; d++){

    // create the canvas per detector
    int ncol = ceil(0.5*nAnodes[d-1]);
    std::string base = "EPIC" + std::to_string(d);
    m_can[d-1][0] = CreateCanvas(base + "_Q2vQ1", ncol);
    m_can[d-1][1] = CreateCanvas(base + "_QmaxvQ1", ncol);
    m_can[d-1][2] = CreateCanvas(base + "_Q1", ncol);
    m_can[d-1][3] = CreateCanvas(base + "_Qmax", ncol);
    m_can[d-1][4] = CreateCanvas(base + "_Tof", ncol);
    m_can[d-1][5] = CreateCanvas(base + "_Q2Q3vQ1", ncol);
    m_can[d-1][6] = CreateCanvas(base + "_DT_Tqmax_Tcfd", ncol);

    // loop over the anodes
    for(unsigned int a = 1 ; a <= nAnodes[d-1] ; a++){

        int i = m_detector->GetIndex(d,a);
        
        ostringstream name;
        name << "det" << d << "_A" << std::setw(2) << std::setfill('0') << a << "_" << actinide[i];
        string prefix = name.str();

        m_hQ2vQ1[i]   = new TH2F((prefix+"_Q2vQ1").c_str(),(prefix+"_Q2vQ1").c_str(),1500,0,300000,500,0,200000); 
        m_hQmvQ1[i]   = new TH2F((prefix+"_QmaxvQ1").c_str(),(prefix+"_QmaxvQ1").c_str(),1000,0,200000,200,0,20000);
        m_hQ2Q3vQ1[i] = new TH2F((prefix+"_Q2Q3vQ1").c_str(),(prefix+"_Q2Q3vQ1").c_str(),1500,0,300000,500,0,10); 

        m_hQm[i]  = new TH1F((prefix+"_Qmax").c_str(),(prefix+"_Qmax").c_str(),2500,0,25000);
        m_hQ1[i]  = new TH1F((prefix+"_Q1").c_str(),(prefix+"_Q1").c_str(),15000,0,300000);
        m_hQ2[i]  = new TH1F((prefix+"_Q2").c_str(),(prefix+"_Q2").c_str(),15000,0,300000); 
        m_hQ3[i]  = new TH1F((prefix+"_Q3").c_str(),(prefix+"_Q3").c_str(),15000,0,300000);
        m_hTof[i] = new TH1F((prefix+"_Tof").c_str(),(prefix+"_Tof").c_str(),3000,-200,2800);
        m_hDT[i]  = new TH1F((prefix+"_Tqmax_Tcfd").c_str(),(prefix+"_Tqmax_Tcfd").c_str(),1000,-50,50);

        m_hQ2[i]->SetLineColor(8);
        m_hQ3[i]->SetLineColor(kCyan);

        m_can[d-1][0]->cd(a); gPad->SetLogz(); m_hQ2vQ1[i]->Draw("colz");     
        m_can[d-1][1]->cd(a); gPad->SetLogz(); m_hQmvQ1[i]->Draw("colz");     
        m_can[d-1][2]->cd(a); gPad->SetLogy(); m_hQ1[i]->Draw(); m_hQ2[i]->Draw("same"); m_hQ3[i]->Draw("same");
        m_can[d-1][3]->cd(a); gPad->SetLogy(); m_hQm[i]->Draw();
        m_can[d-1][4]->cd(a); gPad->SetLogy(); m_hTof[i]->Draw();
        m_can[d-1][5]->cd(a); gPad->SetLogz(); m_hQ2Q3vQ1[i]->Draw("colz");
        m_can[d-1][6]->cd(a); m_hDT[i]->Draw(); 
    }// end of loop over nAnodes[d-1]

    std::string detPrefix = "det" + std::to_string(d);

    m_hAid[d-1]        = new TH1F((detPrefix+"_AnodeID").c_str(),(detPrefix+"_AnodeID").c_str(),13,-0.5,12.5);
    m_hAid_ifQmax[d-1] = new TH1F((detPrefix+"_AnodeID_ifQmax").c_str(),(detPrefix+"_AnodeID_ifQmax").c_str(),13,-0.5,12.5);
    m_hQ1vAid[d-1]     = new TH2F((detPrefix+"_Q1vAnodeID").c_str(),(detPrefix+"_Q1vAnodeID").c_str(),13,-0.5,12.5,1500,0,300000);
  }// end of loop over nDets


}

////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillRaw() {

  char name[100];

  unsigned int nAnodesTot = m_detector->GetNumberOfAnodesTot();
  unsigned int nDets = m_detector->GetNumberOfDets() ;
  std::vector<unsigned int> nAnodes = m_detector->GetNumberOfAnodes();
  std::vector<string> actinide = m_detector->GetActinideMaterial();

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
    m_hAid[det-1]->Fill(anode);
    if (qmax > Qmax[det-1]){
        Qmax[det-1] = qmax;
        IndexMax[det-1] = i;
    }
  }// end of loop over the raw data

  // spectra for Qmax only: suppress cross-talk
  for(int d = 0 ; d < nDets ; d++){
    if(Qmax[d] > 0 && IndexMax[d] >= 0){
        int det = m_RawData->GetDetNbr(IndexMax[d]);
        if(det != (d+1)) std::cout << "ERROR: didn't recover Qmax data to fill raw spectra" << std::endl;
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
        m_hAid_ifQmax[d]->Fill(anode) ;
        m_hQ1vAid[d]->Fill(anode,q1);
        m_hQ2vQ1[index]->Fill(q1,q2);
        m_hQmvQ1[index]->Fill(q1,qm);
        m_hQ1[index]->Fill(q1);
        m_hQ2[index]->Fill(q2);
        m_hQ3[index]->Fill(q3);
        m_hQm[index]->Fill(qm);
        m_hTof[index]->Fill(t_fc-t_hf);
        m_hDT[index]->Fill(t_qmax-t_cfd);
        if(q3>0) m_hQ2Q3vQ1[index]->Fill(q1,q2/q3);
    }

  }

}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::FillPhy() {
}
////////////////////////////////////////////////////////////////////////////////
void EpicSpectra::Clear() {

  // === raw histograms
  // --- 1D histograms (indexed by anode)
  for(auto* h : m_hQm)        if(h) h->Reset();
  for(auto* h : m_hQ1)        if(h) h->Reset();
  for(auto* h : m_hQ2)        if(h) h->Reset();
  for(auto* h : m_hQ3)        if(h) h->Reset();
  for(auto* h : m_hTof)       if(h) h->Reset();
  for(auto* h : m_hDT)        if(h) h->Reset();
  // --- 2D histograms (indexed by anode)
  for(auto* h : m_hQ2vQ1)     if(h) h->Reset();
  for(auto* h : m_hQmvQ1)     if(h) h->Reset();
  for(auto* h : m_hQ2Q3vQ1)   if(h) h->Reset();
  // --- detector histograms
  for(auto* h : m_hAid)            if(h) h->Reset();
  for(auto* h : m_hAid_ifQmax)     if(h) h->Reset();
  for(auto* h : m_hQ1vAid)         if(h) h->Reset();

}

