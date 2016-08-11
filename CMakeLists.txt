# ROOT CMakeList
PROJECT( Realisim )

cmake_minimum_required (VERSION 2.8.11)

SET(THIRDPARTIESDIR thirdParties)
SET(SOURCES)  #all .cpp files
SET(INCLUDES) #all .h files
SET(RESOURCES) #all resources files 
                  # .qrc Qt resources files
SET(MAC_ICONS) #icones pour mac, le path doit etre un path absolu vers le fichier
                  
SET(PROJECT_INCLUDE_DIRS) #additional include directory to add
SET(PROJECT_LINK_LIBRAIRIES) #additional link librairies                 

INCLUDE( "macro_component.cmake" )
INCLUDE( "CMakeModules/tinyObjLoader.cmake" )

#Set a global group for all cxx files ( usualy moc'ed files )
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Widgets finds its own dependencies (QtGui and QtCore).
FIND_PACKAGE(Qt5Widgets)

MACRO( GENERATE_EXE iExecutableName )
#  MESSAGE( "Qt5Widgets_INCLUDE_DIRS: ${Qt5Widgets_INCLUDE_DIRS}" )
#  MESSAGE( "Qt5PrintSupport_INCLUDE_DIRS: ${Qt5PrintSupport_INCLUDE_DIRS}" )
#   MESSAGE( "Project source dir: ${PROJECT_SOURCE_DIR}" )
#   MESSAGE( "PROJECT_INCLUDE_DIRS: ${PROJECT_INCLUDE_DIRS}" )
#   Message("QT_PLUGINS_DIR: ${QT_PLUGINS_DIR}")
#   MESSAGE( "sources: ${SOURCES}" )
#   MESSAGE( "executable: ${iExecutableName}" )
#   MESSAGE( "MAC_ICONS: ${MAC_ICONS}" )
  
  SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} )
  
  INCLUDE_DIRECTORIES( ${PROJECT_INCLUDE_DIRS} )

  IF(APPLE)
    #-fvisibility=hidden sert a enlever les milliers de warnings de Qt depuis la version 4.5 
    ADD_DEFINITIONS(-fvisibility=hidden)
    SET( PROJECT_LINK_LIBRAIRIES
      ${PROJECT_LINK_LIBRAIRIES}
      )

    ADD_EXECUTABLE( ${iExecutableName} MACOSX_BUNDLE ${INCLUDES} ${SOURCES} ${RESOURCES} ${MOC_FILES} ${MAC_ICONS})
  ELSE(APPLE)
    ADD_EXECUTABLE( ${iExecutableName} ${INCLUDES} ${SOURCES} ${RESOURCES} ${MOC_FILES} )
  ENDIF(APPLE)
  TARGET_LINK_LIBRARIES(${iExecutableName} Qt5::Widgets ${PROJECT_LINK_LIBRAIRIES} )
ENDMACRO( GENERATE_EXE  )

MACRO(ADD_QT_OPENGL)
  find_package(Qt5OpenGL)
  
  # OpenGL & GLEW library sur Mac
  find_package(OpenGL REQUIRED)

  IF(WIN32)
    #on ajoute GLEW sur windows, il est dans les thirdParties
    SET( OPENGL_INCLUDE_DIR ${OPENGL_INCLUDE_DIR} ${Realisim_SOURCE_DIR}/thirdParties/glew-1.13.0/include )
    SET( OPENGL_LIBRARIES ${OPENGL_LIBRARIES} ${Realisim_SOURCE_DIR}/thirdParties/glew-1.13.0/lib/Release/x64/glew32.lib)
  ENDIF(WIN32)

  #MESSAGE( "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}" )
  #MESSAGE( "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}" )
  SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
  SET( PROJECT_LINK_LIBRAIRIES ${PROJECT_LINK_LIBRAIRIES} Qt5::OpenGL ${OPENGL_LIBRARIES} )
ENDMACRO(ADD_QT_OPENGL)

MACRO(ADD_OPENCL)
  IF(APPLE)
    FIND_LIBRARY(OPEN_CL OpenCL)
    SET( PROJECT_LINK_LIBRAIRIES ${PROJECT_LINK_LIBRAIRIES} ${OPEN_CL} )
  ENDIF(APPLE)

  #pour windows… je sais pas.
ENDMACRO(ADD_OPENCL)

MACRO(ADD_QT_NETWORK)
  find_package(Qt5Network REQUIRED)
  SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} )
  SET( PROJECT_LINK_LIBRAIRIES ${PROJECT_LINK_LIBRAIRIES} Qt5::Network )
ENDMACRO(ADD_QT_NETWORK)

MACRO(ADD_QT_PRINTSUPPORT)
  find_package(Qt5PrintSupport REQUIRED)
  SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS} )
  SET( PROJECT_LINK_LIBRAIRIES ${PROJECT_LINK_LIBRAIRIES} Qt5::PrintSupport )
ENDMACRO(ADD_QT_PRINTSUPPORT)

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
    MESSAGE("OPENAL FOUND - including directory ${OPENAL_INCLUDE_DIR}, including library ${OPENAL_LIBRARY}")
    SET( PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} )
    SET( PROJECT_LINK_LIBRAIRIES ${PROJECT_LINK_LIBRAIRIES} ${OPENAL_LIBRARY} )
  ENDIF()
  ENDIF(APPLE)
ENDMACRO(ADD_OPENAL)

#PATH est le path vers le fichier qui fini par un /
#FILE est le nom du fichier... abcd.icns
MACRO(ADD_MAC_ICON PATH FILE)
  #concatene le path et le nom de fichier et sera utiliser par GENERATE_EXE
  SET( MAC_ICONS ${MAC_ICONS} ${PATH}${FILE} )
  #change le plist pour indiquer le nom de l'icone
  set(MACOSX_BUNDLE_ICON_FILE ${FILE} )
  #copie le fichier icone vers le repertoire ressources du bundle.
  set_source_files_properties( ${MAC_ICONS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
ENDMACRO(ADD_MAC_ICON)

MACRO(ADD_WIN_ICON RC_FILE)
  SET( SOURCES ${SOURCES} ${RC_FILE} )
ENDMACRO(ADD_WIN_ICON)

option( SPRITEEDITOR_BUILDOPTION "Build SpriteEditor" OFF )
option( SARGAM_BUILDOPTION "Build Sargam" ON )
option( PROTOTYPES_BUILDOPTION "Build Prototypes" OFF )

add_subdirectory( projects )
add_subdirectory( prototypes )