# ROOT CMakeList

INCLUDE( "macro_component.cmake" )

cmake_minimum_required (VERSION 2.6)

SET(SOURCES)  #all .cpp files
SET(INCLUDES) #all .h files
SET(RESOURCES) #all resources files 
                  # .qrc Qt resources files
SET(MOC_HEADERS) #all files that contain Q_OBJECT macro and need to be moc'ed               
                  
SET(PROJECT_INCLUDE_DIRS) #additional include directory to add                 
SET(BINDIR ${CMAKE_SOURCE_DIR}/distribution/bin)

#Set a global group for all cxx files ( usualy moc'ed files )


FIND_PACKAGE(Qt4)
SET(QT_USE_QTXML 1)
SET(QT_USE_QTOPENGL 1)
INCLUDE(${QT_USE_FILE})

MACRO( GENERATE_EXE iExecutableName )
#   MESSAGE( "sources: ${SOURCES}" )
#   MESSAGE( "executable: ${iExecutableName}" )
#   MESSAGE( "BINDIR: ${BINDIR}" )
  
  SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINDIR} )
  
#  MESSAGE( "MOC_HEADER: ${MOC_HEADERS}" )
  
  QT_WRAP_CPP (${PROJECT_NAME} MOC_FILES ${MOC_HEADERS})


  
  include_directories( ${PROJECT_INCLUDE_DIRS} )
  add_executable( ${iExecutableName} ${INCLUDES} ${SOURCES} ${RESOURCES} ${MOC_FILES} )
  target_link_libraries(${iExecutableName} ${QT_LIBRARIES})
  
ENDMACRO( GENERATE_EXE  )

option( REALEDIT_BUILDOPTION "Build RealEdit" ON )
option( PROTOTYPES_BUILDOPTION "Build Prototypes" OFF )

add_subdirectory( projects )
add_subdirectory( prototypes )