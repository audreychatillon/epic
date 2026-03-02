#ifndef EpicData_h
#define EpicData_h
/*****************************************************************************
 * Original Author: Cyril Lenain     contact: lenain@cea.fr                  *
 *                                                                           *
 * Creation Date  : Octtber 2022                                             *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class hold Epic Raw data                                         *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// STL
#include <vector>
#include <iostream>
using namespace std;
// ROOT
#include "TObject.h"
namespace epic {
  class EpicData {
    //////////////////////////////////////////////////////////////
    // data members are hold into vectors in order
    // to allow multiplicity treatment
    private:
        vector<short>  fFC_DetNbr;      // detector number (starting from 1)
        vector<short>  fFC_AnodeNbr;    // anode number (starting from 1)
        vector<bool>   fFC_PulserTrig;  // true => fake fission / false => ion
        vector<double> fFC_Time;        // time of the anode [ns]
        vector<double> fFC_TofRaw;      // tof_raw [ns] of the incoming neutron
        vector<double> fFC_TimeCfd;     // cfd time [ns]
        vector<double> fFC_TimeQmax;    // time of the maximum of the signal
        vector<double> fFC_Qmax;        // maximum amplitude of the signal
        vector<double> fFC_Q1;          // Q1 = integration on the full signal
        vector<double> fFC_Q2;          // Q2 = integration on the "rising"-time
        vector<double> fFC_Q3;          // Q3 = integration on the "decay"-time

        short          fQmax_Index;    // index of the vector with Qmax starting from 0
        vector<double> fQmax_Sampler;  // sample of the anode with Qmax
        
        double         fHF_Time;      // time of the current HF
        double         fHF_TimePrev;  // time of the previous HF

    public:
        //////////////////////////////////////////////////////////////
        // Constructor and destructor
        EpicData(){};
        ~EpicData(){};
        //////////////////////////////////////////////////////////////

    public:
        //////////////////////////////////////////////////////////////
        // Inherited from TObject and overriden to avoid warnings
        void Clear();
        void Clear(const Option_t*) {};
        void Dump() const;
        //////////////////////////////////////////////////////////////

    public:
        //////////////////////////////////////////////////////////////
        // Getters and Setters
        // Prefer inline declaration to avoid unnecessary called of
        // frequently used methods
        // add //! to avoid ROOT creating dictionnary for the methods

        //////////////////////    SETTERS    ////////////////////////
        // fission chamber
        inline void SetDetNbr(const unsigned short& dnum)   {fFC_DetNbr.push_back(dnum);}//!
        inline void SetAnodeNbr(const unsigned short& anum) {fFC_AnodeNbr.push_back(anum);}//!
        inline void SetPulserTrig(const bool& kPulserTrig)  {fFC_PulserTrig.push_back(kPulserTrig);}//!
        inline void SetTimeFC(const double& t_ns)           {fFC_Time.push_back(t_ns);}//!
        inline void SetTofRaw(const double& tof_ns)         {fFC_TofRaw.push_back(tof_ns);}//!
        inline void SetTimeCfd(const double& t_cfd_ns)      {fFC_TimeCfd.push_back(t_cfd_ns);}//!
        inline void SetTimeQmax(const double& t_qmax_ns)    {fFC_TimeQmax.push_back(t_qmax_ns);}//!
        inline void SetQmax(const double& Qmax)             {fFC_Qmax.push_back(Qmax);}//
        inline void SetQ1(const double& Q1)                 {fFC_Q1.push_back(Q1);}//!
        inline void SetQ2(const double& Q2)                 {fFC_Q2.push_back(Q2);}//!
        inline void SetQ3(const double& Q3)                 {fFC_Q3.push_back(Q3);}//!
        // sample with Qmax
        inline void SetQmaxIndex(const short i)           {fQmax_Index = i;}//!
        inline void SetSampler(const vector<double> v_q)  {fQmax_Sampler=v_q;}//!
        // HF
        inline void SetTimeHF(const double& t_ns)     {fHF_Time = t_ns;}//!
        inline void SetTimePrevHF(const double& t_ns) {fHF_TimePrev = t_ns;}//!
        
        //////////////////////    GETTERS    ////////////////////////
        // fission chamber
        inline unsigned short GetFCMult() const {return (unsigned short)fFC_AnodeNbr.size();}//!
        inline int GetDetNbr(const unsigned int &i) const {return fFC_DetNbr[i];}//!
        inline int GetAnodeNbr(const unsigned int &i) const {return fFC_AnodeNbr[i];}//!
        inline bool GetPulserTrig(const unsigned int &i) const {return fFC_PulserTrig[i];}//!
        inline double GetTimeFC(const unsigned int &i) const {return fFC_Time[i];}//!
        inline double GetTofRaw(const unsigned int &i) const {return fFC_TofRaw[i];}//!
        inline double GetTimeCfd(const unsigned int &i) const {return fFC_TimeCfd[i];}//!
        inline double GetTimeQmax(const unsigned int &i) const {return fFC_TimeQmax[i];}//!
        inline double GetQmax(const unsigned int &i) const {return fFC_Qmax[i];}//!
        inline double GetQ1(const unsigned int &i) const {return fFC_Q1[i];}//!
        inline double GetQ2(const unsigned int &i) const {return fFC_Q2[i];}//!
        inline double GetQ3(const unsigned int &i) const {return fFC_Q3[i];}//!
        // sample with Qmax
        inline unsigned short GetQmaxIndex() const {return fQmax_Index;}//!
        inline double GetSample(const unsigned int &i) const {return fQmax_Sampler[i];}//!
        inline vector<double> GetSampler() const {return fQmax_Sampler;}//!   
        inline unsigned short GetSamplerSize() const {return (unsigned short)fQmax_Sampler.size();}//!
        // HF
        inline double GetTimeHF() const {return fHF_Time;}//!
        inline double GetTimePrevHF() const {return fHF_TimePrev;}//!

        //////////////////////////////////////////////////////////////
        // Required for ROOT dictionnary
        ClassDef(EpicData, 1) // EpicData structure
  };
} // namespace fission_chamber
#endif


