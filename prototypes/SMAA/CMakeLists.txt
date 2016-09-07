
PROJECT(SMAA)

include_directories( ./thirdParties/ )

#output apps in bin/clouds
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/SMAA)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/SMAA)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/SMAA)

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/utils" )

ADD_QT_OPENGL()

#copy assets to bin/SMAA/assets
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_SOURCE_DIR}/bin/SMAA/assets )

#copy jiminez smaa assets to bin/SMAA/assets
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/thirdParty/JimenezSmaa/71c806a/SMAA.hlsl DESTINATION ${CMAKE_SOURCE_DIR}/bin/SMAA/assets )

GENERATE_EXE( ${PROJECT_NAME} )