
PROJECT( NetworkClient )

SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "app" )
ADD_COMPONENT("../../reusables/network" )

GENERATE_EXE( ${PROJECT_NAME} )