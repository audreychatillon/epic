#ifndef EpicPhysics_h
#define EpicPhysics_h

// STL
#include <vector>
#include <iostream>
using namespace std;
// ROOT
#include "TObject.h"
#include "TVector3.h"
namespace epic {
class EpicPhysics {
 private:
        short  fFC_det;         // detector number (starting from 1)
        short  fFC_anode;       // anode number (starting from 1)
        double fFC_time_hf;     // Time of the last HF at the phys level to avoid event repeart in nponline bug
        double fFC_tof_raw;     // TofCal
        double fFC_tof_cal;     // TofCal
        double fFC_e;           // energy [MeV]
        double fFC_q1;          // Total charge
        bool   fFC_isAlpha;     // true if (Q1<q1_ALPHA_1Dcut)  //TODO to be changed for isFission (Q2/Q3 vs Q1) in TCutG


 public:
   //////////////////////////////////////////////////////////////
   // Constructor and destructor
   EpicPhysics(){};
   ~EpicPhysics(){};

   //////////////////////////////////////////////////////////////
   // Inherited from TObject and overriden to avoid warnings
 public:
   void Clear(){
        fFC_det     = -1.; 
        fFC_anode   = -1.;
        fFC_time_hf = -1.;
        fFC_tof_raw = -1.;
        fFC_tof_cal = -1.;
        fFC_e       = -1.;
        fFC_q1      = -1.;
        fFC_isAlpha = true;
        //fFC_det.clear();
        //fFC_anode.clear();
        //fFC_tof_cal.clear();
        //fFC_e.clear();
   };//!

 public:
   //////////////////////////////////////////////////////////////
   // Getters and Setters
   // Prefer inline declaration to avoid unnecessary called of
   // frequently used methods
   // add //! to avoid ROOT creating dictionnary for the methods

   //////////////////////    SETTERS    ////////////////////////
   
   inline void SetHit_fFC(
        const short&  det, 
        const short&  anode,
        const double& t_hf,
        const double& tof_raw_ns,
        const double& tof_cal_ns,
        const double& e_MeV,
        const double& q1,
        const bool&   alpha){
            fFC_det     = det;
            fFC_anode   = anode;
	    fFC_time_hf = t_hf;
            fFC_tof_raw = tof_raw_ns;
            fFC_tof_cal = tof_cal_ns;
            fFC_e       = e_MeV;
            fFC_q1      = q1;
            fFC_isAlpha = alpha;
        }; //!


    //////////////////////    GETTERS    ////////////////////////
    inline short  GetDetNbr()   const { return fFC_det ; };
    inline short  GetAnodeNbr() const { return fFC_anode ; };
    inline double GetTimeHF()   const { return fFC_time_hf ; };
    inline double GetTofRaw()   const { return fFC_tof_raw ; };
    inline double GetTofCal()   const { return fFC_tof_cal ; };
    inline double GetE()        const { return fFC_e ; };
    inline double GetQ1()       const { return fFC_q1 ; };
    inline bool   GetIsAlpha()  const { return fFC_isAlpha ; };

   //////////////////////////////////////////////////////////////
   // Required for ROOT dictionnary
   ClassDef(EpicPhysics, 1)  // EpicPhysics structure
};
}  // namespace epic 
#endif

