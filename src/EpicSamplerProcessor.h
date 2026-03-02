#ifndef EPICSAMPLERPROCESSOR_H
#define EPICSAMPLERPROCESSOR_H

#include <vector>
#include <TCanvas.h>

class EpicSamplerProcessorFC {
  private:
    bool applyFilter;                      // if LowPassFiltrer is necessary
    std::vector<double> samples;           // from Signal  = (sample - BLR)
    std::vector<double> filteredSignal;    // after LowPassFilter
    std::vector<int>    samplesX;          // vector to get the time range, with the same size of of filteredSignal
    std::vector<double> signalCFD;         // CFD signal = delayed signal - fraction of the signal

  public:
    EpicSamplerProcessor(const std::vector<double>& inputSamples, bool applyFilter_);
    double calculateCFDnew(double fract, const int delay, const double thres, const int dt, double &Qmax, double &tQmax, bool &Triggered, bool &Threshold);
    double calculateCFD(double fract, const int delay, const double thres, const int dt, double &Qmax, bool &Triggered, bool &Threshold);
    double integrateSignal(int binWidth, double startGate, double endGate);
    double GetAmpStartGate(int binWidth, double startGate);
    void drawRawSignal() const;
    void drawFilteredSignal() const;
    void drawSignalCFD() const;
};

#endif // SIGNALPROCESSOR_H
