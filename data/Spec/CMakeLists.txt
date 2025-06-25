
file(GLOB specdata_files *.txt)
list(REMOVE_ITEM specdata_files "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

install(FILES ${specdata_files} DESTINATION data/Spec)
