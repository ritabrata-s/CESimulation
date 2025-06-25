#----------------------------------------------------------------------------
####################### Build DetLev1 shared library ##################
#file(GLOB lev1_sources ZireDetLev1.cc)
#
#----------------------------------------------------------------------------
# Set target
#add_library(DetLev1 SHARED ${lev1_sources})
#
#----------------------------------------------------------------------------
# Link libraries
#target_link_libraries(DetLev1 
#		      		  ${ROOT_LIBRARIES})
#
#----------------------------------------------------------------------------
# Set version number
#set_target_properties(DetLev1 PROPERTIES VERSION ${VERSION})
#
#----------------------------------------------------------------------------
# Set install rules
#install(TARGETS DetLev1 DESTINATION lib)
#
#----------------------------------------------------------------------------
#
####################### Build DetLev1 dictionary ##################
#set(DetLevel1_DICT ZireDetLevel1Dict.cpp)
#ROOT_GENERATE_DICTIONARY(DetLevel1_DICT 
#						 ${PROJECT_SOURCE_DIR}/include/digitizer/ZireDetLevel1.hh 
#						 MODULE DetLev1 
#						 LINKDEF ${PROJECT_SOURCE_DIR}/include/digitizer/ZireDetLevel1LinkDef.hh)
#
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libDetLev1_rdict.pcm DESTINATION lib)
#
#----------------------------------------------------------------------------
####################### Build Digitizer shared library ##################
#file(GLOB digi_sources *.cc)
#list(REMOVE_ITEM digi_sources ${CMAKE_CURRENT_LIST_DIR}/ZireDigit.cc ${CMAKE_CURRENT_LIST_DIR}/ZireDetLev1.cc)
#
#----------------------------------------------------------------------------
# Set target
#add_library(Digitizer SHARED ${digi_sources})
#
#----------------------------------------------------------------------------
# Link libraries
#target_link_libraries(Digitizer 
#					  DetLev1
#		      		  ${ROOT_LIBRARIES})
#
#----------------------------------------------------------------------------
# Set version number
#set_target_properties(Digitizer PROPERTIES VERSION ${VERSION})
#
#----------------------------------------------------------------------------
# Set install rules
#install(TARGETS Digitizer DESTINATION lib)
#
#----------------------------------------------------------------------------
#
####################### Add the executable ZireDigit ##################
# And link it to the other libraries
#
#set(digi_name "ZireDigit")
#add_executable(${digi_name} ${digi_name}.cc)
#
#target_link_libraries(${digi_name}
#		       		  Digitizer)
#
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS ${digi_name} DESTINATION bin)
#
#----------------------------------------------------------------------------
