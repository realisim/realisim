# ROOT CMakeList
PROJECT( Realisim )

INCLUDE( "macro_component.cmake" )

cmake_minimum_required (VERSION 2.6)

SET(SOURCES)  #all .cpp files
SET(INCLUDES) #all .h files
SET(RESOURCES) #all resources files 
                  # .qrc Qt resources files
SET(MOC_HEADERS) #all files that contain Q_OBJECT macro and need to be moc'ed               
                  
SET(PROJECT_INCLUDE_DIRS) #additional include directory to add
SET(PROJECT_LINK_LIBRAIRIES) #additional link librairies                 
SET(BINDIR ${CMAKE_SOURCE_DIR}/distribution/bin)

#Set a global group for all cxx files ( usualy moc'ed files )

FIND_PACKAGE(Qt4)
INCLUDE(${QT_USE_FILE})

MACRO( GENERATE_EXE iExecutableName )
#   MESSAGE( "PROJECT_INCLUDE_DIRS: ${PROJECT_INCLUDE_DIRS}" )
#   Message("QT_PLUGINS_DIR: ${QT_PLUGINS_DIR}")
#   MESSAGE( "sources: ${SOURCES}" )
#   MESSAGE( "executable: ${iExecutableName}" )
#   MESSAGE( "BINDIR: ${BINDIR}" )
#   MESSAGE( "MOC_HEADER: ${MOC_HEADERS}" )
  
#  SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINDIR} )
  IF( MOC_HEADERS )
    QT_WRAP_CPP (${PROJECT_NAME} MOC_FILES ${MOC_HEADERS})
  ENDIF( MOC_HEADERS )
  
  INCLUDE_DIRECTORIES( ${PROJECT_INCLUDE_DIRS} )

  IF(APPLE)
    #-fvisibility=hidden sert a enlever les milliers de warnings de Qt depuis la version 4.5 
    ADD_DEFINITIONS(-fvisibility=hidden)
    SET( PROJECT_LINK_LIBRAIRIES
      ${PROJECT_LINK_LIBRAIRIES}
      )

    ADD_EXECUTABLE( ${iExecutableName} MACOSX_BUNDLE  ${INCLUDES} ${SOURCES} ${RESOURCES} ${MOC_FILES} )
  ELSE(APPLE)
    ADD_EXECUTABLE( ${iExecutableName} ${INCLUDES} ${SOURCES} ${RESOURCES} ${MOC_FILES} )
  ENDIF(APPLE)
  TARGET_LINK_LIBRARIES(${iExecutableName} ${QT_LIBRARIES} ${PROJECT_LINK_LIBRAIRIES})
ENDMACRO( GENERATE_EXE  )

MACRO(ADD_OPENCL)
  IF(APPLE)
    FIND_LIBRARY(OPEN_CL OpenCL)
    SET( PROJECT_LINK_LIBRAIRIES ${PROJECT_LINK_LIBRAIRIES} ${OPEN_CL} )
  ENDIF(APPLE)

  #pour windows… je sais pas.
ENDMACRO(ADD_OPENCL)

MACRO(ADD_OPENMP)
  IF(APPLE)
  find_package(OpenMP)
    IF(OPENMP_FOUND)
      set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_C_FLAGS}")
    ENDIF()
  ENDIF(APPLE)
ENDMACRO(ADD_OPENMP)

MACRO(ADD_OPENAL)
  IF(APPLE)
  find_package(OpenAL)
  IF(OPENAL_FOUND)
    MESSAGE("OPENAL FOUND - including directory ${OPENAL_INCLUDE_DIR}")
    SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} )
  ENDIF()
  ENDIF(APPLE)
ENDMACRO(ADD_OPENAL)

option( REALEDIT_BUILDOPTION "Build RealEdit" OFF )
option( SPRITEEDITOR_BUILDOPTION "Build SpriteEditor" ON )
option( REMOTEDESKTOP_BUILDOPTION "Build RemoteDesktop" OFF )
option( PROTOTYPES_BUILDOPTION "Build Prototypes" OFF )

add_subdirectory( projects )
add_subdirectory( prototypes )