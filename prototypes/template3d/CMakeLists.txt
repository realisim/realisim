
PROJECT(template3d)

#output apps in bin/clouds
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/template3d)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/template3d)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/template3d)

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )

ADD_QT_OPENGL()

#copy assets to bin/template3d
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_SOURCE_DIR}/bin/template3d/assets )

GENERATE_EXE( ${PROJECT_NAME} )