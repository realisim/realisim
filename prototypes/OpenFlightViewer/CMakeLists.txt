
PROJECT(OpenFlightViewer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/OpenFlightViewer)

#--------------------------------------------------------------------------------
# -DMESSAGE_QUEUE_NO_THREADING: will remove all threading from messagequeues
#		it will be mono thread (mostly filestreamer and gpustreamer)
#--------------------------------------------------------------------------------
#add_definitions(-DMESSAGE_QUEUE_NO_THREADING)

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

ADD_QT_OPENGL()

#openMp
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

#copy assets to bin/SMAA/assets
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_SOURCE_DIR}/bin/OpenFlightViewer/assets )


GENERATE_EXE( ${PROJECT_NAME} )