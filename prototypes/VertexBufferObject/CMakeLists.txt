
PROJECT(Vbo)

#output apps in bin/Vbo
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/Vbo)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/Vbo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Vbo)

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/utils" )

ADD_QT_OPENGL()

#copy assets to bin/Vbo
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_SOURCE_DIR}/bin/Vbo/assets )

GENERATE_EXE( ${PROJECT_NAME} )