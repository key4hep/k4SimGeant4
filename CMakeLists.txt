CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

project(k4SimGeant4 LANGUAGES CXX)

#---------------------------------------------------------------
# Options
option(BUILD_DOCS "Whether or not to create doxygen doc target." ON)
#---------------------------------------------------------------

find_package(ROOT COMPONENTS RIO Tree)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(Gaudi REQUIRED)
find_package(k4FWCore 1.3 REQUIRED)
find_package(EDM4HEP 0.99)
find_package(Geant4 REQUIRED)
find_package(DD4hep REQUIRED)
find_package(CLHEP REQUIRED)
#---------------------------------------------------------------

include(cmake/Key4hepConfig.cmake)

include(GNUInstallDirs)
include(CTest)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/InstallArea/ CACHE PATH
    "Install path prefix, prepended onto install directories." FORCE )
endif()

message(${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
message(${CMAKE_MODULE_PATH})

add_subdirectory(Detector)
add_subdirectory(SimG4Interface)
add_subdirectory(SimG4Common)
add_subdirectory(SimG4Fast)
add_subdirectory(SimG4Components)
add_subdirectory(SimG4Full)


#---------------------------------------------------------------
# Build documentation
if(BUILD_DOCS)
  include(cmake/k4SimGeant4Doxygen.cmake)
endif()
#---------------------------------------------------------------


install(EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  FILE "${PROJECT_NAME}Targets.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/"
  )

gaudi_install(CMAKE cmake/${PROJECT_NAME}Config.cmake)

