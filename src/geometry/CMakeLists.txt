#----------------------------------------------------------------------------
file(GLOB geo_sources *.cc)
#
#----------------------------------------------------------------------------
# Set target
add_library(Geometry SHARED ${geo_sources})
#
#----------------------------------------------------------------------------
# Link libraries
#target_link_libraries(Geometry Field)
#
#----------------------------------------------------------------------------
# Set version number
#set_target_properties(Geometry PROPERTIES VERSION ${VERSION})
#
#----------------------------------------------------------------------------
# Set install rules
install(TARGETS Geometry DESTINATION lib)
#
#----------------------------------------------------------------------------
