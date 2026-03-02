# for data analysis and conversion
add_library(NPEpic SHARED EpicDetector.h EpicDetector.cxx EpicData.h EpicData.cxx EpicPhysics.h EpicSpectra.cxx EpicSpectra.h EpicSamplerProcessor.h EpicSamplerProcessor.cpp)
target_link_libraries(NPEpic PUBLIC ROOT::RIO ROOT::Tree ROOT::Gpad ROOT::Physics ROOT::Imt NPRoot NPPhysicsEssential nptool )
if(FASTERAC_FOUND)
  target_compile_definitions(NPEpic PUBLIC FASTERAC_FOUND)
  target_link_libraries(NPEpic PUBLIC fasterac )
endif()
# if(PHYSICSESSENTIAL_FOUND)
#   target_compile_definitions(NPEpic PUBLIC PHYSICSESENTIAL_FOUND)
#   target_link_libraries(NPEpic PUBLIC physics-essential )
# endif()
# for simulation
if(Geant4_FOUND)
add_library(NPEpicGeant4 SHARED EpicGeant4.h EpicGeant4.cxx)
target_link_libraries(NPEpicGeant4 PUBLIC NPEpic NPGeant4 nptool)
endif()
install(DIRECTORY ./ DESTINATION ${CMAKE_INCLUDE_OUTPUT_DIRECTORY} FILES_MATCHING PATTERN "*.h" )
ROOT_GENERATE_DICTIONARY(EpicDict EpicData.h EpicPhysics.h DEPENDENCIES EpicData.h EpicPhysics.h LINKDEF Epiclinkdef.hh MODULE NPEpic)

