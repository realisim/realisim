
PROJECT(OpenFlightViewer)

#output apps in bin/clouds

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/OpenFlightViewer)

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/utils" )

# openflight thirdParty
SET( OpenFligthPath "../../../OpenFlightReader/openFlight" )
SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} "../../../OpenFlightReader/" )
SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${OpenFligthPath} )
FILE (GLOB_RECURSE OPENFLIGHT_FILES ${OpenFligthPath}/*.h ${OpenFligthPath}/*.cpp)
SOURCE_GROUP("OpenFlight" FILES ${OPENFLIGHT_FILES})
SET( SOURCES ${SOURCES} ${OPENFLIGHT_FILES} )

MESSAGE( ${OpenFligthPath} )
MESSAGE( ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( ${OPENFLIGHT_FILES} )

ADD_QT_OPENGL()

#copy assets to bin/SMAA/assets
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_SOURCE_DIR}/bin/OpenFlightViewer/assets )


GENERATE_EXE( ${PROJECT_NAME} )