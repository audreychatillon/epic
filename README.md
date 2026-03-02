#Epic Plugin for NPTool V4

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

## Installation

for installation of NPTool V4:
https://nptool.in2p3.fr/manual-v4/manual-installation-v4/
for plugin management:
https://nptool.in2p3.fr/manual-v4/plugins-management/

nptool --install epic
 - install the epic plugin in ~/.local/nptool/default/
 - copy all EpicXXX.h in ~/.local/nptool/default/include/ 

## Create your project
