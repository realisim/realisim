
PROJECT( Sound )
#INCLUDE(${QT_USE_FILE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/sound" )
ADD_COMPONENT("../../reusables/utils" )

ADD_OPENAL()

#copy assets to bin/SMAA/assets
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/assets )

GENERATE_EXE( ${PROJECT_NAME} )