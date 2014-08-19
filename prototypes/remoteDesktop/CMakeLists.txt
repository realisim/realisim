
PROJECT( remoteDesktop )
SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/network" )
ADD_COMPONENT("../../reusables/utils" )

GENERATE_EXE( ${PROJECT_NAME} )