cmake_minimum_required (VERSION 3.0)                                            
project(epic CXX)                                                              
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)                                                   
include(CMakePackageConfigHelpers)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_INSTALL_PREFIX}/bin )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib )
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include )
set(CMAKE_INSTALL_LIBDIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
### nptool ###
find_package(nptool QUIET)
include_directories($ENV{NPTOOL}/include)
include_directories($ENV{NPTOOL_HOME}/$ENV{NPTOOL_ENV}/include)
link_directories($ENV{NPTOOL}/lib)
link_directories(../install/lib)
link_directories($ENV{NPTOOL_HOME}/$ENV{NPTOOL_ENV}/lib)
### ROOT ###
# find package 
find_package(ROOT QUIET)
if(${ROOT_FOUND})
  message(STATUS "ROOT found: ${ROOT_VERSION}" )
  # use this definition to isolate ROOT dependent code
  add_compile_definitions(ROOT_FOUND)
  # add usefull root fonction for dictionnary definition
  include(${ROOT_USE_FILE})
  # add include directory
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
else()
  message(STATUS "ROOT not found: compiling with no ROOT support" )
endif()
### GEANT4 ###
find_package(Geant4 QUIET)
if(${Geant4_FOUND})
  message(STATUS "Geant4 found: ${Geant4_VERSION}" )
  # use this definition to isolate Geant4 dependent code
  add_compile_definitions(Geant4_FOUND)
  # add usefull root fonction for dictionnary definition
  include(${Geant4_USE_FILE})
  # add include directory
  include_directories(${Geant4_INCLUDE_DIRS})
else()
  message(STATUS "Geant4 not found: compiling with no Geant4 support" )
endif()
### faster ###
add_library(fasterac UNKNOWN IMPORTED)
if(DEFINED ENV{FASTERAC})
  set(FASTERAC $ENV{FASTERAC})
else()
  set(FASTERAC "/usr/")
endif()
set_property(TARGET fasterac PROPERTY IMPORTED_LOCATION "${FASTERAC}/lib/libfasterac${CMAKE_SHARED_LIBRARY_SUFFIX}")
include_directories("${FASTERAC}/include")
message("Found fasterac library at: ${FASTERAC}")
if(EXISTS "${FASTERAC}/lib/libfasterac${CMAKE_SHARED_LIBRARY_SUFFIX}")
  set(FASTERAC_FOUND true)
else()
  set(FASTERAC_FOUND false)   
endif()
set (duplicate_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)
include_directories(BEFORE "src")

include_directories(AFTER $ENV{NPTOOL}/include)
include_directories(AFTER $ENV{NPTOOL_HOME}/$ENV{NPTOOL_ENV}/include)
add_subdirectory("src")

