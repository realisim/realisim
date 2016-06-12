
PROJECT( sargam )

ADD_COMPONENT( "." "Application" )

#sources from utils
INCLUDE_DIRECTORIES(../../reusables)
FILE( GLOB utils
  ../../reusables/utils/Command.*
  ../../reusables/utils/CommandStack.*
  ../../reusables/utils/Log.*
  ../../reusables/utils/utilities.*
)
SOURCE_GROUP ("utils" FILES ${utils})

SET( SOURCES ${SOURCES}
  ${utils}
  "./CMakeLists.txt" )

ADD_QT_PRINTSUPPORT()
ADD_QT_NETWORK()

IF(APPLE)
  ADD_MAC_ICON( "./artwork/" "dockIcon.icns" )
ENDIF(APPLE)

IF(WIN32)
  ADD_WIN_ICON( "sargam.rc" )
ENDIF(WIN32)

GENERATE_EXE( ${PROJECT_NAME} )