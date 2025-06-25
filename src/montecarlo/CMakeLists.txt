#----------------------------------------------------------------------------
file(GLOB mc_sources *.cc)
list(REMOVE_ITEM mc_sources ${CMAKE_CURRENT_LIST_DIR}/CESimu.cc)
#
#----------------------------------------------------------------------------
# Set target
add_library(Montecarlo SHARED ${mc_sources})
#
#----------------------------------------------------------------------------
# Link libraries
target_link_libraries(Montecarlo 
		      ${Geant4_LIBRARIES})
#
#----------------------------------------------------------------------------
# Set version number
#set_target_properties(Montecarlo PROPERTIES VERSION ${VERSION})
#
#----------------------------------------------------------------------------
# Set install rules
install(TARGETS Montecarlo DESTINATION lib)
#
#----------------------------------------------------------------------------
# Add the executable G4Zire, and link it to the other libraries
#
set(simu_name "CESimu")
add_executable(${simu_name} ${simu_name}.cc)
#
target_link_libraries(${simu_name}
                      Geometry
                      Montecarlo
                     )
#
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ${simu_name} DESTINATION bin)
#
#----------------------------------------------------------------------------
