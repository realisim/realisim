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
SET(QT_USE_QTXML 1)
SET(QT_USE_QTOPENGL 1)
INCLUDE(${QT_USE_FILE})

MACRO( GENERATE_EXE iExecutableName )
MESSAGE( "PROJECT_INCLUDE_DIRS: ${PROJECT_INCLUDE_DIRS}" )
#   MESSAGE( "sources: ${SOURCES}" )
#   MESSAGE( "executable: ${iExecutableName}" )
#   MESSAGE( "BINDIR: ${BINDIR}" )
#   MESSAGE( "MOC_HEADER: ${MOC_HEADERS}" )
  
  SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINDIR} )
  
  QT_WRAP_CPP (${PROJECT_NAME} MOC_FILES ${MOC_HEADERS})
  
  INCLUDE_DIRECTORIES( ${PROJECT_INCLUDE_DIRS} )
  ADD_EXECUTABLE( ${iExecutableName} ${INCLUDES} ${SOURCES} ${RESOURCES} ${MOC_FILES} )
  TARGET_LINK_LIBRARIES(${iExecutableName} ${QT_LIBRARIES} ${PROJECT_LINK_LIBRAIRIES})
ENDMACRO( GENERATE_EXE  )

MACRO(ADD_CHIPMUNK)
  SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/thirdParty/chipmunk/4.1.0/. )
  
  SET( PROJECT_LINK_LIBRAIRIES
    ${PROJECT_LINK_LIBRAIRIES}
    ${CMAKE_SOURCE_DIR}/thirdParty/chipmunk/4.1.0/libChipmunk.a)
ENDMACRO(ADD_CHIPMUNK)

option( REALEDIT_BUILDOPTION "Build RealEdit" ON )
option( PROTOTYPES_BUILDOPTION "Build Prototypes" OFF )

add_subdirectory( projects )
add_subdirectory( prototypes )