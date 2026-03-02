# Epic
Epic plugin for NPTool V4

This plugin is "inspired" by the fission chamber plugin with different features
- no hard coding
- set the number of the fission chamber
- set the number of anodes
- multi-body fission chamber
- distance of the anodes are read out (depending on the grounding, this may change from an experiment to another)
- one sample is saved per event (the anode with the max signal)

namespace epic{
    class EpicData {};
    class EpicPhysics {};
    class EpicSpectra {};
    class EpicGeant4 {};
}
