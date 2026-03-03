###Epic Plugin for NPTool V4

**epic** is a plugin for **NPTool V4**, inspired by the fission chamber plugin but introducing flexible features.  

## Features

- No hard-coded values — fully configurable.  
- Set the number of fission chambers.  
- Set the number of anodes per chamber.  
- Support for multi-body fission chambers.  
- Anode distances are dynamically read (depending on grounding; may vary between experiments).  
- One sampler signal per event is saved — specifically, the anode with the maximum signal.  

## Namespace

All classes are contained in the `epic` namespace:

```cpp
namespace epic {
    class EpicData {};
    class EpicPhysics {};
    class EpicSpectra {};
    class EpicGeant4 {};
}
```

## Installation

For installation of NPTool V4:
[NPTool V4 Installation Manual](https://nptool.in2p3.fr/manual-v4/manual-installation-v4/)

For plugin management:
[Plugins Management](https://nptool.in2p3.fr/manual-v4/plugins-management/)
```bash
nptool --install epic
# This will
# - install the epic plugin in ~/.local/nptool/default/
# - copy all EpicXXX.h in ~/.local/nptool/default/include/ 
nptool --update epic
# This will update the last features of the plugin
```

## Create and manage your project with the EPIC plugin

To create your project:
[Project Management](https://nptool.in2p3.fr/manual-v4/project-management/)
```bash
cd /folder/of/your/nptoolV4/projects/
nptool --new-project EPICproject
# This will
# - update the project.list in ~/.local/nptool/default/
# - create the folder EPICproject in /folder/of/your/nptoolV4/projects/
```
In the EPICproject folder, edit `project.yaml` and uncomment line 10, adding the flag `--disable-mt` 
See an example: [project.yaml](https://github.com/audreychatillon/EPICatGELINA/blob/main/project.yaml)

To compile (after each update of the plugin), in your project folder:
```bash
mkdir build install
cmake -B build -DCMAKE_INSTALL_PREFIX=install ./
make -C build/ install
```

To read FASTER data, configuration files must be provided:
 - `sample.pid`[example](https://github.com/audreychatillon/EPICatGELINA/blob/main/pid_files/sample_EPICproto_run24.pid)
 - `detector/detector.yaml`[example](https://github.com/audreychatillon/EPICatGELINA/blob/main/detector/detector.yaml)
 - `ConfigEPIC.dat`[example](https://github.com/audreychatillon/EPICatGELINA/blob/main/config_files/ConfigEPIC.dat)
```bash
# To write a TTree in output/conversion folder (see project.yaml) 
npconversion --input faster,sample.pid,/path/to/FASTER/data/name_faster_file_num.fast --output root,RawTree,raw_num.root
# for on-line monitoriing in a browser with localhost:8082
npconversion --input faster,sample.pid,/path/to/FASTER/data/name_faster_file_num.fast --output root,8081
nponline --input-raw root,localhost:8081 --interface root,8082
```
