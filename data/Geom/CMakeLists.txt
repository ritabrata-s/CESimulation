
file(GLOB geomdata_files *.txt *.gdml)
list(REMOVE_ITEM geomdata_files "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

install(FILES ${geomdata_files} DESTINATION data/Geom)
