#ifndef EpicPhysics_h
#define EpicPhysics_h

// STL
#include <vector>
using namespace std;
// ROOT
#include "TVector3.h"
namespace epic {
class EpicPhysics {
  //////////////////////////////////////////////////////////////
  // data members are held into vectors in order
  // to allow multiplicity treatment
 private:
  
 public:
        vector<short>  fFC_det;      // detector number (starting from 1)
        vector<short>  fFC_anode;    // anode number (starting from 1)
        vector<bool>   fFC_trigPulser;
        vector<double> fFC_time;        // time of the anode [ns]
        vector<double> fFC_tof_raw;      // TofRaw
        vector<double> fFC_time_cfd;     // cfd time [ns]
        vector<double> fFC_time_qm;    // time of the maxiumum of the signal
        vector<double> fFC_qm;        // maximum amplitude of the signal
        vector<double> fFC_q1;          // Q1 = integration on the full signal
        vector<double> fFC_q2;          // Q2 = integration on the "rising"-time
        vector<double> fFC_q3;          // Q3 = integration on the "decay"-time

        double         fHF_time_current;
        double         fHF_time_previous;

        short          fSampler_det;
        short          fSampler_anode;
        vector<double> fSampler_signal;

   //////////////////////////////////////////////////////////////
   // Constructor and destructor

   //////////////////////////////////////////////////////////////
   // Constructor and destructor

 public:
   EpicPhysics(){};
   ~EpicPhysics(){};

   //////////////////////////////////////////////////////////////
   // Inherited from TObject and overriden to avoid warnings
 public:
   void Clear(){
        fFC_det.clear();
        fFC_anode.clear();
        fFC_trigPulser.clear();
        fFC_time.clear();
        fFC_tof_raw.clear();
        fFC_time_cfd.clear();
        fFC_time_qm.clear();
        fFC_qm.clear();
        fFC_q1.clear();
        fFC_q2.clear();
        fFC_q3.clear();
        fSampler_signal.clear();
   };//!

   //////////////////////////////////////////////////////////////
   // Getters and Setters
   // Prefer inline declaration to avoid unnecessary called of
   // frequently used methods
   // add //! to avoid ROOT creating dictionnary for the methods
 public:

   //////////////////////    SETTERS    ////////////////////////
   
   inline void SetHit_fFC(
        const short&  det, 
        const short&  anode,
        const bool&   trigPulser,
        const double& t_raw_ns,
        const double& t_tofraw_ns,
        const double& t_cfd_ns,
        const double& t_qmax_ns,
        const double& qmax,
        const double& q1,
        const double& q2,
        const double& q3){
            fFC_det.push_back(det);
            fFC_anode.push_back(anode);
            fFC_trigPulser.push_back(trigPulser);
            fFC_time.push_back(t_raw_ns);
            fFC_tof_raw.push_back(t_tofraw_ns);
            fFC_time_cfd.push_back(t_cfd_ns);
            fFC_time_qm.push_back(t_qmax_ns);
            fFC_qm.push_back(qmax);
            fFC_q1.push_back(q1);
            fFC_q2.push_back(q2);
            fFC_q3.push_back(q3);
        }; //!

    inline void SetHit_fHF(
        const double& t_c,
        const double& t_p){
            fHF_time_current  = t_c ;
            fHF_time_previous = t_p ;
    }; //!

    inline void SetHit_fSampler(
        const short& det,
        const short& anode,
        vector<double>& qSampler){
            fSampler_det = det;
            fSampler_anode = anode;
            fSampler_signal = qSampler;
    }; //!

   //////////////////////    GETTERS    ////////////////////////
  
   inline unsigned int GetMult() const { return fFC_det.size(); };


   //////////////////////////////////////////////////////////////
   // Required for ROOT dictionnary
   ClassDef(EpicPhysics, 1)  // EpicPhysics structure
};
}  // namespace epic 
#endif

