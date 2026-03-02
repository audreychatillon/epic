# Epic Plugin for NPTool V4

**Epic** is a plugin for **NPTool V4**, inspired by the fission chamber plugin but introducing flexible features.  

## Features

- No hard-coded values — fully configurable.  
- Set the number of fission chambers.  
- Set the number of anodes per chamber.  
- Support for multi-body fission chambers.  
- Anode distances are dynamically read (depending on grounding; may vary between experiments).  
- One sample per event is saved — specifically, the anode with the maximum signal.  

## Namespace

All classes are contained in the `epic` namespace:

```cpp
namespace epic {
    class EpicData {};
    class EpicPhysics {};
    class EpicSpectra {};
    class EpicGeant4 {};
}
