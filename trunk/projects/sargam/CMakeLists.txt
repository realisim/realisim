
PROJECT( sargam )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/utils" )

ADD_QT_PRINTSUPPORT()

IF(APPLE)
  ADD_MAC_ICON( "./artwork/" "dockIcon.icns" )
ENDIF(APPLE)

IF(WIN32)
  ADD_WIN_ICON( "sargam.rc" )
ENDIF(WIN32)

GENERATE_EXE( ${PROJECT_NAME} )