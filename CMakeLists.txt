#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.9 FATAL_ERROR)
project(CrystalEye)
#
#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
#
#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})
#
#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED)
#
#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(
		${PROJECT_SOURCE_DIR}/include
		${Geant4_INCLUDE_DIR}
		${ROOT_INCLUDE_DIRS})
#
#----------------------------------------------------------------------------
# Link directories
link_directories(${ROOT_LIBRARY_DIR})
#
#----------------------------------------------------------------------------
# Add subdirs for this project
#
add_subdirectory(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/data)
add_subdirectory(${PROJECT_SOURCE_DIR}/macro)
add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
#
#----------------------------------------------------------------------------
# Install the environment setup file to 'bin' directory under CMAKE_INSTALL_PREFIX
file(GLOB env_setup ce-setup.sh)
install(FILES ${env_setup} DESTINATION bin)
#
#----------------------------------------------------------------------------
