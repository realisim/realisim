
PROJECT( echoClient )

SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "app" )
ADD_COMPONENT("../../../../reusables/network" )
ADD_COMPONENT("../../../../reusables/utils" )
ADD_COMPONENT("../" "protocol" )

GENERATE_EXE( ${PROJECT_NAME} )