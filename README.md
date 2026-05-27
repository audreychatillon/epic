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

## Create and manage your project with the epic plugin

To create your project:
[Project Management](https://nptool.in2p3.fr/manual-v4/project-management/)
```bash
cd /folder/of/your/nptoolV4/projects/
nptool --new-project EPICproject
# This will
# - update the project.list in ~/.local/nptool/default/
# - create the folder EPICproject in /folder/of/your/nptoolV4/projects/
```
In the EPICproject folder, edit `project.yaml` and uncomment line 10, `default flag` adding the flag `--disable-mt` 
and puting the proper configuration file for the detector following `--detector`
See an example: [project.yaml](https://github.com/audreychatillon/EPICatGELINA/blob/main/project.yaml)

To compile (after each update of the plugin), in your project folder:
```bash
mkdir build install
cmake -B build -DCMAKE_INSTALL_PREFIX=install ./
make -C build/ install
```

To read FASTER data, configuration files must be provided:
 - `sample.pid`[example with one fission chamber](https://github.com/audreychatillon/EPICatGELINA/blob/main/pid_files/sample_EPICatGELINA.pid), [example with two fission chambers](https://github.com/audreychatillon/EPIC_WNR_90L/blob/main/pid_files/sample_EPICproto_run24.pid)
 - `detector/detector.yaml`[example with one fission chamber](https://github.com/audreychatillon/EPICatGELINA/blob/main/detector/detector.yaml),[example with two fission chambers](https://github.com/audreychatillon/EPIC_WNR_90L/blob/main/detector/detector_run24.yaml)
 - `ConfigEPIC.dat`[example with one fission chamber](https://github.com/audreychatillon/EPICatGELINA/blob/main/config_files/ConfigEPIC.dat),[example with two fission chambers](https://github.com/audreychatillon/EPIC_WNR_90L/blob/main/config_files/ConfigFC_run24.dat)
The command `npconversion` is processing the function `EpicDetector::BuildRawEvent()` to fill data at raw level as defined in `EpicData`
To write a TTree in output/conversion folder (see project.yaml) 
```bash
npconversion --input faster,sample.pid,/path/to/faster_file_num.fast --output root,EpicRawTree,raw_num.root
```

For on-line monitoring of raw histograms only in a browser with localhost:8082
```bash
npconversion --input faster,sample.pid,/path/to/faster_file_num.fast --output root,8080
nponline --input-raw root,localhost:8080 --interface root,8082
```
To convert FASTER data and build physical event by applying calibration parameters, 
you should add in `project.yaml` in the `default flag` line `--calibration calibration.txt` which gives the path of all calibration files.
Then run the command `npanalysis` to process the function `EpicDetector::BuildPhysicalEvent()` to fill data at calibration level as defined in `EpicPhysics`.
To monitore the calibrated spectra use the flag `--input-phy`
```bash
npconversion --input faster,sample.pid,file.fast  --output root,8080
npanalysis --input root,localhost:8080 --output root,8081
nponline --input-raw root,localhost:8080 --input-phy root,localhost:8081 --interface root,8082
```
