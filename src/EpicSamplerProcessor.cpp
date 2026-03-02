#include "EpicSamplerProcessor.h"
#include <iostream>
#include <algorithm>
#include "TCanvas.h"
#include "TGraph.h"
#include "TApplication.h"
#include "TSystem.h"
#include <numeric>

///////////////////////////////////////////////////////////////////////////////////////////////////////
EpicSamplerProcessor::EpicSamplerProcessor(const std::vector<double>& inputSamples, bool applyFilter_ =false) 
    : applyFilter(applyFilter_), 
      samples(inputSamples), 
      filteredSignal(),
      samplesX(),
      signalCFD(){
  
  // === create filteredSignal
  if(applyFilter){ 
    for (size_t i = 1; i < samples.size() - 1; ++i) {
      double filteredValue = (samples[i - 1] + samples[i] + samples[i + 1]) / 3.0;
      filteredSignal.push_back(filteredValue);  
    }
  }
  else filteredSignal = samples; 

  // === resize samplesX and signalCFD to the same size than filteredSignal
  //      and initialize them to 0
  samplesX.resize(filteredSignal.size());
  signalCFD.resize(filteredSignal.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
double EpicSamplerProcessor::calculateCFDnew(
    const double fract, const int delay, const double thres, // CFD constants
    const int sample_size_ns,                                // FASTER specific
    double &Qmax, double &tQmax,                             // Qmax of the filteredSignal vector and its time
    bool &bTriggered, 
    bool &bThreshold) {

  const int nsamples_delay = delay / sample_size_ns; 
  double    CFDmax = -1000.;
  int       index_Qmax   = -1;
  int       index_CFDmax = -1;
  int       index_thres  = -1;
  int       index_first  = -1;
  double    Tcfd = -10000.;

  for (int i = 0; i < filteredSignal.size(); ++i) {
   
    // === fill time scale
    samplesX[i] = i * sample_size_ns + 1;

    // === get Qmax
    double Qi = filteredSignal[i];
    if (Qi > Qmax){
        Qmax  = Qi;
        index_Qmax = i ;
        tQmax = samplesX[i]; 
    }

    // === Convert to CFD signal
    if (i >= nsamples_delay) {
      //    "The input signal is attenuated and inversely added to the delayed input signal"
      //    therefore: CFDsignal = delayed_signal - fraction*signal
      //    for these positive PA signals: signals should first be <0 before to cross 0 
      signalCFD[i] = filteredSignal[i - nsamples_delay] - fract * Qi ;
      if(signalCFD[i]>CFDmax){
        CFDmax = signalCFD[i];
        index_CFDmax = i ;
      }
    }// end of CFD conversion for bin i
  }// end of loop over filtreredSignal

  // === Starting from the maximum value of the CFD
  //     find the first negative value
  for (int i = index_CFDmax ; i > 0 ; i--){
    if(signalCFD[i] < 0){
        index_first = i;
        bTriggered = true ;
        break;
    }
  }

  // === Starting from the first negative value before max
  //     find the first bin above threshold 
  //     check that there are at least 3 points above threshold
  // === check that there is at least 3 consecutive points above threshold
  for (int i = index_first ; i <= index_CFDmax; i++){
    if (signalCFD[i] > thres){
        index_thres = i;
        if (index_thres < (signalCFD.size()-3) && index_thres <= (index_CFDmax-2)) bThreshold = true;
        break;
    }
  }
  if(bThreshold){
    for(int i = index_CFDmax; i >= index_CFDmax-2 ; i--)
        if(signalCFD[i]<thres) bThreshold = false;
  }


  // === Calculate Tcfd = signalCFD cross 0
  //     quadratic function cfd = a.t^2 + b.t + c 
  if (bThreshold && bTriggered){
    double x1 = (double)samplesX[index_first];
    double x2 = (double)samplesX[index_first+1];
    double x3 = (double)samplesX[index_first+2];
    double y1 = (double)signalCFD[index_first];
    double y2 = (double)signalCFD[index_first+1];
    double y3 = (double)signalCFD[index_first+2];
    //Tcfd = x1 - (double)sample_size_ns * y1 / (y2 - y1) ;

    // cfd => y and t => x
    // we have 3 equations with 3 unknowns :  yi = a * xi*xi + b * xi + c
    //    from (3):       c = y3 - a * x3*x3 - b * x3
    // => c in (1): y1 - y3 = a * (x1*x1 - x3*x3) + b * (x1 - x3)
    //    c in (2): y2 - y3 = a * (x2*x2 - x3*x3) + b * (x2 - x3)
    // => from (1):       a = (y1 - y3) / (x1*x1 - x3*x3) - b / (x1 + x3)
    // => a in (2): y2 - y3 = (y1 - y3)*(x2*x2 - x3*x3) / (x1*x1 - x3*x3) - b * (x2*x2 - x3*x3) / (x1 + x3) + b * (x2 - x3)
    //              y2 - y3 = (y1 - y3)*(x2*x2 - x3*x3) / (x1*x1 - x3*x3) + b * (x2 - x3 - (x2*x2 - x3*x3) / (x1 + x3))
    //                    b = ( y2-y3 - (x2*x2-x3*x3)*(y1-y3)/(x1*x1-x3*x3)) ) / ( x2-x3 - (x2*x2-x3*x3)/(x1+x3) )
    double b = ( (y2-y3) - (((x2*x2-x3*x3)*(y1-y3))/(x1*x1-x3*x3)) ) / ((x2-x3) - ((x2*x2-x3*x3)/(x1+x3))) ;
    double a = ( (y1-y2) / (x1*x1 -x2*x2) ) - ( b / (x1+x2) ); 
    double c = y1 - a*x1*x1 - b*x1 ;
    double d = b*b - 4 * a * c ;

    
    if (d<0){
        std::cout << " no CFD time found !!!"  << std::endl;
        std::cout << " quadratic fit gives : " << std::endl;
        std::cout << " a = " << a << ", b = " << b << ", c = " << c << " => DELTA = " << d << std::endl;
    }
    else if (d==0){
        Tcfd = -b / (2*a);
    }
    else{
        if (std::abs(a)>1.e-12){
            double Tcfd_1 = (-b - sqrt(d)) / (2.*a);
            double Tcfd_2 = (-b + sqrt(d)) / (2.*a);
            if(x1 < Tcfd_1 && Tcfd_1 < x2){
                Tcfd = Tcfd_1;
                if(x1 < Tcfd_2 && Tcfd_2 < x3)
                   std::cout << "2 solutions for Tcfd : " << Tcfd_1 << ", and " << Tcfd_2 << std::endl;
            }
            else if(x1 < Tcfd_2 && Tcfd_2 < x3){
                Tcfd = Tcfd_2;
            }
            else{
                // this was always happening when a~0
                std::cout << "----no Tcfd found : min = " << x1 << ", max = " << x3 << " ns" << std::endl;
                std::cout << "    time_x1 = " << x1 << " , cfd_y1 = " << y1 << std::endl;
                std::cout << "    time_x2 = " << x2 << " , cfd_y2 = " << y2 << std::endl;
                std::cout << "    time_x3 = " << x3 << " , cfd_y3 = " << y3 << std::endl;
                std::cout << "       quadratic fit gives : a = " << a << ", b = " << b << ", c = " << c << " => DELTA = " << d << std::endl;
                std::cout << "       Tcfd_1 = " << Tcfd_1 << std::endl;
                std::cout << "       Tcfd_2 = " << Tcfd_2 << std::endl;
            }
        }// end of else abs(a) > 1.e-12
        else{ // linear extrapolation
            Tcfd = x1 - (double)sample_size_ns * y1 / (y2 - y1) ;
        }
    }
  }
  return Tcfd; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
double EpicSamplerProcessor::calculateCFD(const double fract, const int delay, const double thres, const int dt, double &Qmax, bool &Triggered, bool &Threshold) {
 
  samplesX.clear();
  signalCFD.clear();
  signalCFD = filteredSignal;
  
  const int nsamples_delay = delay / dt; 
  double    T_cfd  = -10000.;

  double Q_CFD_min = 1000;
  double CFD_threshold=thres; // used to be 10 and before 80.
  int    iQmax, imax;

  // === Convert to CFD signal
  for (int i = 0; i < filteredSignal.size(); ++i) { 

    // === fill time scale
    samplesX.push_back(i*2+1);

    // === get Qmax
    double Q2 = filteredSignal[i];
    if (Q2 > Qmax) {
      Qmax = Q2;
      iQmax = i;
    }

    // === Convert to CFD signal
    if (i >= nsamples_delay) {
      double cfdValue = Q2 * fract - filteredSignal[i - delay/dt];
      signalCFD[i] = cfdValue;
      if (cfdValue < Q_CFD_min) {
        imax = i - nsamples_delay; 
        Q_CFD_min = cfdValue;
      }
    }// end of conversion for bin i
  }// end of loop pover the filteredSignal


  int min = 0 ;
  int max = 0 ;
  
  // imax -> Peak Position of the CFD signal
  // Test the presence of the positive bump at the beginning of the CFD signal
  for (int i = imax; i >= 0 ; i--) {
    if (signalCFD[i] > 0) {
      min = i;
      max = i + 2;
      Triggered = true;
      break;
    }
  }

  int countThresh = 0;
  int joker = 2;
  
  //Test if the signal is significant compared to the fluctuations
  //We need to have at least 3 consecutive points below -CFD_threshold
  for (int i = min; i <= imax + 3; i++) {
    if (signalCFD[i] < -CFD_threshold) {
      countThresh++;
      if (countThresh >= 2){ //Owen : previous comment says at least 3 but code says at least 2
        Threshold = true;
        break;
      }
    } else if (signalCFD[i] > -CFD_threshold && countThresh > 0) {
      if (joker > 0) {
        joker--;
        continue;
      } else {
        Threshold = false;
        break;
      }
    } else if (signalCFD[i] > -CFD_threshold && countThresh == 0) {
      continue;
    }
  }

  //Extract the CFD time using a quadratic fit on the CFD signal descent
  if (Triggered && Threshold) {

    double x1 = samplesX[min];
    double x2 = samplesX[min + 1];
    double x3 = samplesX[min + 2];
    double y1 = signalCFD[min];
    double y2 = signalCFD[min + 1];
    double y3 = signalCFD[min + 2];
    
    double a, b, cc, d;
    b  = ((y2-y3) - (x2*x2-x3*x3)*(y1-y2)/(x1*x1-x2*x2)) / (-(x2*x2-x3*x3)/(x1+x2) + x2 - x3);
    a  = -b / (x1 + x2) + (y1 - y2) / (x1*x1 - x2*x2);
    cc = y1 - b*x1 - a*x1*x1;
    //std::cout << " b = " << b << std::endl;
    //std::cout << " b must be compared to " << ((y2-y3) - (x2*x2-x3*x3)*(y1-y3)/(x1*x1-x3*x3)) / (-(x2*x2-x3*x3)/(x1+x3) + x2 - x3) << std::endl;

    d = b*b - 4.*a*cc;
    if (std::abs(a) > 1e-15) {
      if (d >= 0) {
        std::cout << " a = " << a << ", b = " << b << ", cc = " << cc << " => DELTA = " << d << std::endl;
        T_cfd = 0.5 * (-b - sqrt(d)) / a;
        std::cout << "other solution would be : " << 0.5 * (-b + sqrt(d)) / a;
      } else {
        Triggered = false;
        T_cfd = -10000;
      }
    } else {
      std::cout << " b = " << b << ", cc = " << cc << std::endl;
      T_cfd = -cc / b;
    }
    //T_cfd = T_cfd * dt; // sampler to ns
  } else {
    T_cfd = -10000;
  }
  // To remove negative Q
  if (T_cfd <= 6) {
    Triggered = false;
  }
  return T_cfd;
}




double EpicSamplerProcessor::integrateSignal(int binWidth, double startTime, double endTime) {
  
  // Temporal bounds verification
  if (startTime >= endTime || filteredSignal.empty()) {
    std::cerr << "!!!! !!!! Warning : Empty signal or invalid gates" << std::endl;
    std::cout << "start " << startTime << std::endl;
    std::cout << "end " << endTime << std::endl;
    return 0.0;
  }

  //std::cout << "signal size" << filteredSignal.size() << std::endl;
  
  // Calculating start and end indices for the temporal boundaries
  int startIndex = static_cast<int>(startTime / binWidth);
  int endIndex = static_cast<int>(endTime / binWidth);

  // Fraction of the initial bin
  double startFraction = 1.0 - (startTime - startIndex * binWidth) / binWidth;

  // Fraction of the final bin
  double endFraction = (endTime - endIndex * binWidth) / binWidth;
  
  /* std::cout << startIndex <<" "<< endIndex << std::endl; */
  // Sum of complete bins between the bounds
  double integral = 0.0;
  /* for (int i = startIndex + 1; i < endIndex; ++i) { */
  for (int i = startIndex +1; i < endIndex; ++i) {
	  if (filteredSignal[i] > 1e6){
	  }
    integral += filteredSignal[i] * binWidth;
  }
  /* std::cout << endTime << " " << endIndex << " " << startFraction <<" " << endFraction <<" " <<" int " << integral << std::endl; */
  // Adding fractions at the endpoints
  integral += filteredSignal[startIndex] * startFraction * binWidth;
  if (endIndex < filteredSignal.size()){ //to avoid going out of the signal
  	integral += filteredSignal[endIndex] * endFraction * binWidth;
  }

  return integral;
}

void EpicSamplerProcessor::drawRawSignal() const {
   /* TCanvas* canvas = new TCanvas("SignalCanvas", "Signal Canvas", 800, 600); */
  //TODO : error if I uncomment these lines
  /////TGraph* graph = new TGraph(samples.size(), samplesX.data(), samples.data());
  /////graph->SetTitle("Signal");
  /////graph->Draw("APL");
   
  /* canvas->Update(); */
  /* gPad->WaitPrimitive(); */
  /* gSystem->Sleep(5000); */
}

double EpicSamplerProcessor::GetAmpStartGate(int binWidth, double startGate) {
	
  // Calculating start index for the temporal boundarie
  int startIndex = static_cast<int>(startGate / binWidth);

  return filteredSignal[startIndex];
}

void EpicSamplerProcessor::drawFilteredSignal() const {
  // TCanvas* canvas = new TCanvas("SignalCanvas", "Signal Canvas", 800, 600);
  //TODO : error if I uncomment these lines
  /////TGraph* graph = new TGraph(filteredSignal.size(), samplesX.data(), filteredSignal.data());
  /////graph->SetTitle("Filtered Signal");
  /////graph->SetMarkerStyle(20);
  /////graph->Draw("APL");
  // canvas->Update();
  /* gPad->WaitPrimitive(); */
  // gSystem->Sleep(5000);  // Pause pendant 5 secondes
}

void EpicSamplerProcessor::drawSignalCFD() const {
  // TCanvas* canvas = new TCanvas("CFDCanvas", "CFD Canvas", 800, 600);
  //TODO : error if I uncomment these lines
  //////TGraph* graph = new TGraph(signalCFD.size(), samplesX.data(), signalCFD.data());
  //////graph->SetTitle("Signal CFD");
  //////graph->Draw("AL");
  // canvas->Update();
  // gSystem->Sleep(5000);  // Pause pendant 5 secondes
  /* gPad->WaitPrimitive(); */
}
