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
        short  fFC_det;         // detector number (starting from 1)
        short  fFC_anode;       // anode number (starting from 1)
        double fFC_tof_cal;     // TofCal
        double fFC_e;           // energy [MeV]

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
        fFC_det   = -1.; 
        fFC_anode = -1.;
        fFC_tof_cal = -1.;
        fFC_e       = -1.;
        //fFC_det.clear();
        //fFC_anode.clear();
        //fFC_tof_cal.clear();
        //fFC_e.clear();
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
        const double& tof_ns,
        const double& e_MeV){
            //fFC_det.push_back(det);
            //fFC_anode.push_back(anode);
            //fFC_tof_cal.push_back(tof_ns);
            //fFC_e.push_back(e_MeV);
            fFC_det = det;
            fFC_anode = anode;
            fFC_tof_cal = tof_ns;
            fFC_e = e_MeV;
        }; //!


   //////////////////////    GETTERS    ////////////////////////


   //////////////////////////////////////////////////////////////
   // Required for ROOT dictionnary
   ClassDef(EpicPhysics, 1)  // EpicPhysics structure
};
}  // namespace epic 
#endif

