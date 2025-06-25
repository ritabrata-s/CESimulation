#----------------------------------------------------------------------------
####################### Build Analysis shared library ##################
file(GLOB ana_sources *.cc)
list(REMOVE_ITEM ana_sources ${CMAKE_CURRENT_LIST_DIR}/CEAna.cc)
#
#----------------------------------------------------------------------------
# Set target
add_library(Analysis SHARED ${ana_sources})
#
#----------------------------------------------------------------------------
# Link libraries
target_link_libraries(Analysis 
                      ${ROOT_LIBRARIES})
#
#----------------------------------------------------------------------------
# Set version number
#set_target_properties(Digitizer PROPERTIES VERSION ${VERSION})
#
#----------------------------------------------------------------------------
# Set install rules
install(TARGETS Analysis DESTINATION lib)
#
#----------------------------------------------------------------------------
#
####################### Add the executable CEAna ##################
# And link it to the other libraries
#
set(ana_name "CEAna")
add_executable(${ana_name} ${ana_name}.cc)
#
target_link_libraries(${ana_name}
		      Analysis)
#
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ${ana_name} DESTINATION bin)
#
#----------------------------------------------------------------------------
#
####################### Add the executable CELoc ##################
# And link it to the other libraries
#
set(loc_name "CELoc")
add_executable(${loc_name} ${loc_name}.cc)
#
target_link_libraries(${loc_name}
		      Analysis)
#
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ${loc_name} DESTINATION bin)
#
#----------------------------------------------------------------------------
