
PROJECT( platform )
SET(QT_USE_QTOPENGL 1)
SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/utils" )
ADD_COMPONENT("../../reusables/network" )
ADD_COMPONENT("./images" )

GENERATE_EXE( ${PROJECT_NAME} )