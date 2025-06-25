#----------------------------------------------------------------------------
####################### Build Eventviewer shared library ##################
file(GLOB eview_sources *.cc)
list(REMOVE_ITEM eview_sources ${CMAKE_CURRENT_LIST_DIR}/CEView.cc)
#
#----------------------------------------------------------------------------
# Set target
add_library(Eventviewer SHARED ${eview_sources})
#
#----------------------------------------------------------------------------
# Link libraries
target_link_libraries(Eventviewer
		      Analysis
		      ${ROOT_LIBRARIES}
		      Geom
		      Eve
		      )
#
#----------------------------------------------------------------------------
# Set version number
#set_target_properties(Reconstructor PROPERTIES VERSION ${VERSION})
#
#----------------------------------------------------------------------------
# Set install rules
install(TARGETS Eventviewer DESTINATION lib)
#
#----------------------------------------------------------------------------
####################### Build EventViewer dictionary ##################
ROOT_GENERATE_DICTIONARY(EventViewer_DICT
	${PROJECT_SOURCE_DIR}/include/eventviewer/CEEventViewer.hh
	MODULE Eventviewer
        LINKDEF ${PROJECT_SOURCE_DIR}/include/eventviewer/CEEventViewerLinkDef.hh)
#
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libEventviewer_rdict.pcm DESTINATION lib)
#
#----------------------------------------------------------------------------
####################### Add the executable CEView ##################
#
set(eview_name "CEView")
add_executable(${eview_name} ${eview_name}.cc)
#
target_link_libraries(${eview_name}
                      Eventviewer
                     )
#
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ${eview_name} DESTINATION bin)
#
#----------------------------------------------------------------------------
